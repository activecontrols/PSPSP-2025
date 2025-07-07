#include <iostream>
using namespace std;

class PID {
private:
    double Kp, Ki, Kd;
    double prev_error, integral;

public:
    PID(double Kp, double Ki, double Kd) 
        : Kp(Kp), Ki(Ki), Kd(Kd), prev_error(0), integral(0) {}
    double compute(double setpoint, double actual, double dt);
    void reset();
};

double PID::compute(double setpoint, double actual, double dt) {
    double error = setpoint - actual;
    integral += error * dt;  
    double derivative = (error - prev_error) / dt; 
    prev_error = error; 
}  

void PID::reset() {
    prev_error = 0;
    integral = 0;
}

int main()
{
    PID::compute(setpoint, actual, dt);

    return 0;
}