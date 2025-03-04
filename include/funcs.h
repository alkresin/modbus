/*
 */
//#include <stdio.h>
//#include <stdlib.h>
#include <cstddef>

extern char * cpp_ReadFile( char * szName, long * pLen = NULL );
extern void cpp_WriteFile( const char * szName, const char * szData, unsigned long ulLen = 0 );