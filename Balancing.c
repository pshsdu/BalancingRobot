 /*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
?Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2019-08-01
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

#define LeftSwitchOn    (!PINC.6)
#define MiddleSwitchOn  (!PINC.4)
#define RightSwitchOn   (!PINC.2)
#define LeftSwitchOff   (PINC.6)
#define MiddleSwitchOff (PINC.4)
#define RightSwitchOff  (PINC.2)
#define MenuLan 10
// Declare your global variables here

//About LCD
unsigned char lcd_data[40];
//About ADC
unsigned char adc_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
unsigned char mux = 0;
unsigned char adc_max[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //tuning에서 최대값 및 최소값을 넣기 위한 배열
unsigned char adc_min[8] = {255, 255, 255, 255, 255, 255, 255, 255}; 
unsigned char emit[8] = {0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x01};
unsigned char adc_normal[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //실제로 사용하는 adc값
float position = 0;
signed int position_array[8] = {0, -25, -15, -5, 5, 15, 25, 0}; //position이 반대인 경우 이 배열을 바꾸세요 
float posGain = 50;
float mutipliedPosition = 0; 

//About Motor
float Kp = 3.5;
float Ki = 0.3;
float desireVel = 0;
//Left
float Sum_err_L = 0;
float u_L = 0;
int Lcnt = 0;
int LVel = 0;
int lOCR = 0; 
//Right
float Sum_err_R = 0;
float u_R = 0;
int Rcnt = 0;
int RVel = 0;
int rOCR = 0;

//About IMU
unsigned char cnt = 0;
unsigned char i = 0;
int rollData[8] = {0,0,0,0,0,0,0,0};   
int rollAngle = 0;
float mAng = 0;
float targetAng = 0;
//Control
float AngKp = 41;
float AngKi = 0.4;
float offset = 0;
float u_ang = 0;
float SumAngErr = 0;

//About Control flag
float target = 0;
unsigned char isNevigated = 0;
int menu = 0; 

//About Target Control
int angVel = 0;
int mBodyVel = 0;
int filteredBodyVel = 0;
int desireBodyVel = 0;
float sumBodyVelErr = 0;
float bodyVelKp = 0.001;
float bodyVelKi = 0.001;   
int centerVel = 0;

// ADC interrupt service routine
interrupt [ADC_INT] void adc_isr(void)
{
    unsigned char i;
    PORTA = emit[mux];
    for (i = 0; i<=7; i+=1);
    adc_data[mux]=ADCH;
    mux++;
    if(mux >= 8) mux=0;
    ADMUX = mux | 0x60;  
    ADCSRA |= 0x40; 
}

void calAngTarget(void){
        float bodyVelErr = 0;
        
        if(target > desireBodyVel) desireBodyVel += 1;

        bodyVelErr = desireBodyVel - filteredBodyVel;  
        sumBodyVelErr += bodyVelErr * 0.02; //i
        
        if(sumBodyVelErr > 500)          sumBodyVelErr = 500;
        else if(sumBodyVelErr < -500) sumBodyVelErr = -500;
        
        targetAng = offset + (bodyVelErr * bodyVelKp + bodyVelKi * sumBodyVelErr);
}

void calTarget(void){
        float angErr = 0;  
        
        angErr = targetAng - mAng;        
        SumAngErr += angErr;
        
        if(SumAngErr > 500)                 SumAngErr = 500;
        else if(SumAngErr < -500)        SumAngErr = -500;
        
        u_ang = AngKp * angErr + AngKi * SumAngErr;
        
        if(u_ang > 1000)                 u_ang = 1000;
        else if(u_ang < -1000)        u_ang = -1000;
        
        desireVel =  -u_ang;                 
}

void RMotorPI(void){
        float err_R = 0; 
        int sign = 0;
           
        err_R = desireVel - RVel;   
        Sum_err_R += err_R;             
        
        if(Sum_err_R > 20)                 Sum_err_R = 20;
        else if(Sum_err_R < -20)        Sum_err_R = -20;     
        
        u_R = Kp * err_R + Ki * Sum_err_R;
        rOCR = u_R + mutipliedPosition;
           
        if(rOCR > 255)                 rOCR = 255;
        else if(rOCR < -255)        rOCR = -255;

        if(rOCR < 0){
              rOCR = -rOCR;
              sign = 1;
        } 
        else{
              sign = 0;
        }      
        
      OCR2A = (unsigned char)rOCR; 
      PORTC.0 = (unsigned char)sign;
}

void LMotorPI(void){
        float err_L = 0; 
        int sign = 0;
        
        err_L = desireVel - LVel;   
        Sum_err_L += err_L;             
        
        if(Sum_err_L > 20)                 Sum_err_L = 20;
        else if(Sum_err_L < -20)        Sum_err_L = -20;     
        
        u_L = Kp * err_L + Ki * Sum_err_L;
        lOCR = u_L - mutipliedPosition;
            
        if(lOCR > 255)                 lOCR = 255;
        else if(lOCR < -255)        lOCR = -255;
        
        if(lOCR < 0){
              lOCR = -lOCR;
              sign = 1;
        } 
        else{
              sign = 0;
        }
        
        OCR2B = (unsigned char)lOCR; 
        PORTC.1 = (unsigned char)sign; 
}

// Timer 0 output compare A interrupt service routine
interrupt [TIM0_COMPA] void timer0_compa_isr(void)
{
        Rcnt = (TCNT4L | (TCNT4H <<8)) - (TCNT5L | (TCNT5H<<8));
        Lcnt = (TCNT1L | (TCNT1H <<8)) - (TCNT3L | (TCNT3H<<8));
        
        RVel = -(Rcnt  * 47) >> 3;          //rpm
        LVel = (Lcnt  * 47) >> 3;       //rpm
        
        angVel = (rollAngle * 76) >> 4;
        centerVel = ((LVel + RVel)  * 67) >> 2;
        
        mBodyVel = centerVel + angVel;
        filteredBodyVel = 0.99 *  filteredBodyVel + 0.01 *  mBodyVel;
        
        calAngTarget();     
        calTarget();
        LMotorPI();
        RMotorPI();
        
        TCNT3L = 0x00;  TCNT1L = 0x00;  TCNT3H = 0x00;  TCNT1H =0x00;
        TCNT4L = 0x00;  TCNT5L = 0x00;  TCNT4H = 0x00;  TCNT5H =0x00;
}

// USART1 Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rxc(void)   //IMU에서 들어오는 센서 값을 받아들이는 코드
{
        int buff = 0;
        buff = UDR1;
                 
        if(cnt >=2){               
                rollData[i]=buff;
                i++;
        }
        
        if(buff==0x55) cnt++;   
      
        if(i >= 8){
                i = 0;
                cnt = 0;
                rollAngle = ((rollData[0]<<8) | rollData[1]);
                mAng = rollAngle * 0.01;
        }  
}

void init(void){
        //interrupt On
        TIMSK0=0x02;
        TCCR0A=0x02;
        TCCR0B=0x04;
        TCNT0=0x00;
        OCR0A=124;   
          
        //motor On
        TCCR2A=0xA3;
        TCCR2B=0x01;
        TCNT2=0x00;
        
        //USART On
        UCSR1B=0x90;
        UCSR1C=0x06;
        UBRR1L=0x08;
        
        //Counter On
        TCCR1B=0x06;
        TCCR3B=0x06;
        TCCR4B=0x06;
        TCCR5B=0x06;
       
        //Nevigate
        isNevigated = 0;
        offset = 1;
        delay_ms(100); 
}

void motorOff(){
        //Interrupt Off 
        TIMSK0=0x00;
        TCCR0A=0x00;
        TCCR0B=0x00;
        
        //Motor Off
        TCCR2A=0x00;
        TCCR2B=0x00;
        TCNT2=0x00;
        
        //USART Off
        UCSR1B=0x00;
        UCSR1C=0x00;
        UBRR1L=0x00;
        
        //Counter Off
        TCCR1B=0x00;
        TCCR3B=0x00;
        TCCR4B=0x00;
        TCCR5B=0x00; 
        delay_ms(100); 
}

void updatePosition(void)
{
    signed long sigma_p_an=0;
    signed long sigma_an=0;
    unsigned char adc = 0;   
    
    for(adc=0; adc < 8; adc++)
    {
        if(adc_data[adc] < adc_min[adc]) adc_normal[adc] = 0;
        else if(adc_data[adc] > adc_max[adc]) adc_normal[adc] = 100;
        else adc_normal[adc] = ((unsigned long)adc_data[adc] - adc_min[adc]) * 100
                                / ((unsigned long)adc_max[adc] - adc_min[adc]);
          
        sigma_p_an += (signed long)position_array[adc] * adc_normal[adc];
          
        sigma_an += (signed long)adc_normal[adc];
    }
    position = sigma_p_an  / sigma_an;
    mutipliedPosition = posGain * position;
}

//About Sensor Tuning
void sensorTuning(void)
{
     unsigned char adc = 0;
     delay_ms(500);
    
     lcd_clear();
     lcd_gotoxy(0, 0);
     lcd_putsf("Tuning");
     delay_ms(100);
     while(RightSwitchOff)
     {
         if(adc_data[adc] < adc_min[adc]) adc_min[adc] = adc_data[adc];
         if(adc_data[adc] > adc_max[adc]) adc_max[adc] = adc_data[adc];
         adc++;
         if(adc > 7) adc=0;
     }
}

void piControl(void){
        delay_ms(500);
        lcd_clear();
        init();
        
        while(RightSwitchOff){
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_gotoxy(0, 1);
                sprintf(lcd_data,"T:%d",(signed int)desireVel);
                lcd_puts(lcd_data);                 
                delay_ms(100);
        }
        motorOff();
}

void navigate(void){
        delay_ms(500);
        lcd_clear();
        init();
        
        while(RightSwitchOff){
                if(LeftSwitchOn){
                        isNevigated = 1;
                        offset = 2;
                }
                
                if(isNevigated){
                        updatePosition();              
                }
        }
        motorOff();
}

void SetKp(void){
     delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)           Kp-=0.1;
        if(MiddleSwitchOn)      Kp+=0.1;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("Kp");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data, "%d.%d", (unsigned char)Kp,(unsigned char)(Kp*10)%10);
        lcd_puts(lcd_data);                  
        delay_ms(100);
     }
}

void setAngKp(void){
     delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)        AngKp-=0.1;
        if(MiddleSwitchOn)      AngKp+=0.1;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("AngKp");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data, "%d.%d", (unsigned char)AngKp,(unsigned char)(AngKp*10)%10);
        lcd_puts(lcd_data);                  
        delay_ms(100);
     }
}

void setAngKi(void){
     delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)        AngKi -=0.02;
        if(MiddleSwitchOn)      AngKi +=0.02;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("AngKi");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data, "%d.%d%d", (unsigned char)AngKi,(unsigned char)(AngKi*10)%10,(unsigned char)(AngKi*100)%10);
        lcd_puts(lcd_data);                  
        delay_ms(100);
     }
}

void SetKi(void){
     delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)                                Ki -= 0.1;
        if(MiddleSwitchOn &&  Ki >= 0)        Ki += 0.1;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("K_i");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data, "%d.%d", (unsigned char)(Ki),(unsigned char)(Ki*10)%10);
        lcd_puts(lcd_data);                 
        delay_ms(100);
     }
}

void setOffset(void){
        delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)                offset  -= 0.1;
        if(MiddleSwitchOn)           offset  += 0.1;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("offset");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data,  "%d.%d", (signed int)(offset),(unsigned int)(offset*10)%10);
        lcd_puts(lcd_data);                 
        delay_ms(100);
     }
}

void setPositionGain(void){
     delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)        posGain -=1;
        if(MiddleSwitchOn)      posGain+=1;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("Gain");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data,  "%d", (signed int)posGain);
        lcd_puts(lcd_data);                  
        delay_ms(100);
     }
}

void setTargetKp(void){
     delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)        bodyVelKp -=0.001;
        if(MiddleSwitchOn)      bodyVelKp+=0.001;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("TargetKp");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data,  "%d", (signed int)(bodyVelKp*1000));
        lcd_puts(lcd_data);                  
        delay_ms(100);
     }
}

void setTargetKi(void){
     delay_ms(500);
     
     while(RightSwitchOff)
     {
        if(LeftSwitchOn)        bodyVelKi -=0.001;
        if(MiddleSwitchOn)      bodyVelKi+=0.001;
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_putsf("TargetKi");
        lcd_gotoxy(0, 1);
        sprintf(lcd_data,  "%d", (signed int)(bodyVelKi*1000));
        lcd_puts(lcd_data);                  
        delay_ms(100);
     }
}

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
DDRA=0xFF;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x10;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x03;

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
DDRH=0x40;

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
// Clock value: 250.000 kHz
// Mode: CTC top=OCR0A
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x00;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: T1 pin Falling Edge
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// OC1C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
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
// Clock value: 16000.000 kHz
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
// Clock source: T3 pin Falling Edge
// Mode: Normal top=0xFFFF
// OC3A output: Discon.
// OC3B output: Discon.
// OC3C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=0x00;
TCCR3B=0x00;
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
// Clock source: T4 pin Falling Edge
// Mode: Normal top=0xFFFF
// OC4A output: Discon.
// OC4B output: Discon.
// OC4C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer4 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR4A=0x00;
TCCR4B=0x00;
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
// Clock source: T4 pin Falling Edge
// Mode: Normal top=0xFFFF
// OC5A output: Discon.
// OC5B output: Discon.
// OC5C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer5 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR5A=0x00;
TCCR5B=0x00;
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

SREG = 0x80;
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
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART1 Receiver: On
// USART1 Transmitter: Off
// USART1 Mode: Asynchronous
// USART1 Baud Rate: 115200
UCSR1A=0x00;
UCSR1B=0x00;
UCSR1C=0x00;
UBRR1H=0x00;
UBRR1L=0x00;

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
ADCSRB=0x08;
DIDR1=0x00;

// ADC initialization
// ADC Clock frequency: 125.000 kHz
// ADC Voltage Reference: AVCC pin
// ADC Auto Trigger Source: Timer1 Overflow
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
// ADC4: On, ADC5: On, ADC6: On, ADC7: On
DIDR0=0x00;
// Digital input buffers on ADC8: On, ADC9: On, ADC10: On, ADC11: On
// ADC12: On, ADC13: On, ADC14: On, ADC15: On
ADMUX=0x20;
ADCSRA=0xCF;
ADCSRB=0x0F;

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
#asm("sei")
while (1)
      {
        if(LeftSwitchOn)                 menu--;
        if(MiddleSwitchOn)            menu++;
        if(menu > MenuLan)         menu = 0;
        if(menu < 0)                      menu = MenuLan;
       
       switch(menu)
        { 
            case 0:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("1.MotorPI");
                    delay_ms(200);
                    if(RightSwitchOn)        piControl();  
                    break;
                    
             case 1:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("2.Set Kp");
                    delay_ms(200);
                    if(RightSwitchOn)          SetKp() ;  
                    break;
                    
             case 2:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("3.Set Ki");
                    delay_ms(200);
                    if(RightSwitchOn)          SetKi() ;  
                    break;
             
             case 3:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("4.AngKp");
                    delay_ms(200);
                    if(RightSwitchOn)          setAngKp() ;  
                    break;
             
             case 4:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("5.AngKi");
                    delay_ms(200);
                    if(RightSwitchOn)          setAngKi() ;  
                    break;
                    
             case 5:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("6.Offset");
                    delay_ms(200);
                    if(RightSwitchOn)          setOffset() ;  
                    break;
                    
             case 6:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("7.position");
                    delay_ms(200);
                    if(RightSwitchOn)          setPositionGain() ;  
                    break;
                                 
             case 7:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("8.targetKp");
                    delay_ms(200);
                    if(RightSwitchOn)          setTargetKp() ;  
                    break;
                                 
             case 8:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("9.targetKi");
                    delay_ms(200);
                    if(RightSwitchOn)          setTargetKi() ;  
                    break;
                    
             case 9:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("10.sensorTuning");
                    delay_ms(200);
                    if(RightSwitchOn)          sensorTuning() ;  
                    break; 
             
             case 10:
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("11.navigate");
                    delay_ms(200);
                    if(RightSwitchOn)          navigate() ;  
                    break;                           
        }
      }
}
