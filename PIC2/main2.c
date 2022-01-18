#include <16f887.h>
#fuses xt,noprotect,nowdt,nobrownout,noput,nolvp
#use delay(clock=4000000,crystal)
#use rs232(baud=57600,xmit=pin_c6,rcv=pin_c7,bits=8)
#use standard_io(B)
#use standard_io(C)
#define bobina_1 PIN_D4                      //Puertos para los motores
#define bobina_2 PIN_D5
#define bobina_3 PIN_D6
#define bobina_4 PIN_D7

void pasos_izquierda(int velocidad);
void pasos_derecha(int velocidad);
void pasos_off(void);

int order;
int direction;
int lastdir = 0;

#int_RDA
RDA_isr()
{
  order=getc();
}

void pasos_izquierda(int velocidad)
{
   output_low(bobina_1);
   output_low(bobina_2);
   output_high(bobina_3);
   output_high(bobina_4);
   delay_ms(velocidad);
   
   output_low(bobina_1);
   output_high(bobina_2);
   output_high(bobina_3);
   output_low(bobina_4);
   delay_ms(velocidad);
   
   output_high(bobina_1);
   output_high(bobina_2);
   output_low(bobina_3);
   output_low(bobina_4);
   delay_ms(velocidad);
   
   output_high(bobina_1);
   output_low(bobina_2);
   output_low(bobina_3);
   output_high(bobina_4);
   delay_ms(velocidad);
   
   direction = 1;
}

void pasos_derecha(int velocidad)
{
   output_high(bobina_1);
   output_high(bobina_2);
   output_low(bobina_3);
   output_low(bobina_4);
   delay_ms(velocidad);
   
   output_low(bobina_1);
   output_high(bobina_2);
   output_high(bobina_3);
   output_low(bobina_4);
   delay_ms(velocidad);
   
   output_low(bobina_1);
   output_low(bobina_2);
   output_high(bobina_3);
   output_high(bobina_4);
   delay_ms(velocidad);
   
   output_high(bobina_1);
   output_low(bobina_2);
   output_low(bobina_3);
   output_high(bobina_4);
   delay_ms(velocidad);
   
   direction = 2;
}

void pasos_off(void)
{
   output_low(bobina_1);
   output_low(bobina_2);
   output_low(bobina_3);
   output_low(bobina_4);
   
   lastdir = direction;
   direction = 0;
}

void main()
{
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
 
   pasos_off();
   
   if(order == 1)
   {
      pasos_izquierda(80);
      
   }
   
   if(order == 2)
   {
      pasos_derecha(80);
   }
   
   if(order == 3)
   {
      if(direction == 1)
      {
         pasos_off();
      }
      
      if(direction == 2)
      {
         pasos_off();
      }
      
      if(direction == 0)
      {
         if(lastdir == 1)
         {
            pasos_izquierda(80);
         }
         
         if(lastdir == 2)
         {
            pasos_derecha(80);
         }
      }
   }
}