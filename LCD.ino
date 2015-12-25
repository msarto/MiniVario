const int clock = 10;
const int data = 11;

/*0*//*1*//*2*//*3*//*4*//*5*//*6*//*7*//*8*//*9*/
uint8_t digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };

void setupLCD() {
	pinMode(clock, OUTPUT);
	pinMode(data, OUTPUT);

	startLCD();
	writeLCDValue(0x8c);
	stopLCD();

	// clear display
	write(0x00, 0x00, 0x00, 0x00);
}

void writeLCD(double lcdValue) {
	double d = abs(lcdValue); //4.32;
	byte minutes = byte(d);
	double integral;
	double fractional = modf(d, &integral);
	byte seconds = byte(fractional * 100);

	write(digits[minutes / 10], digits[minutes % 10] | ((seconds & 0x01) << 7), digits[seconds / 10],
			digits[seconds % 10]);
}

void write(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
	startLCD();
	writeLCDValue(0x40);
	stopLCD();

	startLCD();
	writeLCDValue(0xc0);
	writeLCDValue(first);
	writeLCDValue(second);
	writeLCDValue(third);
	writeLCDValue(fourth);
	stopLCD();
}

void startLCD(void) {
	digitalWrite(clock, HIGH);             //send start signal to TM1637
	digitalWrite(data, HIGH);
	delayMicroseconds(5);
	digitalWrite(data, LOW);
	digitalWrite(clock, LOW);
	delayMicroseconds(5);
}

void stopLCD(void) {
	digitalWrite(clock, LOW);
	digitalWrite(data, LOW);
	delayMicroseconds(5);
	digitalWrite(clock, HIGH);
	digitalWrite(data, HIGH);
	delayMicroseconds(5);
}

bool writeLCDValue(uint8_t value) {
	for (uint8_t i = 0; i < 8; i++) {
		digitalWrite(clock, LOW);
		delayMicroseconds(5);
		digitalWrite(data, (value & (1 << i)) >> i);
		delayMicroseconds(5);
		digitalWrite(clock, HIGH);
		delayMicroseconds(5);
	}

	// wait for ACK
	digitalWrite(clock, LOW);
	delayMicroseconds(5);
	pinMode(data, INPUT);
	digitalWrite(clock, HIGH);
	delayMicroseconds(5);
	bool ack = digitalRead(data) == 0;
	pinMode(data, OUTPUT);
	return ack;
}
