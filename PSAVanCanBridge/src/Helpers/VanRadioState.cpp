#include "VanRadioState.h"
#include "IntUnions.h"

// Radio
#include "../Can/Structs/CAN_165.h"

#include "../Can/Handlers/MessageHandler_165.h"
#include "../Can/Handlers/MessageHandler_1A5.h"
#include "../Can/Handlers/MessageHandler_1E5.h"

// Tuner
#include "../Can/Handlers/MessageHandler_225.h"
#include "../Can/Handlers/MessageHandler_265.h"
#include "../Can/Handlers/MessageHandler_2A5.h"

// CD
#include "../Can/Structs/CAN_325.h"
#include "../Can/Handlers/MessageHandler_325.h"
#include "../Can/Handlers/MessageHandler_365.h"
#include "../Can/Handlers/MessageHandler_3A5.h"

#include "Arduino.h"
#include "../Helpers/DebugPrint.h"

VanRadioState::VanRadioState(Config* config, VanWriterTask* vanWriterTask, VanSoundOptionState* optionState, CanMessageHandlerContainer* canMessageHandlerContainer)
{
    _config = config;
    _vanWriterTask = vanWriterTask;
    _optionState = optionState;
    _canMessageHandlerContainer = canMessageHandlerContainer;
}

void VanRadioState::SetPowerOn(uint8_t powerOn)
{
    PrevPowerOn = PowerOn;
    PowerOn = powerOn;
}

void VanRadioState::SetTapeAvailable(uint8_t tapeAvailable)
{
    PrevTapeAvailable = TapeAvailable;
    TapeAvailable = tapeAvailable;
}

void VanRadioState::SetSource(uint8_t source)
{
    PrevSource = CurrentSource;
    CurrentSource = source;
}

uint8_t VanRadioState::GetIncrementedVal(uint8_t currentValue)
{
    currentValue++;

    if (currentValue < 0x36)
    {
        currentValue = 0x36;
    }
    if (currentValue > 0x48)
    {
        currentValue = 0x48;
    }

    return currentValue;
}

uint8_t VanRadioState::GetDecrementedVal(uint8_t currentValue)
{
    currentValue--;

    if (currentValue > 0x48)
    {
        currentValue = 0x48;
    }
    if (currentValue < 0x36)
    {
        currentValue = 0x36;
    }

    return currentValue;
}

void VanRadioState::IncrementAudioSettingValue()
{
    switch (_optionState->GetState())
    {
        case OptionStateBalance:
            Balance = GetIncrementedVal(Balance);
        break;
        case OptionStateFader:
            Fader = GetIncrementedVal(Fader);
        break;
        case OptionStateBass:
            Bass = GetIncrementedVal(Bass);
        break;
        case OptionStateTreble:
            Treble = GetIncrementedVal(Treble);
        break;

        default:
        break;
    }
}

void VanRadioState::DecrementAudioSettingValue()
{
    switch (_optionState->GetState())
    {
        case OptionStateBalance:
            Balance = GetDecrementedVal(Balance);
        break;
        case OptionStateFader:
            Fader = GetDecrementedVal(Fader);
        break;
        case OptionStateBass:
            Bass = GetDecrementedVal(Bass);
        break;
        case OptionStateTreble:
            Treble = GetDecrementedVal(Treble);
        break;

        default:
        break;
    }
}

void VanRadioState::Process(unsigned long currentTime)
{
    // Hide the options menu after 4 second timeout
    if (_closeOptionSent == 0 && (currentTime - _prevAudioSettingButtonHandledTime > 4000))
    {
        _closeOptionSent = 1;
        _turnOffKeyboard = 1;
        _optionState->Reset();
        _vanWriterTask->SetAudioSettings(currentTime, 1, Balance, Fader, Bass, Treble);
    }
}

