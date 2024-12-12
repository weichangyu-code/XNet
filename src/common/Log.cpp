#include "Log.h"
#include <stdarg.h>
#include <sys/time.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <thread>
#include "Buffer.h"
#include "Utils.h"
#include <sstream>

#ifndef __WIN32__
#include <sys/syscall.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace XNet
{
    Log g_log;
    int g_globalLevel = LOG_LEVEL_DEBUG;

    //add_compile_options(-fPIC)会导致线程局部变量访问崩溃
    // thread_local int g_threadLevel = LOG_LEVEL_DEBUG;

    Log::Log()
    {
        _openTime = 0;
    }

    Log::~Log()
    {

    }

    void Log::init(const char* logFile)
    {
        _filePath = logFile;
        _openTime = time(nullptr);
        _logFile.open(_filePath.c_str(), std::ios_base::app | std::ios_base::out | std::ios_base::binary);
    }
    
    void Log::setGlobalLevel(int level)
    {
        g_globalLevel = level;
    }
    
    int Log::getGlobalLevel()
    {
        return g_globalLevel;
    }

    void Log::write(int level, const char* name, const char* file, unsigned int line, const char* fmt, ...)
    {
        if (level < g_globalLevel)
        {
            return;
        }

        char bufFmt[1024];
        bufFmt[sizeof(bufFmt) - 1] = 0;
        va_list va;
        va_start(va, fmt);
        vsnprintf(bufFmt, sizeof(bufFmt) - 1, fmt, va);
        va_end(va);

        struct timeval now;
        gettimeofday(&now, nullptr);
        struct tm t = {0};
#ifdef __WIN32__
        time_t nowT = now.tv_sec;
        localtime_s(&t, (const time_t*)&nowT);
#else
        localtime_r(&now.tv_sec, &t);
#endif
        char bufTime[64];
        bufTime[sizeof(bufTime) - 1] = 0;
        strftime(bufTime, sizeof(bufTime) - 10, "[%Y-%m-%d %H:%M:%S.", &t);
        sprintf(bufTime + strlen(bufTime), "%03d]", int(now.tv_usec / 1000));

        const char* levelString = nullptr;
        switch (level)
        {
        case LOG_LEVEL_DEBUG:levelString = "[DEBUG]";break;
        case LOG_LEVEL_TRACE:levelString = "[TRACE]";break;
        case LOG_LEVEL_INFO:levelString = "[INFO]";break;
        case LOG_LEVEL_WARN:levelString = "[WARN]";break;
        case LOG_LEVEL_ERROR:levelString = "[ERROR]";break;
        default:levelString = "[DEBUG]";break;
        }

        const char* p = strrchr(file, '/');
        if (p == nullptr)
        {
            p = strrchr(file, '\\');
        }
        if (p != nullptr)
        {
            file = p + 1;
        }
        char bufPos[256];
        bufPos[sizeof(bufPos) - 1] = 0;
        snprintf(bufPos, sizeof(bufPos) - 1, "[%s:%d]", file, line);

        unsigned int threadId = getThreadID();
        char bufTId[32];
        bufTId[sizeof(bufTId) - 1] = 0;
        snprintf(bufTId, sizeof(bufTId) - 1, "[T:%u]", threadId);

        char bufModule[32];
        bufModule[sizeof(bufModule) - 1] = 0;
        snprintf(bufModule, sizeof(bufModule) - 1, "[%s]", name);

        //拼接
        std::stringstream ss;
        ss << bufModule << bufTime << bufTId << bufPos << levelString << bufFmt << std::endl;
        std::string s = ss.str();
        
        //回调
        if (_cb)
        {
            _cb(s.c_str());
            return;
        }

        // 打印到控制台
#ifdef __ANDROID__
        int androidLevel = ANDROID_LOG_DEBUG;
        switch (level)
        {
            case LOG_LEVEL_DEBUG:androidLevel = ANDROID_LOG_DEBUG;break;
            case LOG_LEVEL_INFO:androidLevel = ANDROID_LOG_INFO;break;
            case LOG_LEVEL_WARN:androidLevel = ANDROID_LOG_WARN;break;
            case LOG_LEVEL_ERROR:androidLevel = ANDROID_LOG_ERROR;break;
        }
        __android_log_print(androidLevel, bufModule, "%s", s.c_str());
#else
        std::cout << s;
#endif
        
        // 打印到文件
        if (level >= LOG_LEVEL_INFO && _logFile.is_open())
        {
            std::unique_lock<std::mutex> l(_mtx);

            // 判断是否跨天了
            if (_openTime / (3600*24) != now.tv_sec / (3600*24))
            {
                _logFile.close();

                // 关闭老的
                char bkFile[256] = {0};
                snprintf(bkFile, sizeof(bkFile) - 1, "%s.%d", _filePath.c_str(), t.tm_mday);
                ::remove(bkFile);
                ::rename(_filePath.c_str(), bkFile);

                _openTime = now.tv_sec;
                _logFile.open(_filePath.c_str(), std::ios_base::app | std::ios_base::out | std::ios_base::binary);
            }

            _logFile << s;
        }
    }

    void Log::setCB(std::function<void(const char*)> cb)
    {
        _cb = cb;
    }
        
    TraceLog::TraceLog(const char* module, const char* file, const char* func, unsigned int line, const char* fmt, ...)
    {
        _module = module;
        _file = file;
        _func = func;
        _line = line;
        
        char bufFmt[1024];
        bufFmt[sizeof(bufFmt) - 1] = 0;
        va_list va;
        va_start(va, fmt);
        vsnprintf(bufFmt, sizeof(bufFmt) - 1, fmt, va);
        va_end(va);
        _fmt = bufFmt;

        g_log.write(LOG_LEVEL_TRACE, _module, _file, _line, "[%s:%u][begin]%s", _func, _line, _fmt.c_str());
    }
        
    TraceLog::~TraceLog()
    {
        g_log.write(LOG_LEVEL_TRACE, _module, _file, _line, "[%s:%u][end]%s", _func, _line, _fmt.c_str());
    }
}