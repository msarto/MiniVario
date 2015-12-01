#include "Kalman.h"

double sensitivity = 25.0;

float Q = sensitivity * 0.000004f;
float R = sensitivity * 0.0006f;
float P = sensitivity / (2.0f * 1000.0f);
float X = 0.0f; // one dimensional
float K = 0.0f;

double Kalman::filter(double raw) {
	if (X == 0) {
		X = raw;
	}
	K = (P + Q) / (P + Q + R);
	P = R * K;
	X = X + (raw - X) * K;
	return X;
}

void Kalman::setSensitivity(double s) {
	sensitivity = s;
	Q = sensitivity * 0.000004f;
	R = sensitivity * 0.0006f;
	P = sensitivity / (2.0f * 1000.0f);
	X = 0.0f; // one dimensional
	K = 0.0f;
}

