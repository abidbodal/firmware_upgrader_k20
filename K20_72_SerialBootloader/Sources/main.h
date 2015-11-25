#ifndef MAIN_H
#define MAIN_H

//#include "MK60D10.h"
#include "MK20D7.h"
#include "crc.h"


// typedef definitions
typedef unsigned char Byte;
typedef unsigned long LWord;
typedef unsigned short Word;


typedef union
{
  unsigned all;
  
  struct
  {
    unsigned b25ms  :1;
    unsigned b50ms  : 1;
    unsigned b100ms : 1;
    unsigned b200ms : 1;
    unsigned b400ms : 1;
    unsigned b800ms : 1;
    unsigned b1600ms : 1;    
    unsigned b3200ms : 1;
  }bits;
  
}TIME_FLAGS;



// global variables externs
extern TIME_FLAGS time;
//extern LWord time100sm_cnt;

#endif // MAIN_H