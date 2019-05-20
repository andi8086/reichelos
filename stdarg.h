#ifndef __STDARG_H__
#define __STDARG_H__
	
// without stack protector nonsense :))
// we have no gs:0x14 as canary or something
#define va_list int **
#define va_start(a, f) a = (va_list) &f
#define va_arg(a, t) (t)*(++a)

// #define va_list __builtin_va_list
//#define va_start(ap, param) __builtin_va_start(ap, param)
//#define va_end(ap)          __builtin_va_end(ap)
//#define va_arg(ap, type)    __builtin_va_arg(ap, type)
//#define __va_copy(d,s) __builtin_va_copy(d,s)

//#if __STDC_VERSION__ >= 199901L || __cplusplus >= 201103L || !defined(__STRICT_ANSI__)
//#define va_copy(dest, src)  __builtin_va_copy(dest, src)
//#endif

#endif
