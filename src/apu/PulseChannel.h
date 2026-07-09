#ifndef PULSE_CHANNEL_H
#define PULSE_CHANNEL_H


#include <cstdint>


class PulseChannel
{
public:
    PulseChannel(uint8_t channelId);

    void WriteControl(uint8_t data);    // $4000/$4004
    void WriteSweep(uint8_t data);      // $4001/$4005
    void WriteTimerLow(uint8_t data);   // $4002/$4006
    void WriteTimerHigh(uint8_t data);  // $4003/$4007

    void ClockTimer();         // Called every APU half-cycle (every 2 CPU cycles)
    void ClockEnvelope();      // Called every quarter frame (every 7457.5 CPU cycles)
    void ClockSweep();         // Called every half frame (every 14913 CPU cycles)
    void ClockLengthCounter(); // Called every half frame (every 14913 CPU cycles)

    void SetEnabled(bool enabled);
    bool IsEnabled() const { return _lengthCounter > 0; }

    float GetSample() const; // Returns the current sample value (0.0 to 1.0)

private:
    bool IsMuted() const;             // Returns true if the channel is muted
                                      // (due to sweep or length counter)
    
    uint16_t GetTargetPeriod() const; // Returns the target period for the sweep unit
private:
    uint8_t _channelId; // 1 or 2 for pulse channels (affects sweep behavior)

    // Control $4000
    uint8_t _duty;           // Duty cycle (0-3): which determines the waveform shape
    bool    _lengthHalt;     // Length counter halt flag (also controls envelope loop)
    bool    _constantVolume; // Constant volume flag (if true, envelope is disabled)
    uint8_t _volumePeriod;   // Volume or envelope period (0-15)

    // Sweep $4001
    uint8_t _sweepCounter;   // Sweep counter (counts down to 0)
    bool    _sweepEnabled;   // Sweep unit enabled flag
    uint8_t _sweepPeriod;    // Sweep period (0-7)
    bool    _sweepNegate;    // Sweep negate flag (true for negative sweep)
    uint8_t _sweepShift;     // Sweep shift count (0-7)
    bool    _sweepReload;    // Sweep reload flag (set when writing to $4001)

    // Timer $4002/$4003
    uint16_t _timerPeriod;   // 11-bit timer value (0-2047)
    uint16_t _timerValue;    // Timer counter (counts down to 0)

    // Sequencer
    uint8_t _dutyStep;       // Current step in the duty sequence (0-7)

    // Length Counter
    uint8_t _lengthCounter;  // Length counter value (0-255)
    bool    _lengthEnabled;  // Length counter enabled flag (set when writing to $4003)

    // Envelope
    bool    _envelopeStart;   // Envelope start flag (set when writing to $4000)
    uint8_t _envelopeValue;   // Current volume (0-15)
    uint8_t _envelopeCounter; // Envelope divider (counts down to 0)

    // Duty waveforms: 4 sequences of 8 bits each.
    // 1 = channel outputs, 0 = silent
    static const uint8_t DUTY_TABLE[4][8];

    // Length counter lookup table (from NES APU documentation)
    static const uint8_t LENGTH_TABLE[32];
};

#endif // PULSE_CHANNEL_H