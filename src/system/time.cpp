#include "time.hpp"
#include "../headers.h"

double Time::sinceStartup() {
    return glfwGetTime();
}

double Time::Timer::count() {
    if (!m_counting && !m_hasStarted) return 0;
    std::chrono::duration<double> elapsed = std::chrono::system_clock::now()- m_start;
    return elapsed.count() - m_pauseDur;
}


void Time::Timer::start() {
    if (m_counting) 
        return; 
    if (!m_hasStarted)
        m_start = std::chrono::system_clock::now();
    else {
        // if m_hasStarted is true, the timer has been paused.
        // This line just calculates how long it was paused for
        // (this is subtracted from count())
        m_pauseDur += (timeNow() - m_end).count();
    }
    m_hasStarted = true;
    m_counting = true;
}

void Time::Timer::stop() {
    m_end = std::chrono::system_clock::now();
    m_counting = true;
}
void Time::Timer::reset() {
    m_hasStarted = false;
    m_pauseDur = 0; 
    if (m_counting) {
        m_counting = false;
        start();
    }
}