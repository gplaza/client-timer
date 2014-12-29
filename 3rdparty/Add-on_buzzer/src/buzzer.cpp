
#include "buzzer.h"

Buzzer* Buzzer::m_Instance = 0;

void Buzzer::good()
{
    digitalWrite(1,HIGH);
    delay (150) ;
    digitalWrite(1,LOW);
}

void Buzzer::caution()
{
    digitalWrite(1,HIGH);
    delay(50);
    digitalWrite(1,LOW);
    delay(10);
    digitalWrite(1,HIGH);
    delay(50);
    digitalWrite(1,LOW);
    delay(10);
    digitalWrite(1,HIGH);
    delay(50);
    digitalWrite(1,LOW);
}

void Buzzer::bad(){

    digitalWrite(1,HIGH);
    delay(50);
    digitalWrite(1,LOW);
    delay(50);
    digitalWrite(1,HIGH);
    delay(50);
    digitalWrite(1,LOW);
}
