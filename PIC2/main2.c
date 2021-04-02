#include <16f887.h>
#fuses xt,noprotect,nowdt,nobrownout,noput,nolvp
#use delay(clock=4000000,crystal)
#use rs232(baud=57600,xmit=pin_c6,rcv=pin_c7,bits=8)
