#include <Project64-core/Settings.h>
#include <Project64-core/Settings/SettingType/SettingsType-Application.h>
#include <Project64-core/Settings/SettingType/SettingsType-ApplicationIndex.h>
#include <Project64-core/Settings/SettingType/SettingsType-RelativePath.h>
#include <Project64-core/Settings/SettingType/SettingsType-GameSetting.h>
#include "UISettings.h"

void RegisterUISettings(void)
{
    g_Settings->AddHandler((SettingID)(FirstUISettings + Asserts_Version), new CSettingTypeApplication("Settings", "Asserts Version", (uint32_t)0));
    g_Settings->AddHandler((SettingID)(FirstUISettings + Screen_Orientation), new CSettingTypeApplication("Settings", "Screen Orientation", (uint32_t)0));
    g_Settings->AddHandler((SettingID)(FirstUISettings + File_RecentGameFileCount), new CSettingTypeApplication("Settings", "Remembered ROM Files", (uint32_t)10));
    g_Settings->AddHandler((SettingID)(FirstUISettings + File_RecentGameFileIndex), new CSettingTypeApplicationIndex("Recent File", "Recent ROM", Default_None));
    g_Settings->AddHandler((SettingID)(FirstUISettings + TouchScreen_ButtonScale), new CSettingTypeApplication("Touch Screen", "Button Scale", (uint32_t)100));
    g_Settings->AddHandler((SettingID)(FirstUISettings + TouchScreen_Layout), new CSettingTypeApplication("Touch Screen", "Layout", "Analog"));
    g_Settings->AddHandler((SettingID)(FirstUISettings + Controller_ConfigFile), new CSettingTypeRelativePath("Config", "Controller.cfg"));
    g_Settings->AddHandler((SettingID)(FirstUISettings + Controller_CurrentProfile), new CSettingTypeApplication("Controller", "Profile", "User"));
    g_Settings->AddHandler((SettingID)(FirstUISettings + Controller_Deadzone), new CSettingTypeApplication("Controller", "Deadzone", (uint32_t)0));
    g_Settings->AddHandler((SettingID)(FirstUISettings + Controller_Sensitivity), new CSettingTypeApplication("Controller", "Sensitivity", (uint32_t)100));
    g_Settings->AddHandler((SettingID)(FirstUISettings + SupportWindow_FirstRun), new CSettingTypeApplication("Support Project64", "First Run", ""));
    g_Settings->AddHandler((SettingID)(FirstUISettings + SupportWindow_AlwaysShow), new CSettingTypeApplication("Support Project64", "Always Show", false));
    g_Settings->AddHandler((SettingID)(FirstUISettings + SupportWindow_ShowingSupportWindow), new CSettingTypeApplication("Support Project64", "Showing Support Window", false));
    g_Settings->AddHandler((SettingID)(FirstUISettings + SupportWindow_RunCount), new CSettingTypeApplication("Support Project64", "Run Count", (uint32_t)0));
    g_Settings->AddHandler((SettingID)(FirstUISettings + Game_RunCount), new CSettingTypeGame("Run Count", (uint32_t)0));
}

void UISettingsSaveBool(UISettingID Type, bool Value)
{
    g_Settings->SaveBool((SettingID)(FirstUISettings + Type), Value);
    CSettings::FlushSettings(g_Settings);
}

void UISettingsSaveDword(UISettingID Type, uint32_t Value)
{
    g_Settings->SaveDword((SettingID)(FirstUISettings + Type), Value);
    CSettings::FlushSettings(g_Settings);
}

void UISettingsSaveString(UISettingID Type, const std::string & Value)
{
    g_Settings->SaveString((SettingID)(FirstUISettings + Type), Value);
    CSettings::FlushSettings(g_Settings);
}

bool UISettingsLoadBool(UISettingID Type)
{
    return g_Settings->LoadBool((SettingID)(FirstUISettings + Type));
}

uint32_t UISettingsLoadDword(UISettingID Type)
{
    return g_Settings->LoadDword((SettingID)(FirstUISettings + Type));
}

std::string UISettingsLoadStringVal(UISettingID Type)
{
    return g_Settings->LoadStringVal((SettingID)(FirstUISettings + Type));
}
