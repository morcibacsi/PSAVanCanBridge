#include "MessageHandler_168.h"
#include "../../../Config.h"
#include "../Structs/CAN_168.h"
#include "../Structs/CAN_168_2010.h"

void MessageHandler_168::SetData()
{
    {
        if (_config->CAN_TYPE == 0)
        {
            SetData2004();
        }
        else
        {
            SetData2010();
        }
    }
}

void MessageHandler_168::SetData2004()
{

}

void MessageHandler_168::SetData2010()
{

}
