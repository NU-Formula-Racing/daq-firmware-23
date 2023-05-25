/* InterruptHandler.cpp*/

#include <Arduino.h>
#include "interruptHandler.h"


// Attaches an interrupt sensors functionality to 
void AttachInterruptHandler(IInterruptHandler &handler, int pin, int mode)
{
    attachInterrupt(pin, handler.HandleInterrupt, mode);
}