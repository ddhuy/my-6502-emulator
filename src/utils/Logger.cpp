#include "Logger.h"
#include <iostream>
#include <cstdio>
#include <cstdarg>

Logger::Logger()
    : currentLevel(LogLevel::INFO),
      consoleOutput(true),
      fileOutput(false),
      timestamps(true),
      initialized(false),
      logCount(0),
      errorCount(0)
{
}

Logger::~Logger()
{
    Shutdown();
}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

void Logger::Initialize(const std::string& filename, LogLevel level)
{
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (initialized)
    {
        Shutdown();
    }
    
    logFile.open(filename, std::ios::out | std::ios::trunc);
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }
    
    currentLevel = level;
    fileOutput = true;
    initialized = true;
    
    LOG_INFO("=== Logger initialized ===");
    LOG_INFO("Log file: %s", filename);
    LOG_INFO("Log level: %s", GetLevelString(level));
}

void Logger::Shutdown()
{
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (initialized && logFile.is_open())
    {
        LOG_INFO("=== Logger shutdown ===");
        logFile.close();
    }
    
    initialized = false;
}

void Logger::SetLogLevel(LogLevel level)
{
    currentLevel = level;
}

void Logger::SetConsoleOutput(bool enabled)
{
    consoleOutput = enabled;
}

void Logger::SetFileOutput(bool enabled)
{
    fileOutput = enabled;
}

void Logger::SetTimestamps(bool enabled)
{
    timestamps = enabled;
}

void Logger::Log(LogLevel level, const char* file, int line, const char* func, const std::string& message)
{
    // Check if we should log this level
    if (level < currentLevel)
        return;
    
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string formattedMessage = FormatMessage(level, file, line, func, message);
    
    // Output to console
    if (consoleOutput)
    {
        if (level >= LogLevel::ERROR)
            std::cerr << formattedMessage << std::endl;
        else
            std::cout << formattedMessage << std::endl;
    }
    
    // Output to file
    if (fileOutput && logFile.is_open())
    {
        logFile << formattedMessage << std::endl;
    }
    
    logCount++;
    if (level >= LogLevel::ERROR)
        errorCount++;
}

std::string Logger::FormatMessage(LogLevel level, const char* file, int line, const char* func, const std::string& message) const
{
    std::ostringstream oss;
    
    // Timestamp
    if (timestamps)
    {
        oss << "[" << GetTimestamp() << "] ";
    }
    
    // Level
    oss << "[" << GetLevelString(level) << "] ";

    // File & Line & Function
    oss << "[" << file << ":" << line << ":" << func << "] ";

    // Message
    oss << message;
    
    return oss.str();
}

std::string Logger::GetLevelString(LogLevel level) const
{
    switch (level)
    {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "?????";
    }
}

std::string Logger::GetTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

// Log level methods
void Logger::Trace(const char* file, int line, const char* func, const std::string& message)
{
    Log(LogLevel::TRACE, file, line, func, message);
}

void Logger::Debug(const char* file, int line, const char* func, const std::string& message)
{
    Log(LogLevel::DEBUG, file, line, func, message);
}

void Logger::Info(const char* file, int line, const char* func, const std::string& message)
{
    Log(LogLevel::INFO, file, line, func, message);
}

void Logger::Warn(const char* file, int line, const char* func, const std::string& message)
{
    Log(LogLevel::WARN, file, line, func, message);
}

void Logger::Error(const char* file, int line, const char* func, const std::string& message)
{
    Log(LogLevel::ERROR, file, line, func, message);
}

void Logger::Fatal(const char* file, int line, const char* func, const std::string& message)
{
    Log(LogLevel::FATAL, file, line, func, message);
}

// Formatted logging using variadic arguments
void Logger::TraceF(const char* file, int line, const char* func, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Trace(file, line, func, buffer);
}

void Logger::DebugF(const char* file, int line, const char* func, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Debug(file, line, func, buffer);
}

void Logger::InfoF(const char* file, int line, const char* func, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Info(file, line, func, buffer);
}

