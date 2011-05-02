#ifndef __IPtr_h__
#define __IPtr_h__

template <typename Interface>
struct IPtr
{  
    IPtr()
    : pointer(nullptr)
    {}
    
    IPtr(const IPtr& other)
    : pointer( other.pointer )
    {
        if (pointer) pointer->AddRef();
    }
    
    ~IPtr()
    {
        if (pointer) pointer->Release();
    }
    
    const IPtr& operator=(const IPtr& other)
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

#endif