void VanRadioState::RadioEvent(unsigned long currentTime, VanEventRadioStructs &radioEvent)
{
    #ifdef PRINT_DEBUG
    char strBuf[50];
    sprintf(strBuf, "radioEvent.Button.data.radio_button: %02X", radioEvent.Button.data.radio_button);debug_println(strBuf);
    sprintf(strBuf, "radioEvent.Button.data.push_type: %d", radioEvent.Button.data.push_type);debug_println(strBuf);
    sprintf(strBuf, "radioEvent.Event.data.buffer_part: %d", radioEvent.Event.data.buffer_part);debug_println(strBuf);
    sprintf(strBuf, "radioEvent.Event.data.event_cd: %d", radioEvent.Event.data.event_cd);debug_println(strBuf);
    sprintf(strBuf, "radioEvent.Event.data.event_general: %d", radioEvent.Event.data.event_general);debug_println(strBuf);
    sprintf(strBuf, "radioEvent.Event.data.event_keyboard: %d", radioEvent.Event.data.event_keyboard);debug_println(strBuf);
    sprintf(strBuf, "radioEvent.Event.data.event_radio: %d", radioEvent.Event.data.event_radio);debug_println(strBuf);
    sprintf(strBuf, "radioEvent.Event.data.event_tape: %d", radioEvent.Event.data.event_tape);debug_println(strBuf);
    #endif

    BufferPart = radioEvent.Event.data.buffer_part;

    // 8C 4C 8A 21 40 3D 54
    if (radioEvent.Event.data.event_general == 1 && radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_NONE)
    {
        //_vanRadioState->BufferPart = static_cast<RadioEvent>(radioEvent.Event.data.buffer_part);
        _vanWriterTask->GetRadioSettings(currentTime);
    }
///*
    if (_turnOffKeyboard == 1)
    {
        _turnOffKeyboard = 0;
        // 8D 4C 11 C2 F4 04
        _vanWriterTask->SetKeyboardState(currentTime, 0);
    }
//*/
    // 8C 4C 8A 22 16 97 40
    if (radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_AUDIO && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_RELEASE)
    {
        _prevAudioSettingButtonHandledTime = currentTime;

        if (_optionState->GetState() == OptionStateNone) //when we first push the button we activate the "keyboard" on the radio
        {
            //debug_println("Activate keyboard");
            _closeOptionSent = 0;
            // 8D 4C 11 C2 F4 04
            _vanWriterTask->SetKeyboardState(currentTime, 1);
            _prevAudioSettingButtonHandledTime = currentTime;
        }

        _optionState->Next();
        if (_optionState->GetState() == OptionStateNone) //because the _optionState->Next() on the previous line this occurs when we reach the end of the audio setup
        {
            //debug_println("Finalize settings");
            _closeOptionSent = 1;
            _turnOffKeyboard = 1;
            // 8D 4C 14 BF 3F 3F 3F 48 F0
            _vanWriterTask->SetAudioSettings(currentTime, 1, Balance, Fader, Bass, Treble);
        }
    }
/*
    // 8C 4C 8A 22 56 04 66
    if (radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_AUDIO && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_RELEASE)
    {
        _prevAudioSettingButtonHandledTime = currentTime;
        //if (_optionState->IsVisible() == 1)
        {
            _optionState->Next();
            if (_optionState->GetState() == OptionStateNone)
            {
                _closeOptionSent = 1;
                _turnOffKeyboard = 1;
                // 8D 4C 14 BF 3F 3F 3F 48 F0
                _vanWriterTask->SetAudioSettings(currentTime, 1, Balance, Fader, Bass, Treble);
            }
        }
    }
*/
    if (radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_UP_AUDIO_PLUS && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_PUSH)
    {
        _prevAudioSettingButtonHandledTime = currentTime;

        switch (_optionState->GetState())
        {
            case OptionStateAutoVol:
            {
                uint8_t autoVolToSet = AutoVolumeOn == 1 ? 0 : 1;
                _vanWriterTask->SetRadioState(currentTime, 1, 1, 1, autoVolToSet, LoudnessOn, Muted);
                break;
            }

            case OptionStateLoudness:
            {
                uint8_t loudnessToSet = LoudnessOn == 1 ? 0 : 1;
                _vanWriterTask->SetRadioState(currentTime, 1, 1, 1, AutoVolumeOn, loudnessToSet, Muted);
                break;
            }

            default:
                IncrementAudioSettingValue();
                _vanWriterTask->SetAudioSettings(currentTime, 0, Balance, Fader, Bass, Treble);
            break;
        }
    }

    if (radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_DOWN_AUDIO_MINUS && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_PUSH)
    {
        _prevAudioSettingButtonHandledTime = currentTime;

        switch (_optionState->GetState())
        {
            case OptionStateAutoVol:
            {
                uint8_t autoVolToSet = AutoVolumeOn == 1 ? 0 : 1;
                _vanWriterTask->SetRadioState(currentTime, 1, 1, 1, autoVolToSet, LoudnessOn, Muted);
                break;
            }

            case OptionStateLoudness:
            {
                uint8_t loudnessToSet = LoudnessOn == 1 ? 0 : 1;
                _vanWriterTask->SetRadioState(currentTime, 1, 1, 1, AutoVolumeOn, loudnessToSet, Muted);
                break;
            }

            default:
                DecrementAudioSettingValue();
                _vanWriterTask->SetAudioSettings(currentTime, 0, Balance, Fader, Bass, Treble);
            break;
        }
    }

    if ((radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_CD_MD && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_RELEASE))
    {
        _vanWriterTask->SendRadioSetSource(currentTime, (int)RadioSourceCd);
    }

    if ((radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_SOURCE && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_RELEASE))
    {
        uint8_t source = VAN_RADIO_SRC_TUNER;
        if (CurrentSource == VAN_RADIO_SRC_TUNER)
        {
            source = VAN_RADIO_SRC_K7_MD_CD;
        }
        _vanWriterTask->SendRadioSetSource(currentTime, source);
    }

    if ((radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_TUNER && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_RELEASE)
        //||(radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_NONE  && radioEvent.Event.data.event_radio == 1 && radioEvent.Event.data.buffer_part == VAN_EVENT_RADIO_BUF_TUNER)
    )
    {
        _vanWriterTask->SendRadioSetSource(currentTime, (int)RadioSourceTuner);
    }

    if (radioEvent.Event.data.event_cd == 1 ||
        radioEvent.Event.data.event_radio == 1 ||
        radioEvent.Event.data.event_tape == 1
        //|| (radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_NONE && radioEvent.Event.data.buffer_part == VAN_EVENT_RADIO_BUF_CD_STATE)
        )
    {
        _vanWriterTask->GetRadioStationData(currentTime);
    }

    if (radioEvent.Button.data.radio_button == VAN_EVENT_RADIO_BUTTON_ON_OFF && radioEvent.Button.data.push_type == VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_PUSH)
    {
        _vanWriterTask->SetRadioState(currentTime, 0, 0, 0, AutoVolumeOn, LoudnessOn, Muted);
    }

    ShowAudioSettings(currentTime);
}

