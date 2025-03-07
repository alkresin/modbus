/*
 */

#include "ini.h"
#include "funcs.h"

ini::ini( char * szName ) {

   long ulLen;
   char * pBuffer = cpp_ReadFile( szName, &ulLen ), *ptr, *ptr2, *ptr3;
   std::string pKeySect( "MAIN" );
   std::string pKeyItem, pValue;

   szFileName = szName;
   iUpdated = 0;
   iStatus = 1;
   if( !pBuffer )
      return;

   iStatus = 2;
   ptr = pBuffer;
   do {
      while( *ptr && *ptr <= ' ' ) ptr++;
      if( *ptr == '#' || *ptr == ';' )
         continue;
      else if( *ptr == '[' ) {
         ptr2 = strpbrk( ++ptr, "]\n" );
         if( !ptr2 || *ptr2 == '\n' ) {
            delete pBuffer;
            return;
         }
         pKeySect.assign( ptr, ptr2-ptr );
      } else {
         ptr2 = strpbrk( ptr, "=\n" );
         if( !ptr2 || *ptr2 == '\n' ) {
            delete pBuffer;
            return;
         }
         ptr3 = ptr2 - 1;
         while( *ptr3 == ' ' ) ptr3 --;
         pKeyItem.assign( ptr, ptr3-ptr+1 );
         ptr = ptr2 + 1;
         while( *ptr == ' ' ) ptr ++;
         ptr2 = ptr;
         while( *ptr2 > ' ' ) ptr2 ++;
         pValue.assign( ptr, ptr2-ptr );

         pIni[pKeySect][pKeyItem] = pValue;
      }
   } while( (ptr = strchr( ptr, '\n' )) != NULL );

   iStatus = 0;
   delete pBuffer;
}

ini::~ini() {
}

int ini::hasSection( std::string sSection ) {

   return pIni.count( sSection );
}

int ini::hasItem( std::string sSection, std::string sItemKey ) {

   if( pIni.count( sSection ) == 0 )
      return 0;
   return pIni[sSection].count( sItemKey );
}

std::string ini::getValue( std::string sSection, std::string sItemKey ) {

   if( pIni.count( sSection ) == 0 || pIni[sSection].count( sItemKey ) == 0 )
      return std::string();
   return pIni[sSection][sItemKey];
}

void ini::setValue( std::string sSection, std::string sItemKey, std::string sValue ) {

   pIni[sSection][sItemKey] = sValue;
   iUpdated = 1;
}

void ini::save( char * szName ) {

   std::stringstream ss;
   for (const auto& outerPair : pIni) {
      ss << std::endl << "[" << outerPair.first << "]" << std::endl;
      for (const auto& innerPair : outerPair.second)
         ss << innerPair.first << "=" << innerPair.second << std::endl;
   }
   std::string s(ss.str());

   if( szName )
      cpp_WriteFile( (const char*)szName, s.c_str(), 0 );
   else
      cpp_WriteFile( szFileName.c_str(), s.c_str(), 0 );
}

std::unique_ptr<std::vector<std::string>> ini::getList( void ) {

   int iCount = pIni.size(), i = 0;
   auto v = std::make_unique<std::vector<std::string>>( iCount );

   if( iCount > 0 ) {
      for (const auto& p : pIni ) {
         v->at(i) = p.first;
         i ++;
      }
   }
   return v;
}

std::unique_ptr<std::vector<std::string>> ini::getList( std::string sSection ) {

   if( pIni.count( sSection ) > 0 ) {
      auto pSection = pIni[sSection];
      int iCount = pSection.size(), i = 0;
      auto v = std::make_unique<std::vector<std::string>>( iCount );

      if( iCount > 0 ) {
         for (const auto& p : pSection ) {
            v->at(i) = p.first;
            i ++;
         }
      }
      return v;
   } else
      return std::make_unique<std::vector<std::string>>();
}