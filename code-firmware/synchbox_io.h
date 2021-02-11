// Attention Circuits Control laboratory - I/O SynchBox project
// I/O wrapper routines
// Written by Christopher Thomas.
// Copyright (c) 2021 by Vanderbilt University. This work is released under
// the Creative Commons Attribution-ShareAlike 4.0 International License.

//
// Macros

// These use Arduino library functionality by default, and neuravr library
// functionality if USE_NEURAVR is defined.

#ifdef USE_NEURAVR


#define InitSerialLink(X,Y) UART_Init(X,Y)


#define GetNextLine() UART_GetNextLine()
#define DoneWithLine() UART_DoneWithLine()


#define PrintChar(X) UART_PrintChar(X)

#define PrintConstString(X) UART_QueueSend_P(PSTR(X))
#define PrintString(X) UART_QueueSend(X)

#define PrintUInt(X) UART_PrintUInt(X)
#define PrintSInt(X) UART_PrintSInt(X)

#define PrintHex8(X) UART_PrintHex8(X)
#define PrintHex16(X) UART_PrintHex16(X)
#define PrintHex32(X) UART_PrintHex32(X)


#else


#define InitSerialLink(X,Y) Arduino_SerialInit(Y)


#define GetNextLine() Arduino_GetNextLine()
#define DoneWithLine() Arduino_DoneWithLine()


#define PrintChar(X) Serial.print(X)

#define PrintConstString(X) Serial.print(X)
#define PrintString(X) Serial.print(X)

#define PrintUInt(X) Serial.print(X)
#define PrintSInt(X) Serial.print(X)

#define PrintHex8(X) Arduino_PrintHex(X, 8)
#define PrintHex16(X) Arduino_PrintHex(X, 16)
#define PrintHex32(X) Arduino_PrintHex(X, 32)


#endif


//
// Functions

#ifdef USE_NEURAVR

#else

void Arduino_SerialInit(uint32_t baud);

char *Arduino_GetNextLine(void);
void Arduino_DoneWithLine(void);

void Arduino_PrintHex(uint32_t value, uint8_t width);

#endif


//
// This is the end of the file.
