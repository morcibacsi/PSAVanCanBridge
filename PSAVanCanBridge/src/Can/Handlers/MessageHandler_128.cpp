#include "MessageHandler_128.h"
#include "../Structs/CAN_128.h"
#include "../Structs/CAN_128_2010.h"
#include "../../Config.h"

void MessageHandler_128::SetData()
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

void MessageHandler_128::SetData2004()
{

}

void MessageHandler_128::SetData2010()
{

}
