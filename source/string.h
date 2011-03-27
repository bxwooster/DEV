#ifndef __common_string_h__
#define __common_string_h__

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

template <class T> 
inline std::wstring to_wstring(const T& obj)
{
    std::wstringstream stream;
    stream << obj;
    return stream.str();
}

} //namespace common

#endif