void VanRadioState::RadioSettings(unsigned long currentTime, VanRadioInfoStruct &settings)
{
    Active = settings.General.data.power_on;
    AutoVolumeOn = settings.Status.data.auto_volume_active;
    LoudnessOn = settings.Status.data.loudness_on;
    Muted = settings.Status.data.muted;
    CurrentSource = settings.Source.data.source;

    #ifdef PRINT_DEBUG
    char strBuf[50];
    sprintf(strBuf, "vanPacket.General.data.cd_request: %d", settings.General.data.cd_request);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.General.data.tape_request: %d", settings.General.data.tape_request);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.General.data.activation_request: %d", settings.General.data.activation_request);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.General.data.power_on: %d", settings.General.data.power_on);debug_println(strBuf);
///*
    sprintf(strBuf, "vanPacket.General.data.stop_request: %d", settings.General.data.stop_request);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.General.data.phone_request: %d", settings.General.data.phone_request);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.General.data.vin_ok: %d", settings.General.data.vin_ok);debug_println(strBuf);
//*/
    sprintf(strBuf, "vanPacket.Source.data.source: %d", settings.Source.data.source);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.Source.data.cd_available: %d", settings.Source.data.cd_available);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.Source.data.tape_available: %d", settings.Source.data.tape_available);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.Source.data.tuner_available: %d", settings.Source.data.tuner_available);debug_println(strBuf);

    sprintf(strBuf, "vanPacket.Status.data.properties_open: %d", settings.Status.data.properties_open);debug_println(strBuf);
    sprintf(strBuf, "settings.Volume.data.updating: %d", settings.Volume.data.updating);debug_println(strBuf);
