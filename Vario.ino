#include <SFE_BMP180.h>
#include "Kalman.h"

const boolean useLogger = true;
const char BEEPER_PORT = 3;
const char SAMPLE_RATE = 5;
const char BEEP_INTERVAL = 7;
const int DESC_START_FREQ = 250;
const int CLIMB_START_FREQ = 370;
const int FREQ_STEP = 200;

SFE_BMP180 pressure;
Kalman filter;

double referrence; // the reference altitude pressure
double temperature;
char noSamples = SAMPLE_RATE;

void setup() {
	if (useLogger) {
		pinMode(BEEPER_PORT, OUTPUT);
		//pinMode(13, OUTPUT);
		Serial.begin(9600);
		Serial.println("REBOOT");
	}
	// Initialize the sensor (it is important to get calibration values stored on the device).
	if (pressure.begin()) {
		if (useLogger)
			Serial.println("BMP180 init success");
	} else {
		// Oops, something went wrong, this is usually a connection problem,
		// see the comments at the top of this sketch for the proper connections.
		if (useLogger)
			Serial.println("BMP180 init fail (disconnected?)\n\n");
		while (1)
			; // Pause forever.
	}

	// Get the baseline pressure:
	referrence = getPressure();
	//filter.setSensitivity(filterSensitivity, referrence);
	if (useLogger) {
		Serial.print("baseline pressure: ");
		Serial.print(referrence, 3);
		Serial.println(" mb");
	}
	// make a sound to know setup was fine initialize
	beep(1);
	delay(50);
	stopBeep();
	delay(100);
	beep(2);
	delay(50);
	stopBeep();
	delay(100);
	beep(3);
	delay(50);
	stopBeep();
	delay(100);
}

void loop() {
	// Put a new pressure reading in the filter:
	double prs = getPressure();
	double currentPresure = filter.filter(prs);
	int var = 20;
	if (noSamples == SAMPLE_RATE) {
		// The relative altitude from the reference point (not from the GND)
		double relativeAltitude = pressure.altitude(currentPresure, referrence);
		referrence = currentPresure;
		noSamples = 0;
		beep(relativeAltitude);
	}
	noSamples++;
}

double getPressure() {
	double pRet;
	if (refreshTemperature()) {
		int status = pressure.startPressure(3);
		if (status != 0) {
			delay(status);
			status = pressure.getPressure(pRet, temperature);
			if (status != 0) {
				return pRet;
			}
		}
	}
	return referrence;
}

char refreshTemperature() {
	if (noSamples == SAMPLE_RATE) {
		char status = pressure.startTemperature();
		if (status != 0) {
			delay(status);
			return pressure.getTemperature(temperature);
		}
	}
	return 1;
}

int iterationSoundStart = 0;
void beep(double vSpeed) {
	if (useLogger) {
		Serial.print("Tone: ");
		Serial.println(vSpeed, 2);
	}
	if (abs(vSpeed) < 5) {
		if (iterationSoundStart > BEEP_INTERVAL) {
			stopBeep();
			iterationSoundStart = -BEEP_INTERVAL;
			if (useLogger)
				Serial.println("pause beep");
		}

		if (vSpeed > 0.1 && iterationSoundStart >= 0) {
			//digitalWrite(13, HIGH); // turn the LED on (HIGH is the voltage level)
			unsigned int climbFreq = round(vSpeed * FREQ_STEP) + CLIMB_START_FREQ;//350;
			tone(BEEPER_PORT, climbFreq, BEEP_INTERVAL * 100);
		} else if (vSpeed < -0.6 && iterationSoundStart >= 0) {
			unsigned int descFreq = DESC_START_FREQ - round(-vSpeed * FREQ_STEP);
			tone(BEEPER_PORT, descFreq, BEEP_INTERVAL * 100);
		} else {
			stopBeep();
		}
		iterationSoundStart++;
	}
}

void stopBeep() {
	//digitalWrite(13, LOW);   // turn the LED off
	iterationSoundStart = 0;
	noTone(BEEPER_PORT);
}

