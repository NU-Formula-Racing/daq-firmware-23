/* InterruptHandler*/
#pragma once
#include <Arduino.h>
#include "ISensor.h"

class IInterruptHandler
{
public:
    virtual void HandleInterrupt() = 0;
};

// Attaches an interrupt sensors functionality to 
void AttachInterruptHandler(IInterruptHandler &handler, int pin, int mode);

// We need to maintain a dictionary of <pin, Vector<IInterruptHandler>> pairs
// Whenever an interrupt for a given pin occurs, then a static function that calls handle interrupt to all those added

// Either that or find a simpler solution -- seems like work for another day
// Especially cause we only have one flow rate sensor...