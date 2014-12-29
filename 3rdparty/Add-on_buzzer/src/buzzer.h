#ifndef BUZZER_H
#define BUZZER_H

#include <QMutex>
#include <wiringPi.h>

class Buzzer
{
public:
    static Buzzer* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
            {
                m_Instance = new Buzzer;
                wiringPiSetup();
                pinMode(1, OUTPUT);
            }

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

private:
    Buzzer() {}
    Buzzer(const Buzzer &);
    Buzzer& operator=(const Buzzer &);
    static Buzzer* m_Instance;

public:
    void good();
    void bad();
    void caution();
};


#endif // BUZZER_H
