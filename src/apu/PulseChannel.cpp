#include "PulseChannel.h"


// Duty waveforms — each row is one of 4 duty cycle shapes
// Read left-to-right as the sequencer steps through positions 0-7
const uint8_t PulseChannel::DUTY_TABLE[4][8] = {
    { 0, 1, 0, 0, 0, 0, 0, 0 },  // 12.5%
    { 0, 1, 1, 0, 0, 0, 0, 0 },  // 25%
    { 0, 1, 1, 1, 1, 0, 0, 0 },  // 50%
    { 1, 0, 0, 1, 1, 1, 1, 1 },  // 75% (25% negated)
};

// Maps the 5-bit length counter index to actual note lengths (in frames at 60Hz)
const uint8_t PulseChannel::LENGTH_TABLE[32] = {
    10, 254, 20, 2,  40, 4,  80, 6,  160, 8,  60,  10, 14, 12, 26, 14,
    12, 16,  24, 18, 48, 20, 96, 22, 192, 24, 72,  26, 16, 28, 32, 30
};


PulseChannel::PulseChannel(uint8_t channelId)
    : _channelId(channelId),
      _duty(0),
      _lengthHalt(false),
      _constantVolume(false),
      _volumePeriod(0),
      _sweepEnabled(false),
      _sweepPeriod(0),
      _sweepNegate(false),
      _sweepShift(0),
      _sweepReload(false),
      _timerPeriod(0),
      _timerValue(0),
      _dutyStep(0),
      _lengthCounter(0),
      _lengthEnabled(false),
      _envelopeStart(false),
      _envelopeValue(0),
      _envelopeCounter(0)
{
}

void PulseChannel::WriteControl(uint8_t data)
{
    _duty           = (data >> 6) & 0x03;
    _lengthHalt     = (data >> 5) & 0x01;
    _constantVolume = (data >> 4) & 0x01;
    _volumePeriod   = data & 0x0F;
    _envelopeStart  = true; // Reset envelope on write
}

void PulseChannel::WriteSweep(uint8_t data)
{
    _sweepEnabled = (data >> 7) & 0x01;
    _sweepPeriod  = (data >> 4) & 0x07;
    _sweepNegate  = (data >> 3) & 0x01;
    _sweepShift   = data & 0x07;
    _sweepReload  = true; // Reload sweep on write
}

void PulseChannel::WriteTimerLow(uint8_t data)
{
    _timerPeriod = (_timerPeriod & 0xFF00) | data;
}

void PulseChannel::WriteTimerHigh(uint8_t data)
{
    _timerPeriod   = (_timerPeriod & 0x00FF) | ((data & 0x07) << 8);
    _lengthCounter = LENGTH_TABLE[(data >> 3) & 0x1F];
    _dutyStep      = 0;    // Reset duty step on write
    _envelopeStart = true; // Reset envelope on write
}

void PulseChannel::SetEnabled(bool enabled)
{
    _lengthEnabled = enabled;
    if (!_lengthEnabled)
        _lengthCounter = 0; // Disable channel by clearing length counter
}

void PulseChannel::ClockTimer()
{
    // Timer counts down; when it hits 0 it reloads and steps the sequencer
    if (_timerValue == 0)
    {
        _timerValue = _timerPeriod;
        _dutyStep   = (_dutyStep + 1) & 0x07; // Advance duty step
    }
    else
    {
        _timerValue--;
    }
}

void PulseChannel::ClockEnvelope()
{
    if (_envelopeStart)
    {
        _envelopeValue   = 15; // Reset volume to max
        _envelopeCounter = _volumePeriod;
        _envelopeStart   = false;
    }
    else
    {
        if (_envelopeCounter > 0)
        {
            _envelopeCounter--;
        }
        else
        {
            _envelopeCounter = _volumePeriod;
            if (_envelopeValue > 0)
                _envelopeValue--;
            else if (_lengthHalt) // Loop envelope if length halt is set
                _envelopeValue = 15;
        }
    }
}

void PulseChannel::ClockLengthCounter()
{
    if (!_lengthHalt && _lengthCounter > 0)
        _lengthCounter--;
}

void PulseChannel::ClockSweep()
{
    if (_sweepReload)
    {
        _sweepCounter = _sweepPeriod;
        _sweepReload  = false;
    }
    else if (_sweepCounter > 0)
    {
        _sweepCounter--;
    }
    else
    {
        _sweepCounter = _sweepPeriod;
        if (_sweepEnabled && _sweepShift > 0 && !IsMuted())
        {
            uint16_t targetPeriod = GetTargetPeriod();
            if (targetPeriod <= 0x7FF) // Ensure target period is valid
                _timerPeriod = targetPeriod;
        }
    }
}

uint16_t PulseChannel::GetTargetPeriod() const
{
    uint16_t delta = _timerPeriod >> _sweepShift;
    if (_sweepNegate)
    {
        // Channel 1 uses ones complement (subtract and subtract 1)
        // Channel 2 uses twos complement (just subtract)
        return _timerPeriod - delta - (_channelId == 1 ? 1 : 0);
    }
    else
    {
        return _timerPeriod + delta;
    }
}

bool PulseChannel::IsMuted() const
{
    // Muted when: timer period too low (< 8), target period overflow (> $7FF),
    // length counter is 0, or channel disabled
    return (_timerPeriod < 8)
        || (GetTargetPeriod() > 0x7FF)
        || (_lengthCounter == 0)
        || (_lengthEnabled == false);
}

float PulseChannel::GetSample() const
{
    if (IsMuted() || DUTY_TABLE[_duty][_dutyStep] == 0)
        return 0.0f;

    // Determine the output volume
    uint8_t volume = _constantVolume ? _volumePeriod : _envelopeValue;

    // Return normalized sample value (0.0 to 1.0)
    return (volume / 15.0f);
}
