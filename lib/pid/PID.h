/*
 * PID.h
 *
 *  Created on: 2025-06-28 by Nikhil Annamraju
 *  Description: TBD
 */

#ifndef PID_CONTROLLER
#define PID_CONTROLLER

class PID {
    public:
        PID(double kp, double ki, double kd, double min, double max);
        double Calculate(double setPoint, double processVar, double dt);
        void Reset();
    private:
        double Kp, Ki, Kd, Min, Max;
        double LastErr, IntSum;
};

#endif