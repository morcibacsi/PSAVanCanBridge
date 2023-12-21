#pragma once

#ifndef _MessageHandler_297_h
    #define _MessageHandler_297_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//The parameter's unit is given by another network information: the UNITE_CONSIGNE_LVV_RVV parameter in the GESTION_VITESSE frame

/*
*/
class MessageHandler_297 : public ICanMessageHandler
{
    public:
    MessageHandler_297(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x297, 2, 200-10) { };

    void SetData() override;
};
#endif
