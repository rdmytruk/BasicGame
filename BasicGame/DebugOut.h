#pragma once
#include <Windows.h>
#include <iostream>
#include <sstream>
// useful debug out macro
// https://stackoverflow.com/a/1149623
#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}