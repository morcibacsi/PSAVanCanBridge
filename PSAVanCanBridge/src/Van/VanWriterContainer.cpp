#include "VanWriterContainer.h"

VanWriterContainer::VanWriterContainer(IVanMessageSender *VANInterface, Config *config, DataBroker *dataBroker) {
    vanInterface = VANInterface;
    _config = config;
    _dataBroker = dataBroker;

    _queue = xQueueCreate(10, sizeof(VanCommand));

    tripComputerQuery = new VanQueryTripComputer(vanInterface);
    displayStatus = new VanDisplayStatus(vanInterface);

    acQuery = new VanQueryAirCon(vanInterface);

    parkingAidQuery = new VanQueryParkingAid(vanInterface);

    acceptRadioEvent = new VanAcceptRadioEvent(vanInterface);
    radioTuner = new VanQueryRadioTuner(vanInterface);
    radioSettings = new VanQueryRadioSettings(vanInterface);
    radioCommand = new VanRadioCommandPacketSender(vanInterface, 10);
}

void VanWriterContainer::Process(unsigned long currentTime)
{
    VanCommand command;

    if (xQueueReceive(_queue, &command, (TickType_t)5) == pdTRUE)
    {
        switch (command.Type)
        {
            case CarStatusQuery:
                // 564
                if (!_config->ORIGINAL_DISPLAY_PRESENT)
                {
                    displayStatus->Stop();
                    delay(5);
                    tripComputerQuery->SetData(_dataBroker->Ignition);
                    tripComputerQuery->Process(currentTime, true);
                }
            break;
            case CmdSetRadioState:
                // 8D4
                if (_config->BRIDGE_RADIO_DATA && !_config->ORIGINAL_DISPLAY_PRESENT)
                {
                    radioCommand->SetRadioState(command.PowerOn, command.Active, command.KeyboardState, command.AutoVol, command.Loudness, command.Mute);
                }
            break;
            case SetSource:
                // 8D4
                if (_config->BRIDGE_RADIO_DATA && !_config->ORIGINAL_DISPLAY_PRESENT)
                {
                    radioCommand->SendSource(command.Source);
                }
            break;
            case CmdSetKeyboardState:
                // 8D4
                if (_config->BRIDGE_RADIO_DATA && !_config->ORIGINAL_DISPLAY_PRESENT)
                {
                    radioCommand->SetKeyboardState(command.KeyboardState);
                }
            break;
            case CmdSetAudioSettings:
                // 8D4
                if (_config->BRIDGE_RADIO_DATA && !_config->ORIGINAL_DISPLAY_PRESENT)
                {
                    radioCommand->SetAudioSettings(command.ExitOptions, command.Balance, command.Fader, command.Bass, command.Treble);
                }
            break;
            case StationDataQuery:
                // 554
                if (_config->BRIDGE_RADIO_DATA && !_config->ORIGINAL_DISPLAY_PRESENT)
                {
                    radioSettings->Stop();
                    delay(5);
                    radioTuner->SetData(_dataBroker->Ignition);
                    radioTuner->Process(currentTime, true);
                }
            break;
            case RadioSettings:
                // 4D4
                GetRadioSettings(currentTime);
            break;
            case ParkingRadarDataQuery:
                // AE8
                parkingAidQuery->QueryParkingRadarData();
            break;
            case AirConActuatorStatusQuery:
                // ADC
                acQuery->QueryAirConData();
            break;
            default:
            break;
        }
    }
    else
    {
        // uncomment to test V2C without BSI
        //uint8_t packet[7] = { 0x0F, 0x07, 0x81, 0x1D, 0xA4 ,0x93, 7 * 2 + 0x50 };
        //vanInterface->set_channel_for_transmit_message(0, 0x8a4, packet, sizeof(packet), 0);

        parkingAidQuery->SetData(_dataBroker->Ignition, _dataBroker->IsReverseEngaged);

        bool runParkingAidQuery = _config->PARKING_AID_TYPE == 1 && _dataBroker->IsReverseEngaged;

        if (runParkingAidQuery)
        {
            parkingAidQuery->Process(currentTime, false);
        }
        else
        {
            SendDisplayStatus(currentTime);

            GetRadioSettings(currentTime);

            if (_config->QUERY_AC_STATUS)
            {
                acQuery->SetData(_dataBroker->Ignition);
                acQuery->Process(currentTime, false);
            }
        }
    }
}

void VanWriterContainer::SendDisplayStatus(unsigned long currentTime)
{
    if (!_config->ORIGINAL_DISPLAY_PRESENT)
    {
        tripComputerQuery->Stop();
        delay(5);
        displayStatus->SetData(_dataBroker->Ignition, _dataBroker->IsTripPressed, _dataBroker->PageDisplayedOnTypeC, currentTime);
        displayStatus->Process(currentTime, false);
    }
}

void VanWriterContainer::GetRadioSettings(unsigned long currentTime)
{
    // Get radio settings (4D4)
    if (_config->BRIDGE_RADIO_DATA && !_config->ORIGINAL_DISPLAY_PRESENT)
    {
        radioTuner->Stop();
        delay(5);
        radioSettings->SetData(_dataBroker->Ignition);
        radioSettings->Process(currentTime, false);
    }
}

void VanWriterContainer::SetRadioState(uint8_t powerOn, uint8_t active, uint8_t keyboardActive, uint8_t autoVolume, uint8_t loudness, uint8_t mute)
{
    VanCommand command;
    command.Type = CmdSetRadioState;
    command.AutoVol = autoVolume;
    command.Loudness = loudness;
    command.Mute = mute;
    command.PowerOn = powerOn;
    command.Active = active;
    command.KeyboardState = keyboardActive;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::SendRadioSetSource(uint8_t source)
{
    VanCommand command;
    command.Type = SetSource;
    command.Source = source;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::QueryParkingRadarData()
{
    VanCommand command;
    command.Type = ParkingRadarDataQuery;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::QueryAirConActuatorData()
{
    VanCommand command;
    command.Type = AirConActuatorStatusQuery;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::GetCarStatus()
{
    VanCommand command;
    command.Type = CarStatusQuery;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::GetRadioSettings()
{
    VanCommand command;
    command.Type = RadioSettings;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::GetRadioStationData()
{
    VanCommand command;
    command.Type = StationDataQuery;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::SetKeyboardState(uint8_t enabled)
{
    VanCommand command;
    command.Type = CmdSetKeyboardState;
    command.KeyboardState = enabled;
    xQueueSendToFront(_queue, &command, 0);
}

void VanWriterContainer::SetAudioSettings(uint8_t exitOptions, uint8_t balance, uint8_t  fader, uint8_t bass, uint8_t treble)
{
    VanCommand command;
    command.Type = CmdSetAudioSettings;
    command.ExitOptions = exitOptions;
    command.Balance = balance;
    command.Fader = fader;
    command.Bass = bass;
    command.Treble = treble;
    xQueueSendToFront(_queue, &command, 0);
}