/*
 */

#include "cpp_common.h"

class ini {

   std::map<std::string,std::map<std::string,std::string> > pIni;

public:
   std::string szFileName;
   int iUpdated;
   int iStatus;

   ini( char * szName );
   ~ini();

   int hasSection( std::string sSection );
   int hasItem( std::string sSection, std::string sItemKey );
   std::string getValue( std::string sSection, std::string sItemKey );
   void setValue( std::string sSection, std::string sItemKey, std::string sValue );
   std::shared_ptr<std::vector<std::string>> getList( void );
   std::shared_ptr<std::vector<std::string>> getList( std::string sSection );
   void save( char * szName = NULL );
};