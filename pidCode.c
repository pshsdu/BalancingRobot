/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2019-07-31
Author  : 
Company : 
Comments: 


Chip type               : ATmega2560
Program type            : Application
AVR Core Clock frequency: 16.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 2048
*****************************************************/

#include <mega2560.h>
#include <delay.h>
#include <stdio.h>
// Alphanumeric LCD Module functions
#include <alcd.h>

#define LeftSwitchOn    (!PINC.4)
#define MiddleSwitchOn  (!PINC.3)
#define RightSwitchOn   (!PINC.2)
#define LeftSwitchOff   (PINC.4)
#define MiddleSwitchOff (PINC.3)
#define RightSwitchOff  (PINC.2)

unsigned char menu = 0; 
//About LCD
unsigned char lcd_data[40];

float Sum_err = 0;
float RdesireVel = 50;
float LdesireVel = 50;
float speed = 0;

unsigned char Kp = 10;
unsigned char Ki = 0;

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
        // Right Motor
        float Rcnt;
        float RVel;
        float err;
        float absSpeed;
        float sign;

        Rcnt = (TCNT1L || (TCNT1H <<8)) - (TCNT3L || (TCNT3H <<8));
        TCNT1L =  TCNT3L = TCNT1H = TCNT3H =0x00;
        RVel = Rcnt / 0.016384;
        
        err = RdesireVel - RVel;
        Sum_err += err;
        
        if(Sum_err > 255)                 Sum_err = 255;
        else if(Sum_err < -255)        Sum_err = -255;
        speed = Kp * err + Ki * Sum_err;    
        
        if(speed < 0){
            absSpeed = -speed;
            sign = 0;
        } 
        else sign = 1;
        
        OCR2A = (unsigned char)absSpeed;
        PINC.0 = (unsigned char)sign;     
}

void RMotorOff(void){
        TIMSK0=0x00;
        TCCR0A=0x00;
        TCCR0B=0x00;
        DDRH = 0x00;        
}

