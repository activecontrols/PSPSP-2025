//
// Created by Nikhil Annamraju on 6/28/2025.
//

#include <algorithm>
#include "PID.h"

PID::PID(double kp, double ki, double kd, double min, double max):
    Kp(kp), Ki(ki), Kd(kd), Min(min), Max(max), LastErr(0), IntSum(0), LastTime(std::chrono::milliseconds(-1))
{}

double PID::Calculate(double setPoint, double processVar) {
    auto now = std::chrono::high_resolution_clock::now();
    auto epoch = std::chrono::duration<double, std::milli>(now.time_since_epoch());

    if (LastTime.count() == -1) {
        LastTime = epoch;
    }

    double dt = (epoch.count() - LastTime.count()) / 1000;
    double Err = setPoint - processVar;
    IntSum += Err * dt;

    double POut = Kp * Err;
    double IOut = Ki * IntSum;
    double DOut = Kd * (Err - LastErr) / dt;

    if (dt == 0) {
        DOut = 0;
    }

    double Output = POut + IOut + DOut;
    Output = std::max(Min, std::min(Output, Max));

    LastErr = Err;
    LastTime = epoch;
    return Output;
}

void PID::Reset() {
    LastErr = 0;
    IntSum = 0;
    LastTime = std::chrono::milliseconds(-1);
}

