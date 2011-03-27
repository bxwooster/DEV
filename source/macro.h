#ifndef __common_macro_h__
#define __common_macro_h__

#define __WIDEN(x) L ## x
#define _WIDEN(x) __WIDEN(x)
#define __CONC(x,y) x ## y
#define _CONC(x,y) __CONC(x,y)
#define __QUOTE(x) #x
#define _QUOTE(x) __QUOTE(x)

#define INFO \
	"File \"" __FILE__ "\", line " \
    _QUOTE(__LINE__) ", in " __FUNCTION__

#endif