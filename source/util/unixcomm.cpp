/*
 *  Set of functions for communication port handling under Unix
 */

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/select.h>

#define INVALID_HANDLE_VALUE  -1

int phCom[4] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };
int piParity[4] = { -1, -1, -1, -1 };
unsigned int uiPortNum = 0;
int nError;
unsigned char bInput;

int iSpeedDef = 9600;
int nByteSizeDef = 8;
int nParityDef = 0;
int nStopBitsDef = 1;

int commOpen( const char * portName, int iSpeed, int nByteSize, int nParity, int nStopBits )
{
   struct termios options;
   int baud = (iSpeed==9600)? B9600 : ( (iSpeed==4800)? B4800 : B19200 );

   phCom[uiPortNum] = open( portName, O_RDWR | O_NOCTTY | O_NDELAY );
   if( phCom[uiPortNum] == -1 )
      return 0;

   fcntl( phCom[uiPortNum], F_SETFL, 0 );

   tcgetattr( phCom[uiPortNum], &options );

   cfsetispeed( &options, baud );
   cfsetospeed( &options, baud );

   /* Enable the receiver and set local mode... */
   options.c_cflag |= ( CLOCAL | CREAD );

   /* Raw input from device */
   cfmakeraw( &options );

   /* Reset data bits ( cfmakeraw() puts it to CS8 ) */
   options.c_cflag &= ~CSIZE;

   /* Data bits */
   options.c_cflag |= (nByteSize==8)? CS8 : CS7;

   /* Stop bits */
   if( nStopBits == 2 )
      options.c_cflag |= CSTOPB;
   else
      options.c_cflag &= ~CSTOPB;

   /* Parity, only No, Even, Odd supported */
   if( nParity == 0 )
   {
      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~PARODD;
      options.c_iflag &= ~(INPCK);   /* disable input parity checking */
   }
   else
   {
      options.c_cflag |= PARENB;
      if( nParity == 1 )
         options.c_cflag |= PARODD;
   }

   /* UnSet "hardware" flow control */
   options.c_cflag &= ~CRTSCTS;

   /* Every read() call returns as soon as a char is available OR after 3 tenths of a second */
   options.c_cc[ VMIN ] = 0;
   options.c_cc[ VTIME ] = 3;

   // options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
   // options.c_cflag |= IGNPAR;
   // options.c_oflag &= ~OPOST;

   /* Set the new options for the port... */
   tcsetattr( phCom[uiPortNum], TCSAFLUSH, &options );

   return 1;

}

void commSetParity( void )
{
   struct termios options;

   if( piParity[uiPortNum] <= 0 )
   {
      tcgetattr( phCom[uiPortNum], &options );

      options.c_cflag |= PARENB;
      options.c_cflag |= PARODD;

      tcsetattr( phCom[uiPortNum], TCSAFLUSH, &options );
      piParity[uiPortNum] = 1;
   }

   return;
}

void commClearParity( void )
{
   struct termios options;

   if( piParity[uiPortNum] < 0 || piParity[uiPortNum] > 0 )
   {
      tcgetattr( phCom[uiPortNum], &options );

      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~PARODD;
      options.c_iflag &= ~(INPCK);   /* disable input parity checking */

      tcsetattr( phCom[uiPortNum], TCSAFLUSH, &options );
      piParity[uiPortNum] = 0;
   }

   return;
}

void commClose( void )
{
   if( phCom[uiPortNum] != INVALID_HANDLE_VALUE )
      close( phCom[uiPortNum] );
}

int commRead( void )
{

   int nRead = read( phCom[uiPortNum], &bInput, 1 );

   return ( nRead > 0 )? 1 : 0;

}

int commGet( void )
{
   return (int)bInput;
}

int commWrite( unsigned char b )
{
   long nWritten = write( phCom[uiPortNum], &b, 1 );

   return ( nWritten < 0 )? 0 : 1;
}

int commGetErr( void )
{
   return nError;
}

void setrts( void )
{
   int status;

   ioctl( phCom[uiPortNum], TIOCMGET, &status );
   status |= TIOCM_RTS;
   ioctl( phCom[uiPortNum], TIOCMSET, status );

}

void clrrts( void )
{
   int status;

   ioctl( phCom[uiPortNum], TIOCMGET, &status );
   status &= ~TIOCM_RTS;
   ioctl( phCom[uiPortNum], TIOCMSET, status );

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
   struct timeval tv;
   tv.tv_sec = milliseconds / 1000;
   tv.tv_usec = milliseconds % 1000 * 1000;
   select(0, NULL, NULL, NULL, &tv);
}