/*
    sprintf(strBuf, "vanPacket.Status.data.loudness_on: %d", settings.Status.data.loudness_on);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.Status.data.auto_volume_active: %d", settings.Status.data.auto_volume_active);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.Status.data.muted: %d", settings.Status.data.muted);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.Status.data.muted_request: %d", settings.Status.data.muted_request);debug_println(strBuf);
    sprintf(strBuf, "vanPacket.Status.data.speaker_count: %d", settings.Status.data.speaker_count);debug_println(strBuf);
*/
    #endif
    SetPowerOn(settings.General.data.power_on);
    SetTapeAvailable(settings.Source.data.tape_available);
    Balance = settings.Balance.data.value;
    Fader = settings.Fader.data.value;
    Bass = settings.Bass.data.value;
    Treble = settings.Treble.data.value;
    OptionsVisible = settings.Status.data.properties_open;
    _optionState->SetVisible(settings.Status.data.properties_open);

/*
    if (vanPacket.General.data.cd_request == 1)
    {
        _vanRadioState->BufferPart = EventCD;
    }
    if (vanPacket.General.data.tape_request == 1)
    {
        _vanRadioState->BufferPart = EventTape;
    }
    if (vanPacket.General.data.activation_request == 1)
    {
        _vanRadioState->BufferPart = EventRadio;
    }
*/

    if (settings.General.data.activation_request == 1 || settings.General.data.cd_request == 1 || settings.General.data.tape_request == 1)
    {
        _vanWriterTask->SetRadioState(currentTime, 1, 1, 0, settings.Status.data.auto_volume_active, settings.Status.data.loudness_on, settings.Status.data.muted);
    }

    if (PrevPowerOn == 0 && PowerOn == 1)
    {
        uint8_t source = VAN_RADIO_SRC_TUNER;

        switch (BufferPart)
        {
            case EventRadio:
                source = VAN_RADIO_SRC_TUNER;
                break;
            case EventCD:
                source = VAN_RADIO_SRC_K7_MD_CD;
                break;
            case EventTape:
                //TODO need to check the actual value with a tape unit
                source = VAN_RADIO_SRC_K7_MD_CD;
                break;
            default:
                source = VAN_RADIO_SRC_TUNER;
                break;
        }

        _vanWriterTask->SendRadioSetSource(currentTime, source);
    }

    // when cd is ejected we set the radio source to tuner
    if (PrevPowerOn == 1 && PowerOn == 1)
    {
        if (settings.Source.data.cd_available == 0 && settings.Source.data.tape_available == 0 && settings.Source.data.source == VAN_RADIO_SRC_K7_MD_CD)
        {
            _vanWriterTask->SendRadioSetSource(currentTime, VAN_RADIO_SRC_TUNER);
        }

        if (PrevTapeAvailable == 0 && TapeAvailable == 1)
        {
            _vanWriterTask->SendRadioSetSource(currentTime, VAN_RADIO_SRC_K7_MD_CD);
        }
    }

    if (settings.General.data.stop_request == 1 && settings.General.data.power_on == 1)
    {
        _vanWriterTask->SetRadioState(currentTime, 0, 0, 0, settings.Status.data.auto_volume_active, settings.Status.data.loudness_on, settings.Status.data.muted);
    }
    //_vanRadioState->BufferPart = EventNone;

    static_cast<MessageHandler_165*>(_canMessageHandlerContainer->GetHandler(0x165))->SetData(settings.General.data.power_on, settings.Status.data.muted);
    static_cast<MessageHandler_1A5*>(_canMessageHandlerContainer->GetHandler(0x1A5))->SetData(settings.Volume.data.updating, settings.Volume.data.value);
    static_cast<MessageHandler_1A5*>(_canMessageHandlerContainer->GetHandler(0x1A5))->SendMessage(currentTime, true);

    ShowAudioSettings(currentTime);
}

