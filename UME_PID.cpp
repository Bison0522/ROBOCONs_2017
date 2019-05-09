// pid.h
#pragma once
class pid
{
private:
	double Kp;
	double Ki;
	double Kd;
	double En;
	double En_1;
	double En_2;
	double b;
	double integ;

public:
	pid(double, double, double);

	double get_pid(double, double);

	~pid();
};

/////////////////////////////////////////////

#include "pid.h"
#include <stdio.h>

pid::pid( double p, double i, double d ) {
	Kp = p;
	Ki = i;
	Kd = d;
	integ = 0;
	En_1 = 0;
	En_2 = 0;
}

double pid::get_pid(double target, double now) {
	En = target - now;
	integ += Ki * En;
	b = integ + Kd * En + Kd * ((En - En_1) - (En_1 - En_2));
	En_1 = En;
	En_2 = En_1;
	return b;
}

pid::~pid() {

}
