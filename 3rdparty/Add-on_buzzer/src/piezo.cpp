#include "piezo.h"

Piezo* Piezo::m_Instance = 0;

void Piezo::good()
{
    softToneWrite (1, 800) ;
    delay (150) ;
    softToneWrite (1, 0) ;
}

void Piezo::caution()
{
    softToneWrite(1,800);
    delay(50);
    softToneWrite(1,0);
    delay(50);
    softToneWrite(1,800);
    delay(50);
    softToneWrite(1,0);
    delay(50);
    softToneWrite(1,800);
    delay(50);
    softToneWrite(1,0);
}

void Piezo::bad(){

    softToneWrite(1,450);
    delay(100);
    softToneWrite(1,0);
    delay(50);
    softToneWrite(1,450);
    delay(100);
    softToneWrite(1,0);
}
