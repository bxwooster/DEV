#ifndef __IPtr_h__
#define __IPtr_h__

template <typename Interface>
class IPtr
{  
	Interface* pointer;

public:
    IPtr() : pointer(nullptr) {}

	~IPtr()
    {
        if (pointer) pointer->Release();
    }
    
    IPtr(const IPtr& other)
    : pointer( other.pointer )
    {
        if (pointer) pointer->AddRef();
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
    
    operator Interface*()
    {
        return pointer;
    }

	Interface* operator->()
    {
        return pointer;
    }
    
    Interface* const* operator &()
    {
        return &pointer;
    }

	// For intrusive operations
	Interface** operator ~()
    {
		if (pointer) 
		{
			pointer->Release();
			pointer = nullptr;
		}
        return &pointer;
    }
};

#endif