#pragma once
#include <memory>

template <typename Type, typename Deleter = std::default_delete<Type>>
class owned_ptr
{  
	Type* pointer;

public:
    owned_ptr() : pointer(nullptr) {}

	~owned_ptr()
    {
		Deleter del;
        del(pointer);
    }
    
    owned_ptr(void* uncast)
    {
		pointer = (Type*)uncast;
    }

    void operator=(void* uncast)
    {
        Deleter del;
		del(pointer);
		pointer = (Type*)uncast;
    }   
    
    operator Type*()
    {
        return pointer;
    }

	Type* operator->()
    {
        return pointer;
    }
    
    Type* const* operator &()
    {
        return &pointer;
    }

	// const-doubles
	operator Type const*() const
    {
        return pointer;
    }

	Type const* operator->() const
    {
        return pointer;
    }
    
    Type const* const* operator &() const
    {
        return &pointer;
    }
};
