#include "VanQueryRadioSettings.h"

void VanQueryRadioSettings::InternalProcess()
{
    if (_ignition)
    {
        radioSettingsSender->GetSettings(RADIO_SETTINGS_CHANNEL);
    }
}

void VanQueryRadioSettings::QueryNow()
{
    radioSettingsSender->GetSettings(RADIO_SETTINGS_CHANNEL);
}

void VanQueryRadioSettings::SetData(uint8_t ignition)
{
    _ignition = ignition;
}

void VanQueryRadioSettings::Stop()
{
    radioSettingsSender->Disable(RADIO_SETTINGS_CHANNEL);
}
