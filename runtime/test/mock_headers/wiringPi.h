// Copyright 2019 Smarter Grid Solutions
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.

#define INPUT 0
#define OUTPUT 1

#define    PUD_DOWN 1
#define    PUD_UP 2

#define    PWM_OUTPUT 2
#define    PWM_MODE_MS    0

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
