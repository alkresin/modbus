/*
 *  Set of functions for communication port handling under Win32
 */
#include <stdio.h>
#include <windows.h>

long int  dwError;
unsigned char bInput;

HANDLE phCom[4] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };
char pcName[4][20];
int piParity[4] = { -1, -1, -1, -1 };
int piSpeed[4] = { -1, -1, -1, -1 };
int piByteSize[4] = { -1, -1, -1, -1 };
int piStopBits[4] = { -1, -1, -1, -1 };
unsigned int uiPortNum = 0;

int commOpen( const char * portName, int iSpeed, int nByteSize, int nParity, int nStopBits )
{
   DCB dcb;
   COMMTIMEOUTS comtime;

   dwError = 0;
   phCom[uiPortNum] = CreateFile( portName, /* "COM1", */
       GENERIC_READ | GENERIC_WRITE,
       0,    /* comm devices must be opened w/exclusive-access */
       NULL, /* no security attrs */
       OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
       0,    /* not overlapped I/O */
       NULL  /* hTemplate must be NULL for comm devices */
       );

   if( phCom[uiPortNum] == INVALID_HANDLE_VALUE )
   {
      dwError = GetLastError();
      return 0;
   }

   if( !GetCommState(phCom[uiPortNum], &dcb) )
   {
      dwError = GetLastError();
      return 0;
   }

   dcb.BaudRate = iSpeed;
   dcb.ByteSize = nByteSize;
   dcb.Parity = nParity;               // EVENPARITY, MARKPARITY, NOPARITY, ODDPARITY, SPACEPARITY
   dcb.StopBits = nStopBits;           // ONESTOPBIT - 0, ONE5STOPBIT - 1, TWOSTOPBITS - 2
   dcb.fRtsControl = RTS_CONTROL_ENABLE; // RTS_CONTROL_TOGGLE;
   dcb.fDtrControl = DTR_CONTROL_DISABLE;

   if( !SetCommState(phCom[uiPortNum], &dcb) )
   {
      dwError = GetLastError();
      return 0;
   }

   comtime.ReadIntervalTimeout         = MAXDWORD;
   comtime.ReadTotalTimeoutMultiplier  = MAXDWORD;
   comtime.ReadTotalTimeoutConstant    = 200;
   comtime.WriteTotalTimeoutMultiplier = 0;
   comtime.WriteTotalTimeoutConstant   = 0;

   if( !SetCommTimeouts(phCom[uiPortNum],&comtime) )
   {
      dwError = GetLastError();
      return 0;
   }

   memcpy( (void*)(pcName[uiPortNum]), (void*)portName, strlen(portName) );
   pcName[uiPortNum][strlen(portName)] = '\0';
   piSpeed[uiPortNum] = iSpeed;
   piByteSize[uiPortNum] = nByteSize;
   piParity[uiPortNum] = nParity;
   piStopBits[uiPortNum] = nStopBits;

   return 1;
}

void commSetParity( void )
{

   if( piParity[uiPortNum] != ODDPARITY )
   {
      CloseHandle( phCom[uiPortNum] );
      commOpen( pcName[uiPortNum], piSpeed[uiPortNum], piByteSize[uiPortNum], ODDPARITY, piStopBits[uiPortNum] );
   }

   return;
}

void commClearParity( void )
{

   if( piParity[uiPortNum] != NOPARITY )
   {
      CloseHandle( phCom[uiPortNum] );
      commOpen( pcName[uiPortNum], piSpeed[uiPortNum], piByteSize[uiPortNum], NOPARITY, piStopBits[uiPortNum] );
   }

   return;
}

void commClose( void )
{
   if( phCom[uiPortNum] != INVALID_HANDLE_VALUE )
      CloseHandle( phCom[uiPortNum] );
}

int commRead( void )
{
   long int BytesRead = 0;

   dwError = 0;
   if( !ReadFile( phCom[uiPortNum], &bInput, 1, (LPDWORD)(&BytesRead), NULL ) )
   {
      dwError = GetLastError();
      return 0;
   }
   return( BytesRead>0 )? 1:0;
}

int commGet( void )
{
   return (int)bInput;
}

int commWrite( unsigned char b )
{
   long int   BytesToWrite = 1;
   long int   BytesWritten;

   if( !WriteFile( phCom[uiPortNum], &b, BytesToWrite, (LPDWORD)(&BytesWritten), NULL ) )
   {
      dwError = GetLastError();
      return 0;
   }
   return 1;
}

long int commGetErr( void )
{
   return dwError;
}

void setrts( void )
{
   EscapeCommFunction( phCom[uiPortNum],SETRTS );
}

void clrrts( void )
{
   EscapeCommFunction( phCom[uiPortNum],CLRRTS );
}

void uc_setcommnum( unsigned int uiNum )
{

   if( uiNum > 0 && uiNum < 5 )
   {
      uiPortNum = uiNum - 1;
   }
}

void ms_sleep( long int milliseconds )
{
   Sleep( milliseconds );
}