void VanRadioState::Tuner(unsigned long currentTime, VanRadioTunerStruct &vanPacket)
{
    UInt16 freq;
    freq.data.rightByte = vanPacket.Frequency1MSB.data.freq_msb;
    freq.data.leftByte = vanPacket.Frequency2LSB;

    static_cast<MessageHandler_165*>(_canMessageHandlerContainer->GetHandler(0x165))->SetSource(CAN_RADIO_SRC_TUNER);

    static_cast<MessageHandler_225*>(_canMessageHandlerContainer->GetHandler(0x225))->SetData(vanPacket.SearchOptions.data.frequency_scan_direction, vanPacket.SearchOptions.data.tuner_sensitivity, vanPacket.BandAndPosition.data.memory_postition, vanPacket.BandAndPosition.data.band, freq.data.rightByte, freq.data.leftByte);
    static_cast<MessageHandler_225*>(_canMessageHandlerContainer->GetHandler(0x225))->SendMessage(currentTime, true);

    static_cast<MessageHandler_265*>(_canMessageHandlerContainer->GetHandler(0x265))->SetData(vanPacket.RdsAndTa.data.rds_selected, vanPacket.RdsAndTa.data.rds_available, vanPacket.RdsAndTa.data.ta_selected, vanPacket.RdsAndTa.data.ta_available, vanPacket.RdsAndTa.data.reg_selected, vanPacket.PTYInfo.data.pty_inactive);
    static_cast<MessageHandler_265*>(_canMessageHandlerContainer->GetHandler(0x265))->SendMessage(currentTime, true);

    static_cast<MessageHandler_2A5*>(_canMessageHandlerContainer->GetHandler(0x2A5))->SetData(vanPacket.RDSTextLetter1, vanPacket.RDSTextLetter2, vanPacket.RDSTextLetter3, vanPacket.RDSTextLetter4, vanPacket.RDSTextLetter5, vanPacket.RDSTextLetter6, vanPacket.RDSTextLetter7, vanPacket.RDSTextLetter8);
    static_cast<MessageHandler_2A5*>(_canMessageHandlerContainer->GetHandler(0x2A5))->SendMessage(currentTime, true);
}

uint8_t bcd_to_decimal(uint8_t x) {
    return x - 6 * (x >> 4);
}

void VanRadioState::CD(unsigned long currentTime, VanRadioCDStruct &cdVanPacket)
{
    CAN_325_Byte1Struct cd1;
    cd1.data.repeat_mode = cdVanPacket.PlayMode.data.repeat_mode;
    cd1.data.play_mode = cdVanPacket.PlayMode.data.play_mode;

    CAN_325_Byte2Struct cd2;
    cd2.data.access_cd = cdVanPacket.CDStatus1.data.access_cd;
    cd2.data.cd_playback_status = cdVanPacket.CDStatus1.data.playback_status;

    uint8_t trackCount = bcd_to_decimal(cdVanPacket.TrackCount);
    uint8_t totalDiskMinutes = bcd_to_decimal(cdVanPacket.TotalDiskMinutes);
    uint8_t totalDiskSeconds = bcd_to_decimal(cdVanPacket.TotalDiskSeconds);

    uint8_t currentTrack = bcd_to_decimal(cdVanPacket.CurrentTrack);
    uint8_t totalTrackMinutes = bcd_to_decimal(cdVanPacket.TotalTrackMinutes);
    uint8_t totalTrackSeconds = bcd_to_decimal(cdVanPacket.TotalTrackSeconds);

    uint8_t minutes = bcd_to_decimal(cdVanPacket.CurrentMinutes);
    uint8_t seconds = bcd_to_decimal(cdVanPacket.CurrentSeconds);

    static_cast<MessageHandler_165*>(_canMessageHandlerContainer->GetHandler(0x165))->SetSource(CAN_RADIO_SRC_CD);
    static_cast<MessageHandler_325*>(_canMessageHandlerContainer->GetHandler(0x325))->SetData(cd1.asByte, cd2.asByte, cdVanPacket.CDStatus2.asByte);
    static_cast<MessageHandler_365*>(_canMessageHandlerContainer->GetHandler(0x365))->SetData(trackCount, totalDiskMinutes, totalDiskSeconds);
    static_cast<MessageHandler_3A5*>(_canMessageHandlerContainer->GetHandler(0x3A5))->SetData(currentTrack, totalTrackMinutes, totalTrackSeconds, minutes, seconds);
}