void Logger::WarnF(const char* file, int line, const char* func, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Warn(file, line, func, buffer);
}

void Logger::ErrorF(const char* file, int line, const char* func, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Error(file, line, func, buffer);
}

void Logger::FatalF(const char* file, int line, const char* func, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Fatal(file, line, func, buffer);
}

// CPU instruction logging (nestest compatible format)
void Logger::LogCPUInstruction(uint16_t pc, uint8_t opcode,
                               uint8_t a, uint8_t x, uint8_t y,
                               uint8_t sp, uint8_t status,
                               const std::string& disassembly)
{
    if (currentLevel > LogLevel::TRACE)
        return;
    
    std::lock_guard<std::mutex> lock(logMutex);
    
    // Format: PC:$XXXX OP [disasm] A:XX X:XX Y:XX P:XX SP:XX
    std::ostringstream oss;
    
    oss << std::hex << std::uppercase << std::setfill('0');
    oss << std::setw(4) << pc << "  ";
    oss << std::setw(4) << opcode << "  ";
    oss << disassembly;
    
    // Pad to align registers
    int padding = 48 - disassembly.length();
    if (padding > 0)
        oss << std::string(padding, ' ');
    
    oss << "A:" << std::setw(2) << (int)a << " ";
    oss << "X:" << std::setw(2) << (int)x << " ";
    oss << "Y:" << std::setw(2) << (int)y << " ";
    oss << "P:" << std::setw(2) << (int)status << " ";
    oss << "SP:" << std::setw(2) << (int)sp;
    
    if (fileOutput && logFile.is_open())
    {
        logFile << oss.str() << std::endl;
    }
    
    if (consoleOutput)
    {
        std::cout << oss.str() << std::endl;
    }
    
    logCount++;
}

// PPU event logging
void Logger::LogPPUEvent(const std::string& event)
{
    LOG_DEBUG("[PPU] %s", event.c_str());
}

void Logger::LogPPUWrite(uint16_t address, uint8_t value)
{
    std::ostringstream oss;
    oss << "[PPU] Write $" << std::hex << std::uppercase 
        << std::setw(4) << std::setfill('0') << address
        << " = $" << std::setw(2) << (int)value;
    LOG_TRACE("%s", oss.str());
}

void Logger::LogPPURead(uint16_t address, uint8_t value)
{
    std::ostringstream oss;
    oss << "[PPU] Read $" << std::hex << std::uppercase 
        << std::setw(4) << std::setfill('0') << address
        << " = $" << std::setw(2) << (int)value;
    LOG_TRACE("%s", oss.str());
}

// Memory access logging
void Logger::LogMemoryRead(uint16_t address, uint8_t value)
{
    std::ostringstream oss;
    oss << "[MEM] Read $" << std::hex << std::uppercase 
        << std::setw(4) << std::setfill('0') << address
        << " = $" << std::setw(2) << (int)value;
    LOG_TRACE("%s", oss.str());
}

void Logger::LogMemoryWrite(uint16_t address, uint8_t value)
{
    std::ostringstream oss;
    oss << "[MEM] Write $" << std::hex << std::uppercase 
        << std::setw(4) << std::setfill('0') << address
        << " = $" << std::setw(2) << (int)value;
    LOG_TRACE("%s", oss.str());
}

// Frame logging
void Logger::LogFrameStart(uint64_t frameNumber)
{
    std::ostringstream oss;
    oss << "=== Frame " << std::dec << frameNumber << " START ===";
    LOG_DEBUG("%s", oss.str());
}

void Logger::LogFrameEnd(uint64_t frameNumber)
{
    std::ostringstream oss;
    oss << "=== Frame " << std::dec << frameNumber << " END ===";
    LOG_DEBUG("%s", oss.str());
}

// Performance logging
void Logger::LogPerformance(const std::string& metric, double value)
{
    std::ostringstream oss;
    oss << "[PERF] " << metric << ": " << std::fixed 
        << std::setprecision(2) << value;
    LOG_INFO("%s", oss.str());
}

void Logger::Flush()
{
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (logFile.is_open())
    {
        logFile.flush();
    }
}