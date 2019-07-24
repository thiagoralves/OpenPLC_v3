#define INPUT 0
#define OUTPUT 1

#define	PUD_DOWN 1
#define	PUD_UP 2

#define	PWM_OUTPUT 2
#define	PWM_MODE_MS	0

int wiringPiSetup(void) {}

int  digitalRead(int pin)
{
	return 0;
}
void digitalWrite(int pin, int value) {}
void pwmWrite(int pin, int value) {}
void pwmSetMode(int mode) {}
void pwmSetRange(unsigned int range) {}
void pwmSetClock(int divisor) {}

void pinMode(int pin, int mode) {}
void delay(unsigned int howLong) {}
void pullUpDnControl(int pin, int pud) {}
unsigned int millis(void) {}
