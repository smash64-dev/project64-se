#include "trace.h"
#include "Config.h"
#include "settings.h"

#include <string.h>
#include <Common/Trace.h>
#include <Common/path.h>
#include <Common/Log.h>

#ifdef ANDROID
#include <android/log.h>

class AndroidLogger : public CTraceModule
{
    void Write(uint32_t module, uint8_t severity, const char * file, int line, const char * function, const char * Message)
    {
        switch (severity)
        {
        case TraceError: __android_log_print(ANDROID_LOG_ERROR, TraceModule(module), "%s: %s", function, Message); break;
        case TraceWarning:  __android_log_print(ANDROID_LOG_WARN, TraceModule(module), "%s: %s", function, Message); break;
        case TraceNotice: __android_log_print(ANDROID_LOG_INFO, TraceModule(module), "%s: %s", function, Message); break;
        case TraceInfo: __android_log_print(ANDROID_LOG_INFO, TraceModule(module), "%s: %s", function, Message); break;
        case TraceDebug: __android_log_print(ANDROID_LOG_DEBUG, TraceModule(module), "%s: %s", function, Message); break;
        case TraceVerbose: __android_log_print(ANDROID_LOG_VERBOSE, TraceModule(module), "%s: %s", function, Message); break;
        default: __android_log_print(ANDROID_LOG_UNKNOWN, TraceModule(module), "%s: %s", function, Message); break;
        }
    }

    void FlushTrace(void)
    {
    }
};
static AndroidLogger * g_AndroidLogger = nullptr;
#endif

static CTraceFileLog * g_LogFile = nullptr;

void SetupTrace(void)
{
    if (g_LogFile != nullptr)
    {
        return;
    }

#ifdef ANDROID
    if (g_AndroidLogger == nullptr)
    {
        g_AndroidLogger = new AndroidLogger();
    }
    TraceAddModule(g_AndroidLogger);
#endif
#ifdef _DEBUG
    TraceSetMaxModule(MaxTraceModuleGlide64, TraceInfo);
#else
    TraceSetMaxModule(MaxTraceModuleGlide64, TraceError);
#endif

    TraceSetModuleName(TraceMD5, "MD5");
    TraceSetModuleName(TraceThread, "Thread");
    TraceSetModuleName(TracePath, "Path");
    TraceSetModuleName(TraceSettings, "Settings");
    TraceSetModuleName(TraceUnknown, "Unknown");
    TraceSetModuleName(TraceGlide64, "Glide64");
    TraceSetModuleName(TraceInterface, "Interface");
    TraceSetModuleName(TraceResolution, "Resolution");
    TraceSetModuleName(TraceGlitch, "Glitch");
    TraceSetModuleName(TraceRDP, "RDP");
    TraceSetModuleName(TraceTLUT, "TLUT");
    TraceSetModuleName(TracePNG, "PNG");
    TraceSetModuleName(TraceOGLWrapper, "OGL Wrapper");
    TraceSetModuleName(TraceRDPCommands, "RDP Command");
}

void StartTrace(void)
{
    const char * log_dir = g_settings ? g_settings->log_dir() : nullptr;
    if (log_dir == nullptr || log_dir[0] == '\0')
    {
        return;
    }

    CPath LogFilePath(log_dir, "Glide64.log");
    if (!LogFilePath.DirectoryExists())
    {
        LogFilePath.DirectoryCreate();
    }
    g_LogFile = new CTraceFileLog(LogFilePath, g_settings->FlushLogs(), CLog::Log_New, 500);
    TraceAddModule(g_LogFile);
}

void StopTrace(void)
{
    if (g_LogFile)
    {
        TraceRemoveModule(g_LogFile);
        delete g_LogFile;
        g_LogFile = nullptr;
    }
}
