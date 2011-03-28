#ifndef __iPtr_h__
#define __iPtr_h__

namespace common {

struct IUnknown;

template <typename Interface>
struct iptr
{  
    iptr()
    : pointer(NULL)
    {}
    
    iptr(const iptr& other)
    : pointer( other.pointer )
    {
        if (pointer) pointer->AddRef();
    }
    
    ~iptr()
    {
        if (pointer) pointer->Release();
    }
    
    const iptr& operator=(const iptr& other)
    {
        if ( pointer != other.pointer )
        {
            if (pointer) pointer->Release();
            pointer = other.pointer;
            if (pointer) pointer->AddRef();
        }
        return *this;
    }   
    
	Interface* self()
	{
		return pointer;
	}

    operator Interface*()
    {
        return pointer;
    }
    
    operator IUnknown*()
    {
        return (IUnknown*)pointer;
    }
    
    Interface** operator&()
    {
        return &pointer;
    }
    
    Interface* operator->()
    {
        return pointer;
    }
    
  private:
    Interface* pointer;
};

} //namespace common

#endif