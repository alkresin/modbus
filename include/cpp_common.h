
#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>

#if defined( __NT__ ) || defined( __WINDOWS_386__ ) || defined( __WINDOWS__ ) || defined( WIN32 ) || defined( _WIN32 )
   #define _OS_WIN_
#endif

#endif