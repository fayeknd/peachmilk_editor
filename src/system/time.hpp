#pragma once
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <ctime>

class Time {

  /*   auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now(); */
    static inline double s_deltaTime = 0;
public:

    static double deltaTime() { return s_deltaTime; }
    static double deltaTimeNow() { return get().m_deltaTimer.count(); }
    static void resetDeltaTimer() { 
        s_deltaTime = get().m_deltaTimer.count();
        get().m_deltaTimer.reset();
    }
    static double sinceStartup();
    

    static Time& get() {
        static Time instance;
        return instance;
    }

    class Timer {
    private:  
        bool m_counting;
        bool m_hasStarted = false;

        std::chrono::system_clock::time_point m_start;
        std::chrono::system_clock::time_point m_end;

        double m_pauseDur = 0;

        std::chrono::system_clock::time_point timeNow() {
            std::chrono::system_clock::time_point t;
            t = (m_counting) ? std::chrono::system_clock::now() : m_end;
            return t;
        }

    public:

        double count();
        bool isCounting() { return m_counting; }

        void start();
        void stop();
        void reset();
    };

    Timer m_deltaTimer;

};
