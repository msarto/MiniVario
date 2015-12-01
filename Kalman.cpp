#include "Kalman.h"

double sensitivity = 2.0f;

double Q = sensitivity * 0.000004f;
double R = sensitivity * 0.0006f;
double P = sensitivity / (2.0f * 1000.0f);
double X = 0.0f; // one dimensional
double K = 0.0f;

double Kalman::filter(double raw) {
	if (X == 0) {
		X = raw;
	}
	K = (P + Q) / (P + Q + R);
	P = R * K;
	X = X + (raw - X) * K;
	return X;
}

void Kalman::setSensitivity(double s, double initialValue) {
	sensitivity = s;
	Q = sensitivity * 0.000004;
	R = sensitivity * 0.0006;
	P = sensitivity / (1000.0);
	X = initialValue; // one dimensional
	K = 0.0;
}
