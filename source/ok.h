#ifndef __common_ok_h__
#define __common_ok_h__

#include <exception>
#include <string>
#include <sstream>

namespace common {   

template <class T> 
inline std::string to_string(const T& obj)
{
    std::stringstream stream;
    stream << obj;
    return stream.str();
}

}

#define __QUOTE(x) #x
#define _QUOTE(x) __QUOTE(x)

#define __INFO__ \
	"File \"" __FILE__ "\", line " \
    _QUOTE(__LINE__) ", in " __FUNCTION__

#define OK( RESULT ) \
    { \
        HRESULT h = (RESULT); \
		if ( FAILED(h) ) throw std::exception \
			( ( __INFO__ "\n" #RESULT "\nError code: " + common::to_string(h) ).c_str() ); \
    }

#define OK_P( RESULT ) \
        if ( !(RESULT) ) throw std::exception( __INFO__ "\n" #RESULT);

#define OK_EX( RESULT, MESSAGE ) \
    { \
        HRESULT h = (RESULT); \
		if ( FAILED(h) ) throw std::exception \
			( ( __INFO__ "\n" #RESULT "\nError code: " + common::to_string(h) + \
			"\n" + std::string(MESSAGE) ).c_str() ); \
    }

#define OK_P_EX( RESULT, MESSAGE ) \
        if ( !(RESULT) ) throw std::exception \
			( ( __INFO__ "\n" #RESULT "\n" + std::string(MESSAGE) ).c_str() );

#endif