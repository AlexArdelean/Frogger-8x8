#ifndef __bit_h__
#define __bit_h__

void A2D_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}
void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) { asm("nop"); }
}

// Ensure DDRC is setup as output
void transmit_data_Green(unsigned char data) {
	int i = 0;
	for (i = 7; i >= 0; i--) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x08;
		// set SER = next bit of data to be sent.
		PORTB |= ((~data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x04;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the
	//“Storage” register
	PORTB |= 0x02;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}

// Ensure DDRC is setup as output
void transmit_data_Col(unsigned char data) {
	int i = 0;
	for (i = 7; i >= 0; i--) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x80;
		// set SER = next bit of data to be sent.
		PORTB |= ((data >> i) & 0x01) << 4;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x40;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the
	//“Storage” register
	PORTB |= 0x20;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}
void transmit_data_Red(unsigned char data) {
	int i = 0;
	for (i = 7; i >= 0; i--) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((~data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x04;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the
	//“Storage” register
	PORTC |= 0x02;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

// Ensure DDRC is setup as output
void transmit_data_Blue(unsigned char data) {
	int i = 0;
	for (i = 7; i >= 0; i--) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x80;
		// set SER = next bit of data to be sent.
		PORTC |= ((~data >> i) & 0x01) << 4;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x40;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the
	//“Storage” register
	PORTC |= 0x20;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}


#endif

