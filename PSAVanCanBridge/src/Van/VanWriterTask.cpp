#include "VanWriterTask.h"
#include "VanMessageSenderTSS46x.h"

#ifdef BOARD_TYPE_ESP32
    #include "BoardConfig_ESP32.h"
#endif
#ifdef BOARD_TYPE_ESP32_v15
    #include "VanMessageSenderUlp.h"
    #include "BoardConfig_ESP32_v15.h"
#endif
#ifdef BOARD_TYPE_tamc_termod_s3
    #include "BoardConfig_ESP32_tamc_termod_s3.h"
#endif

VanWriterTask::VanWriterTask(Config *config, DataBroker *dataBroker)
{
    #if BOARD_ULP_VAN_TX_ENABLED
        VANInterface = new VanMessageSenderUlp(BOARD_VAN_DATA_RX_PIN, BOARD_VAN_DATA_TX_PIN);
        VANInterface->begin();
    #else
        spi = new SPIClass(BOARD_SPI_INSTANCE);
        spi->begin(BOARD_SCK_PIN, BOARD_MISO_PIN, BOARD_MOSI_PIN, BOARD_CS_PIN);

        VANInterface = new VanMessageSenderTSS46x(BOARD_CS_PIN, spi, VAN_COMFORT);
        VANInterface->begin();
    #endif

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

void VanWriterTask::QueryParkingRadarData(unsigned long currentTime)
{
    vanWriterContainer->QueryParkingRadarData();
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