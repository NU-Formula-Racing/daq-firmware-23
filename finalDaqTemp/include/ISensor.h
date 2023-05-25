/* ISensor.h */
#pragma once
class ISensor
{
public:
    virtual float Read() = 0;
    virtual void Print() = 0;
};