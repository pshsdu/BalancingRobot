#include <mega2560.h>
#include <delay.h>
#include <alcd.h>                

unsigned char  FONT1[8] = {    
        0x00, 0x00, 0x04, 0x04, 0x0A, 0x11, 0x00, 0x00   
};

unsigned char  FONT2[8] = {    
        0x00, 0x04, 0x04, 0x1C, 0x04, 0x04, 0x04, 0x00   
}; 

unsigned char  FONT3[8] = {    
        0x00, 0x00, 0x0E, 0x08, 0x08, 0x0E, 0x00, 0x00   
};

unsigned char  FONT4[8] = {    
        0x00, 0x04, 0x04, 0x06, 0x04, 0x04, 0x04, 0x00  
};  

unsigned char  FONT5[8] = {    
        0x03, 0x04, 0x03, 0x00, 0x0F, 0x00, 0x04, 0x07   
};

unsigned char  FONT6[8] = {    
        0x18, 0x04, 0x18, 0x00, 0x1E, 0x00, 0x00, 0x18   
};                                    
            

void lcd_data(unsigned char address, unsigned char data[])
{
    unsigned char cnt, ADDR;
    
    ADDR = address <<3 | 0x40;
    
    for(cnt = 0; cnt <8; cnt++)
    {
        lcd_write_byte(ADDR+cnt,data[cnt]);
    }
}

void main(void)
{  
    lcd_init(8);
         
    lcd_data(0, FONT1);
    lcd_gotoxy(0,0);
    lcd_putchar(0);
    
    lcd_data(1, FONT2);
    lcd_gotoxy(1,0);
    lcd_putchar(1);
    
    lcd_data(2, FONT3);
    lcd_gotoxy(2,0);
    lcd_putchar(2);
    
    lcd_data(3, FONT4);
    lcd_gotoxy(3,0);
    lcd_putchar(3); 
    
    lcd_data(4, FONT5); 
    lcd_gotoxy(4,0);
    lcd_putchar(4);
    
    lcd_data(5, FONT6);
    lcd_gotoxy(5,0);
    lcd_putchar(5);
                    
    while(1)
    {                            
        ;
    }
}
