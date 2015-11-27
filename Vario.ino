#include <SFE_BMP180.h>
#include <Filters.h>

const boolean useLogger = true;
const char BEEPER_PORT = 3;
const double filterFrequency = 0.4;
const char SAMPLE_RATE = 8;
const char BEEP_INTERVAL = 4;

FilterOnePole presureFilter(LOWPASS, filterFrequency);
SFE_BMP180 pressure;

double referrence; // the reference altitude pressure
double temperature;
float noSamples = SAMPLE_RATE;
float pSum = 0;

void setup() {
	pinMode(BEEPER_PORT, OUTPUT);
	pinMode(13, OUTPUT);
	if (useLogger)
		Serial.begin(9600);
	if (useLogger)
		Serial.println("REBOOT");

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
	presureFilter.setToNewValue(referrence);
	if (useLogger)
		Serial.print("baseline pressure: ");
	if (useLogger)
		Serial.print(referrence);
	if (useLogger)
		Serial.println(" mb");

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
	presureFilter.input(prs);
	if (noSamples == SAMPLE_RATE) {
		double currentPresure = presureFilter.output();
		double relativeAltitude = pressure.altitude(currentPresure, referrence); // The relative altitude from the referrence point (not from the GND)
		//Serial.print("relative altitude: ");
		//Serial.println(relativeAltitude /*altSum / noSamples*/, 2);
		referrence = currentPresure;
		noSamples = 0;
		beep(relativeAltitude);
	}
	noSamples++;
}

double getPressure() {
	double pRet;
	char status = refreshTemperature();
	if (status != 0) {
		// Start a pressure measurement:
		// The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
		// If request is successful, the number of ms to wait is returned.
		// If request is unsuccessful, 0 is returned.
		status = pressure.startPressure(3);
		if (status != 0) {
			// Wait for the measurement to complete:
			delay(status);
			// Retrieve the completed pressure measurement:
			// Note that the measurement is stored in the variable P.
			// Note also that the function requires the previous temperature measurement (T).
			// !!!! (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
			// Function returns 1 if successful, 0 if failure.

			status = pressure.getPressure(pRet, temperature);
			if (status != 0) {
				return pRet;
			}
		}
	}
	return 0;
}

char refreshTemperature() {
	if (noSamples == SAMPLE_RATE) {
		char status = pressure.startTemperature();
		if (status != 0) {
			// Wait for the measurement to complete:
			delay(status);
			// Retrieve the completed temperature measurement:
			// Note that the measurement is stored in the variable T.
			// Use '&T' to provide the address of T to the function.
			// Function returns 1 if successful, 0 if failure.
			return pressure.getTemperature(temperature);
		}
	}
	return 1;
}

int iterationSoundStart = 0;
void beep(double vSpeed) {
	if (useLogger)
		Serial.print("Tone: ");
	if (useLogger)
		Serial.println(vSpeed /*altSum / noSamples*/, 2);

	if (abs(vSpeed) < 5) {
		if (iterationSoundStart > BEEP_INTERVAL) {
			stopBeep();
			iterationSoundStart = -BEEP_INTERVAL;

		}
		if (vSpeed > 0.1 && iterationSoundStart > 0) {
			//digitalWrite(13, HIGH);  // turn the LED on (HIGH is the voltage level)
			unsigned int climbFreq = round(vSpeed * 300) + 350;
			tone(BEEPER_PORT, climbFreq, round(vSpeed * 2000));
		} else if (vSpeed < -0.6 && iterationSoundStart > 0) {
			unsigned int descFreq = 150 - round(-vSpeed * 100);
			tone(BEEPER_PORT, descFreq, round(-vSpeed * 2000));
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
