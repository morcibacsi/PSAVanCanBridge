#include "VanWriterTask.h"

VanWriterTask::VanWriterTask(Config *config, DataBroker *dataBroker)
{
    const int SCK_PIN = 25;
    const int MISO_PIN = 5;
    const int MOSI_PIN = 33;
    const int VAN_PIN = 32;

    spi = new SPIClass();
    spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, VAN_PIN);

    VANInterface = new VanMessageSenderTSS46x(VAN_PIN, spi, VAN_COMFORT);
    VANInterface->begin();

    vanWriterContainer = new VanWriterContainer(VANInterface, config, dataBroker);
}

void VanWriterTask::SetRadioState(unsigned long currentTime, uint8_t powerOn, uint8_t active, uint8_t keyboardActive, uint8_t autoVolume, uint8_t loudness, uint8_t mute)
{
    vanWriterContainer->SetRadioState(powerOn, active, keyboardActive, autoVolume, loudness, mute);
    vanWriterContainer->Process(currentTime);
}

void VanWriterTask::SendRadioSetSource(unsigned long currentTime, uint8_t source)
{
    vanWriterContainer->SendRadioSetSource(source);
    vanWriterContainer->Process(currentTime);
}

void VanWriterTask::GetCarStatus(unsigned long currentTime)
{
    vanWriterContainer->GetCarStatus();
    vanWriterContainer->Process(currentTime);
}

void VanWriterTask::GetRadioSettings(unsigned long currentTime)
{
    vanWriterContainer->GetRadioSettings();
    vanWriterContainer->Process(currentTime);
}

void VanWriterTask::GetRadioStationData(unsigned long currentTime)
{
    vanWriterContainer->GetRadioStationData();
    vanWriterContainer->Process(currentTime);
}

void VanWriterTask::SetKeyboardState(unsigned long currentTime, uint8_t enabled)
{
    vanWriterContainer->SetKeyboardState(enabled);
    vanWriterContainer->Process(currentTime);
}

void VanWriterTask::SetAudioSettings(unsigned long currentTime, uint8_t exitOptions, uint8_t  balance, uint8_t  fader, uint8_t  bass, uint8_t treble)
{
    vanWriterContainer->SetAudioSettings(exitOptions, balance, fader, bass, treble);
    vanWriterContainer->Process(currentTime);
}

void VanWriterTask::Process(unsigned long currentTime)
{
    vanWriterContainer->Process(currentTime);
}