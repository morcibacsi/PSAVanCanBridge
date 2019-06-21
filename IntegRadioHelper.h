// IntegRadioHelper.h
#pragma once

#ifndef _IntegRadioHelper_h
    #define _IntegRadioHelper_h

// Struct to hold all the radio related data in one place
typedef struct {
    uint8_t vanRadioSource = 0;

    uint8_t vanRadioBand = 0;
    uint8_t vanRDS = 0;
    uint8_t vanTA = 0;
    uint8_t vanRadioPosition = 0;
    uint8_t vanRadioPoweredOn = 0;
    uint8_t vanRadioVolume = 0;
    uint8_t vanRadioVolumeUpdating = 0;

    uint8_t vanRadioTapeSide = 0;
    uint8_t vanRadioTapeForward = 0;
    uint8_t vanRadioTapeIsPlaying = 0;
    float vanRadioFrequency = 0;

    uint8_t Preset1[10];
    uint8_t Preset2[10];
    uint8_t Preset3[10];
    uint8_t Preset4[10];
    uint8_t Preset5[10];
    uint8_t Preset6[10];


} IntegRadioHelper;

#endif
