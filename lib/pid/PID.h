/*
 * PID.h
 *
 *  Created on: 2025-06-28 by Nikhil Annamraju
 *  Description: TBD
 */

#ifndef PID_CONTROLLER
#define PID_CONTROLLER

#include <chrono>

class PID {
    public:
        PID(double kp, double ki, double kd, double min, double max);
        double Calculate(double setPoint, double processVar);
        void Reset();
    private:
        double Kp, Ki, Kd, Min, Max;
        double LastErr, IntSum;
        std::chrono::duration<double, std::milli> LastTime;
};

#endif