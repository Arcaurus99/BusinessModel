#include <16f887.h>
#fuses xt,nowdt
#use delay(crystal=4000000)
#use rs232(baud=57600,xmit=pin_c6,rcv=pin_c7,bits=8)
#include <lcd.c>
#byte PORTC= 7
#define LED_G PORTC,3
#define LED_Y PORTC,4
#define LED_R PORTC,5

int opc;
float tem;
int valor;


// ---------- MAIN ----------
void main()
{
   while(TRUE)
   {
      lcd_init();
      opc=getc();

      switch(opc)
      {
         case 1:
            output_high(pin_c3);
            for(valor=0;valor<=10;valor++)
            {
               putc(valor); 
               lcd_gotoxy(1,1);
               printf(lcd_putc,"\f OPCION %i",opc);
               lcd_gotoxy(1,2);
               printf(lcd_putc,"Enviando= %1D",valor);
               delay_ms(200);
            }
            output_low(pin_c3);
            lcd_putc("\f");
            break;

         case 2:
            opc=0;

            output_high(pin_c3);
            tem=getc();
            tem=tem/100;

            while(opc==0)
            {  
               lcd_gotoxy(1,1);
               lcd_putc("\fTEMPERATURA");
               lcd_gotoxy(2,2);
               printf(lcd_putc,"C= %f    ",tem);

               opc=getc();
            }
            output_high(pin_c4);
            output_low(pin_c3);
            delay_ms(40);
            output_low(pin_c4);
            lcd_putc("\f");
            break;

         case 3:
            opc=0;

            output_high(pin_c3);

            while(opc==0)
            {  
               tem=getc();
               lcd_gotoxy(1,1);
               lcd_putc("\fTEMPERATURA");
               lcd_gotoxy(2,2);
               printf(lcd_putc,"C= %f    ",tem);
            }
            output_high(pin_c4);
            output_low(pin_c3);
            delay_ms(40);
            output_low(pin_c4);
            lcd_putc("\f");
            break;
      }
   }
}


/*// ---------- INT_RDA ----------
#int_RDA
RDA_isr()
{
   valor=getc();
}
// ---------- MAIN ----------
void main()
{  
   lcd_init();
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   
   for(;;)
   {
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Recibido= %1D",valor);
   }
}*/
