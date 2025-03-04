/*
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * cpp_ReadFile( char * szName, long * pLen )
{
   char * pBuffer = NULL;
   FILE *f = fopen( szName, "rb" );
   long lSize;

   if( f )
   {
      fseek( f, 0, SEEK_END );
      lSize = ftell( f );
      fseek(f, 0, SEEK_SET);

      if( lSize )
      {
         pBuffer = new char[lSize + 1];
         fread( pBuffer, lSize, 1, f );
         fclose( f );

         pBuffer[lSize] = 0;
         if( pLen )
            *pLen = lSize;
      }
   }
   return pBuffer;
}

void cpp_WriteFile( const char * szName, const char * szData, unsigned long ulLen )
{
   FILE *f;

   if( !ulLen )
      ulLen = strlen( szData );

   f = fopen( szName, "wb" );
   fwrite( szData, 1, ulLen, f );
   fclose( f );

}