#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <filesystem> // C++17
#include <sstream>
#include <fstream>
#include <memory>
#include <ctime>
#include <unistd.h>
#include "Mutex.hpp"

namespace yzq
{
    using namespace yzq;
    const std::string gsep = "\r\n";

    // 刷新策略基类
    class LogStrategy
    {
    public:
        ~LogStrategy() = default;
        virtual void SyncLog(const std::string message) = 0; // 子类必须重写
    };

    // 显示器打印日志的策略 : 子类
    class ConsoleLogStrategy : public LogStrategy
    {
    public:
        ConsoleLogStrategy()
        {
        }

        void SyncLog(const std::string message) override
        {
            LockGuard lockguard(_mutex);
            std::cout << message << gsep;
        }

        ~ConsoleLogStrategy()
        {
        }

    private:
        Mutex _mutex;
    };

    // 文件打印日志的策略 : 子类
    const std::string defaultpath = "./log";
    const std::string defaultfile = "my.log";
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string &path = defaultpath, const std::string &file = defaultfile)
            : _path(path), _file(file)
        {
            // 创建文件目录
            LockGuard lockguard(_mutex);
            if (std::filesystem::exists(_path)) // 查看是否存在该路径
            {
                return;
            }
            try
            {
                std::filesystem::create_directories(_path);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        void SyncLog(const std::string message) override
        {
            LockGuard lockguard(_mutex);
            std::string filename = _path + (_path.back() == '/' ? "" : "/") + _file;
            std::ofstream out(filename, std::ios::app); // 以追加模式打开文件
            if (!out.is_open())
            {
                return; // 没打开返回
            }
            out << message << gsep; // 输入到文件中
            out.close();
        }

        ~FileLogStrategy()
        {
        }

    private:
        std::string _path; // 日志文件所在路径
        std::string _file; // 日志文件本身
        Mutex _mutex;
    };

    // 1. 形成日志等级
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    // 根据枚举类型返回对应字符串
    std::string Level2Str(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    // 获取当前时间
    std::string GetTimeStamp()
    {
        time_t curr = time(nullptr); // 获取当前时间
        struct tm curr_tm;
        localtime_r(&curr, &curr_tm); // 通过返回型参数将时间返回
        char timebuffer[128];
        snprintf(
            timebuffer, sizeof(timebuffer), "%4d-%02d-%02d %02d:%02d:%02d",
            curr_tm.tm_year + 1900,
            curr_tm.tm_mon + 1,
            curr_tm.tm_mday,
            curr_tm.tm_hour,
            curr_tm.tm_min,
            curr_tm.tm_sec);
        return std::string(timebuffer);
    }

    // 1. 形成日志 && 2. 根据不同的策略，完成刷新
    class Logger
    {
    public:
        // 刷新策略-控制台刷新
        void EnableConsoleLogStrategy()
        {
            _fflush_strategy = std::make_unique<ConsoleLogStrategy>();
        }

        // 刷新策略-文件刷新
        void EnableFileLogStrategy()
        {
            _fflush_strategy = std::make_unique<FileLogStrategy>();
        }

        Logger()
        {
            EnableConsoleLogStrategy(); // 默认控制台刷新
        }

        class LogMessage
        {
        public:
            LogMessage(LogLevel &level, std::string &src_name, int line_number, Logger &logger)
                : _curr_time(GetTimeStamp()), _level(level), _pid(getpid()), _src_name(src_name),
                  _line_number(line_number), _logger(logger)
            {
                // 日志前半部分
                std::stringstream ss;
                ss << "[" << _curr_time << "] "
                   << "[" << Level2Str(_level) << "] "
                   << "[" << _pid << "] "
                   << "[" << _src_name << "] "
                   << "[" << _line_number << "] "
                   << "- ";
                _loginfo = ss.str();
            }

            // LogMessage() << "hell world" << "XXXX" << 3.14 << 1234
            template <typename T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str(); // 将传进来的参数转化为字符串形式
                return *this;
            }

            ~LogMessage()
            {
                _logger._fflush_strategy->SyncLog(_loginfo);
            }

        private:
            std::string _curr_time; // 当前时间
            LogLevel _level;        // 日志等级
            pid_t _pid;             // 进程pid
            std::string _src_name;  // 来源
            int _line_number;       // 行号
            std::string _loginfo;   // 合并之后，一条完整的信息
            Logger &_logger;        // 返回自身
        };

        // LogMessage(DEBUG,name,line)
        LogMessage operator()(LogLevel level, std::string name, int line)
        {
            return LogMessage(level, name, line, *this);
        }

        ~Logger()
        {
        }

    private:
        std::unique_ptr<LogStrategy> _fflush_strategy; // 刷新策略
    };

    // 全局日志对象
    Logger logger;

// 使用宏，简化用户操作，获取文件名和行号
#define LOG(level) logger(level, __FILE__, __LINE__) // __FILE__魔术方法，获取当前文件名
#define Enable_Console_Log_Strategy() logger.EnableConsoleLogStrategy()
#define Enable_File_Log_Strategy() logger.EnableFileLogStrategy()

};