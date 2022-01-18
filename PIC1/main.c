#include <16f887.h>
#fuses xt,noprotect,nowdt,nobrownout,noput,nolvp
#use delay(clock=4000000,crystal)            //Crystal 4MHz
#use rs232(baud=57600,xmit=pin_c6,rcv=pin_c7,bits=8)
#define use_portb_lcd True                   //Definiciones para colocar el LCD
#define use_portb_kbd True                   //y el teclado en el puerto B
#include <lcd.c>                             //Librer�as del LCD y Teclado
#include <kbd.c> 
#byte PORTD= 7                               //Definiciones
#byte PORTB= 6
#define LED1 PORTD,4
#define LED2 PORTD,7

char K;                                      //Variables Globales
int8 MIL,CEN,DEC,UNI,VAL;
int16 contra=2021;               //Variable entera para almacenar la contrase�a
int8 sinal;                      //Variable para saber si tengo contrase�a
int direction;

void teclado()
{
   k=kbd_getc();                 //Llamar la funci�n del teclado y almacenar
                                 //el valor digitado en una variable tipo
                                 //car�cter. Si no se oprime ninguna tecla el
                                 //teclado retornara el car�cter nulo.*/
   while(k=='\0')    //si no se oprime ninguna tecla sigue llamando al teclado.
   {
      k=kbd_getc();
   }
   if( (k!='\0'))
   {
      if(K=='0')                             //Si K es igual a cero
         VAL=0;                              //Val es igual a cero
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

void pedir_clave(void)                       //Rutina para pedir la clave
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

int validar_clave(void)
{
   //Variables locales
   int16 clave=0,m,c,d,u;
   
   m=MIL*1000;                               //Convierto miles a numero
   //c=_mul(CEN,100);            //Alternativa para multiplicar int8*int8=int16
   c=(int16)CEN*100;                   //Convierto centemas, haciendo un CAST
   d=DEC*10;                                 //Convierto decenas a numero 
   u=UNI;                                    //Convierto unidades a numero 
   
   clave=m+c+d+u;
   MIL=0;CEN=0;DEC=0;UNI=0;      //Borra lo �ltimo que fue digitado en teclado
   
   if(clave==contra) //Si la clave es igual a la contrase�a
      return(1);
   else
      return(0);
}

void cambio_clave(void)
{
   int bandera=0;
   int16 clave=0,m,c,d,u;
   LCD_PUTC("\f");
   LCD_GOTOXY(1,1);
   LCD_PUTC("CLAVE ACTUAL");
   LCD_GOTOXY(1,2);      
   pedir_clave(); //Llama la funcion de pedir la clave
   bandera=validar_clave(); //Compruebo si la clave actual es correcta
   
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
      
      m=MIL*1000;                              //Convierto miles a numero
      c=CEN*10;            //Convierto centemas a numero y lo sumo al anterior
      c=c*10;
      d=DEC*10;            //Convierto decenas a numero y lo sumo al anterior
      u=UNI;               //Convierto unidades a numero y lo sumo al anterior
      clave=m+c+d+u;
      
      contra=clave;
      WRITE_EEPROM(0,MIL); //Guarda en la eemprom posicion cero la nueva contrase�a
      WRITE_EEPROM(1,CEN);
      WRITE_EEPROM(2,DEC);
      WRITE_EEPROM(3,UNI);
      WRITE_EEPROM(4,12); //Guardo un 12 en la posicion 1 de la EEPROM, para decir que tengo
      //una contrase�a guardada
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

void imprimir_direccion(void)
{
   if(direction == 1);
   {
      LCD_PUTC(" Avanza ");
   }
   
   if(direction == 2);
   {
      LCD_PUTC(" Retrocede ");
   }
   
   if(direction == 0);
   {
      LCD_PUTC(" Stop ");   
   }
}

void main()
{
   int16 m,c,d,u;                            //Variables Locales
   INT8 m1,c1,d1,u1;
   int flag = 0;
   int order;
   
   SET_TRIS_C(0B00000000);                   //Puerto C como Salida
   PORTD=0;                                  //Limpia Puerto C
   lcd_init();                               //Inicializa el LCD
   kbd_init();                               //Inicializa el Teclado
   
                              //Configura las Resistencias PULL-UP del Puerto B
   port_b_pullups(0xFF);                     //PIC 16F887
   
   sinal=READ_EEPROM(4);       //Averiguo si tengo una contrase�a guardada o no
   
   if(sinal!=12)
      contra=9999;
   else
   {
      m1=READ_EEPROM(0);                     //Lee los datos del EEPROM
      c1=READ_EEPROM(1); 
      d1=READ_EEPROM(2); 
      u1=READ_EEPROM(3);
      
      m=m1*1000;                             //Convierto miles a numero
      //c=_mul(c1,100);          //Alternativa para multiplicar int8*int8=int16
      c=(int16)c1*100;                 //Convierto centemas, haciendo un CAST
      d=d1*10;                               //Convierto decenas a numero 
      u=u1;                                  //Convierto unidades a numero 
      contra=m+c+d+u;
   }
   
   while(TRUE)
   {
      LCD_PUTC("\f");
      LCD_GOTOXY(1,1);
      LCD_PUTC(" DIGITE CLAVE ");
      LCD_GOTOXY(1,2);
      DELAY_MS(1000);
      pedir_clave();                      //Llama la funcion de pedir la clave
      
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
         if(validar_clave())                 //Validaci�n clave
         {
            flag = 1;
                        
            LCD_PUTC("\f"); //Se borra LCD
            LCD_GOTOXY(1,1);//Se ubica en la posici�n 1,1
            LCD_PUTC(" CLAVE CORRECTA ");
            BIT_SET(LED1);
            
            while(flag == 1)
            {  
               LCD_PUTC(" Encendido y esperando ");
               order = teclado;
               if(order != 0)
               {
                  putc(order);
                  direction = getc();
                  imprimir_direccion();
               }
               else
               {
                  LCD_PUTC(" SISTEMA BLOQUEADO ");
                  flag = 0;
               }
            }  
         }
         else
         {
            LCD_PUTC("\f");
            LCD_GOTOXY(1,2);
            LCD_PUTC(" CLAVE INVALIDA ");
            BIT_SET(LED2);
            DELAY_MS(4000);
            BIT_CLEAR(LED2);
         }
      }
   }
}