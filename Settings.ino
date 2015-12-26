const boolean useSettings = false;
const char BUTTON_TEST = 9;
const char BUTTON_SETTINGS = 8;

void initSettings() {
	if (!useSettings) {
		return;
	}
	pinMode(BUTTON_TEST, INPUT);
	pinMode(13, OUTPUT);
}

int buttonState = 0;
void updateSettings() {
	if (!useSettings) {
		return;
	}
	buttonState = digitalRead(BUTTON_TEST);
	if (buttonState == LOW) {
		buttonState = digitalRead(BUTTON_SETTINGS);
		if (buttonState == HIGH) {

		}
	} else {
		digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
		delay(2000);              // wait for a second
		digitalWrite(13, LOW);
	}
}
