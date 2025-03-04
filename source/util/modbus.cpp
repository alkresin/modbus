/*
 */

#include "ini.h"
#include <iostream>
#include <algorithm>
#include <chrono>

#define  READ_TIMEOUT  1500

extern int commOpen( const char * portName, int iSpeed, int nByteSize, int nParity, int nStopBits );
extern void commClose( void );
extern void setrts( void );
extern void clrrts( void );
extern unsigned int fCRC( unsigned char binput, unsigned int uiCRC );
extern int commWrite( unsigned char b );
extern int commRead( void );
extern int commGet( void );
extern void ms_sleep( long int milliseconds );

#if defined( _OS_WIN_ )
static std::string szPort = "COM1";
static std::vector<std::string> vports = { "COM1", "COM2", "COM3","COM4","COM5" };
#else
static std::string szPort = "/dev/ttyS0";
static std::vector<std::string> vports = { "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4" };
#endif
static int iPortSpeed = 9600, iByteSize = 8, iParity = 0, iStopBits = 2;
static int bPortOpened = 0;

void wrcmd( std::string cmd ) {

   cmd.erase( remove(cmd.begin(), cmd.end(), ' '), cmd.end() );
   int iLen = cmd.size(), i1, i2;
   unsigned int iCRC = 65535;

   if( iLen%2 == 1 ) {
      std::cout << "Wrong command" << std::endl;
      return;
   }
   setrts();
   for( int i = 0; i < iLen; i+=2 ) {
      i1 = (int)cmd[i];
         //std::cout << i1 << '/';
      if( i1 > 96 )
         i1 -= 32;
      if( i1 < 48 || i1 > 70 ) {
         std::cout << "Wrong command" << std::endl;
         return;
      }
      i1 = ( (i1 > 64)? i1 - 55 : i1 - 48 ) * 16;
         //std::cout << i1 << '/';
      i2 = (int)cmd[i+1];
      if( i2 > 96 )
         i2 -= 32;
      if( i2 < 48 || i2 > 70 ) {
         std::cout << "Wrong command" << std::endl;
         return;
      }
      i1 = i1 + ( (i2 > 64)? i2 - 55 : i2 - 48 );
      std::cout << i1 << ' ';
      commWrite( (unsigned char) i1 );
      iCRC = fCRC( (unsigned char)i1, iCRC );
   }
   i1 = (iCRC & 0xff00) >> 8;
   std::cout << i1 << ' ';
   commWrite( (unsigned char) i1 );
   i1 = iCRC & 0xff;
   std::cout << i1 << ' ' << std::endl;
   commWrite( (unsigned char) i1 );
   ms_sleep( 30 );

   clrrts();
}

std::vector<int> readanswer( void ) {

   int iByte;
   std::vector<int> v;
   auto t_start = std::chrono::high_resolution_clock::now();

   while( 1 ) {
      if( commRead() ) {
         iByte = commGet();
         std::cout << iByte << ' ';
         v.push_back(iByte);
      } else {
         if( v.size() > 0 && std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > 100 ) {
            break;
         } else if( std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-t_start).count() > READ_TIMEOUT ) {
            std::cout << "No answer!";
            break;
         }
      }
   }

   std::cout << std::endl;
   return v;
}

void readini( char * szName ) {
   ini oIni( szName );

   if( oIni.hasItem( "MAIN", "port" ) )
      szPort = oIni.getValue( "MAIN", "port" );
   if( oIni.hasItem( "MAIN", "speed" ) )
      iPortSpeed = std::stoi( oIni.getValue( "MAIN", "speed" ) );
   if( oIni.hasItem( "MAIN", "bytesize" ) )
      iByteSize = std::stoi( oIni.getValue( "MAIN", "bytesize" ) );
   if( oIni.hasItem( "MAIN", "parity" ) )
      iParity = std::stoi( oIni.getValue( "MAIN", "parity" ) );
   if( oIni.hasItem( "MAIN", "stopbits" ) )
      iStopBits = std::stoi( oIni.getValue( "MAIN", "stopbits" ) );
   if( oIni.hasItem( "MAIN", "portslist" ) ) {
      std::string tmp;
      std::stringstream ss( oIni.getValue( "MAIN", "portslist" ) );
      vports.clear();
      while(getline(ss, tmp, ' ')){
          vports.push_back(tmp);
      }
   }
}

