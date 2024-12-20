#pragma once
#include "CommonPublic.h"
#include <fstream>
#include <iostream>
#include <functional>

namespace XNet
{
    enum
    {
        LOG_LEVEL_DEBUG = 0,
        LOG_LEVEL_TRACE,
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_MAX,
    };

    class Log
    {
    public:
        Log();
        ~Log();

        void init(const char* logFile);
        void write(int level, const char* name, const char* file, unsigned int line, const char* fmt, ...);

        void setCB(std::function<void(const char*)> cb);            //如果设置了回调，串口和文件就不输出

        static void setGlobalLevel(int level);
        static int  getGlobalLevel();

    protected:
        std::mutex _mtx;
        std::string _filePath;
        std::ofstream _logFile;
        time_t _openTime;
        std::function<void(const char*)> _cb;
    };

    extern Log g_log;

    class TraceLog
    {
    public:
        TraceLog(const char* module, const char* file, const char* func, unsigned int line, const char* fmt, ...);
        ~TraceLog();

    protected:
        const char* _module = nullptr;
        const char* _file = nullptr;
        const char* _func = nullptr;
        unsigned int _line = 0;
        std::string _fmt;
    };
}

#ifndef LOG_MODULE_NAME
#define LOG_MODULE_NAME "Unkown"
#endif

#define LOGD(fmt, ...)      XNet::g_log.write(XNet::LOG_LEVEL_DEBUG, LOG_MODULE_NAME, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...)      XNet::g_log.write(XNet::LOG_LEVEL_INFO, LOG_MODULE_NAME, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...)      XNet::g_log.write(XNet::LOG_LEVEL_WARN, LOG_MODULE_NAME, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...)      XNet::g_log.write(XNet::LOG_LEVEL_ERROR, LOG_MODULE_NAME, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_ATTIME(level, interval, fmt, ...) \
{ \
    static XNet::WatchTimer wt; \
    if (wt.check(interval)) \
    { \
        XNet::g_log.write(level, LOG_MODULE_NAME, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
}

#define LOGD_ATTIME(interval, fmt, ...) LOG_ATTIME(XNet::LOG_LEVEL_DEBUG, interval, fmt, ##__VA_ARGS__)
#define LOGI_ATTIME(interval, fmt, ...) LOG_ATTIME(XNet::LOG_LEVEL_INFO, interval, fmt, ##__VA_ARGS__)
#define LOGW_ATTIME(interval, fmt, ...) LOG_ATTIME(XNet::LOG_LEVEL_WARN, interval, fmt, ##__VA_ARGS__)
#define LOGE_ATTIME(interval, fmt, ...) LOG_ATTIME(XNet::LOG_LEVEL_ERROR, interval, fmt, ##__VA_ARGS__)

#define TRACE_LOG(fmt, ...) XNet::TraceLog __traceLog__(LOG_MODULE_NAME, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)