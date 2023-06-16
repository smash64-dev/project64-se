#include <stdafx.h>
#include "ScriptSystem.h"
#include "Debugger-Scripts.h"

#include "ScriptInstance.h"
#include "ScriptHook.h"

CScriptSystem::CScriptSystem(CDebuggerUI* debugger)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    m_NextCallbackId = 0;
    m_NumCallbacks = 0;

    m_Debugger = debugger;

    m_HookCPUExec = new CScriptHook(this);
    m_HookCPUExecOpcode = new CScriptHook(this);
    m_HookCPURead = new CScriptHook(this);
    m_HookCPUWrite = new CScriptHook(this);
    m_HookCPUGPRValue = new CScriptHook(this);
    m_HookFrameDrawn = new CScriptHook(this);

    RegisterHook("exec", m_HookCPUExec);
    RegisterHook("read", m_HookCPURead);
    RegisterHook("write", m_HookCPUWrite);
    RegisterHook("opcode", m_HookCPUExecOpcode);
    RegisterHook("gprvalue", m_HookCPUGPRValue);
    RegisterHook("draw", m_HookFrameDrawn);

    HMODULE hInst = GetModuleHandle(nullptr);
    HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(IDR_JSAPI_TEXT), L"TEXT");

    HGLOBAL hGlob = LoadResource(hInst, hRes);
    DWORD resSize = SizeofResource(hInst, hRes);
    m_APIScript = (char*)malloc(resSize + 1);

    void* resData = LockResource(hGlob);
    memcpy(m_APIScript, resData, resSize);
    m_APIScript[resSize] = '\0';
    FreeResource(hGlob);
}

CScriptSystem::~CScriptSystem()
{
    for (size_t i = 0; i < m_Hooks.size(); i++)
    {
        delete m_Hooks[i].cbList;
    }

    UnregisterHooks();
    free(m_APIScript);
}

const char* CScriptSystem::APIScript()
{
    return m_APIScript;
}

void CScriptSystem::RunScript(const char * path)
{
    CGuard guard(m_CS);
    CScriptInstance* scriptInstance = new CScriptInstance(m_Debugger);
    char* pathSaved = (char*)malloc(strlen(path)+1); // Freed via DeleteStoppedInstances
    strcpy(pathSaved, path);

    m_RunningInstances.push_back({ pathSaved, scriptInstance });
    scriptInstance->Start(pathSaved);
}

void CScriptSystem::StopScript(const char* path)
{
    CScriptInstance* scriptInstance = GetInstance(path);

    if (scriptInstance == nullptr)
    {
        return;
    }

    scriptInstance->ForceStop();
    DeleteStoppedInstances();
}

void CScriptSystem::DeleteStoppedInstances()
{
    CGuard guard(m_CS);

    int lastIndex = m_RunningInstances.size() - 1;
    for (int i = lastIndex; i >= 0; i--)
    {
        if (m_RunningInstances[i].scriptInstance->GetState() == STATE_STOPPED)
        {
            free(m_RunningInstances[i].path);
            CScriptInstance* instance = m_RunningInstances[i].scriptInstance;
            delete instance;
            m_RunningInstances.erase(m_RunningInstances.begin() + i);
        }
    }
}

INSTANCE_STATE CScriptSystem::GetInstanceState(const char* path)
{
    CGuard guard(m_CS);

    for (size_t i = 0; i < m_RunningInstances.size(); i++)
    {
        if (strcmp(m_RunningInstances[i].path, path) == 0)
        {
            INSTANCE_STATE ret = m_RunningInstances[i].scriptInstance->GetState();
            return ret;
        }
    }

    return STATE_INVALID;
}

CScriptInstance* CScriptSystem::GetInstance(const char* path)
{
    CGuard guard(m_CS);

    for (size_t i = 0; i < m_RunningInstances.size(); i++)
    {
        if (strcmp(m_RunningInstances[i].path, path) == 0)
        {
            CScriptInstance *ret = m_RunningInstances[i].scriptInstance;
            return ret;
        }
    }

    return nullptr;
}

bool CScriptSystem::HasCallbacksForInstance(CScriptInstance* scriptInstance)
{
    for (size_t i = 0; i < m_Hooks.size(); i++)
    {
        if (m_Hooks[i].cbList->HasContext(scriptInstance))
        {
            return true;
        }
    }
    return false;
}

void CScriptSystem::ClearCallbacksForInstance(CScriptInstance* scriptInstance)
{
    for (size_t i = 0; i < m_Hooks.size(); i++)
    {
        m_Hooks[i].cbList->RemoveByInstance(scriptInstance);
    }
}

void CScriptSystem::RemoveCallbackById(int callbackId)
{
    for (size_t i = 0; i < m_Hooks.size(); i++)
    {
        m_Hooks[i].cbList->RemoveById(callbackId);
    }
}

void CScriptSystem::RegisterHook(const char* hookId, CScriptHook* cbList)
{
    HOOKENTRY hook = { hookId, cbList };
    m_Hooks.push_back(hook);
}

void CScriptSystem::UnregisterHooks()
{
    m_Hooks.clear();
}

CScriptHook* CScriptSystem::GetHook(const char* hookId)
{
    size_t size = m_Hooks.size();
    for (size_t i = 0; i < size; i++)
    {
        if (strcmp(m_Hooks[i].hookId, hookId) == 0)
        {
            return m_Hooks[i].cbList;
        }
    }
    return nullptr;
}

int CScriptSystem::GetNextCallbackId()
{
    return m_NextCallbackId++;
}

void CScriptSystem::CallbackAdded()
{
    m_NumCallbacks++;
}

void CScriptSystem::CallbackRemoved()
{
    if (m_NumCallbacks > 0)
    {
        m_NumCallbacks--;
    }
}
