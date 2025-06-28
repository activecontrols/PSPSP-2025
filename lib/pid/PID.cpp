//
// Created by Nikhil Annamraju on 6/28/2025.
//

#include "PID.h"

PID::PID(double kp, double ki, double kd, double min, double max):
    Kp(kp), Ki(ki), Kd(kd), Min(min), Max(max), LastErr(0), IntSum(0)
{}

double PID::Calculate(double setPoint, double processVar, double dt) {
    double Err = setPoint - processVar;
    IntSum += Err * dt;

    double POut = Kp * Err;
    double IOut = Ki * IntSum;
    double DOut = Kd * (Err - LastErr) / dt;

    double Output = POut + IOut + DOut;

    LastErr = Err;
    return Output;
}

