#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <mutex>
#include <memory>

// Log levels
enum class LogLevel
{
    TRACE = 0,    // Very detailed, every instruction
    DEBUG = 1,    // Debugging information
    INFO = 2,     // General information
    WARN = 3,     // Warnings
    ERROR = 4,    // Errors
    FATAL = 5     // Fatal errors
};

class Logger
{
public:
    // Singleton instance
    static Logger& GetInstance();
    
    // Initialize logger
    void Initialize(const std::string& filename, LogLevel level = LogLevel::INFO);
    
    // Close logger
    void Shutdown();
    
    // Set log level
    void SetLogLevel(LogLevel level);
    
    // Enable/disable console output
    void SetConsoleOutput(bool enabled);
    
    // Enable/disable file output
    void SetFileOutput(bool enabled);
    
    // Enable/disable timestamps
    void SetTimestamps(bool enabled);
    
    // Log methods
    void Trace(const char* file, int line, const char* func, const std::string& message);
    void Debug(const char* file, int line, const char* func, const std::string& message);
    void Info(const char* file, int line, const char* func, const std::string& message);
    void Warn(const char* file, int line, const char* func, const std::string& message);
    void Error(const char* file, int line, const char* func, const std::string& message);
    void Fatal(const char* file, int line, const char* func, const std::string& message);
    
    // Formatted log (like printf)
    void TraceF(const char* file, int line, const char* func, const char* format, ...);
    void DebugF(const char* file, int line, const char* func, const char* format, ...);
    void InfoF(const char* file, int line, const char* func, const char* format, ...);
    void WarnF(const char* file, int line, const char* func, const char* format, ...);
    void ErrorF(const char* file, int line, const char* func, const char* format, ...);
    void FatalF(const char* file, int line, const char* func, const char* format, ...);
    
    // CPU instruction logging (nestest format)
    void LogCPUInstruction(uint16_t pc, uint8_t opcode, 
                          uint8_t a, uint8_t x, uint8_t y, 
                          uint8_t sp, uint8_t status,
                          const std::string& disassembly);
    
    // PPU event logging
    void LogPPUEvent(const std::string& event);
    void LogPPUWrite(uint16_t address, uint8_t value);
    void LogPPURead(uint16_t address, uint8_t value);
    
    // Memory access logging
    void LogMemoryRead(uint16_t address, uint8_t value);
    void LogMemoryWrite(uint16_t address, uint8_t value);
    
    // Frame logging
    void LogFrameStart(uint64_t frameNumber);
    void LogFrameEnd(uint64_t frameNumber);
    
    // Performance logging
    void LogPerformance(const std::string& metric, double value);
    
    // Flush logs to disk
    void Flush();
    
    // Get statistics
    uint64_t GetLogCount() const { return logCount; }
    uint64_t GetErrorCount() const { return errorCount; }
    
private:
    Logger();
    ~Logger();
    
    // Delete copy constructor and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Internal log method
    void Log(LogLevel level, const char* file, int line, const char* func, const std::string& message);
    
    // Get level string
    std::string GetLevelString(LogLevel level) const;
    
    // Get timestamp
    std::string GetTimestamp() const;
    
    // Format message with level and timestamp
    std::string FormatMessage(LogLevel level, const char* file, int line, const char* func, const std::string& message) const;
    
    // Member variables
    std::ofstream logFile;
    LogLevel currentLevel;
    bool consoleOutput;
    bool fileOutput;
    bool timestamps;
    bool initialized;
    uint64_t logCount;
    uint64_t errorCount;
    std::mutex logMutex;  // Thread safety
};

// Convenience macros
// #define LOG_TRACE(msg) Logger::GetInstance().Trace(msg)
// #define LOG_DEBUG(msg) Logger::GetInstance().Debug(msg)
// #define LOG_INFO(msg)  Logger::GetInstance().Info(msg)
// #define LOG_WARN(msg)  Logger::GetInstance().Warn(msg)
// #define LOG_ERROR(msg) Logger::GetInstance().Error(msg)
// #define LOG_FATAL(msg) Logger::GetInstance().Fatal(msg)

// Formatted logging macros
#define LOG_TRACE(...) Logger::GetInstance().TraceF(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::GetInstance().DebugF(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)  Logger::GetInstance().InfoF(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARN(...)  Logger::GetInstance().WarnF(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...) Logger::GetInstance().ErrorF(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_FATAL(...) Logger::GetInstance().FatalF(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif // LOGGER_H