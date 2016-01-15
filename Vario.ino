#include <SFE_BMP180.h>
#include <Filters.h>
#include "Kalman.h"

const boolean useLogger = false;
const char BEEPER_PORT = 5;
const char SAMPLE_RATE = 8;
const char BEEP_INTERVAL = 3;
const int DESC_START_FREQ = 200;
const int CLIMB_START_FREQ = 350;
const int FREQ_STEP = 200;

SFE_BMP180 pressure;
const float filterSensitivity = 0.2f;
FilterOnePole filter(LOWPASS, filterSensitivity);
FilterOnePole filterAltitude(LOWPASS, 0.8, 0);

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
	initSettings();
	setupLCD();
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
	filter.setFilter(LOWPASS, filterSensitivity, referrence);

	if (useLogger) {
		Serial.print("baseline pressure: ");
		Serial.print(referrence, 3);
		Serial.println(" mb");
	}
	// make a sound to know setup was fine initialize
	beep(1);
	writeLCD(11.11);
	delay(50);
	stopBeep();
	delay(100);
	beep(2);
	writeLCD(22.22);
	delay(50);
	stopBeep();
	delay(100);
	beep(3);
	writeLCD(33.33);
	delay(50);
	stopBeep();
	delay(100);
	writeLCD(0);
}

void loop() {
	// Put a new pressure reading in the filter:
	double prs = getPressure();
	double currentPresure = filter.input(prs);
	if (noSamples == SAMPLE_RATE) {
		//double currentPresure = derivative.input(prs);//filter.output();
		//double currentPresure = filter.output();
		if (useLogger) {
			Serial.print("current pressure: ");
			Serial.print(currentPresure);
			Serial.println(" mb");
		}
		// The relative altitude from the reference point (not from the GND)
		double relativeAltitude = pressure.altitude(currentPresure, referrence);
		relativeAltitude = filterAltitude.input(relativeAltitude);
		referrence = currentPresure;
		noSamples = 0;
		beep(relativeAltitude);
		updateSettings();
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
		if (abs(vSpeed) > 0.1) {
			filterAltitude.setFrequency(1);
			writeLCD(vSpeed);
		} else {
			filterAltitude.setFrequency(0.8);
			writeLCD(0);
		}

		if (vSpeed > 0.1 && iterationSoundStart >= 0) {
			//digitalWrite(13, HIGH); // turn the LED on (HIGH is the voltage level)
			unsigned int climbFreq = round(vSpeed * FREQ_STEP) + CLIMB_START_FREQ; //350;
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
