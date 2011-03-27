#ifndef __common_ok_h__
#define __common_ok_h__

#include <exception>

#include "string.h"
#include "macro.h"

#define OK( RESULT ) \
    { \
        HRESULT h = RESULT; \
		if ( FAILED(h) ) throw std::exception \
			( ( INFO "\n" #RESULT "\nError code: " + common::to_string(h) ).c_str() ); \
    }

#define OK_P( RESULT ) \
        if ( !(RESULT) ) throw std::exception(INFO "\n" #RESULT);

#define OK_EX( RESULT, MESSAGE ) \
    { \
        HRESULT h = RESULT; \
		if ( FAILED(h) ) throw std::exception \
			( ( INFO "\n" #RESULT "\nError code: " + common::to_string(h) + \
			"\n" + std::string(MESSAGE) ).c_str() ); \
    }
#define OK_P_EX( RESULT, MESSAGE ) \
        if ( !(RESULT) ) throw std::exception \
			( ( INFO "\n" #RESULT "\n" + std::string(MESSAGE) ).c_str() );

#endif