int fmenu( void ) {

   std::string szScr, sInput;

   std::cout << "1.Port 2.Speed 3.Bytesize 4.Parity 5.Stopbits 6.Exit" << std::endl;
   getline( std::cin, sInput );
   if( sInput.empty() )
      return 0;
   else if( sInput == "1" ) {
      for( int i = 0; i < (int)(vports.size()); i++ )
         std::cout << i+1 << "." << vports[i] << " ";
      std::cout << std::endl;
      getline( std::cin, sInput );
      if( sInput.empty() )
         return 0;
      else {
         int iv = std::stoi( sInput );
         if( iv >= 1 && iv <= (int)(vports.size()) ) {
            szPort = vports[iv-1];
            return 1;
         }
      }
   }
   else if( sInput == "2" ) {
      std::cout << "1.4800 2.9600 3.19200" << std::endl;
      getline( std::cin, sInput );
      if( sInput.empty() )
         return 0;
      else if( sInput >= "1" && sInput <= "3" ) {
         iPortSpeed = (sInput == "1")? 4800 : ( (sInput == "2")? 9600 : 19200 );
         return 1;
      }
   }
   else if( sInput == "3" ) {
      std::cout << "1. 7  2. 8" << std::endl;
      getline( std::cin, sInput );
      if( sInput.empty() )
         return 0;
      else if( sInput >= "1" && sInput <= "2" ) {
         iByteSize = (sInput == "1")? 7 : 8;
         return 1;
      }
   }
   else if( sInput == "4" ) {
      std::cout << "1.no 2.odd 3.even" << std::endl;
      getline( std::cin, sInput );
      if( sInput.empty() )
         return 0;
      else if( sInput >= "1" && sInput <= "3" ) {
         iParity = (sInput == "1")? 0 : ( (sInput == "2")? 1 : 2 );
         return 1;
      }
   }
   else if( sInput == "5" ) {
      std::cout << "1. 1 2. 2" << std::endl;
      getline( std::cin, sInput );
      if( sInput.empty() )
         return 0;
      else if( sInput >= "1" && sInput <= "2" ) {
         iStopBits = (sInput == "1")? 0 : 2;
         return 1;
      }
   } else if( sInput == "6" )
      return 2;

   return 0;
}

int reopen( void ) {

   if( bPortOpened )
      commClose();

   std::cout << "port: " << szPort << " speed: " << iPortSpeed << " bytesize: "
      << iByteSize << " parity: " << iParity << " stopbits: " << iStopBits << std::endl;

   if( commOpen( szPort.c_str(), iPortSpeed, iByteSize, iParity, iStopBits ) ) {
      std::cout << "Port opened" << std::endl;
      bPortOpened = 1;
   }
   else {
      std::cout << "Can not open port" << std::endl;
      bPortOpened = 0;
      return 1;
   }
   return 0;
}

int main( int argc, char ** argv ) {

   std::string szScr, sInput;
   int iChoic;

   readini( (char*)"modbus.ini" );
   if (argc >= 2) {
      szScr = argv[1];
   }

   if( reopen() )
      return 1;

   while( 1 ) {
      std::cout << "Command: ";
      getline( std::cin, sInput );
      if( sInput.empty() ) {
         iChoic = fmenu();
         if( iChoic == 2 )
            break;
         else if( iChoic == 1 )
            reopen();
      }
      else if( sInput == "exit" )
         break;
      else {
         wrcmd( sInput );
         readanswer();
      }
   }

   if( bPortOpened )
      commClose();

   return 0;

}