void VanRadioState::Tape(unsigned long currentTime, VanRadioTapeStruct &vanPacket)
{
    UInt16 freq;
    freq.data.rightByte = 0xFF;
    freq.data.leftByte = 0xFF;

    static_cast<MessageHandler_165*>(_canMessageHandlerContainer->GetHandler(0x165))->SetSource(CAN_RADIO_SRC_TUNER);

    static_cast<MessageHandler_225*>(_canMessageHandlerContainer->GetHandler(0x225))->SetData(0, 0, 0, VAN_RADIO_TUNER_BAND_AM, 0x0, 0x0);
    static_cast<MessageHandler_225*>(_canMessageHandlerContainer->GetHandler(0x225))->SendMessage(currentTime, true);

    static_cast<MessageHandler_265*>(_canMessageHandlerContainer->GetHandler(0x265))->SetData(0, 0, 0, 0, 0, 0);
    static_cast<MessageHandler_265*>(_canMessageHandlerContainer->GetHandler(0x265))->SendMessage(currentTime, true);

    if (vanPacket.Status.data.casette_side == VAN_RADIO_TAPE_DIRECTION_SIDE1_A)
    {
        static_cast<MessageHandler_2A5*>(_canMessageHandlerContainer->GetHandler(0x2A5))->SetData('T', 'A', 'P', 'E', ' ', 'S', '1', ' ');
    }
    if (vanPacket.Status.data.casette_side == VAN_RADIO_TAPE_DIRECTION_SIDE2_B)
    {
        static_cast<MessageHandler_2A5*>(_canMessageHandlerContainer->GetHandler(0x2A5))->SetData('T', 'A', 'P', 'E', ' ', 'S', '2', ' ');
    }
    if (vanPacket.Status.data.playback_status == VAN_RADIO_TAPE_PLAYBACK_STATUS_FAST_FORWARD)
    {
        static_cast<MessageHandler_2A5*>(_canMessageHandlerContainer->GetHandler(0x2A5))->SetData('T', 'A', 'P', 'E', ' ', '>', '>', ' ');
    }

    static_cast<MessageHandler_2A5*>(_canMessageHandlerContainer->GetHandler(0x2A5))->SendMessage(currentTime, true);
}

void VanRadioState::StationList(unsigned long currentTime, VanRadioTapeStruct &vanPacket)
{
    //0x125
    //0x0A4 is used for the album title and stuff like that
}

void VanRadioState::ShowAudioSettings(unsigned long currentTime)
{
    OptionState state = _optionState->GetState();
    debug_print("OptionState: ");debug_println(state);

    //innentől lefelé ezt a részt kell valahogy átrakni a gomb kezelő eventbe, és akkor jó lesz elvileg

    VanRadioInfoAudioSettingsByteStruct bass;
    bass.data.value = Bass;
    bass.data.updating = (state == OptionStateBass)    ? 1 : 0;

    VanRadioInfoAudioSettingsByteStruct treble;
    treble.data.value = Treble;
    treble.data.updating = (state == OptionStateTreble)    ? 1 : 0;

    VanRadioInfoAudioSettingsByteStruct fader;
    fader.data.value = Fader;
    fader.data.updating = (state == OptionStateFader)    ? 1 : 0;

    VanRadioInfoAudioSettingsByteStruct balance;
    balance.data.value = Balance;
    balance.data.updating = (state == OptionStateBalance)    ? 1 : 0;

    uint8_t autoVol  = (state == OptionStateAutoVol)  ? 1 : 0;
    uint8_t loudness = (state == OptionStateLoudness) ? 1 : 0;

    static_cast<MessageHandler_1E5*>(_canMessageHandlerContainer->GetHandler(0x1E5))->SetData(balance.asByte, fader.asByte, bass.asByte, treble.asByte, loudness, LoudnessOn, autoVol, AutoVolumeOn);
    static_cast<MessageHandler_1E5*>(_canMessageHandlerContainer->GetHandler(0x1E5))->SendMessage(currentTime, true);
}
