#ifndef Kalman_h
#define Kalman_h

struct Kalman {
	double filter(double raw);
	void setSensitivity(double s);
};

#endif
