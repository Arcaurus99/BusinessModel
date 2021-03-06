#include <16f887.h>
#use delay(clock=4000000,crystal)
#use rs232(baud=57600,xmit=pin_c6,rcv=pin_c7)
#fuses xt,noprotect,nowdt,nobrownout,noput,nolvp
#define use_portb_lcd True
#define use_portb_kbd True
#include <lcd.c>
#include <kbd.c> 
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#byte PORTA= 5
#byte PORTB= 6
#byte PORTC= 7
#define LED1 PORTC,4
#define LED2 PORTC,5

// VAR PARA RS232
int valor;
// ----- INT_RDA ----
#int_RDA
RDA_isr()
{
   valor=getc();
   return valor;
}
// VAR PARA EEPROM
char K;
int8 MIL,CEN,DEC,UNI,VAL;
int16 contra=9999;
int8 sinal;

// VAR PARA ADC
long bits;
float tem=0;
int i=0;


// ------------------- TECLADO -------------------
void teclado()
{
   k=kbd_getc(); 
   while(k=='\0')
   {
      k=kbd_getc();
   }
   if( (k!='\0'))
   {
      if(K=='0')
         VAL=0;
      if(K=='1')
         VAL=1;
      if(K=='2')
         VAL=2;
      if(K=='3')
         VAL=3;
      if(K=='4')
         VAL=4;
      if(K=='5')
         VAL=5;
      if(K=='6')
         VAL=6;
      if(K=='7')
         VAL=7;
      if(K=='8')
         VAL=8;
      if(K=='9')
         VAL=9;
   }
}
// ------------------- PEDIR CLAVE -------------------
void pedir_clave(void)
{
   teclado();
   if((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      MIL=VAL;
   }
   if(k=='*' || k=='#')
      return;
   teclado();
   if((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      CEN=VAL;
   }
   if(k=='*' || k=='#')
      return;
   teclado();
   if((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      DEC=VAL;
   }
   if(k=='*' || k=='#')
      return;
   teclado();
   if((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      UNI=VAL;
   }
   if(k=='*' || k=='#')
      return;
}
// ------------------- VALIDAR CLAVE -------------------
int validar_clave(void)
{
   //Variables locales
   int16 clave=0,m,c,d,u;
   
   m=MIL*1000;
   //c=_mul(CEN,100);            //Alternativa para multiplicar int8*int8=int16
   c=(int16)CEN*100;                   //Convierto centemas, haciendo un CAST
   d=DEC*10;
   u=UNI;
   
   clave=m+c+d+u;
   MIL=0;CEN=0;DEC=0;UNI=0;
   
   if(clave==contra)
      return(1);
   else
      return(0);
}
// ------------------- CAMBIO CLAVE -------------------
void cambio_clave(void)
{
   int bandera=0;
   int16 clave=0,m,c,d,u;
   LCD_PUTC("\f");
   LCD_GOTOXY(1,1);
   LCD_PUTC("CLAVE ACTUAL");
   LCD_GOTOXY(1,2);      
   pedir_clave();
   bandera=validar_clave();
   
   if(bandera==1)
   {
      LCD_PUTC("\f");
      LCD_GOTOXY(1,1);
      LCD_PUTC("CLAVE NUEVA  ");
      LCD_GOTOXY(1,2);   
      teclado();
      if((k!='#')&&(k!='*'))
      {
         lcd_putc('*');
         MIL=VAL;
      }
      teclado();
      if((k!='#')&&(k!='*'))
      {
         lcd_putc('*');
         CEN=VAL;
      }
      teclado();
      if((k!='#')&&(k!='*'))
      {
         lcd_putc('*');
         DEC=VAL;
      }
      teclado();
      if((k!='#')&&(k!='*'))
      {
         lcd_putc('*');
         UNI=VAL;
      }
      
      m=MIL*1000;
      c=CEN*10;
      c=c*10;
      d=DEC*10;
      u=UNI;
      clave=m+c+d+u;
      
      contra=clave;
      WRITE_EEPROM(0,MIL);
      WRITE_EEPROM(1,CEN);
      WRITE_EEPROM(2,DEC);
      WRITE_EEPROM(3,UNI);
      WRITE_EEPROM(4,12);
   }
   else
   {
      LCD_PUTC("\f");
      LCD_GOTOXY(1,2);
      LCD_PUTC(" CLAVE INVALIDA ");
      BIT_SET(LED2);
      DELAY_MS(4000);
      BIT_CLEAR(LED2);
      LCD_PUTC("\f");
   }
}
// ------------------- OBTENER TEMPERATURA -------------------
void tomar_temp(void)
{
   //while(i>=5)
   //{
      set_adc_channel(0);
      delay_ms(1);
      bits=read_adc();
      tem=bits*0.4882;
      tem=((int)(tem * 100 + .5) / 100.0);
      i+=1;
   //}
}
// ------------------- MAIN -------------------
void main()
{
   int16 m,c,d,u;
   INT8 m1,c1,d1,u1;

   set_tris_a(0b00000001);
   setup_adc_ports(sAN0);
   setup_adc(adc_clock_internal);
   
   SET_TRIS_C(0B00000000);
   PORTC=0;
   lcd_init();
   kbd_init();
   //Configura las Resistencias PULL-UP del Puerto B
   port_b_pullups(0xFF);                     //PIC16F887
   //port_b_pullups(TRUE);                     //PIC16F877A
   
   sinal=READ_EEPROM(4);
   
   if(sinal!=12)
      contra=9999;
   else
   {
      m1=READ_EEPROM(0);
      c1=READ_EEPROM(1); 
      d1=READ_EEPROM(2); 
      u1=READ_EEPROM(3); 
      
      m=m1*1000;
      //c=_mul(c1,100);          //Alternativa para multiplicar int8*int8=int16
      c=(int16)c1*100;                 //Convierto centemas, haciendo un CAST
      d=d1*10;
      u=u1;
      contra=m+c+d+u;
   }
   
   while(TRUE)
   {
      LCD_PUTC("\f");
      LCD_GOTOXY(1,1);
      LCD_PUTC(" DIGITE CLAVE ");
      LCD_GOTOXY(1,2);
      DELAY_MS(100);
      pedir_clave();
      
      if(k=='*')
         cambio_clave();
      else
      {
         while((k!='#'))
         {
            TECLADO();
            if(k=='*')
            cambio_clave();
         }
         if(validar_clave())
         {
            LCD_PUTC("\f");
            LCD_GOTOXY(1,1);
            LCD_PUTC(" CLAVE CORRECTA ");
            BIT_SET(LED1);
            DELAY_MS(200);
            BIT_CLEAR(LED1);
            while(TRUE)
            {
               LCD_PUTC("\f");
               LCD_GOTOXY(1,1);
               LCD_PUTC(" INGRESE SU OPC ");
               teclado();
               int opc=VAL;
               LCD_GOTOXY(1,2);
               printf(lcd_putc,"%i", opc);
               delay_ms(200);
               switch(opc)
               {
                  case 1:
                     putc(opc);
                     opc=null;

                     enable_interrupts(INT_RDA);
                     enable_interrupts(GLOBAL);
                     
                     lcd_putc("\f");
                     for(;;)
                     {
                        lcd_gotoxy(1,1);
                        lcd_putc(" OPCION 1 ");
                        lcd_gotoxy(1,2);
                        //printf(lcd_putc,"Recibido= %1D",valor);
                     }
                     /*for(valor=0;valor<=10;valor++)
                     {
                        putc(valor); 
                        printf(lcd_putc,"\fEnviando= %1D",valor);
                        delay_ms(200);
                     }*/
                     lcd_putc("\f");
                     break;
                  case 2:
                     putc(opc);
                     opc=null;
                     
                     while(opc==null)
                     {
                        tomar_temp();
                        lcd_gotoxy(1,1);
                        lcd_putc("\fTEMPERATURA");
                        lcd_gotoxy(2,2);
                        printf(lcd_putc,"C= %f    ",tem);
                        //tem=roundf(tem * 100) / 100;
                        tem=tem*100;
                        putc(tem);
                        teclado();
                        opc=VAL;
                        putc(opc);
                     }
                     lcd_putc("\f");
                     break;

                  case 3:
                     putc(opc);
                     opc=null;
                     
                     while(opc==null)
                     {
                        tomar_temp();
                        i=0;
                        lcd_gotoxy(1,1);
                        lcd_putc("\fTEMPERATURA");
                        lcd_gotoxy(2,2);
                        printf(lcd_putc,"C= %f    ",tem);
                        putc(tem);
                        delay_ms(200);
                     }
                     lcd_putc("\f");
                     break;
               }
            }
         }
         else
         {
            LCD_PUTC("\f");
            LCD_GOTOXY(1,1);
            LCD_PUTC(" CLAVE INVALIDA ");
            BIT_SET(LED2);
            DELAY_MS(500);
            BIT_CLEAR(LED2);
         }
      }
   }
}