void RMotorOn(void){
        TIMSK0=0x01;
        TCCR0A=0x03;
        TCCR0B=0x04;
        TCCR2A=0xA3;
        TCCR2B=0x06;
        DDRH = 0xFF;
        
        
        if(MiddleSwitchOff){
                while(MiddleSwitchOff){
                        lcd_clear();
                        lcd_gotoxy(0,0);
                        sprintf(lcd_data, "%d", TCNT1L + TCNT1H >>8);
                        lcd_puts(lcd_data);
                        lcd_gotoxy(0,1);
                        lcd_puts(lcd_data); 
                }; 
        RMotorOff();
        }       
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=Out Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=0 State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Port E initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0x00;

// Port F initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTF=0x00;
DDRF=0x00;

// Port G initialization
// Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State5=T State4=T State3=T State2=T State1=T State0=T 
PORTG=0x00;
DDRG=0x00;

// Port H initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTH=0x00;
DDRH=0x00;

// Port J initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTJ=0x00;
DDRJ=0x00;

// Port K initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTK=0x00;
DDRK=0x00;

// Port L initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTL=0x00;
DDRL=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
// Mode: Fast PWM top=0xFF
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x00;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
// Mode: Fast PWM top=0x00FF
// OC1A output: Discon.
// OC1B output: Discon.
// OC1C output: Discon.
// Noise Canceler: On
// Input Capture on Rising Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=0x01;
TCCR1B=0xCC;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
// Mode: Fast PWM top=0xFF
// OC2A output: Non-Inverted PWM
// OC2B output: Non-Inverted PWM
ASSR=0x00;
TCCR2A=0x00;
TCCR2B=0x00;
TCNT2=0x00;
OCR2A=0x00;
OCR2B=0x00;

// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
// Mode: Fast PWM top=0x00FF
// OC3A output: Discon.
// OC3B output: Discon.
// OC3C output: Discon.
// Noise Canceler: On
// Input Capture on Rising Edge
// Timer3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=0x01;
TCCR3B=0xCC;
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;
OCR3CH=0x00;
OCR3CL=0x00;

// Timer/Counter 4 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
// Mode: Fast PWM top=0x00FF
// OC4A output: Discon.
// OC4B output: Discon.
// OC4C output: Discon.
// Noise Canceler: On
// Input Capture on Rising Edge
// Timer4 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR4A=0x01;
TCCR4B=0xCC;
TCNT4H=0x00;
TCNT4L=0x00;
ICR4H=0x00;
ICR4L=0x00;
OCR4AH=0x00;
OCR4AL=0x00;
OCR4BH=0x00;
OCR4BL=0x00;
OCR4CH=0x00;
OCR4CL=0x00;

// Timer/Counter 5 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
// Mode: Fast PWM top=0x00FF
// OC5A output: Discon.
// OC5B output: Discon.
// OC5C output: Discon.
// Noise Canceler: On
// Input Capture on Rising Edge
// Timer5 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR5A=0x01;
TCCR5B=0xCC;
TCNT5H=0x00;
TCNT5L=0x00;
ICR5H=0x00;
ICR5L=0x00;
OCR5AH=0x00;
OCR5AL=0x00;
OCR5BH=0x00;
OCR5BL=0x00;
OCR5CH=0x00;
OCR5CL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: Off
// INT5: Off
// INT6: Off
// INT7: Off
EICRA=0x00;
EICRB=0x00;
EIMSK=0x00;
// PCINT0 interrupt: Off
// PCINT1 interrupt: Off
// PCINT2 interrupt: Off
// PCINT3 interrupt: Off
// PCINT4 interrupt: Off
// PCINT5 interrupt: Off
// PCINT6 interrupt: Off
// PCINT7 interrupt: Off
// PCINT8 interrupt: Off
// PCINT9 interrupt: Off
// PCINT10 interrupt: Off
// PCINT11 interrupt: Off
// PCINT12 interrupt: Off
// PCINT13 interrupt: Off
// PCINT14 interrupt: Off
// PCINT15 interrupt: Off
// PCINT16 interrupt: Off
// PCINT17 interrupt: Off
// PCINT18 interrupt: Off
// PCINT19 interrupt: Off
// PCINT20 interrupt: Off
// PCINT21 interrupt: Off
// PCINT22 interrupt: Off
// PCINT23 interrupt: Off
PCMSK0=0x00;
PCMSK1=0x00;
PCMSK2=0x00;
PCICR=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x00;

// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=0x00;

// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=0x00;

// Timer/Counter 3 Interrupt(s) initialization
TIMSK3=0x00;

// Timer/Counter 4 Interrupt(s) initialization
TIMSK4=0x00;

// Timer/Counter 5 Interrupt(s) initialization
TIMSK5=0x00;

// USART0 initialization
// USART0 disabled
UCSR0B=0x00;

// USART1 initialization
// USART1 disabled
UCSR1B=0x00;

// USART2 initialization
// USART2 disabled
UCSR2B=0x00;

// USART3 initialization
// USART3 disabled
UCSR3B=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
ADCSRB=0x00;
DIDR1=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// Alphanumeric LCD initialization
// Connections specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTJ Bit 0
// RD - PORTJ Bit 1
// EN - PORTJ Bit 2
// D4 - PORTJ Bit 3
// D5 - PORTJ Bit 4
// D6 - PORTJ Bit 5
// D7 - PORTJ Bit 6
// Characters/line: 8
lcd_init(8);

// Global enable interrupts
#asm("sei")

while (1)
      {
      if(LeftSwitchOn) menu++;
      if(RightSwitchOn)  menu--;
      if(menu > 2)             menu = 0; 
       
       switch(menu)
        { 
            case 0:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("1.RMotorPID");
                    delay_ms(100);
                    if(MiddleSwitchOn)        RMotorOn();  
                    break;
             case 1:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("2.IMU test!");
                    delay_ms(100);
                    if(MiddleSwitchOn)   ;  
                    break;
        }
      }
}
