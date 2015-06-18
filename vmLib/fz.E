# 1 "fz.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "fz.c"
# 32 "fz.c"
# 1 "../inc/stdInc.h" 1
# 40 "../inc/stdInc.h"
# 1 "../inc/BuildNr.h" 1
# 41 "../inc/stdInc.h" 2


# 1 "../inc/osFirst.h" 1
# 44 "../inc/stdInc.h" 2



# 1 "../inc/osDef.h" 1
# 772 "../inc/osDef.h"
typedef struct
{




  unsigned long L;
    long H;


} sint64;

typedef struct
{




 unsigned long L;
 unsigned long H;


} uint64;
# 48 "../inc/stdInc.h" 2



# 1 "../inc/osTarget.h" 1
# 46 "../inc/osTarget.h"
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 1 3 4
# 26 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/features.h" 1 3 4
# 323 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/features.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/cdefs.h" 1 3 4
# 313 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/cdefs.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/wordsize.h" 1 3 4
# 314 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/cdefs.h" 2 3 4
# 324 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/features.h" 2 3 4
# 346 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/features.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/gnu/stubs.h" 1 3 4
# 347 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/features.h" 2 3 4
# 27 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 2 3 4






# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 214 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 3 4
typedef unsigned int size_t;
# 34 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 2 3 4




extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

# 82 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

# 130 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4
extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 165 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4


extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

# 181 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4



extern size_t strcspn (__const char *__s, __const char *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strpbrk (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strstr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 240 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4


extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

# 254 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4


extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

# 270 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
# 288 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4
extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (__const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *index (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


extern char *rindex (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));



extern int ffs (int __i) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 325 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4
extern int strcasecmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 348 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4
extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
# 426 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/string.h" 3 4

# 47 "../inc/osTarget.h" 2
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 1 3 4
# 33 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 326 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 3 4
typedef unsigned int wchar_t;
# 34 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 2 3 4


# 96 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;



# 140 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__)) ;




extern double atof (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





__extension__ extern long long int atoll (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





extern double strtod (__const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

# 182 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4


extern long int strtol (__const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern unsigned long int strtoul (__const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




__extension__
extern long long int strtoq (__const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtouq (__const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





__extension__
extern long long int strtoll (__const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtoull (__const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

# 279 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern double __strtod_internal (__const char *__restrict __nptr,
     char **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
extern float __strtof_internal (__const char *__restrict __nptr,
    char **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
extern long double __strtold_internal (__const char *__restrict __nptr,
           char **__restrict __endptr,
           int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern long int __strtol_internal (__const char *__restrict __nptr,
       char **__restrict __endptr,
       int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern unsigned long int __strtoul_internal (__const char *__restrict __nptr,
          char **__restrict __endptr,
          int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




__extension__
extern long long int __strtoll_internal (__const char *__restrict __nptr,
      char **__restrict __endptr,
      int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



__extension__
extern unsigned long long int __strtoull_internal (__const char *
         __restrict __nptr,
         char **__restrict __endptr,
         int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 429 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__)) ;


extern long int a64l (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;




# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 1 3 4
# 29 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/types.h" 1 3 4
# 28 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/types.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/wordsize.h" 1 3 4
# 29 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/types.h" 2 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 32 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;




__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;







__extension__ typedef long long int __quad_t;
__extension__ typedef unsigned long long int __u_quad_t;
# 134 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/types.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/typesizes.h" 1 3 4
# 135 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/types.h" 2 3 4


__extension__ typedef __u_quad_t __dev_t;
__extension__ typedef unsigned int __uid_t;
__extension__ typedef unsigned int __gid_t;
__extension__ typedef unsigned long int __ino_t;
__extension__ typedef __u_quad_t __ino64_t;
__extension__ typedef unsigned int __mode_t;
__extension__ typedef unsigned int __nlink_t;
__extension__ typedef long int __off_t;
__extension__ typedef __quad_t __off64_t;
__extension__ typedef int __pid_t;
__extension__ typedef struct { int __val[2]; } __fsid_t;
__extension__ typedef long int __clock_t;
__extension__ typedef unsigned long int __rlim_t;
__extension__ typedef __u_quad_t __rlim64_t;
__extension__ typedef unsigned int __id_t;
__extension__ typedef long int __time_t;
__extension__ typedef unsigned int __useconds_t;
__extension__ typedef long int __suseconds_t;

__extension__ typedef int __daddr_t;
__extension__ typedef long int __swblk_t;
__extension__ typedef int __key_t;


__extension__ typedef int __clockid_t;


__extension__ typedef void * __timer_t;


__extension__ typedef long int __blksize_t;




__extension__ typedef long int __blkcnt_t;
__extension__ typedef __quad_t __blkcnt64_t;


__extension__ typedef unsigned long int __fsblkcnt_t;
__extension__ typedef __u_quad_t __fsblkcnt64_t;


__extension__ typedef unsigned long int __fsfilcnt_t;
__extension__ typedef __u_quad_t __fsfilcnt64_t;

__extension__ typedef int __ssize_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


__extension__ typedef int __intptr_t;


__extension__ typedef unsigned int __socklen_t;
# 32 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 2 3 4



typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;
# 62 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;
# 100 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
typedef __pid_t pid_t;




typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 133 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 1 3 4
# 75 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4


typedef __time_t time_t;



# 93 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 105 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 134 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 2 3 4
# 147 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 148 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
# 195 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 217 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/endian.h" 1 3 4
# 37 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/endian.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/endian.h" 1 3 4
# 38 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/endian.h" 2 3 4
# 218 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 2 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 1 3 4
# 31 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/select.h" 1 3 4
# 32 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 2 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sigset.h" 1 3 4
# 23 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 35 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 1 3 4
# 121 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
# 45 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 2 3 4

# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/time.h" 1 3 4
# 69 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
# 47 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 2 3 4


typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
# 67 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
# 99 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 3 4

# 109 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 121 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);



# 221 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 2 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/sysmacros.h" 1 3 4
# 29 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/sysmacros.h" 3 4
__extension__
extern __inline unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern __inline unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern __inline unsigned long long int gnu_dev_makedev (unsigned int __major,
       unsigned int __minor)
     __attribute__ ((__nothrow__));


__extension__ extern __inline unsigned int
__attribute__ ((__nothrow__)) gnu_dev_major (unsigned long long int __dev)
{
  return ((__dev >> 8) & 0xfff) | ((unsigned int) (__dev >> 32) & ~0xfff);
}

__extension__ extern __inline unsigned int
__attribute__ ((__nothrow__)) gnu_dev_minor (unsigned long long int __dev)
{
  return (__dev & 0xff) | ((unsigned int) (__dev >> 12) & ~0xff);
}

__extension__ extern __inline unsigned long long int
__attribute__ ((__nothrow__)) gnu_dev_makedev (unsigned int __major, unsigned int __minor)
{
  return ((__minor & 0xff) | ((__major & 0xfff) << 8)
   | (((unsigned long long int) (__minor & ~0xff)) << 12)
   | (((unsigned long long int) (__major & ~0xfff)) << 32));
}
# 224 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 2 3 4
# 235 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 270 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/pthreadtypes.h" 1 3 4
# 36 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/pthreadtypes.h" 3 4
typedef unsigned long int pthread_t;


typedef union
{
  char __size[36];
  long int __align;
} pthread_attr_t;


typedef struct __pthread_internal_slist
{
  struct __pthread_internal_slist *__next;
} __pthread_slist_t;




typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;


    int __kind;
    unsigned int __nusers;
    __extension__ union
    {
      int __spins;
      __pthread_slist_t __list;
    };
  } __data;
  char __size[24];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[4];
  long int __align;
} pthread_mutexattr_t;




typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[4];
  long int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;





typedef union
{
  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;


    unsigned int __flags;
    int __writer;
  } __data;
  char __size[32];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[20];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 271 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/types.h" 2 3 4



# 439 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 2 3 4






extern long int random (void) __attribute__ ((__nothrow__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern int rand (void) __attribute__ ((__nothrow__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__));




extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__));







extern double drand48 (void) __attribute__ ((__nothrow__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    unsigned long long int __a;
  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));









extern void *malloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;







extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__warn_unused_result__));

extern void free (void *__ptr) __attribute__ ((__nothrow__));




extern void cfree (void *__ptr) __attribute__ ((__nothrow__));



# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/alloca.h" 1 3 4
# 25 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/alloca.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 26 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__));






# 613 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 2 3 4




extern void *valloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern void abort (void) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern void exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));

# 658 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4


extern char *getenv (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern char *__secure_getenv (__const char *__name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern int putenv (char *__string) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (__const char *__name, __const char *__value, int __replace)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (__const char *__name) __attribute__ ((__nothrow__));






extern int clearenv (void) __attribute__ ((__nothrow__));
# 698 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 709 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 729 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;








extern int system (__const char *__command) ;

# 757 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern char *realpath (__const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__)) ;






typedef int (*__compar_fn_t) (__const void *, __const void *);









extern void *bsearch (__const void *__key, __const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;



extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));



extern int abs (int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;












extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;

# 822 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));







extern int mblen (__const char *__s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int mbtowc (wchar_t *__restrict __pwc,
     __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__)) ;



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__));

extern size_t wcstombs (char *__restrict __s,
   __const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__));








extern int rpmatch (__const char *__response) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 927 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern int posix_openpt (int __oflag) ;
# 962 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 978 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdlib.h" 3 4

# 48 "../inc/osTarget.h" 2
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 1 3 4
# 30 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4




# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 35 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 2 3 4
# 44 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4


typedef struct _IO_FILE FILE;





# 62 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 72 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 1 3 4
# 32 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/_G_config.h" 1 3 4
# 14 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/_G_config.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 355 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 3 4
typedef unsigned int wint_t;
# 15 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/_G_config.h" 2 3 4
# 24 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/_G_config.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 1 3 4
# 48 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 49 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 2 3 4

# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/wchar.h" 1 3 4
# 51 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 2 3 4
# 76 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
# 25 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/_G_config.h" 2 3 4

typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 44 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/_G_config.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/gconv.h" 1 3 4
# 28 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/gconv.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 1 3 4
# 48 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 49 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/wchar.h" 2 3 4
# 29 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/gconv.h" 2 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 32 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/gconv.h" 2 3 4





enum
{
  __GCONV_OK = 0,
  __GCONV_NOCONV,
  __GCONV_NODB,
  __GCONV_NOMEM,

  __GCONV_EMPTY_INPUT,
  __GCONV_FULL_OUTPUT,
  __GCONV_ILLEGAL_INPUT,
  __GCONV_INCOMPLETE_INPUT,

  __GCONV_ILLEGAL_DESCRIPTOR,
  __GCONV_INTERNAL_ERROR
};



enum
{
  __GCONV_IS_LAST = 0x0001,
  __GCONV_IGNORE_ERRORS = 0x0002
};



struct __gconv_step;
struct __gconv_step_data;
struct __gconv_loaded_object;
struct __gconv_trans_data;



typedef int (*__gconv_fct) (struct __gconv_step *, struct __gconv_step_data *,
       __const unsigned char **, __const unsigned char *,
       unsigned char **, size_t *, int, int);


typedef wint_t (*__gconv_btowc_fct) (struct __gconv_step *, unsigned char);


typedef int (*__gconv_init_fct) (struct __gconv_step *);
typedef void (*__gconv_end_fct) (struct __gconv_step *);



typedef int (*__gconv_trans_fct) (struct __gconv_step *,
      struct __gconv_step_data *, void *,
      __const unsigned char *,
      __const unsigned char **,
      __const unsigned char *, unsigned char **,
      size_t *);


typedef int (*__gconv_trans_context_fct) (void *, __const unsigned char *,
       __const unsigned char *,
       unsigned char *, unsigned char *);


typedef int (*__gconv_trans_query_fct) (__const char *, __const char ***,
     size_t *);


typedef int (*__gconv_trans_init_fct) (void **, const char *);
typedef void (*__gconv_trans_end_fct) (void *);

struct __gconv_trans_data
{

  __gconv_trans_fct __trans_fct;
  __gconv_trans_context_fct __trans_context_fct;
  __gconv_trans_end_fct __trans_end_fct;
  void *__data;
  struct __gconv_trans_data *__next;
};



struct __gconv_step
{
  struct __gconv_loaded_object *__shlib_handle;
  __const char *__modname;

  int __counter;

  char *__from_name;
  char *__to_name;

  __gconv_fct __fct;
  __gconv_btowc_fct __btowc_fct;
  __gconv_init_fct __init_fct;
  __gconv_end_fct __end_fct;



  int __min_needed_from;
  int __max_needed_from;
  int __min_needed_to;
  int __max_needed_to;


  int __stateful;

  void *__data;
};



struct __gconv_step_data
{
  unsigned char *__outbuf;
  unsigned char *__outbufend;



  int __flags;



  int __invocation_counter;



  int __internal_use;

  __mbstate_t *__statep;
  __mbstate_t __state;



  struct __gconv_trans_data *__trans;
};



typedef struct __gconv_info
{
  size_t __nsteps;
  struct __gconv_step *__steps;
  __extension__ struct __gconv_step_data __data [];
} *__gconv_t;
# 45 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/_G_config.h" 2 3 4
typedef union
{
  struct __gconv_info __cd;
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;

typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
# 33 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 2 3 4
# 53 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stdarg.h" 1 3 4
# 43 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 54 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 2 3 4
# 166 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 176 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 199 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 267 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;



  int _flags2;

  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 315 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
  __off64_t _offset;
# 324 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;

  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 360 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
# 412 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
extern int __underflow (_IO_FILE *) __attribute__ ((__nothrow__));
extern int __uflow (_IO_FILE *) __attribute__ ((__nothrow__));
extern int __overflow (_IO_FILE *, int) __attribute__ ((__nothrow__));
extern wint_t __wunderflow (_IO_FILE *) __attribute__ ((__nothrow__));
extern wint_t __wuflow (_IO_FILE *) __attribute__ ((__nothrow__));
extern wint_t __woverflow (_IO_FILE *, wint_t) __attribute__ ((__nothrow__));
# 450 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_putc (int __c, _IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));

extern int _IO_peekc_locked (_IO_FILE *__fp) __attribute__ ((__nothrow__));





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));
# 480 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t) __attribute__ ((__nothrow__));
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t) __attribute__ ((__nothrow__));

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int) __attribute__ ((__nothrow__));
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int) __attribute__ ((__nothrow__));

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));
# 73 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 2 3 4
# 86 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4


typedef _G_fpos_t fpos_t;




# 138 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/stdio_lim.h" 1 3 4
# 139 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;







extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));














extern FILE *tmpfile (void);
# 183 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__));





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__));
# 201 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern char *tempnam (__const char *__dir, __const char *__pfx)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__));








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 226 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 240 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4






extern FILE *fopen (__const char *__restrict __filename,
      __const char *__restrict __modes);




extern FILE *freopen (__const char *__restrict __filename,
        __const char *__restrict __modes,
        FILE *__restrict __stream);
# 267 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4

# 278 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern FILE *fdopen (int __fd, __const char *__modes) __attribute__ ((__nothrow__));
# 298 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4



extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__));





extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__));








extern int fprintf (FILE *__restrict __stream,
      __const char *__restrict __format, ...);




extern int printf (__const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      __const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       __const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

# 392 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4





extern int fscanf (FILE *__restrict __stream,
     __const char *__restrict __format, ...) ;




extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
     __const char *__restrict __format, ...) __attribute__ ((__nothrow__));

# 434 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4





extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

# 458 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 469 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 502 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;






extern char *gets (char *__s) ;

# 583 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4





extern int fputs (__const char *__restrict __s, FILE *__restrict __stream);





extern int puts (__const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s) ;

# 636 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream) ;








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 672 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 691 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, __const fpos_t *__pos);
# 714 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4

# 723 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;








extern void perror (__const char *__s);






# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sys_errlist.h" 1 3 4
# 27 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern __const char *__const sys_errlist[];
# 753 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
# 772 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern FILE *popen (__const char *__command, __const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__));
# 812 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__));
# 842 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/stdio.h" 3 4

# 49 "../inc/osTarget.h" 2

# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/errno.h" 1 3 4
# 32 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/errno.h" 3 4




# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/errno.h" 1 3 4
# 25 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/errno.h" 3 4
# 1 "/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include/linux/errno.h" 1 3 4



# 1 "/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include/asm/errno.h" 1 3 4



# 1 "/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include/asm-generic/errno.h" 1 3 4



# 1 "/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include/asm-generic/errno-base.h" 1 3 4
# 5 "/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include/asm-generic/errno.h" 2 3 4
# 5 "/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include/asm/errno.h" 2 3 4
# 5 "/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include/linux/errno.h" 2 3 4
# 26 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/errno.h" 2 3 4
# 43 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/errno.h" 3 4
extern int *__errno_location (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 37 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/errno.h" 2 3 4
# 59 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/errno.h" 3 4

# 51 "../inc/osTarget.h" 2
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 1 3 4
# 23 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sched.h" 1 3 4
# 29 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sched.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 1 3 4
# 30 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sched.h" 2 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sched.h" 1 3 4
# 65 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sched.h" 3 4
struct sched_param
  {
    int __sched_priority;
  };





extern int clone (int (*__fn) (void *__arg), void *__child_stack,
    int __flags, void *__arg, ...) __attribute__ ((__nothrow__));


extern int unshare (int __flags) __attribute__ ((__nothrow__));










struct __sched_param
  {
    int __sched_priority;
  };
# 104 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sched.h" 3 4
typedef unsigned long int __cpu_mask;






typedef struct
{
  __cpu_mask __bits[1024 / (8 * sizeof (__cpu_mask))];
} cpu_set_t;
# 33 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sched.h" 2 3 4







extern int sched_setparam (__pid_t __pid, __const struct sched_param *__param)
     __attribute__ ((__nothrow__));


extern int sched_getparam (__pid_t __pid, struct sched_param *__param) __attribute__ ((__nothrow__));


extern int sched_setscheduler (__pid_t __pid, int __policy,
          __const struct sched_param *__param) __attribute__ ((__nothrow__));


extern int sched_getscheduler (__pid_t __pid) __attribute__ ((__nothrow__));


extern int sched_yield (void) __attribute__ ((__nothrow__));


extern int sched_get_priority_max (int __algorithm) __attribute__ ((__nothrow__));


extern int sched_get_priority_min (int __algorithm) __attribute__ ((__nothrow__));


extern int sched_rr_get_interval (__pid_t __pid, struct timespec *__t) __attribute__ ((__nothrow__));
# 84 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sched.h" 3 4

# 24 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 2 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 1 3 4
# 31 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4








# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 40 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 2 3 4



# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/time.h" 1 3 4
# 44 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 2 3 4
# 59 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4


typedef __clock_t clock_t;



# 132 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4


struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;


  long int tm_gmtoff;
  __const char *tm_zone;




};








struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };


struct sigevent;
# 181 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4



extern clock_t clock (void) __attribute__ ((__nothrow__));


extern time_t time (time_t *__timer) __attribute__ ((__nothrow__));


extern double difftime (time_t __time1, time_t __time0)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));


extern time_t mktime (struct tm *__tp) __attribute__ ((__nothrow__));





extern size_t strftime (char *__restrict __s, size_t __maxsize,
   __const char *__restrict __format,
   __const struct tm *__restrict __tp) __attribute__ ((__nothrow__));

# 229 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4



extern struct tm *gmtime (__const time_t *__timer) __attribute__ ((__nothrow__));



extern struct tm *localtime (__const time_t *__timer) __attribute__ ((__nothrow__));





extern struct tm *gmtime_r (__const time_t *__restrict __timer,
       struct tm *__restrict __tp) __attribute__ ((__nothrow__));



extern struct tm *localtime_r (__const time_t *__restrict __timer,
          struct tm *__restrict __tp) __attribute__ ((__nothrow__));





extern char *asctime (__const struct tm *__tp) __attribute__ ((__nothrow__));


extern char *ctime (__const time_t *__timer) __attribute__ ((__nothrow__));







extern char *asctime_r (__const struct tm *__restrict __tp,
   char *__restrict __buf) __attribute__ ((__nothrow__));


extern char *ctime_r (__const time_t *__restrict __timer,
        char *__restrict __buf) __attribute__ ((__nothrow__));




extern char *__tzname[2];
extern int __daylight;
extern long int __timezone;




extern char *tzname[2];



extern void tzset (void) __attribute__ ((__nothrow__));



extern int daylight;
extern long int timezone;





extern int stime (__const time_t *__when) __attribute__ ((__nothrow__));
# 312 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4
extern time_t timegm (struct tm *__tp) __attribute__ ((__nothrow__));


extern time_t timelocal (struct tm *__tp) __attribute__ ((__nothrow__));


extern int dysize (int __year) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 327 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4
extern int nanosleep (__const struct timespec *__requested_time,
        struct timespec *__remaining);



extern int clock_getres (clockid_t __clock_id, struct timespec *__res) __attribute__ ((__nothrow__));


extern int clock_gettime (clockid_t __clock_id, struct timespec *__tp) __attribute__ ((__nothrow__));


extern int clock_settime (clockid_t __clock_id, __const struct timespec *__tp)
     __attribute__ ((__nothrow__));






extern int clock_nanosleep (clockid_t __clock_id, int __flags,
       __const struct timespec *__req,
       struct timespec *__rem);


extern int clock_getcpuclockid (pid_t __pid, clockid_t *__clock_id) __attribute__ ((__nothrow__));




extern int timer_create (clockid_t __clock_id,
    struct sigevent *__restrict __evp,
    timer_t *__restrict __timerid) __attribute__ ((__nothrow__));


extern int timer_delete (timer_t __timerid) __attribute__ ((__nothrow__));


extern int timer_settime (timer_t __timerid, int __flags,
     __const struct itimerspec *__restrict __value,
     struct itimerspec *__restrict __ovalue) __attribute__ ((__nothrow__));


extern int timer_gettime (timer_t __timerid, struct itimerspec *__value)
     __attribute__ ((__nothrow__));


extern int timer_getoverrun (timer_t __timerid) __attribute__ ((__nothrow__));
# 416 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/time.h" 3 4

# 25 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 2 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/signal.h" 1 3 4
# 31 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/signal.h" 3 4


# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/sigset.h" 1 3 4
# 34 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/signal.h" 2 3 4
# 400 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/signal.h" 3 4

# 28 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 2 3 4

# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/setjmp.h" 1 3 4
# 35 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/setjmp.h" 3 4
typedef int __jmp_buf[64] __attribute__((aligned (8)));
# 30 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 2 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/wordsize.h" 1 3 4
# 31 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 2 3 4



enum
{
  PTHREAD_CREATE_JOINABLE,

  PTHREAD_CREATE_DETACHED

};



enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
# 61 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
};
# 113 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
enum
{
  PTHREAD_RWLOCK_PREFER_READER_NP,
  PTHREAD_RWLOCK_PREFER_WRITER_NP,
  PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
  PTHREAD_RWLOCK_DEFAULT_NP = PTHREAD_RWLOCK_PREFER_READER_NP
};
# 143 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
enum
{
  PTHREAD_INHERIT_SCHED,

  PTHREAD_EXPLICIT_SCHED

};



enum
{
  PTHREAD_SCOPE_SYSTEM,

  PTHREAD_SCOPE_PROCESS

};



enum
{
  PTHREAD_PROCESS_PRIVATE,

  PTHREAD_PROCESS_SHARED

};
# 178 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
struct _pthread_cleanup_buffer
{
  void (*__routine) (void *);
  void *__arg;
  int __canceltype;
  struct _pthread_cleanup_buffer *__prev;
};


enum
{
  PTHREAD_CANCEL_ENABLE,

  PTHREAD_CANCEL_DISABLE

};
enum
{
  PTHREAD_CANCEL_DEFERRED,

  PTHREAD_CANCEL_ASYNCHRONOUS

};
# 216 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4





extern int pthread_create (pthread_t *__restrict __newthread,
      __const pthread_attr_t *__restrict __attr,
      void *(*__start_routine) (void *),
      void *__restrict __arg) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 3)));





extern void pthread_exit (void *__retval) __attribute__ ((__noreturn__));







extern int pthread_join (pthread_t __th, void **__thread_return);
# 259 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_detach (pthread_t __th) __attribute__ ((__nothrow__));



extern pthread_t pthread_self (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));


extern int pthread_equal (pthread_t __thread1, pthread_t __thread2) __attribute__ ((__nothrow__));







extern int pthread_attr_init (pthread_attr_t *__attr) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_attr_destroy (pthread_attr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_attr_getdetachstate (__const pthread_attr_t *__attr,
     int *__detachstate)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_attr_setdetachstate (pthread_attr_t *__attr,
     int __detachstate)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_attr_getguardsize (__const pthread_attr_t *__attr,
          size_t *__guardsize)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_attr_setguardsize (pthread_attr_t *__attr,
          size_t __guardsize)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_attr_getschedparam (__const pthread_attr_t *__restrict
           __attr,
           struct sched_param *__restrict __param)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_attr_setschedparam (pthread_attr_t *__restrict __attr,
           __const struct sched_param *__restrict
           __param) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_attr_getschedpolicy (__const pthread_attr_t *__restrict
     __attr, int *__restrict __policy)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_attr_setschedpolicy (pthread_attr_t *__attr, int __policy)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_attr_getinheritsched (__const pthread_attr_t *__restrict
      __attr, int *__restrict __inherit)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_attr_setinheritsched (pthread_attr_t *__attr,
      int __inherit)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_attr_getscope (__const pthread_attr_t *__restrict __attr,
      int *__restrict __scope)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_attr_setscope (pthread_attr_t *__attr, int __scope)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_attr_getstackaddr (__const pthread_attr_t *__restrict
          __attr, void **__restrict __stackaddr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) __attribute__ ((__deprecated__));





extern int pthread_attr_setstackaddr (pthread_attr_t *__attr,
          void *__stackaddr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__));


extern int pthread_attr_getstacksize (__const pthread_attr_t *__restrict
          __attr, size_t *__restrict __stacksize)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));




extern int pthread_attr_setstacksize (pthread_attr_t *__attr,
          size_t __stacksize)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_attr_getstack (__const pthread_attr_t *__restrict __attr,
      void **__restrict __stackaddr,
      size_t *__restrict __stacksize)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2, 3)));




extern int pthread_attr_setstack (pthread_attr_t *__attr, void *__stackaddr,
      size_t __stacksize) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 409 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_setschedparam (pthread_t __target_thread, int __policy,
      __const struct sched_param *__param)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3)));


extern int pthread_getschedparam (pthread_t __target_thread,
      int *__restrict __policy,
      struct sched_param *__restrict __param)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));


extern int pthread_setschedprio (pthread_t __target_thread, int __prio)
     __attribute__ ((__nothrow__));
# 462 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_once (pthread_once_t *__once_control,
    void (*__init_routine) (void)) __attribute__ ((__nonnull__ (1, 2)));
# 474 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_setcancelstate (int __state, int *__oldstate);



extern int pthread_setcanceltype (int __type, int *__oldtype);


extern int pthread_cancel (pthread_t __th);




extern void pthread_testcancel (void);




typedef struct
{
  struct
  {
    __jmp_buf __cancel_jmp_buf;
    int __mask_was_saved;
  } __cancel_jmp_buf[1];
  void *__pad[4];
} __pthread_unwind_buf_t __attribute__ ((__aligned__));
# 508 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
struct __pthread_cleanup_frame
{
  void (*__cancel_routine) (void *);
  void *__cancel_arg;
  int __do_it;
  int __cancel_type;
};
# 648 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern void __pthread_register_cancel (__pthread_unwind_buf_t *__buf)
     ;
# 659 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern void __pthread_unregister_cancel (__pthread_unwind_buf_t *__buf)
  ;
# 699 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern void __pthread_unwind_next (__pthread_unwind_buf_t *__buf)
     __attribute__ ((__noreturn__))

     __attribute__ ((__weak__))

     ;



struct __jmp_buf_tag;
extern int __sigsetjmp (struct __jmp_buf_tag *__env, int __savemask) __attribute__ ((__nothrow__));





extern int pthread_mutex_init (pthread_mutex_t *__mutex,
          __const pthread_mutexattr_t *__mutexattr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_mutex_destroy (pthread_mutex_t *__mutex)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_mutex_trylock (pthread_mutex_t *__mutex)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_mutex_lock (pthread_mutex_t *__mutex)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_mutex_timedlock (pthread_mutex_t *__restrict __mutex,
                                    __const struct timespec *__restrict
                                    __abstime) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern int pthread_mutex_unlock (pthread_mutex_t *__mutex)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 770 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_mutexattr_init (pthread_mutexattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_mutexattr_getpshared (__const pthread_mutexattr_t *
      __restrict __attr,
      int *__restrict __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,
      int __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 842 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_rwlock_init (pthread_rwlock_t *__restrict __rwlock,
    __const pthread_rwlockattr_t *__restrict
    __attr) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_rwlock_destroy (pthread_rwlock_t *__rwlock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_rwlock_rdlock (pthread_rwlock_t *__rwlock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_rwlock_tryrdlock (pthread_rwlock_t *__rwlock)
  __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_rwlock_timedrdlock (pthread_rwlock_t *__restrict __rwlock,
           __const struct timespec *__restrict
           __abstime) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern int pthread_rwlock_wrlock (pthread_rwlock_t *__rwlock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_rwlock_trywrlock (pthread_rwlock_t *__rwlock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_rwlock_timedwrlock (pthread_rwlock_t *__restrict __rwlock,
           __const struct timespec *__restrict
           __abstime) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern int pthread_rwlock_unlock (pthread_rwlock_t *__rwlock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int pthread_rwlockattr_init (pthread_rwlockattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_rwlockattr_destroy (pthread_rwlockattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_rwlockattr_getpshared (__const pthread_rwlockattr_t *
       __restrict __attr,
       int *__restrict __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_rwlockattr_setpshared (pthread_rwlockattr_t *__attr,
       int __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_rwlockattr_getkind_np (__const pthread_rwlockattr_t *
       __restrict __attr,
       int *__restrict __pref)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_rwlockattr_setkind_np (pthread_rwlockattr_t *__attr,
       int __pref) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));







extern int pthread_cond_init (pthread_cond_t *__restrict __cond,
         __const pthread_condattr_t *__restrict
         __cond_attr) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_cond_destroy (pthread_cond_t *__cond)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_cond_signal (pthread_cond_t *__cond)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_cond_broadcast (pthread_cond_t *__cond)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int pthread_cond_wait (pthread_cond_t *__restrict __cond,
         pthread_mutex_t *__restrict __mutex)
     __attribute__ ((__nonnull__ (1, 2)));
# 954 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_cond_timedwait (pthread_cond_t *__restrict __cond,
       pthread_mutex_t *__restrict __mutex,
       __const struct timespec *__restrict
       __abstime) __attribute__ ((__nonnull__ (1, 2, 3)));




extern int pthread_condattr_init (pthread_condattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_condattr_destroy (pthread_condattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_condattr_getpshared (__const pthread_condattr_t *
                                        __restrict __attr,
                                        int *__restrict __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_condattr_setpshared (pthread_condattr_t *__attr,
                                        int __pshared) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_condattr_getclock (__const pthread_condattr_t *
          __restrict __attr,
          __clockid_t *__restrict __clock_id)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_condattr_setclock (pthread_condattr_t *__attr,
          __clockid_t __clock_id)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 998 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_spin_init (pthread_spinlock_t *__lock, int __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_spin_destroy (pthread_spinlock_t *__lock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_spin_lock (pthread_spinlock_t *__lock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_spin_trylock (pthread_spinlock_t *__lock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_spin_unlock (pthread_spinlock_t *__lock)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int pthread_barrier_init (pthread_barrier_t *__restrict __barrier,
     __const pthread_barrierattr_t *__restrict
     __attr, unsigned int __count)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_barrier_destroy (pthread_barrier_t *__barrier)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_barrier_wait (pthread_barrier_t *__barrier)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int pthread_barrierattr_init (pthread_barrierattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_barrierattr_destroy (pthread_barrierattr_t *__attr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_barrierattr_getpshared (__const pthread_barrierattr_t *
        __restrict __attr,
        int *__restrict __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int pthread_barrierattr_setpshared (pthread_barrierattr_t *__attr,
                                           int __pshared)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 1065 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_key_create (pthread_key_t *__key,
          void (*__destr_function) (void *))
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int pthread_key_delete (pthread_key_t __key) __attribute__ ((__nothrow__));


extern void *pthread_getspecific (pthread_key_t __key) __attribute__ ((__nothrow__));


extern int pthread_setspecific (pthread_key_t __key,
    __const void *__pointer) __attribute__ ((__nothrow__)) ;




extern int pthread_getcpuclockid (pthread_t __thread_id,
      __clockid_t *__clock_id)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
# 1099 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4
extern int pthread_atfork (void (*__prepare) (void),
      void (*__parent) (void),
      void (*__child) (void)) __attribute__ ((__nothrow__));
# 1113 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/pthread.h" 3 4

# 52 "../inc/osTarget.h" 2
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 1 3 4
# 28 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4

# 171 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/posix_opt.h" 1 3 4
# 172 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 2 3 4
# 195 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../lib/gcc/arm-none-linux-gnueabi/4.1.2/include/stddef.h" 1 3 4
# 196 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 2 3 4
# 224 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
typedef __useconds_t useconds_t;
# 236 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
typedef __intptr_t intptr_t;






typedef __socklen_t socklen_t;
# 256 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int access (__const char *__name, int __type) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 299 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern __off_t lseek (int __fd, __off_t __offset, int __whence) __attribute__ ((__nothrow__));
# 318 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int close (int __fd);






extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;





extern ssize_t write (int __fd, __const void *__buf, size_t __n) ;
# 382 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int pipe (int __pipedes[2]) __attribute__ ((__nothrow__)) ;
# 391 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern unsigned int alarm (unsigned int __seconds) __attribute__ ((__nothrow__));
# 403 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern unsigned int sleep (unsigned int __seconds);






extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     __attribute__ ((__nothrow__));






extern int usleep (__useconds_t __useconds);
# 427 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int pause (void);



extern int chown (__const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchown (int __fd, __uid_t __owner, __gid_t __group) __attribute__ ((__nothrow__)) ;




extern int lchown (__const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 455 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int chdir (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchdir (int __fd) __attribute__ ((__nothrow__)) ;
# 469 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern char *getcwd (char *__buf, size_t __size) __attribute__ ((__nothrow__)) ;
# 482 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern char *getwd (char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__)) ;




extern int dup (int __fd) __attribute__ ((__nothrow__)) ;


extern int dup2 (int __fd, int __fd2) __attribute__ ((__nothrow__));


extern char **__environ;







extern int execve (__const char *__path, char *__const __argv[],
     char *__const __envp[]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 514 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int execv (__const char *__path, char *__const __argv[])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int execle (__const char *__path, __const char *__arg, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int execl (__const char *__path, __const char *__arg, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int execvp (__const char *__file, char *__const __argv[])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int execlp (__const char *__file, __const char *__arg, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int nice (int __inc) __attribute__ ((__nothrow__)) ;




extern void _exit (int __status) __attribute__ ((__noreturn__));





# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/confname.h" 1 3 4
# 26 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/confname.h" 3 4
enum
  {
    _PC_LINK_MAX,

    _PC_MAX_CANON,

    _PC_MAX_INPUT,

    _PC_NAME_MAX,

    _PC_PATH_MAX,

    _PC_PIPE_BUF,

    _PC_CHOWN_RESTRICTED,

    _PC_NO_TRUNC,

    _PC_VDISABLE,

    _PC_SYNC_IO,

    _PC_ASYNC_IO,

    _PC_PRIO_IO,

    _PC_SOCK_MAXBUF,

    _PC_FILESIZEBITS,

    _PC_REC_INCR_XFER_SIZE,

    _PC_REC_MAX_XFER_SIZE,

    _PC_REC_MIN_XFER_SIZE,

    _PC_REC_XFER_ALIGN,

    _PC_ALLOC_SIZE_MIN,

    _PC_SYMLINK_MAX,

    _PC_2_SYMLINKS

  };


enum
  {
    _SC_ARG_MAX,

    _SC_CHILD_MAX,

    _SC_CLK_TCK,

    _SC_NGROUPS_MAX,

    _SC_OPEN_MAX,

    _SC_STREAM_MAX,

    _SC_TZNAME_MAX,

    _SC_JOB_CONTROL,

    _SC_SAVED_IDS,

    _SC_REALTIME_SIGNALS,

    _SC_PRIORITY_SCHEDULING,

    _SC_TIMERS,

    _SC_ASYNCHRONOUS_IO,

    _SC_PRIORITIZED_IO,

    _SC_SYNCHRONIZED_IO,

    _SC_FSYNC,

    _SC_MAPPED_FILES,

    _SC_MEMLOCK,

    _SC_MEMLOCK_RANGE,

    _SC_MEMORY_PROTECTION,

    _SC_MESSAGE_PASSING,

    _SC_SEMAPHORES,

    _SC_SHARED_MEMORY_OBJECTS,

    _SC_AIO_LISTIO_MAX,

    _SC_AIO_MAX,

    _SC_AIO_PRIO_DELTA_MAX,

    _SC_DELAYTIMER_MAX,

    _SC_MQ_OPEN_MAX,

    _SC_MQ_PRIO_MAX,

    _SC_VERSION,

    _SC_PAGESIZE,


    _SC_RTSIG_MAX,

    _SC_SEM_NSEMS_MAX,

    _SC_SEM_VALUE_MAX,

    _SC_SIGQUEUE_MAX,

    _SC_TIMER_MAX,




    _SC_BC_BASE_MAX,

    _SC_BC_DIM_MAX,

    _SC_BC_SCALE_MAX,

    _SC_BC_STRING_MAX,

    _SC_COLL_WEIGHTS_MAX,

    _SC_EQUIV_CLASS_MAX,

    _SC_EXPR_NEST_MAX,

    _SC_LINE_MAX,

    _SC_RE_DUP_MAX,

    _SC_CHARCLASS_NAME_MAX,


    _SC_2_VERSION,

    _SC_2_C_BIND,

    _SC_2_C_DEV,

    _SC_2_FORT_DEV,

    _SC_2_FORT_RUN,

    _SC_2_SW_DEV,

    _SC_2_LOCALEDEF,


    _SC_PII,

    _SC_PII_XTI,

    _SC_PII_SOCKET,

    _SC_PII_INTERNET,

    _SC_PII_OSI,

    _SC_POLL,

    _SC_SELECT,

    _SC_UIO_MAXIOV,

    _SC_IOV_MAX = _SC_UIO_MAXIOV,

    _SC_PII_INTERNET_STREAM,

    _SC_PII_INTERNET_DGRAM,

    _SC_PII_OSI_COTS,

    _SC_PII_OSI_CLTS,

    _SC_PII_OSI_M,

    _SC_T_IOV_MAX,



    _SC_THREADS,

    _SC_THREAD_SAFE_FUNCTIONS,

    _SC_GETGR_R_SIZE_MAX,

    _SC_GETPW_R_SIZE_MAX,

    _SC_LOGIN_NAME_MAX,

    _SC_TTY_NAME_MAX,

    _SC_THREAD_DESTRUCTOR_ITERATIONS,

    _SC_THREAD_KEYS_MAX,

    _SC_THREAD_STACK_MIN,

    _SC_THREAD_THREADS_MAX,

    _SC_THREAD_ATTR_STACKADDR,

    _SC_THREAD_ATTR_STACKSIZE,

    _SC_THREAD_PRIORITY_SCHEDULING,

    _SC_THREAD_PRIO_INHERIT,

    _SC_THREAD_PRIO_PROTECT,

    _SC_THREAD_PROCESS_SHARED,


    _SC_NPROCESSORS_CONF,

    _SC_NPROCESSORS_ONLN,

    _SC_PHYS_PAGES,

    _SC_AVPHYS_PAGES,

    _SC_ATEXIT_MAX,

    _SC_PASS_MAX,


    _SC_XOPEN_VERSION,

    _SC_XOPEN_XCU_VERSION,

    _SC_XOPEN_UNIX,

    _SC_XOPEN_CRYPT,

    _SC_XOPEN_ENH_I18N,

    _SC_XOPEN_SHM,


    _SC_2_CHAR_TERM,

    _SC_2_C_VERSION,

    _SC_2_UPE,


    _SC_XOPEN_XPG2,

    _SC_XOPEN_XPG3,

    _SC_XOPEN_XPG4,


    _SC_CHAR_BIT,

    _SC_CHAR_MAX,

    _SC_CHAR_MIN,

    _SC_INT_MAX,

    _SC_INT_MIN,

    _SC_LONG_BIT,

    _SC_WORD_BIT,

    _SC_MB_LEN_MAX,

    _SC_NZERO,

    _SC_SSIZE_MAX,

    _SC_SCHAR_MAX,

    _SC_SCHAR_MIN,

    _SC_SHRT_MAX,

    _SC_SHRT_MIN,

    _SC_UCHAR_MAX,

    _SC_UINT_MAX,

    _SC_ULONG_MAX,

    _SC_USHRT_MAX,


    _SC_NL_ARGMAX,

    _SC_NL_LANGMAX,

    _SC_NL_MSGMAX,

    _SC_NL_NMAX,

    _SC_NL_SETMAX,

    _SC_NL_TEXTMAX,


    _SC_XBS5_ILP32_OFF32,

    _SC_XBS5_ILP32_OFFBIG,

    _SC_XBS5_LP64_OFF64,

    _SC_XBS5_LPBIG_OFFBIG,


    _SC_XOPEN_LEGACY,

    _SC_XOPEN_REALTIME,

    _SC_XOPEN_REALTIME_THREADS,


    _SC_ADVISORY_INFO,

    _SC_BARRIERS,

    _SC_BASE,

    _SC_C_LANG_SUPPORT,

    _SC_C_LANG_SUPPORT_R,

    _SC_CLOCK_SELECTION,

    _SC_CPUTIME,

    _SC_THREAD_CPUTIME,

    _SC_DEVICE_IO,

    _SC_DEVICE_SPECIFIC,

    _SC_DEVICE_SPECIFIC_R,

    _SC_FD_MGMT,

    _SC_FIFO,

    _SC_PIPE,

    _SC_FILE_ATTRIBUTES,

    _SC_FILE_LOCKING,

    _SC_FILE_SYSTEM,

    _SC_MONOTONIC_CLOCK,

    _SC_MULTI_PROCESS,

    _SC_SINGLE_PROCESS,

    _SC_NETWORKING,

    _SC_READER_WRITER_LOCKS,

    _SC_SPIN_LOCKS,

    _SC_REGEXP,

    _SC_REGEX_VERSION,

    _SC_SHELL,

    _SC_SIGNALS,

    _SC_SPAWN,

    _SC_SPORADIC_SERVER,

    _SC_THREAD_SPORADIC_SERVER,

    _SC_SYSTEM_DATABASE,

    _SC_SYSTEM_DATABASE_R,

    _SC_TIMEOUTS,

    _SC_TYPED_MEMORY_OBJECTS,

    _SC_USER_GROUPS,

    _SC_USER_GROUPS_R,

    _SC_2_PBS,

    _SC_2_PBS_ACCOUNTING,

    _SC_2_PBS_LOCATE,

    _SC_2_PBS_MESSAGE,

    _SC_2_PBS_TRACK,

    _SC_SYMLOOP_MAX,

    _SC_STREAMS,

    _SC_2_PBS_CHECKPOINT,


    _SC_V6_ILP32_OFF32,

    _SC_V6_ILP32_OFFBIG,

    _SC_V6_LP64_OFF64,

    _SC_V6_LPBIG_OFFBIG,


    _SC_HOST_NAME_MAX,

    _SC_TRACE,

    _SC_TRACE_EVENT_FILTER,

    _SC_TRACE_INHERIT,

    _SC_TRACE_LOG,


    _SC_LEVEL1_ICACHE_SIZE,

    _SC_LEVEL1_ICACHE_ASSOC,

    _SC_LEVEL1_ICACHE_LINESIZE,

    _SC_LEVEL1_DCACHE_SIZE,

    _SC_LEVEL1_DCACHE_ASSOC,

    _SC_LEVEL1_DCACHE_LINESIZE,

    _SC_LEVEL2_CACHE_SIZE,

    _SC_LEVEL2_CACHE_ASSOC,

    _SC_LEVEL2_CACHE_LINESIZE,

    _SC_LEVEL3_CACHE_SIZE,

    _SC_LEVEL3_CACHE_ASSOC,

    _SC_LEVEL3_CACHE_LINESIZE,

    _SC_LEVEL4_CACHE_SIZE,

    _SC_LEVEL4_CACHE_ASSOC,

    _SC_LEVEL4_CACHE_LINESIZE,



    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,

    _SC_RAW_SOCKETS

  };


enum
  {
    _CS_PATH,


    _CS_V6_WIDTH_RESTRICTED_ENVS,


    _CS_GNU_LIBC_VERSION,

    _CS_GNU_LIBPTHREAD_VERSION,


    _CS_LFS_CFLAGS = 1000,

    _CS_LFS_LDFLAGS,

    _CS_LFS_LIBS,

    _CS_LFS_LINTFLAGS,

    _CS_LFS64_CFLAGS,

    _CS_LFS64_LDFLAGS,

    _CS_LFS64_LIBS,

    _CS_LFS64_LINTFLAGS,


    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,

    _CS_XBS5_ILP32_OFF32_LDFLAGS,

    _CS_XBS5_ILP32_OFF32_LIBS,

    _CS_XBS5_ILP32_OFF32_LINTFLAGS,

    _CS_XBS5_ILP32_OFFBIG_CFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LIBS,

    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,

    _CS_XBS5_LP64_OFF64_CFLAGS,

    _CS_XBS5_LP64_OFF64_LDFLAGS,

    _CS_XBS5_LP64_OFF64_LIBS,

    _CS_XBS5_LP64_OFF64_LINTFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LIBS,

    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LIBS,

    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V6_LP64_OFF64_CFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LIBS,

    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS

  };
# 553 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 2 3 4


extern long int pathconf (__const char *__path, int __name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int fpathconf (int __fd, int __name) __attribute__ ((__nothrow__));


extern long int sysconf (int __name) __attribute__ ((__nothrow__)) __attribute__ ((__const__));



extern size_t confstr (int __name, char *__buf, size_t __len) __attribute__ ((__nothrow__));




extern __pid_t getpid (void) __attribute__ ((__nothrow__));


extern __pid_t getppid (void) __attribute__ ((__nothrow__));




extern __pid_t getpgrp (void) __attribute__ ((__nothrow__));
# 589 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern __pid_t __getpgid (__pid_t __pid) __attribute__ ((__nothrow__));
# 598 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int setpgid (__pid_t __pid, __pid_t __pgid) __attribute__ ((__nothrow__));
# 615 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int setpgrp (void) __attribute__ ((__nothrow__));
# 632 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern __pid_t setsid (void) __attribute__ ((__nothrow__));







extern __uid_t getuid (void) __attribute__ ((__nothrow__));


extern __uid_t geteuid (void) __attribute__ ((__nothrow__));


extern __gid_t getgid (void) __attribute__ ((__nothrow__));


extern __gid_t getegid (void) __attribute__ ((__nothrow__));




extern int getgroups (int __size, __gid_t __list[]) __attribute__ ((__nothrow__)) ;
# 665 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int setuid (__uid_t __uid) __attribute__ ((__nothrow__));




extern int setreuid (__uid_t __ruid, __uid_t __euid) __attribute__ ((__nothrow__));




extern int seteuid (__uid_t __uid) __attribute__ ((__nothrow__));






extern int setgid (__gid_t __gid) __attribute__ ((__nothrow__));




extern int setregid (__gid_t __rgid, __gid_t __egid) __attribute__ ((__nothrow__));




extern int setegid (__gid_t __gid) __attribute__ ((__nothrow__));
# 721 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern __pid_t fork (void) __attribute__ ((__nothrow__));






extern __pid_t vfork (void) __attribute__ ((__nothrow__));





extern char *ttyname (int __fd) __attribute__ ((__nothrow__));



extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2))) ;



extern int isatty (int __fd) __attribute__ ((__nothrow__));





extern int ttyslot (void) __attribute__ ((__nothrow__));




extern int link (__const char *__from, __const char *__to)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;
# 767 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int symlink (__const char *__from, __const char *__to)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern ssize_t readlink (__const char *__restrict __path,
    char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;
# 790 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int unlink (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 799 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int rmdir (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern __pid_t tcgetpgrp (int __fd) __attribute__ ((__nothrow__));


extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) __attribute__ ((__nothrow__));






extern char *getlogin (void);







extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)));




extern int setlogin (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 835 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/getopt.h" 1 3 4
# 59 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/getopt.h" 3 4
extern char *optarg;
# 73 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/getopt.h" 3 4
extern int optind;




extern int opterr;



extern int optopt;
# 152 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/getopt.h" 3 4
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       __attribute__ ((__nothrow__));
# 836 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 2 3 4







extern int gethostname (char *__name, size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int sethostname (__const char *__name, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int sethostid (long int __id) __attribute__ ((__nothrow__)) ;





extern int getdomainname (char *__name, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
extern int setdomainname (__const char *__name, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern int vhangup (void) __attribute__ ((__nothrow__));


extern int revoke (__const char *__file) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;







extern int profil (unsigned short int *__sample_buffer, size_t __size,
     size_t __offset, unsigned int __scale)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int acct (__const char *__name) __attribute__ ((__nothrow__));



extern char *getusershell (void) __attribute__ ((__nothrow__));
extern void endusershell (void) __attribute__ ((__nothrow__));
extern void setusershell (void) __attribute__ ((__nothrow__));





extern int daemon (int __nochdir, int __noclose) __attribute__ ((__nothrow__)) ;






extern int chroot (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern char *getpass (__const char *__prompt) __attribute__ ((__nonnull__ (1)));
# 921 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int fsync (int __fd);






extern long int gethostid (void);


extern void sync (void) __attribute__ ((__nothrow__));




extern int getpagesize (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));




extern int getdtablesize (void) __attribute__ ((__nothrow__));




extern int truncate (__const char *__file, __off_t __length)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 968 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int ftruncate (int __fd, __off_t __length) __attribute__ ((__nothrow__)) ;
# 988 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int brk (void *__addr) __attribute__ ((__nothrow__)) ;





extern void *sbrk (intptr_t __delta) __attribute__ ((__nothrow__));
# 1009 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern long int syscall (long int __sysno, ...) __attribute__ ((__nothrow__));
# 1032 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int lockf (int __fd, int __cmd, __off_t __len) ;
# 1063 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4
extern int fdatasync (int __fildes) __attribute__ ((__nothrow__));
# 1101 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/unistd.h" 3 4

# 53 "../inc/osTarget.h" 2
# 52 "../inc/stdInc.h" 2



# 1 "../inc/vmShared.h" 1
# 69 "../inc/vmShared.h"
# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 70 "../inc/vmShared.h" 2
# 94 "../inc/vmShared.h"
typedef struct
{
 unsigned char byCommand __attribute__ ((packed));
 unsigned char bySequence __attribute__ ((packed));

 unsigned char pData[25000u] __attribute__ ((packed));

} XCommand;





typedef struct
{
 unsigned short uBlock __attribute__ ((packed));
 unsigned short uLen __attribute__ ((packed));
 unsigned char byLast __attribute__ ((packed));
 unsigned char usSource __attribute__ ((packed));

 XCommand CMD __attribute__ ((packed));

} XBlock;





typedef struct
{
 unsigned char byType __attribute__ ((packed));
 unsigned char byCRC __attribute__ ((packed));

 XBlock BLK __attribute__ ((packed));

} XFrame;
# 143 "../inc/vmShared.h"
typedef struct
{
 unsigned int ulOffset __attribute__ ((packed));
 unsigned short uLen __attribute__ ((packed));
 unsigned short uSegment __attribute__ ((packed));
 unsigned char byBit __attribute__ ((packed));
 unsigned char byDummy __attribute__ ((packed));

} XVariable;



typedef struct
{
 unsigned char usType __attribute__ ((packed));
 unsigned char usSegment __attribute__ ((packed));
 unsigned char usOffset __attribute__ ((packed));
 unsigned char usLen __attribute__ ((packed));

} XVariableS;

typedef struct
{
 unsigned char usType __attribute__ ((packed));
 unsigned char usSegment __attribute__ ((packed));
 unsigned short uOffset __attribute__ ((packed));
 unsigned short uLen __attribute__ ((packed));

} XVariableM;

typedef struct
{
 unsigned char usType __attribute__ ((packed));
 unsigned char byDummy __attribute__ ((packed));
 unsigned short uLen __attribute__ ((packed));
 unsigned int ulOffset __attribute__ ((packed));
 unsigned short uSegment __attribute__ ((packed));

} XVariableL;




typedef struct
{
 XVariable VAR __attribute__ ((packed));

 unsigned char pValue[2] __attribute__ ((packed));

} XValue;
# 203 "../inc/vmShared.h"
typedef struct
{
 unsigned short uCode __attribute__ ((packed));
 unsigned short uInst __attribute__ ((packed));
 unsigned int ulCodePos __attribute__ ((packed));
 unsigned short uType __attribute__ ((packed));

} XBreakpoint;



typedef struct
{
 XBreakpoint BP __attribute__ ((packed));
 unsigned short uTask __attribute__ ((packed));
 unsigned short uState __attribute__ ((packed));

} XBPNotification;





typedef struct
{
 unsigned short uCode __attribute__ ((packed));
 unsigned short uInst __attribute__ ((packed));

} XIndex;
# 293 "../inc/vmShared.h"
typedef struct
{
 unsigned char usPrograms __attribute__ ((packed));
 unsigned char usAttrib __attribute__ ((packed));
 unsigned int ulPara1 __attribute__ ((packed));
 unsigned int ulPara2 __attribute__ ((packed));
 unsigned int ulWatchDogMS __attribute__ ((packed));
 unsigned short uRegionsRd __attribute__ ((packed));
 unsigned short uRegionsWr __attribute__ ((packed));
 unsigned short uCpyRegions __attribute__ ((packed));
 unsigned short uCpyRegOff __attribute__ ((packed));
 unsigned char szName[33] __attribute__ ((packed));
 unsigned char usPriority __attribute__ ((packed));


} XTask_21001;





typedef struct
{
 unsigned char * pAdr __attribute__ ((packed));
 unsigned int ulSize __attribute__ ((packed));

} XObject;





typedef struct
{
 unsigned int ulOffset __attribute__ ((packed));
 unsigned short uSize __attribute__ ((packed));
 unsigned char bySegment __attribute__ ((packed));
 unsigned char bRead __attribute__ ((packed));

} XMemRegion;
# 394 "../inc/vmShared.h"
typedef struct
{
 unsigned int ulBinDLVersion __attribute__ ((packed));

 unsigned int ulStateVarOffs __attribute__ ((packed));
 unsigned short uTasks __attribute__ ((packed));
 unsigned short uCodeObjects __attribute__ ((packed));
 unsigned short uInstObjects __attribute__ ((packed));
 unsigned short uDirectVars __attribute__ ((packed));
 unsigned int ulIECOffset __attribute__ ((packed));
 unsigned int ulIECSize __attribute__ ((packed));
 unsigned short uDebugCode __attribute__ ((packed));
 unsigned short uCpyRegions __attribute__ ((packed));
 unsigned short uCpyRegConst __attribute__ ((packed));
 unsigned short uCpyRegCOff __attribute__ ((packed));
 unsigned short uProjInfo __attribute__ ((packed));
 unsigned short uIOLayer __attribute__ ((packed));

} XProject_213;




typedef struct
{
 char szProjVers[32u] __attribute__ ((packed));
 char szProjName[32u] __attribute__ ((packed));
 char szModified[100u] __attribute__ ((packed));
 char szCreated[100u] __attribute__ ((packed));
 char szOwner[32u] __attribute__ ((packed));
 char szComment1[100u] __attribute__ ((packed));
 char szComment2[100u] __attribute__ ((packed));
 char szComment3[100u] __attribute__ ((packed));

} XProjInfo;






typedef struct
{
 unsigned int ulResState __attribute__ ((packed));

 unsigned short uBPCount __attribute__ ((packed));
 unsigned short uStrCount __attribute__ ((packed));

} XInfo;
# 451 "../inc/vmShared.h"
typedef struct
{
 unsigned short uIndex __attribute__ ((packed));
 unsigned short uSegment __attribute__ ((packed));
 unsigned int ulSize __attribute__ ((packed));
 unsigned int ulOffset __attribute__ ((packed));
 unsigned char byFixed __attribute__ ((packed));

 unsigned char byType __attribute__ ((packed));

} XDownHeader;



typedef struct
{
 unsigned char bySegment __attribute__ ((packed));
 unsigned char byBit __attribute__ ((packed));
 unsigned short uSize __attribute__ ((packed));
 unsigned int ulOffset __attribute__ ((packed));

} XDownDirect;






typedef struct
{
 unsigned short uiInst __attribute__ ((packed));
 unsigned short uiOffSrc __attribute__ ((packed));
 unsigned short uiSize __attribute__ ((packed));

} XCopyRegion;
# 509 "../inc/vmShared.h"
typedef struct
{
 unsigned short uCode __attribute__ ((packed));
 unsigned short uData __attribute__ ((packed));
 unsigned short uToCopy __attribute__ ((packed));
 unsigned short uDirVars __attribute__ ((packed));
 signed short iCodeDiff __attribute__ ((packed));
 signed short iDataDiff __attribute__ ((packed));
 unsigned short uTaskInfo __attribute__ ((packed));

} XOnlineChange;

typedef struct
{
 unsigned short uOld __attribute__ ((packed));
 unsigned short uNew __attribute__ ((packed));
 unsigned short uOldOff __attribute__ ((packed));
 unsigned short uNewOff __attribute__ ((packed));
 unsigned int ulSize __attribute__ ((packed));
 unsigned char usOldBit __attribute__ ((packed));
 unsigned char usNewBit __attribute__ ((packed));

} XOCInstCopy;
# 555 "../inc/vmShared.h"
typedef struct
{
 unsigned int ulDataLen __attribute__ ((packed));
 unsigned short uNameLen __attribute__ ((packed));
 unsigned char usCRC __attribute__ ((packed));
 unsigned char usHash __attribute__ ((packed));

} XFileDef;






typedef struct
{
 char cDummy __attribute__ ((packed));
 double lDummy __attribute__ ((packed));

} XAlignTest;






typedef struct
{
 unsigned char usBigEndian __attribute__ ((packed));
 unsigned char usDummy __attribute__ ((packed));
 unsigned short uFirmware __attribute__ ((packed));
 unsigned short uMaxData __attribute__ ((packed));
 unsigned short uAddConfig __attribute__ ((packed));

 unsigned char Reserved[100] __attribute__ ((packed));

} XConfig;






typedef struct
{
 unsigned char usType __attribute__ ((packed));
 unsigned char usNameSize __attribute__ ((packed));
 unsigned short uInst __attribute__ ((packed));
 unsigned short uChildren __attribute__ ((packed));
 unsigned short uIndex __attribute__ ((packed));

 unsigned int ulFOBrother __attribute__ ((packed));
 unsigned int ulFOAbsolut __attribute__ ((packed));

} XDBIInstance;


typedef struct
{
 unsigned int ulOffset __attribute__ ((packed));
 unsigned short uMember __attribute__ ((packed));
 unsigned short uType __attribute__ ((packed));

} XDBIType;


typedef struct
{
 unsigned char usType __attribute__ ((packed));
 unsigned char usNameSize __attribute__ ((packed));
 unsigned short uInst __attribute__ ((packed));
 unsigned int ulOffset __attribute__ ((packed));
 unsigned short uLen __attribute__ ((packed));
 unsigned char usBit __attribute__ ((packed));
 unsigned char usDummy __attribute__ ((packed));

} XDBIVar;


typedef struct
{
 XDBIVar xVar __attribute__ ((packed));

 char szName[0] __attribute__ ((packed));

} XVisuVar;






typedef struct
{
 unsigned int ulIOffs __attribute__ ((packed));
 unsigned int ulISize __attribute__ ((packed));
 unsigned int ulQOffs __attribute__ ((packed));
 unsigned int ulQSize __attribute__ ((packed));
 unsigned int ulMOffs __attribute__ ((packed));
 unsigned int ulMSize __attribute__ ((packed));

 unsigned char usChannel __attribute__ ((packed));
 char szName[33] __attribute__ ((packed));

} XIOLayer;






typedef struct
{
 unsigned int uForwardID __attribute__ ((packed));


 unsigned short uNameLen __attribute__ ((packed));


 char szCfgFile[0] __attribute__ ((packed));


} XFWDownload;
# 687 "../inc/vmShared.h"
typedef struct
{
 unsigned int ulFirmware __attribute__ ((packed));

 char szMain[20] __attribute__ ((packed));
 char szFull[80] __attribute__ ((packed));

} XVersionInfo;




typedef struct
{
 char szType[100] __attribute__ ((packed));

} XTypeInfo;




typedef struct
{
 unsigned int ulSerialNo __attribute__ ((packed));

} XSerialNo;




typedef struct
{
 unsigned char MacAddr[6] __attribute__ ((packed));

} XMacAddr;




typedef struct
{
 char szInstKey[35] __attribute__ ((packed));

 unsigned char usReserved __attribute__ ((packed));

} XInstKey;




typedef struct
{
 char szLicKey[25] __attribute__ ((packed));

 unsigned char usReserved __attribute__ ((packed));

} XLicKey;




typedef struct
{
 unsigned short uLicensed __attribute__ ((packed));

 unsigned short uAvailable __attribute__ ((packed));

} XFeatDef;




typedef struct
{
 unsigned short uLicense __attribute__ ((packed));
 unsigned short uLength __attribute__ ((packed));

 unsigned char pLicEx[0] __attribute__ ((packed));

} XLicEx;
# 1230 "../inc/vmShared.h"
# 1 "../inc/osAlign.h" 1
# 39 "../inc/osAlign.h"
#pragma pack()
# 1231 "../inc/vmShared.h" 2
# 56 "../inc/stdInc.h" 2



# 1 "../inc/vmmDef.h" 1
# 67 "../inc/vmmDef.h"
typedef struct
{
 unsigned char CurLen;
 unsigned char MaxLen;
 char Contents[0];

} IEC_STRING;




typedef struct
{
 unsigned char CurLen;
 unsigned char MaxLen;
 char Contents[255];

} IEC_STRMAX;





typedef struct
{

 long mtype __attribute__ ((packed));


 unsigned short uRespQueue __attribute__ ((packed));

 unsigned short uID __attribute__ ((packed));

 unsigned short uLen __attribute__ ((packed));
 unsigned char pData[(25000u + (2 * sizeof(unsigned short) + 2 * sizeof(unsigned char) + (2 * sizeof(unsigned char))))] __attribute__ ((packed));

} SMessage;
# 114 "../inc/vmmDef.h"
typedef struct
{
 unsigned short uCode;
 unsigned short uData;
 unsigned int uCodePos;

} SBreakpoint;



typedef struct
{
 SBreakpoint BP;
 unsigned short uTask;
 unsigned short uState;

} SBPNotification;



typedef struct
{
 unsigned int ulBPId;
 unsigned char byNext;

 SBreakpoint BP;

} SBPEntry;



typedef struct
{
 unsigned short uNext;
 unsigned short uLast;

 SBPNotification pQueue[(25u+1u)];

} SBPQueue;





typedef struct
{
 unsigned short uCode;
 unsigned short uData;

} SIndex;





typedef struct
{
 unsigned short uData;
 unsigned short uOffSrc;
 unsigned int ulOffDst;
 unsigned short uSize;

} SRetainReg;
# 185 "../inc/vmmDef.h"
typedef struct
{
 unsigned int ulOffset;
 unsigned short uSize;
 unsigned char usSegment;

 unsigned char pSetQ[5u];
 unsigned char pGetI[5u];
 unsigned char pGetQ[5u];

} SRegion;

typedef struct
{
 unsigned short uRegionsRd;
 unsigned short uRegionsWr;


 SRegion pRegionRd[2100u];
 SRegion pRegionWr[2100u];

 unsigned char pSetQ[5u];
 unsigned char pGetI[5u];
 unsigned char pGetQ[5u];


} SImageReg;





typedef struct
{
 unsigned char usAttrib;
 unsigned char usPrograms;

 unsigned int ulPara1;
 unsigned int ulPara2;


 unsigned short uCpyRegions;
 unsigned short uCpyRegOff;


 unsigned int ulWDogCounter;
 unsigned int ulWDogTrigger;
 unsigned char bWDogEnable;
 unsigned char bWDogOldEnable;

 char szName[33u];
 unsigned char usPriority;

 SImageReg *pIR;

} STask;





typedef struct
{





 unsigned int ulState;
 unsigned int ulPrio;
 unsigned int ulCycle;
 unsigned int ulETMax;
 unsigned int ulErrNo;

} STaskState;





typedef struct
{
 unsigned int ulExecMax;
 unsigned int ulExecMin;
 unsigned int ulExecAver;

 unsigned int ulExecCounter;
 unsigned long long ullExecSum;

} SStatistic;





typedef struct
{
 unsigned char *pAdr;
 unsigned int ulSize;

} SObject;





typedef struct
{
 unsigned int ulStateVarOffs;
 unsigned short uTasks;
 unsigned short uCode;
 unsigned short uData;
 unsigned short uDirectVars;
 unsigned int ulIECOffset;


 unsigned short uCpyRegions;
 unsigned short uCpyRegConst;
 unsigned short uCpyRegCOff;



 unsigned short uProjInfo;


 unsigned short uIOLayer;

 unsigned int ulRetainUsed;

} SProject;



typedef struct
{
 char szProjVers[32u];
 char szProjName[32u];
 char szModified[100u];
 char szCreated[100u];
 char szOwner[32u];
 char szComment1[100u];
 char szComment2[100u];
 char szComment3[100u];

} SProjInfo;





typedef struct
{
 unsigned int ulDataLen;
 unsigned int ulOffset;
 unsigned short uNameLen;
 unsigned char usCRC;
 unsigned char usHash;

} SFileDef;





typedef struct
{
 unsigned int ulUser;
 unsigned int ulNice;
 unsigned int ulSyst;
 unsigned int ulIdle;

} SProcTime;

typedef struct
{
 unsigned int ulUser;
 unsigned int ulSyst;

} STaskTime;





typedef struct
{
 unsigned short uLastIndex;
 unsigned int ulOffset;

} SSegInfo;





typedef struct
{
 unsigned short uCode;
 unsigned short uData;

 unsigned short uErrNo;
 unsigned int ulOffset;

} SException;





typedef struct
{
 unsigned short uCode;
 unsigned short uData;

 unsigned short uCodePos;

 unsigned char *pData;

} SContext;





typedef struct
{
 unsigned short uNext;
 unsigned short uLast;

 char *ppMessage[(25u+1u)];

} SMsgQueue;
# 425 "../inc/vmmDef.h"
typedef struct
{
 unsigned short uOld;
 unsigned short uNew;
 unsigned int ulSize;
 unsigned short uOldOff;
 unsigned short uNewOff;
 unsigned char usOldBit;
 unsigned char usNewBit;

} SOCCopy;



typedef struct
{
 STask Task;

 SIndex pProg[25u];

} SOCTask;




typedef struct
{
 unsigned short uCode;
 unsigned short uData;
 unsigned short uToCopy;
 unsigned short uDirVars;
 signed short iCodeDiff;
 signed short iDataDiff;
 unsigned short uTaskInfo;
 unsigned short uProjInfo;




 SSegInfo RetainInfo;


 unsigned char pState[25u];

 unsigned short uRetry;
 unsigned long long ullTime;

 SOCCopy *pCopy;

 unsigned short *pCodeInd;
 unsigned short *pDataInd;

 unsigned char __attribute__((packed)) **ppDataAdr;

 SOCTask *pTask;

 SProject Project;

} SOnlChg;







typedef struct
{
 unsigned short uContext;
 SObject pSeg[0x0006u];





 unsigned char usCurrent;

 SIndex pProg[25u];
 SContext pContext[25u];
 SException Exception;





 STaskState __attribute__((packed)) *pState;

 unsigned short uHalt;







 unsigned char *pStack;
 unsigned char *pSP;

 char __attribute__((packed)) *pBuffer;

} SIntLocal;





typedef struct
{
 SObject *pCode;



 SObject pData[6000u];




 unsigned char pDataSeg[6000u];





 SSegInfo RetainInfo;


 unsigned char byFirstBP;
 SBPEntry pBPList[25u];

 SMsgQueue MsgQueue;
 SBPQueue BPQueue;


 unsigned short uCpyRegions;
 SRetainReg CopyRegions[(6000u / 2)];






 void *pVMM;

} SIntShared;





typedef struct
{
 unsigned char usTask;
 unsigned char usFlags;

 STask Task;
 SIntLocal Local;

 SIntShared *pShared;
 SProject *pProject;


 SRegion *pRegionRd;
 SRegion *pRegionWr;



 unsigned char usException;
 unsigned char usEvent;



 SStatistic Stat;






} STaskInfoVM;
# 644 "../inc/vmmDef.h"
typedef struct
{
 unsigned short uState;
 unsigned short uLen;

 unsigned short uTask;
 unsigned int hSocket;
 unsigned int hTask;

} SComTCP;







typedef struct
{
 unsigned int ulOffset;
 unsigned int ulSize;
 unsigned short uIndex;
 unsigned short uSegment;
 unsigned char byFixed;

 unsigned char byType;

} SDownHeader;





typedef struct
{
 unsigned int ulOffset;
 unsigned short uSegment;
 unsigned short uBit;
 unsigned short uSize;

} SDownDirect;





typedef struct
{
 unsigned int ulSize;
 unsigned int ulInitSize;
 unsigned int ulOffset;
 unsigned short uIndex;
 unsigned short uSegment;
 unsigned short uBit;

} SDLHeader;







typedef struct
{
 unsigned int hLoad;
 unsigned int hSave;

} SDLProj;
# 722 "../inc/vmmDef.h"
typedef struct
{
 unsigned int hLoad;
 unsigned int hSave;
 unsigned int hDir;

 unsigned short uRetry;

} SDLData;
# 740 "../inc/vmmDef.h"
typedef struct
{
 unsigned int hF1;
 unsigned int hF2;

 SFileDef FD;

} SDLFile;
# 757 "../inc/vmmDef.h"
typedef struct
{
 unsigned int ulID;

 char szTask[8];
 char szInfo[28];

} SSysLoadTask;
# 774 "../inc/vmmDef.h"
typedef struct
{
 unsigned char pRecv[25000u];

 unsigned short uRecv;
 unsigned short uProc;

} SDLDebug;

typedef struct
{
 XDBIVar xVar;

 char *szName;

} SDBIVar;

typedef struct
{
 unsigned int ulOffset;
 unsigned short uMember;
 unsigned short uType;

 SDBIVar *pMember;

} SDBIType;

typedef struct tagSDBIInst
{
 unsigned char usType;
 unsigned char usNameSize;
 unsigned short uInst;
 unsigned short uIndex;
 unsigned short uChildren;

 unsigned int ulFOBrother;
 unsigned int ulFOAbsolut;

 char *szName;

 struct tagSDBIInst **ppChildren;

} SDBIInstance;

typedef struct
{
 unsigned char bInitialized;

 SDBIInstance *pInstRoot;

 unsigned short uTypes;
 SDBIType *pTypes;

 unsigned int hInst;
 unsigned int hVar;

} SDBIInfo;
# 841 "../inc/vmmDef.h"
typedef struct
{
 unsigned char pRecv[25000u];

 unsigned short uRecv;
 unsigned short uDone;

} SDLGetVal;



typedef struct
{
 unsigned short uTask;

} SDLTask;



typedef struct
{
 SBreakpoint BP;

 unsigned int ulBPId;
 unsigned short uTask;

} SDLBreak;





typedef struct
{
 unsigned short uDone;

} SDLProjInfo;







typedef struct
{
 unsigned short uRetry;

} SDLClrFlash;





typedef struct
{
 unsigned short uRetry;

} SDLInitClear;


typedef struct
{
 unsigned char pRecv[25000u];



 unsigned short uRecv;
 unsigned char bDone;
 unsigned char bOther;



 unsigned short uObj;
 unsigned short uObj2;
 unsigned char bSwitch;



 unsigned short uError;
 unsigned short uDomain;
 unsigned short uMode;

 unsigned char bOCStarted;
 unsigned char usResState;

 unsigned short uRetry;



 SDLHeader HDR;


 SDLFile FIL;


 SDLProj PRJ;


 SDLData DAT;


 SDLDebug DBI;


 SDLGetVal GET;
 SDLTask TSK;
 SDLBreak BRK;

 SDLProjInfo PIN;


 SDLClrFlash CFL;


 SDLInitClear CIC;

} SDLBuffer;





typedef struct
{
 unsigned int ulIOffs;
 unsigned int ulISize;
 unsigned int ulQOffs;
 unsigned int ulQSize;
 unsigned int ulMOffs;
 unsigned int ulMSize;

 unsigned short uState;

 unsigned char bEnabled;
 unsigned char bCreated;
 unsigned char usChannel;

 unsigned char usNotifyRd;
 unsigned char usNotifyWr;

 unsigned char bAsyncConfig;

 char szIOLayer[33u];
 unsigned short uIOLType;

} SIOLayer;





typedef struct
{
 unsigned short uIOLayer;

 unsigned char bAsyncConfig;
 unsigned char bWarmStart;

} SIOLayerIniVal;





typedef struct
{
 unsigned int ulOffs __attribute__ ((packed));
 unsigned int ulSize __attribute__ ((packed));
 unsigned int ulSegSize __attribute__ ((packed));

 unsigned char * pAdr __attribute__ ((packed));

} SIOSegment;

typedef struct
{
 SIOSegment I;
 SIOSegment Q;
 SIOSegment W;
 SIOSegment C;
 SIOSegment R;

 unsigned char usChannel __attribute__ ((packed));
 char szName[33u] __attribute__ ((packed));

} SIOConfig;

typedef struct
{
 unsigned short uIOLayer;
 unsigned short uRes;

 unsigned char usChannel;
 char szName[33u];

} SIODone;

typedef struct
{
 unsigned short uTask __attribute__ ((packed));




 unsigned short uLen __attribute__ ((packed));
 unsigned int ulOffset __attribute__ ((packed));
 unsigned char usBit __attribute__ ((packed));
 unsigned char usSegment __attribute__ ((packed));

} SIONotify;





typedef struct
{
 unsigned short uMode;
 SObject Retain;

 unsigned char pProjectID[16u];

} SRetExt;

typedef struct
{
 unsigned int ulID;

 unsigned int ulSize;
 unsigned int ulUsed;

 unsigned short uCRC1;
 unsigned short uCRC2;

 unsigned char pProjectID[16u];

 unsigned short uValid;

 unsigned char pRetain[0];

} SRetFile;






typedef struct
{
 unsigned char bProjLoaded;
 unsigned char bProjActive;

 unsigned short uCreatedTasks;

 unsigned char bDebugMode;
 unsigned char pLogin[5u];

 unsigned char bExecColdStart;
 unsigned char bExecWarmStart;

 unsigned char bWarmStart;

 unsigned int ulResState;
 unsigned int ulUniqueNumber;
 unsigned int ulLastStateReq;

 STaskInfoVM *ppVM[25u];

 SIntShared Shared;
 SProject Project;







 SComTCP pTCP[5u];

 SDLBuffer DLB;

 unsigned char pProjectGUID[16u];
 unsigned char pDownLoadGUID[16u];



 unsigned char pBuffer[((255 + (2 * sizeof(unsigned char))) > (2 * 260u) ? (255 + (2 * sizeof(unsigned char))) : (2 * 260u)) + 2];





 SProjInfo ProjInfo;



 SOnlChg OnlChg;



 SDBIInfo DBI;



 SIOLayer pIOLayer[5u];



 unsigned short pVMEventCount[64u];
 unsigned short *ppVMEvents[64u];






} STaskInfoVMM;
# 60 "../inc/stdInc.h" 2



# 1 "../inc/intDef.h" 1
# 58 "../inc/intDef.h"
typedef unsigned char * SPTR_TYP;
typedef unsigned char * CPTR_TYP;
typedef unsigned char __attribute__((packed)) * DPTR_TYP;

unsigned short intConvert (STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
unsigned short intArith (STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
unsigned short intWide16Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
unsigned short intWide32Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
unsigned short intWide64Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
# 64 "../inc/stdInc.h" 2



# 1 "../inc/libDef.h" 1
# 42 "../inc/libDef.h"
typedef void (* EXECUTE_FUN) (STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
typedef void (* EXECUTE_FB ) (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN );
# 379 "../inc/libDef.h"
typedef struct
{
 unsigned char var __attribute__ ((packed)); unsigned char dummy_08_var __attribute__ ((packed)); unsigned short dummy_16_var __attribute__ ((packed));

} SExtDec_BYTE;

typedef struct
{
 unsigned short var __attribute__ ((packed)); unsigned short dummy_16_var __attribute__ ((packed));

} SExtDec_WORD;

typedef struct
{
 unsigned var __attribute__ ((packed));

} SExtDec_DWORD;




typedef struct
{
 signed char var __attribute__ ((packed)); unsigned char dummy_08_var __attribute__ ((packed)); unsigned short dummy_16_var __attribute__ ((packed));

} SExtDec_SINT;

typedef struct
{
 signed short var __attribute__ ((packed)); unsigned short dummy_16_var __attribute__ ((packed));

} SExtDec_INT;

typedef struct
{
 signed int var __attribute__ ((packed));

} SExtDec_DINT;




typedef struct
{
 unsigned char __attribute__((packed)) * * ptr __attribute__ ((packed));

} SExtDec_PTR;
# 68 "../inc/stdInc.h" 2



# 1 "../inc/vmmMain.h" 1
# 40 "../inc/vmmMain.h"
unsigned short vmmMainST(void);
unsigned short vmmMainMT(void);
unsigned short vmmMainIPC(void);

unsigned short vmmQueueMessage(SMsgQueue *pQueue, char *szMessage);
unsigned short vmmQueueBPNotification(STaskInfoVM *pVM, unsigned short uState);
unsigned short vmmFormatException(SException *pExcept, STaskInfoVM *pVM, unsigned int ulErrNo, char *szBuffer);

unsigned short vmmStartTask(STaskInfoVMM *pVMM, unsigned short uTask);
unsigned short vmmStopTask(STaskInfoVMM *pVMM, unsigned short uTask, unsigned char bSync);
unsigned short vmmContinueTask(STaskInfoVMM *pVMM, unsigned short uTask);
unsigned short vmmSingleStep(STaskInfoVMM *pVMM, unsigned short uTask);
unsigned short vmmDeleteTask(STaskInfoVMM *pVMM, unsigned short uTask);
unsigned short vmmResetTask(STaskInfoVMM *pVMM, unsigned short uTask);

unsigned short vmmBuildVMEvents(STaskInfoVMM *pVMM);
unsigned short vmmClearVMEvents(STaskInfoVMM *pVMM);
unsigned short vmmSetEvent(STaskInfoVMM *pVMM, unsigned short uEvent);





unsigned short cmdRun(STaskInfoVMM *pVMM, unsigned short uSignal, XBlock *pBlock);
unsigned short cmdExecute(STaskInfoVMM *pVMM, XBlock *pBlock);

unsigned short cmdBeforeExecute(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdAfterExecute(STaskInfoVMM *pVMM, XBlock *pBlock);






unsigned short comBlock(STaskInfoVMM *pVMM);
unsigned short comClient(STaskInfoVMM *pVMM);


  unsigned short sockInitialize(STaskInfoVMM *pVMM);
  unsigned short sockListen(void *pPara);
  unsigned short sockComm(void *pPara);






unsigned short bpInitialize(STaskInfoVMM *pVMM);
unsigned short bpAddBreakpoint(STaskInfoVMM *pVMM, SBreakpoint *pBP, unsigned int *pBPId);
unsigned short bpDeleteBreakpoint(STaskInfoVMM *pVMM, unsigned int ulBPId);
unsigned short bpDeleteAllBreakpoints(STaskInfoVMM *pVMM);
unsigned short bpValidateBP(STaskInfoVMM *pVMM, SBreakpoint *pBP);
unsigned short bpQueryBreakpoint(SIntShared *pShared, unsigned short uClassId, unsigned short uInstP, unsigned short uCodePos, SBPEntry **ppBP);
unsigned short bpIsBPInList(SIntShared *pShared, unsigned short uClassId, unsigned short uCodePos, SBPEntry **ppBP);





typedef unsigned short (*ACTPROC_FUN)(STaskInfoVMM *pVMM, XBlock *pBlock);

unsigned short cmdGetState(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdLogin(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdLogout(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdStart(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdResource(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdAllTasks(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdTask(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOpenDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdCloseDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdSetBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdClearBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdClearAllBreakpoints(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdContinue(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdSetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadCode(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadIOL(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadClear(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdInitialize(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdClearFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDBIGet(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDownloadDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetProjectVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetProjectInfo(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdLoadProject(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdSaveProject(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdLoadFile(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDelFile(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdDir(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCCode(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCCommit(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdOCFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdRetWrite(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdRetCycle(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetInstKey(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdSetLicKey(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetSerialNo(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetFeature(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetType(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdGetVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short cmdSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);

unsigned short resGetState(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resLogin(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resLogout(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resStart(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resResource(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resAllTasks(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resTask(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOpenDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resCloseDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resSetBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resClearBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resClearAllBreakpoints(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resContinue(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resSetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadCode(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadIOL(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadClear(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resInitialize(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resClearFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDBIGetChildren(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDBIGetAddress(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDownloadDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDBIGetTaskNr(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetProjectVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetProjectInfo(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resLoadProject(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resSaveProject(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resLoadFile(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDelFile(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resDir(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCCode(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCCommit(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resOCFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resRetWrite(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resRetCycle(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetInstKey(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resSetLicKey(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetSerialNo(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetFeature(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetType(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resGetVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short resSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);

unsigned short actClearResource(STaskInfoVMM *pVMM, unsigned short *upRetry, unsigned char bWarmStart);
unsigned short actClearCustomDL(STaskInfoVMM *pVMM);
unsigned short actOCCleanUp(STaskInfoVMM *pVMM);

unsigned short actDLBegin (STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uMode);
unsigned short actDLConfig(STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uMode, unsigned short uDomain);
unsigned short actDLCode (STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uMode, unsigned short uDomain, unsigned short uCode, signed short iIncr);
unsigned short actDLInit (STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uMode, unsigned short uDomain, unsigned short uData, unsigned short uDirect, signed short iIncr);
unsigned short actDLFile (STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uMode, unsigned short uDomain, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szMap);
unsigned short actDLFinish(STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uMode, unsigned short uDomain);

unsigned short actCloseFile(unsigned int *pFile);

unsigned short actCmdSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uCmd);
unsigned short actResSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uCmd);

unsigned short actClearDownload(STaskInfoVMM *pVMM, unsigned char byCommand);





unsigned short actGetOCConfig (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, SOnlChg *pOC, SProject const *pProject);
unsigned short actGetOCList (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uObj, SOCCopy *pOCC);
unsigned short actGetProjectInfo (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uObj, SProject *pProject, unsigned int *ulpBinDLVersion);
unsigned short actGetExtProjectInfo (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uObj, SProjInfo *pProjInfo);
unsigned short actGetRetainList (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uObj, SRetainReg *pRegion);
unsigned short actGetSegments (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, SObject *pData, SSegInfo *pSegInfo);
unsigned short actGetIOLayer (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uObj, SIOLayer *pIOLayer);
unsigned short actGetPrograms (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uObj, SIndex *pProg);
unsigned short actGetImageRegions (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uRd, unsigned short uWr, SRegion *pRd, SRegion *pWr);
unsigned short actGetTask (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uObj, STaskInfoVM **ppVM, STask *pTSK, unsigned short uTasks);
unsigned short actGetFileData (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned int hFile);
unsigned short actGetDownHeader (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, SDownHeader *pHeader);
unsigned short actGetDownDirect (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, SDownDirect *pDirect);
unsigned short actGetFileDef (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, SFileDef *pFileDef);
unsigned short actGetFileName (SDLBuffer *pDL, XBlock *pBlock, unsigned short *upCount, unsigned short uSize, char *szFile);






signed short utilIECStrCmp(IEC_STRING *pStr1, IEC_STRING *pStr2);
unsigned short utilIECStrToLong(IEC_STRING *inStr, unsigned short bUnsigned, char *szBuffer, signed int *lpValue);

signed short utilStrnicmp(char *pStr1, char *pStr2, unsigned int uLen);
signed short utilStricmp(char *pStr1, char *pStr2);

char *utilFormatString(char *szBuffer, char *szFormat, ...);
unsigned short utilFormatIECString(IEC_STRING *strIEC, char *szBuffer, char *szFormat, ...);
char *utilIecToAnsi(IEC_STRING *strIEC, char *szAnsi);
unsigned short utilAnsiToIec(char *szAnsi, IEC_STRING *strIEC);
unsigned short utilCopyIecString(IEC_STRING *strDest, IEC_STRING *strSrc);
unsigned short utilTruncCRLF(char *szString);

signed int utilRealToDint (float fValue, signed int lCast, signed int lMin, signed int lMax);
signed int utilLrealToDint(double fValue, signed int lCast, signed int lMin, signed int lMax);

unsigned short utilAppendPath(char *szDir, unsigned short uSize, char *szSub, unsigned char bDir);
unsigned char utilIs_8_3(char *szPath);

unsigned char utilCheckString(char *szString, unsigned short uMax);


unsigned short utilCreateDir (char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szFile);
unsigned short utilCreateFile(unsigned int *hpFile, char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szFile);
unsigned short utilOpenFile(unsigned int *hpFile, char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szFile, unsigned short uMode);
unsigned short utilExistFile (char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szFile, unsigned char *bpExist);
unsigned short utilDeleteFile(char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szFile);
unsigned short utilRenameFile(char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szFrom, char *szTo);
unsigned short utilCreatePath(char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szFile);



unsigned short utilDeleteList(char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szMap, char *szFile);
unsigned short utilCreateList(char *pBuffer, unsigned short uLen, unsigned short (* fpGetDir) (char *, unsigned short), char *szDir, char *szMap, char *szFile);


unsigned short utilPow2(unsigned short n);

unsigned int utilGetTimeDiffMS(unsigned long long ullStart);
unsigned long long utilGetTimeDiffUS(unsigned long long ullStart);

unsigned int utilGetTimeDiffMSEx(unsigned long long ullStart);
unsigned long long utilGetTimeDiffUSEx(unsigned long long ullStart);





unsigned short msgSend(SMessage *pMsg, unsigned short uQueue);
unsigned short msgRecv(SMessage *pMsg, unsigned short uQueue, unsigned int ulTimeOut);
unsigned short msgSendCommand(unsigned short uID, unsigned short uQueue, unsigned short uRespQueue);
unsigned short msgTXCommand(unsigned short uID, unsigned short uQueue, unsigned short uRespQueue, unsigned int ulTimeOut, unsigned char bSend);
unsigned short msgTXMessage(SMessage *pMsg, unsigned short uQueue, unsigned int ulTimeOut, unsigned char bSend);
unsigned short msgTransfer(SMessage *pMsg, unsigned short uQueue, unsigned int ulTimeOut, unsigned char bSend);
unsigned short msgSetError(SMessage *pMessage, unsigned short uError);





unsigned short ocMain(void *pPara);
unsigned short ocApply(STaskInfoVMM *pVMM);





unsigned short retMain(void *pPara);





unsigned short fwMain(void *pPara);





unsigned short vmMain(STaskInfoVM *pVM);

STaskInfoVM *vmCreateTaskInfoVM(unsigned short uTask, unsigned short uTasks);
unsigned short vmFreeTaskInfoVM(STaskInfoVM *pVM);

unsigned short vmSetException(STaskInfoVM *pVM, unsigned int ulErrNo);

unsigned short vmInitializeLocalSegments(STaskInfoVM *pVM);

  unsigned short vmFinalizeLocalSegments(STaskInfoVM *pVM);



  unsigned char vmmFindVarInCpyList(STaskInfoVMM *pVMM, XVariable *pxVar, unsigned int *pulOffset);
# 363 "../inc/vmmMain.h"
unsigned short prcGetTaskImage(STaskInfoVM *pVM);
unsigned short prcSetTaskImage(STaskInfoVM *pVM);
# 374 "../inc/vmmMain.h"
unsigned short ldInitTaskInfo(void);
unsigned short ldWriteTaskInfo(unsigned short uTask, unsigned int ulID);
unsigned short ldClearTaskInfo(unsigned short uTask);
unsigned short ldFindTask(unsigned int ulID, unsigned short *upTask);
unsigned short ldGetTaskName(unsigned short uTask, char *szTask);
unsigned short ldHandleSysLibCall(unsigned short wLoadType, unsigned short uTaskID, IEC_STRING *pLoad);
# 389 "../inc/vmmMain.h"
unsigned short dbiInitialize(STaskInfoVMM *pVMM);
unsigned short dbiFinalize(STaskInfoVMM *pVMM);
unsigned short dbiGetChildren(STaskInfoVMM *pVMM, SDBIInstance *pInst, unsigned char *pParent, unsigned short uParent, XBlock *pBlock, unsigned short *upProc);
unsigned short dbiGetAddress(STaskInfoVMM *pVMM, SDBIInstance *pInst, unsigned char *pParent, unsigned short uParent, XBlock *pBlock);
unsigned short dbiGetTaskNr(STaskInfoVMM *pVMM, unsigned char *pTask, XBlock *pBlock);
# 403 "../inc/vmmMain.h"
unsigned short fileInitialize(STaskInfoVMM *pVMM);
unsigned short fileOpen(unsigned int *hpFile, char *szName, unsigned short uMode, unsigned char bText);
unsigned short fileClose(unsigned int hFile);
unsigned short fileSeek(unsigned int hFile, unsigned int ulOffset, signed short iOrigin);
unsigned short fileRead(unsigned int hFile, unsigned char *pData, unsigned short *upLen);
unsigned short fileReadLine(unsigned int hFile, char *szData, unsigned short uLen);
unsigned short fileWrite(unsigned int hFile, unsigned char *pData, unsigned short uLen);
unsigned short fileWriteLine(unsigned int hFile, char *szData);
unsigned short fileCreateDir(char *szPath, unsigned char bDirOnly, unsigned short uMode);
unsigned short fileRename(char *szFrom, char *szTo);
unsigned short fileRemove(char *szFile);
unsigned short fileExist(char *szFile, unsigned char *bpExist);







unsigned short libGetFBCount(void);
unsigned short libGetFunCount(void);

unsigned short libSetError(STaskInfoVM *pVM, unsigned short uError);
unsigned short libSetException(STaskInfoVM *pVM, unsigned short uError);

unsigned short libGetTaskNo(STaskInfoVM *pVM, IEC_STRING __attribute__((packed)) *strTask, unsigned short *upTask);

unsigned int libSubstring(STaskInfoVM *pVM, IEC_STRING __attribute__((packed)) *in, IEC_STRING __attribute__((packed)) *out,
        unsigned int startPos, unsigned int endPos, unsigned char bAppend);







unsigned short ioCreateLayer(STaskInfoVMM *pVMM);
unsigned short ioTerminateLayer(STaskInfoVMM *pVMM, unsigned char bWarmStart);
unsigned short ioVerifyTermination(STaskInfoVMM *pVMM, unsigned int ulClearTime);
unsigned short ioDestroyLayer(STaskInfoVMM *pVMM);
unsigned short ioConfigLayer(STaskInfoVMM *pVMM);
unsigned short ioStartLayer(STaskInfoVMM *pVMM, unsigned char bStart, unsigned char bAlways);
unsigned short ioIsBootable(STaskInfoVMM *pVMM, unsigned char *bpBoot);
unsigned short ioNotifyLayer(STaskInfoVMM *pXXX, STaskInfoVM *pVM, unsigned char bSet, unsigned short uSegment, unsigned int ulOffset, unsigned short uLen, unsigned char usBit);
unsigned short ioOptimizeLayer(STaskInfoVMM *pVMM);
unsigned short ioMain(void *pPara);







unsigned short timMain(void *pPara);

unsigned short timEnableAllWDog(STaskInfoVMM *pVMM, unsigned char bEnable);
unsigned short timRestoreAllWDog(STaskInfoVMM *pVMM);





unsigned short sysInitialize(void);
unsigned short sysEnableIOLayer(char *szName);
unsigned short sysGetFeatureAvail(unsigned short *upFeature);
unsigned short sysGetFeatureLic(unsigned short *upFeature);
unsigned short sysGetVersion(char *szVersion);
unsigned short sysGetType(char *szType);
unsigned short sysGetVersionInfo(char *szVersion);
unsigned short sysGetCommPort(unsigned short *upPort);
unsigned char sysIsAvailable(unsigned short uFeature);
unsigned short sysGetMacAddr(char pMacAddr[6]);
unsigned short sysGetSerialNo(unsigned int *ulpSerialNo);
unsigned short sysGetInstKey(char *szInstKey);
unsigned short sysSetLicKey(char *szLicKey);





unsigned short osMain(unsigned short argc, char *argv[]);

unsigned short osInitializeVM(STaskInfoVM *pVM);

unsigned short osOnCmdReceived(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short osOnCmdHandled(STaskInfoVMM *pVMM, XBlock *pBlock, unsigned short uResult);

unsigned short osOnCmdComputed(STaskInfoVMM *pVMM, XBlock *pBlock);







unsigned short osCmdCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short osResCustom(STaskInfoVMM *pVMM, XBlock *pBlock);



unsigned short osBeginGetTaskImage(STaskInfoVM *pVM);
unsigned short osEndGetTaskImage(STaskInfoVM *pVM);
unsigned short osBeginSetTaskImage(STaskInfoVM *pVM);
unsigned short osEndSetTaskImage(STaskInfoVM *pVM);


unsigned short osInitializeSegment(unsigned short uSegment, SObject *pSegment);


unsigned short vmmCopyRegions(SIntShared *pShared, unsigned short uStart, unsigned short uCount, unsigned char bReverse);


char *osCreateMsgStr(unsigned short uMessage);
unsigned short osFreeMsgStr(unsigned short uMessage);


unsigned char __attribute__((packed)) *osCreateLocSegment(unsigned short uTask, unsigned short uSegment, unsigned int ulSize);
unsigned short osFreeLocSegment(unsigned short uTask, unsigned short uSegment, unsigned char __attribute__((packed)) **ppSegment);
unsigned char __attribute__((packed)) *osCreateWFSegment(unsigned short uTask, unsigned int ulSize);
unsigned short osFreeWFSegment(unsigned short uTask, unsigned char __attribute__((packed)) **ppSegment);


unsigned short osHandleException(STaskInfoVM *pVM, SException *pException);

unsigned short osNotifyGetValue(STaskInfoVMM *pVMM, unsigned char *pVal, unsigned short uSegment, unsigned int ulOffset, unsigned short uLen, unsigned char usBit);
unsigned short osNotifySetValue(STaskInfoVMM *pVMM, unsigned char *pVal, unsigned short uSegment, unsigned int ulOffset, unsigned short uLen, unsigned char usBit);

unsigned short osExecNativeCode(SIntShared *pShared, SIntLocal *pLocal, void *pIN, void *pSP, void *pIP);

unsigned short osGetProjectDir(char *szDir, unsigned short uSize);
unsigned short osGetFileDir(char *szDir, unsigned short uSize);
unsigned short osGetDBIDir(char *szDir, unsigned short uSize);
unsigned short osGetCustDownDir(char *szDir, unsigned short uSize);
unsigned short osGetFlashDir(char *szDir, unsigned short uSize);
unsigned short osGetTraceDir(char *szDir, unsigned short uSize);
unsigned short osGetRetainDir(char *szDir, unsigned short uSize);
unsigned short osGetFWDir(char *szDir, unsigned short uSize);
unsigned short osGetLoadDir(char *szDir, unsigned short uSize);
unsigned short osGetBACnetDBDir(char *szDir, unsigned short uSize);
unsigned short osGetBACnetFlashDir(char *szDir, unsigned short uSize);

unsigned short osCheckFilePath(char *szPath, unsigned short uCmd);
unsigned short osGetFreeDiskSpace(char *szPath, unsigned int *ulpFree);
unsigned short osTriggerSystemWatchdog(void);
unsigned short osReboot(void);

unsigned short osFWPrepareUpdate(XFWDownload *pxFW);
unsigned short osFWExecuteUpdate(void);

unsigned short osCmdSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);
unsigned short osResSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);
# 572 "../inc/vmmMain.h"
unsigned short osInitialize(void);
unsigned short osInitializeVMM(STaskInfoVMM *pVMM);

STaskInfoVMM *osCreateTaskInfoVMM(void);
SObject *osCreateCodeList(void);

STaskInfoVM *osCreateTaskInfoVM(unsigned short uTask);
unsigned short osFreeTaskInfoVM(STaskInfoVM *pVM);

unsigned short osCreateVMMembers(STaskInfoVM *pVM);
unsigned short osFreeVMMembers(STaskInfoVM *pVM);

SImageReg *osCreateImageReg(STaskInfoVM *pVM, unsigned short uTasks);
unsigned short osFreeImageReg(STaskInfoVM *pVM);

unsigned char osConvertVMPriority(unsigned char usPriority);
unsigned char osConvert4CPriority(unsigned char usPriority);


  unsigned short osCreateListenTask(SComTCP *pTCP);


unsigned short osCreateVMTask(STaskInfoVM *pVM);
unsigned short osKillVMTask(STaskInfoVM *pVM);
unsigned short osOnVMTerminate(STaskInfoVM *pVM);


unsigned short osCreateCommTask(SComTCP *pTCP);



unsigned short osCreateOnlineChangeTask(STaskInfoVMM *pVMM);



unsigned short osCreateVMTimerTask(STaskInfoVMM *pVMM);



unsigned short osCreateRetainTask(STaskInfoVMM *pVMM);
# 627 "../inc/vmmMain.h"
unsigned short osOnCommTerminate(STaskInfoVMM *pVMM, unsigned short uTask);


unsigned short osCreateSemaphore(unsigned short uSemaphore);
unsigned short osDeleteSemaphore(unsigned short uSemaphore);
unsigned short osBeginCriticalSection(unsigned short uSemaphore);
unsigned short osEndCriticalSection(unsigned short uSemaphore);

unsigned short *osCreateVMEvents(unsigned short uEvent);
unsigned short osFreeVMEvents(unsigned short uEvent);

unsigned short osOnClearBegin(STaskInfoVMM *pVMM);
unsigned short osOnClearAfterDelVM(STaskInfoVMM *pVMM);
unsigned short osOnClearEnd(STaskInfoVMM *pVMM);



unsigned int osGetTaskID(void);
# 654 "../inc/vmmMain.h"
unsigned short osInitIOLayer(STaskInfoVMM *pVMM);
unsigned short osEnableIOLayer(STaskInfoVMM *pVMM, unsigned short uIOLayer);
unsigned short osCreateIOLayer(STaskInfoVMM *pVMM, unsigned short uIOLayer);
unsigned short osKillIOLayer(STaskInfoVMM *pVMM, unsigned short uIOLayer);
# 668 "../inc/vmmMain.h"
unsigned short osClearFlash(unsigned short *upRetry);
unsigned short osFWInit(void);
unsigned short osFWFinish(void);
unsigned short osFlashWrite(unsigned char *pData, unsigned short uLen);
unsigned short osFRInit(void);
unsigned short osFRFinish(void);
unsigned short osFRInitDomain(unsigned short uDomain, unsigned int *ulpLen);
unsigned short osFRFinishDomain(unsigned short uDomain);
unsigned short osFlashRead(unsigned short uDomain, unsigned short uBlock, unsigned char *pData, unsigned short uLen);
# 686 "../inc/vmmMain.h"
unsigned short osGetFunCount(void);
unsigned short osGetFBCount(void);
# 716 "../inc/vmmMain.h"
unsigned short osSleep(unsigned int ulTime);

unsigned int osGetTime32(void);
unsigned long long osGetTime64(void);
unsigned long long osGetTimeUS(void);

unsigned int osGetTime32Ex(void);
unsigned long long osGetTime64Ex(void);
unsigned long long osGetTimeUSEx(void);


unsigned short osTrace(char *szFormat, ...);


unsigned char *osMalloc(unsigned int ulSize);
unsigned short osFree(unsigned char **pData);





unsigned short osInitializeShared(void);

unsigned short osCreateIPCQueue(unsigned short uQueue);
unsigned short osDestroyIPCQueue(unsigned short uQueue);

unsigned short osRecvMessage(SMessage *pMessage, unsigned short uQueue, unsigned int ulTimeOut);
unsigned short osSendMessage(SMessage *pMessage, unsigned short uQueue);
unsigned short osGetMessageCount(unsigned short uQueue, unsigned short *upMsgCount);







unsigned short osGetLoadAvg(float *fp1, float *fp5, float *fp15, unsigned int *ulpPReady, unsigned int *ulpPSum);
unsigned short osStrLoadAvg(char *szBuff, unsigned int uLen);

unsigned short osGetMemInfo(unsigned int *ulpTotal, unsigned int *ulpUsed, unsigned int *ulpFree);
unsigned short osStrMemInfo(char *szBuff, unsigned int uLen, unsigned int *ulpMemTotal);

unsigned short osGetStat(SProcTime *pPT);
unsigned short osStrStat(char *szBuff, unsigned int uLen);

unsigned short osGetStatDiff(SProcTime *pPT, SProcTime *pPT_Diff);
unsigned short osStrStatDiff(char *szBuff, unsigned int uLen, unsigned int *ulpOveral, SProcTime *pPT);

unsigned short osGetTaskStat(unsigned int ulID, STaskTime *pTT);
unsigned short osGetTaskStatDiff(unsigned int ulID, STaskTime *pTT, STaskTime *pTT_Diff);
unsigned short osStrTaskStatDiff(unsigned int ulID, char *szBuff, unsigned int uLen, unsigned int ulOveral, STaskTime *pTT);

unsigned short osGetTaskStatM(unsigned int ulID, unsigned int *ulpSize, unsigned int *ulpRes);
unsigned short osStrTaskStatM(unsigned int ulID, char *szBuff, unsigned int uLen, unsigned int ulOveral);
# 778 "../inc/vmmMain.h"
unsigned short intRun(STaskInfoVM *pVM);
unsigned short intInitialize(STaskInfoVM *pVM);

unsigned short intExecute(STaskInfoVM *pVM);

unsigned short intSetException(STaskInfoVM *pVM, unsigned short uErrNo, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
unsigned short intHalt(STaskInfoVM *pVM);





unsigned short flCleanUp(void);
# 72 "../inc/stdInc.h" 2



# 1 "../inc/osLast.h" 1
# 45 "../inc/osLast.h"
# 1 "../inc.fc/fcMain.h" 1
# 40 "../inc.fc/fcMain.h"
void *SOCKET_ListenThread(void *lpParam);
void *SOCKET_CommThread(void *lpParam);
void *VM_Proc(void *lpParam);
void *VMM_OnlineChangeThread(void *lpParam);
void *VM_TimerThread(void *lpParam);
void *IO_Layer_Test(void *lpParam);
void *IO_Layer_BACnet(void *lpParam);
void *IO_Layer_ProfiDP(void *lpParam);
void *VMM_RetainThread(void *lpParam);
void *BAC_DeviceThread(void *lpParam);
void *BAC_COVThread(void *lpParam);
void *BAC_ScanThread(void *lpParam);
void *BAC_FlashThread(void *lpParam);
void *BAC_ConfigThread(void *lpParam);
void *PDP_ManagementThread(void *lpParam);

void VMM_CleanUp_Mutex(void *lpParam);

void VM_CleanUp_Common(void *lpParam);
# 75 "../inc.fc/fcMain.h"
void *LED_Thread(void *lpParam);

unsigned short fcSetLed(unsigned short uLed, unsigned short uState);

unsigned short fcSetSystemWatchdog(void);

unsigned short fcInitJiffies(void);





void cleanup_comm_lib(void);
void cleanup_emb_comm_lib(void);
void cleanup_mbus_lib(void);
void cleanup_modbus_lib(void);
# 46 "../inc/osLast.h" 2
# 76 "../inc/stdInc.h" 2
# 33 "fz.c" 2



# 1 "libIec.h" 1
# 45 "libIec.h"
void LeftString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void RightString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void MidString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void ConcatString(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void InsertString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void DeleteString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void ReplaceString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void FindString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void LenString_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void bool_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void byte_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void word_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void dword_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void int_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void dint_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void real_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void lreal_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void string_to_bool(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_byte(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_word(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_dword(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_dint(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void string_to_time(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void iec_sin_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_sin_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_asin_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_asin_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_cos_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_cos_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_acos_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_acos_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_tan_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_tan_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_atan_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_atan_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_ln_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_ln_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_log_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_log_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_sqrt_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_sqrt_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_exp_real(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void iec_exp_lreal(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void time32_to_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void mux_bit(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void mux_byte(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void mux_int(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void mux_dint(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void mux_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void mux_64bit(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void trunc_real_dint(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void trunc_lreal_dint(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);




void rs (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void sr (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void tp (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void ton (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void tof (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void ctu (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void ctd (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void ctud (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void r_trig (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void f_trig (STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);



# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 123 "libIec.h" 2







 typedef struct LEN_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed short retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));
 } LEN_STRING_PAR_TYP;


 typedef struct LEFT_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed short len __attribute__ ((packed)); unsigned short dummy_16_len __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } LEFT_STRING_PAR_TYP;


 typedef struct RIGHT_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed short len __attribute__ ((packed)); unsigned short dummy_16_len __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } RIGHT_STRING_PAR_TYP;

 typedef struct MID_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed short len __attribute__ ((packed)); unsigned short dummy_16_len __attribute__ ((packed));
  signed short startPos __attribute__ ((packed)); unsigned short dummy_16_startPos __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } MID_STRING_PAR_TYP;

 typedef struct CONCAT_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } CONCAT_STRING_PAR_TYP;

 typedef struct INSERT_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr1 __attribute__ ((packed));
  IEC_STRING * inStr2 __attribute__ ((packed));
  signed short startPos __attribute__ ((packed)); unsigned short dummy_16_startPos __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } INSERT_STRING_PAR_TYP;

 typedef struct DELETE_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed short len __attribute__ ((packed)); unsigned short dummy_16_len __attribute__ ((packed));
  signed short startPos __attribute__ ((packed)); unsigned short dummy_16_startPos __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } DELETE_STRING_PAR_TYP;

 typedef struct REPLACE_STRING_PAR_TYP_TAG
 {
  IEC_STRING * inStr1 __attribute__ ((packed));
  IEC_STRING * inStr2 __attribute__ ((packed));
  signed short len __attribute__ ((packed)); unsigned short dummy_16_len __attribute__ ((packed));
  signed short startPos __attribute__ ((packed)); unsigned short dummy_16_startPos __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } REPLACE_STRING_PAR_TYP;

 typedef struct
 {
  IEC_STRING * inStr1 __attribute__ ((packed));
  IEC_STRING * inStr2 __attribute__ ((packed));
  signed short iRetVal __attribute__ ((packed)); unsigned short dummy_16_iRetVal __attribute__ ((packed));

 } FIND_STRING_PAR_TYP;



 typedef struct STRING_TO_BOOL_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  unsigned char retVal __attribute__ ((packed)); unsigned char dummy_08_retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));
 } STRING_TO_BOOL;



 typedef struct STRING_TO_BYTE_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  unsigned char retVal __attribute__ ((packed)); unsigned char dummy_08_retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));
 } STRING_TO_BYTE;



 typedef struct STRING_TO_WORD_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  unsigned short retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));
 } STRING_TO_WORD;



 typedef struct STRING_TO_DWORD_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  unsigned retVal __attribute__ ((packed));
 } STRING_TO_DWORD;



 typedef struct STRING_TO_DINT_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed int retVal __attribute__ ((packed));
 } STRING_TO_DINT;



 typedef struct STRING_TO_INT_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed short retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));
 } STRING_TO_INT;



 typedef struct STRING_TO_REAL_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  float retVal __attribute__ ((packed));
 } STRING_TO_REAL;



 typedef struct STRING_TO_LREAL_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  double retVal __attribute__ ((packed));
 } STRING_TO_LREAL;



 typedef struct STRING_TO_TIME_TAG
 {
  IEC_STRING * inStr __attribute__ ((packed));
  signed long retVal __attribute__ ((packed));
 } STRING_TO_TIME;



 typedef struct BOOL_TO_STRING_PAR_TAG{
  unsigned char in __attribute__ ((packed)); unsigned char dummy_08_in __attribute__ ((packed)); unsigned short dummy_16_in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } BOOL_TO_STRING_PAR;



 typedef struct BYTE_TO_STRING_PAR_TAG{
  unsigned char in __attribute__ ((packed)); unsigned char dummy_08_in __attribute__ ((packed)); unsigned short dummy_16_in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } BYTE_TO_STRING_PAR;



 typedef struct WORD_TO_STRING_PAR_TAG{
  unsigned short in __attribute__ ((packed)); unsigned short dummy_16_in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } WORD_TO_STRING_PAR;



 typedef struct DWORD_TO_STRING_PAR_TAG
 {
  unsigned in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } DWORD_TO_STRING_PAR;



 typedef struct INT_TO_STRING_PAR_TAG
 {
  signed short in __attribute__ ((packed)); unsigned short dummy_16_in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } INT_TO_STRING_PAR;



 typedef struct DINT_TO_STRING_PAR_TAG
 {
  signed int in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } DINT_TO_STRING_PAR;



 typedef struct REAL_TO_STRING_PAR_TAG
 {
  float in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } REAL_TO_STRING_PAR;



 typedef struct LREAL_TO_STRING_PAR_TAG
 {
  double in __attribute__ ((packed));
  IEC_STRING __attribute__((packed))* retStr;
 } LREAL_TO_STRING_PAR;



 typedef struct SIN_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } SIN_REAL_PAR;



 typedef struct SIN_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } SIN_LREAL_PAR;



 typedef struct ASIN_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } ASIN_REAL_PAR;



 typedef struct ASIN_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } ASIN_LREAL_PAR;



 typedef struct COS_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } COS_REAL_PAR;



 typedef struct COS_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } COS_LREAL_PAR;



 typedef struct ACOS_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } ACOS_REAL_PAR;



 typedef struct ACOS_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } ACOS_LREAL_PAR;



 typedef struct TAN_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } TAN_REAL_PAR;



 typedef struct TAN_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } TAN_LREAL_PAR;



 typedef struct ATAN_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } ATAN_REAL_PAR;



 typedef struct ATAN_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } ATAN_LREAL_PAR;



 typedef struct LN_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } LN_REAL_PAR;



 typedef struct LN_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } LN_LREAL_PAR;



 typedef struct LOG_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } LOG_REAL_PAR;



 typedef struct LOG_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } LOG_LREAL_PAR;



 typedef struct SQRT_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } SQRT_REAL_PAR;



 typedef struct SQRT_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } SQRT_LREAL_PAR;



 typedef struct EXP_REAL_PAR_TAG
 {
  float in __attribute__ ((packed));
  float ret __attribute__ ((packed));
 } EXP_REAL_PAR;



 typedef struct EXP_LREAL_PAR_TAG
 {
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } EXP_LREAL_PAR;



 typedef struct TIME_TO_STRING_PAR_TAG
 {
  signed long in __attribute__ ((packed));
  IEC_STRING * retStr __attribute__ ((packed));
 } TIME_TO_STRING_PAR;



 typedef struct MUX_BIT_PAR_TYP_TAG
 {
  signed short sel __attribute__ ((packed)); unsigned short dummy_16_sel __attribute__ ((packed));
  unsigned char in __attribute__ ((packed)); unsigned char dummy_08_in __attribute__ ((packed)); unsigned short dummy_16_in __attribute__ ((packed));
  unsigned char ret __attribute__ ((packed)); unsigned char dummy_08_ret __attribute__ ((packed)); unsigned short dummy_16_ret __attribute__ ((packed));
 } MUX_BIT_PAR_TYP;



 typedef struct MUX_BYTE_PAR_TYP_TAG
 {
  signed short sel __attribute__ ((packed)); unsigned short dummy_16_sel __attribute__ ((packed));
  unsigned char in __attribute__ ((packed)); unsigned char dummy_08_in __attribute__ ((packed)); unsigned short dummy_16_in __attribute__ ((packed));
  unsigned char ret __attribute__ ((packed)); unsigned char dummy_08_ret __attribute__ ((packed)); unsigned short dummy_16_ret __attribute__ ((packed));
 } MUX_BYTE_PAR_TYP;



 typedef struct MUX_INT_PAR_TYP_TAG
 {
  signed short sel __attribute__ ((packed)); unsigned short dummy_16_sel __attribute__ ((packed));
  signed short in __attribute__ ((packed)); unsigned short dummy_16_in __attribute__ ((packed));
  signed short ret __attribute__ ((packed)); unsigned short dummy_16_ret __attribute__ ((packed));
 } MUX_INT_PAR_TYP;



 typedef struct MUX_DINT_PAR_TYP_TAG
 {
  signed short sel __attribute__ ((packed)); unsigned short dummy_16_sel __attribute__ ((packed));
  signed int in __attribute__ ((packed));
  signed int ret __attribute__ ((packed));
 } MUX_DINT_PAR_TYP;



 typedef struct MUX_STRING_PAR_TYP_TAG
 {
  signed short sel __attribute__ ((packed)); unsigned short dummy_16_sel __attribute__ ((packed));
  IEC_STRING * in __attribute__ ((packed));
  IEC_STRING * ret __attribute__ ((packed));
 } MUX_STRING_PAR_TYP;



 typedef struct MUX_64BIT_PAR_TYP_TAG
 {
  signed short sel __attribute__ ((packed)); unsigned short dummy_16_sel __attribute__ ((packed));
  double in __attribute__ ((packed));
  double ret __attribute__ ((packed));
 } MUX_64BIT_PAR_TYP;



 typedef struct TRUNC_REAL_DINT_PAR_TYP_TAG
 {
  float in __attribute__ ((packed));
  signed int ret __attribute__ ((packed));
 } TRUNC_REAL_DINT_PAR_TYP;



 typedef struct TRUNC_LREAL_DINT_PAR_TYP_TAG
 {
  double in __attribute__ ((packed));
  signed int ret __attribute__ ((packed));
 } TRUNC_LREAL_DINT_PAR_TYP;


 typedef struct FUNC_ALI_TEST_TAG
 {
  unsigned char bDummy __attribute__ ((packed)); unsigned char dummy_08_bDummy __attribute__ ((packed)); unsigned short dummy_16_bDummy __attribute__ ((packed));
  double lDummy __attribute__ ((packed));

 } FUNC_ALI_TEST;
# 782 "libIec.h"
 typedef struct
 {
  unsigned char b __attribute__ ((packed));

 } SR_TYP;

 typedef struct
 {
  unsigned char b __attribute__ ((packed));

 } RS_TYP;



 typedef struct
 {


  unsigned char IN __attribute__ ((packed));
  unsigned char dummy_08_bIn __attribute__ ((packed));
  unsigned short dummy_16_bIn __attribute__ ((packed));
  unsigned int PT __attribute__ ((packed));



  unsigned char Q __attribute__ ((packed));
  unsigned char dummy_08_bOut __attribute__ ((packed));
  unsigned short dummy_16_bOut __attribute__ ((packed));
  unsigned int ET __attribute__ ((packed));



  unsigned int tStart __attribute__ ((packed));
  unsigned char bTrig __attribute__ ((packed));
  unsigned char LastIN __attribute__ ((packed));
  unsigned short dummy_16_m __attribute__ ((packed));

 } TP_TON_TOF_PAR;



 typedef struct
 {


  unsigned char bIn __attribute__ ((packed));
  unsigned char dummy_08_bIn __attribute__ ((packed));
  signed short pvIn __attribute__ ((packed));



  unsigned char qOut __attribute__ ((packed));
  unsigned char dummy_08_bOut __attribute__ ((packed));
  signed short cvOut __attribute__ ((packed));

 } CTU_PAR;

 typedef struct
 {


  unsigned char bIn __attribute__ ((packed));
  unsigned char dummy_08_bIn __attribute__ ((packed));
  signed short pvIn __attribute__ ((packed));



  unsigned char qOut __attribute__ ((packed));
  unsigned char dummy_08_bOut __attribute__ ((packed));
  signed short cvOut __attribute__ ((packed));

 } CTD_PAR;

 typedef struct
 {


  unsigned char bIn __attribute__ ((packed));
  unsigned char dummy_08_bIn __attribute__ ((packed));
  signed short pvIn __attribute__ ((packed));



  unsigned char bOut __attribute__ ((packed));
  unsigned char dummy_08_bOut __attribute__ ((packed));
  signed short cvOut __attribute__ ((packed));

 } CTUD_PAR;



 typedef struct
 {


  unsigned char b __attribute__ ((packed));

 } TRIG_PAR;




 typedef struct
 {
  unsigned char bDummy __attribute__ ((packed));
  double lDummy __attribute__ ((packed));

 } FB_ALI_TEST;


# 1 "../inc/osAlign.h" 1
# 39 "../inc/osAlign.h"
#pragma pack()
# 893 "libIec.h" 2
# 37 "fz.c" 2



# 1 "libSys.h" 1
# 40 "libSys.h"
void ClearTaskErrno(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void GetLocalTaskErrno(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void GetLocalTaskState(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void GetTaskErrno(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void GetTimeSinceSystemBoot(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void OutputDebugString(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void SignalError(STaskInfoVM *pVM, unsigned char *pIP, unsigned char __attribute__((packed)) *pIN);
void StartLocalTask(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void StopLocalTask(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);



void ClearLocalTaskErrno(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void ThrowException(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void GetLocalTaskInfo(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);



# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 59 "libSys.h" 2


typedef struct
{
 signed int lState __attribute__ ((packed));
 signed int lPriority __attribute__ ((packed));
 signed int tCycle __attribute__ ((packed));
 signed int tETMax __attribute__ ((packed));
 signed int lErrno __attribute__ ((packed));

} TASKINFO;





typedef struct
{
 IEC_STRING * strTask __attribute__ ((packed));
 signed int lRet __attribute__ ((packed));

} SGetLocalTaskErrno;

typedef struct
{
 IEC_STRING * strTask __attribute__ ((packed));
 signed int lRet __attribute__ ((packed));

} SGetLocalTaskState;

typedef struct
{
 signed int lRet __attribute__ ((packed));

} SGetTaskErrno;

typedef struct
{
 signed long tRet __attribute__ ((packed));

} SGetTimeSinceSystemBoot;

typedef struct
{
 IEC_STRING * strMsg __attribute__ ((packed));

} SOutputDebugString;
# 127 "libSys.h"
typedef struct
{
 unsigned char bEnable __attribute__ ((packed));
 unsigned char dummy_08_bEnable __attribute__ ((packed));
 unsigned short dummy_16_bEnable __attribute__ ((packed));
 signed int lErrNo __attribute__ ((packed));

} SSignalError;


typedef struct
{
 IEC_STRING * strTask __attribute__ ((packed));

} SStartLocalTask;

typedef struct
{
 IEC_STRING * strTask __attribute__ ((packed));

} SStopLocalTask;

typedef struct
{
 IEC_STRING * strTask __attribute__ ((packed));

} SClearLocalTaskErrno;

typedef struct
{
 IEC_STRING * strTask __attribute__ ((packed));
 TASKINFO * pInfo __attribute__ ((packed));

} SGetLocalTaskInfo;


# 1 "../inc/osAlign.h" 1
# 39 "../inc/osAlign.h"
#pragma pack()
# 164 "libSys.h" 2
# 41 "fz.c" 2



# 1 "libUtil.h" 1
# 34 "libUtil.h"
void copy_dword_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_dword_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_lword_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_lword_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_int_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_int_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_dint_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_dint_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_lint_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_lint_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_lreal_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_lreal_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_real_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_real_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_bool_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_bool_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_time_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_time_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_byte_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_byte_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_word_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_word_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_string_to_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void copy_string_from_byte_array(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void byte_swap_word(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void byte_swap_dword(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void byte_swap_lword(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void hi_byte(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void hi_word(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void hi_dword(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void lo_byte(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void lo_word(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void lo_dword(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void make_word(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void make_dword(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void make_lword(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
# 87 "libUtil.h"
# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 88 "libUtil.h" 2



typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 unsigned retVal __attribute__ ((packed));

} COPY_DWORD_FROM_BA_PAR;

typedef struct
{
 unsigned inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_DWORD_TO_BA_PAR;
# 127 "libUtil.h"
typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 signed int retVal __attribute__ ((packed));

} COPY_INT_FROM_BA_PAR;

typedef struct
{
 signed short inVal __attribute__ ((packed)); unsigned short dummy_16_inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_INT_TO_BA_PAR;



typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 signed int retVal __attribute__ ((packed));

} COPY_DINT_FROM_BA_PAR;

typedef struct
{
 signed int inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_DINT_TO_BA_PAR;
# 181 "libUtil.h"
typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 double retVal __attribute__ ((packed));

} COPY_LREAL_FROM_BA_PAR;

typedef struct
{
 double inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_LREAL_TO_BA_PAR;



typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 float retVal __attribute__ ((packed));

} COPY_REAL_FROM_BA_PAR;

typedef struct
{
 float inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_REAL_TO_BA_PAR;



typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 unsigned char retVal __attribute__ ((packed)); unsigned char dummy_08_retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));

} COPY_BOOL_FROM_BA_PAR;

typedef struct
{
 unsigned char inVal __attribute__ ((packed)); unsigned char dummy_08_inVal __attribute__ ((packed)); unsigned short dummy_16_inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_BOOL_TO_BA_PAR;



typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 signed long retVal __attribute__ ((packed));

} COPY_TIME_FROM_BA_PAR;

typedef struct
{
 signed long inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_TIME_TO_BA_PAR;



typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 unsigned char retVal __attribute__ ((packed)); unsigned char dummy_08_retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));

} COPY_BYTE_FROM_BA_PAR;

typedef struct
{
 unsigned char inVal __attribute__ ((packed)); unsigned char dummy_08_inVal __attribute__ ((packed)); unsigned short dummy_16_inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_BYTE_TO_BA_PAR;



typedef struct
{
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 unsigned short retVal __attribute__ ((packed)); unsigned short dummy_16_retVal __attribute__ ((packed));

} COPY_WORD_FROM_BA_PAR;

typedef struct
{
 unsigned short inVal __attribute__ ((packed)); unsigned short dummy_16_inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_WORD_TO_BA_PAR;



typedef struct
{

 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));
 IEC_STRING * retVal __attribute__ ((packed));

} COPY_STRING_FROM_BA_PAR;

typedef struct
{
 IEC_STRING * inVal __attribute__ ((packed));
 signed int * pPos __attribute__ ((packed));
 unsigned char * pbArray __attribute__ ((packed));

} COPY_STRING_TO_BA_PAR;



typedef struct
{
 unsigned short wVal __attribute__ ((packed)); unsigned short dummy_16_wVal __attribute__ ((packed));
 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} BYTE_SWAP_WORD_PAR;



typedef struct
{
 unsigned dwVal __attribute__ ((packed));
 unsigned dwRet __attribute__ ((packed));

} BYTE_SWAP_DWORD_PAR;
# 335 "libUtil.h"
typedef struct
{
 unsigned short wVal __attribute__ ((packed)); unsigned short dummy_16_wVal __attribute__ ((packed));
 unsigned char byRet __attribute__ ((packed)); unsigned char dummy_08_byRet __attribute__ ((packed)); unsigned short dummy_16_byRet __attribute__ ((packed));

} HI_BYTE_PAR;



typedef struct
{
 unsigned dwVal __attribute__ ((packed));
 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} HI_WORD_PAR;
# 362 "libUtil.h"
typedef struct
{
 unsigned short wVal __attribute__ ((packed)); unsigned short dummy_16_wVal __attribute__ ((packed));
 unsigned char byRet __attribute__ ((packed)); unsigned char dummy_08_byRet __attribute__ ((packed)); unsigned short dummy_16_byRet __attribute__ ((packed));

} LO_BYTE_PAR;



typedef struct
{
 unsigned dwVal __attribute__ ((packed));
 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} LO_WORD_PAR;
# 389 "libUtil.h"
typedef struct
{
 unsigned char byLO __attribute__ ((packed)); unsigned char dummy_08_byLO __attribute__ ((packed)); unsigned short dummy_16_byLO __attribute__ ((packed));
 unsigned char byHI __attribute__ ((packed)); unsigned char dummy_08_byHI __attribute__ ((packed)); unsigned short dummy_16_byHI __attribute__ ((packed));
 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} MAKE_WORD_PAR;



typedef struct
{
 unsigned short wLO __attribute__ ((packed)); unsigned short dummy_16_wLO __attribute__ ((packed));
 unsigned short wHI __attribute__ ((packed)); unsigned short dummy_16_wHI __attribute__ ((packed));
 unsigned dwRet __attribute__ ((packed));

} MAKE_DWORD_PAR;
# 420 "libUtil.h"
# 1 "../inc/osAlign.h" 1
# 39 "../inc/osAlign.h"
#pragma pack()
# 421 "libUtil.h" 2
# 45 "fz.c" 2



# 1 "libSfc.h" 1
# 44 "libSfc.h"
void sfc_calcdotrans (STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void sfc_calcdoact (STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
void sfc_finalise (STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
# 49 "fz.c" 2



# 1 "libSys2.h" 1
# 35 "libSys2.h"
void EVT_GetException(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void EVT_Set(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);


void MSG_SendToEng(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MSG_Trace(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);



void TIM_Get(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_ClearError(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_Exception(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetCount(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetError(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetInfo(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetMyNumber(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetName(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetState(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_Start(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_Stop(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void WD_Disable(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void WD_Enable(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);



void EVT_GetEvent(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void WD_IsEnabled(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetMyName(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void SYS_Coldstart(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void SYS_Reboot(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void SYS_RetainWrite(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void SYS_Warmstart(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void LD_Get(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void WD_SetTrigger(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void WD_GetTrigger(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_GetStatistic(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void TSK_ClearStatistic(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);


# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 100 "libSys2.h" 2




typedef struct
{
 unsigned short * upTask __attribute__ ((packed));
 unsigned int * ulpEx __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_EVT_GetException;



typedef struct
{
 unsigned short uEvent __attribute__ ((packed)); unsigned short dummy_16_uEvent __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_EVT_Set;





typedef struct
{
 IEC_STRING * sMsg __attribute__ ((packed));

} S_MSG_SendToEng;



typedef struct
{
 IEC_STRING * sMsg __attribute__ ((packed));

} S_MSG_Trace;
# 148 "libSys2.h"
typedef struct
{
 signed long tRet __attribute__ ((packed));

} S_TIM_Get;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_ClearError;



typedef struct
{
 unsigned short uException __attribute__ ((packed)); unsigned short dummy_16_uException __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_Exception;



typedef struct
{
 signed short iRet __attribute__ ((packed)); unsigned short dummy_16_iRet __attribute__ ((packed));

} S_TSK_GetCount;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));
 unsigned int * ulErrNo __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_GetError;



typedef struct
{
 unsigned int ulState __attribute__ ((packed));
 signed int ulPriority __attribute__ ((packed));
 unsigned int tCycle __attribute__ ((packed));
 unsigned int tETMax __attribute__ ((packed));
 signed int ulErrNo __attribute__ ((packed));

} ETaskInfo;

typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));
 ETaskInfo * pInfo __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_GetInfo;



typedef struct
{
 unsigned short uTask __attribute__ ((packed)); unsigned short dummy_16_uTask __attribute__ ((packed));

} S_TSK_GetMyNumber;



typedef struct
{
 unsigned short uTask __attribute__ ((packed)); unsigned short dummy_16_uTask __attribute__ ((packed));
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_GetName;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));
 unsigned int * ulErrNo __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_GetState;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_Start;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_Stop;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_WD_Disable;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_WD_Enable;







typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));
 unsigned short * upEvent __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_EVT_GetEvent;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));
 unsigned char * bEnabled __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_WD_IsEnabled;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

} S_TSK_GetMyName;



typedef struct
{
 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_SYS_Coldstart;



typedef struct
{
 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_SYS_Reboot;



typedef struct
{

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_SYS_RetainWrite;



typedef struct
{
 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_SYS_Warmstart;



typedef struct
{
 unsigned short wLoadType __attribute__ ((packed)); unsigned short dummy_16_wLoadType __attribute__ ((packed));
 unsigned short uTaskID __attribute__ ((packed)); unsigned short dummy_16_uTaskID __attribute__ ((packed));

 IEC_STRING * pLoad __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_LD_Get;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned int ulWDogTrigger __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_WD_SetTrigger;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned int * ulpWDogTrigger __attribute__ ((packed));
 unsigned int * ulpWDogCounter __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_WD_GetTrigger;



typedef struct
{
 unsigned int ulTaskState __attribute__ ((packed));
 unsigned int ulCycleTime __attribute__ ((packed));
 unsigned int ulExeMax __attribute__ ((packed));
 unsigned int ulExeMin __attribute__ ((packed));
 unsigned int ulExeAver __attribute__ ((packed));
 unsigned int ulExeCount __attribute__ ((packed));
 unsigned int ulWDCounter __attribute__ ((packed));
 unsigned int ulWDTrigger __attribute__ ((packed));

} ETaskStat;

typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));
 ETaskStat * pStat __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_GetStatistic;



typedef struct
{
 IEC_STRING * sTask __attribute__ ((packed));

 unsigned short wRet __attribute__ ((packed)); unsigned short dummy_16_wRet __attribute__ ((packed));

} S_TSK_ClearStatistic;


# 1 "../inc/osAlign.h" 1
# 39 "../inc/osAlign.h"
#pragma pack()
# 425 "libSys2.h" 2
# 53 "fz.c" 2



# 1 "libFile.h" 1
# 35 "libFile.h"
void fa_sync_openFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_readFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_renameFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_writeFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_closeFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_createDirectory(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_deleteFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_existsFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_getSize(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_get_diskFreeSpace(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_error_string(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_arrayReadFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_sync_arrayWriteFile(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void fa_flush(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);
# 106 "libFile.h"
# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 107 "libFile.h" 2







typedef struct
{
 unsigned char CurLen;
 unsigned char MaxLen;
 char Contents[255];

} LOC_STRING;
# 153 "libFile.h"
typedef struct
{
 LOC_STRING sName __attribute__ ((packed));
 unsigned char dummy_08_fileName __attribute__ ((packed));
 unsigned short dummy_16_fileName __attribute__ ((packed));
 unsigned int ulMode __attribute__ ((packed));
 signed int lPos __attribute__ ((packed));
 unsigned int ulError __attribute__ ((packed));
 unsigned char bOpen __attribute__ ((packed));

} FA_SYNC_FILE;




typedef struct fa_fd_pair_tag
{
 FA_SYNC_FILE* pFile;
 unsigned int hFile;
 struct fa_fd_pair_tag *pNext;

} fa_fd_pair, fa_fd_map;




typedef struct
{
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_OPENFILE_PAR;



typedef struct
{
 FA_SYNC_FILE * file __attribute__ ((packed));
 unsigned int * ulLength __attribute__ ((packed));
 IEC_STRING * pData __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_READFILE_PAR;



typedef struct
{
 unsigned int ulPos __attribute__ ((packed));
 FA_SYNC_FILE * file __attribute__ ((packed));
 unsigned int * ulLength __attribute__ ((packed));
 unsigned char * pData __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_ARRAYREADFILE_PAR;



typedef struct
{
 IEC_STRING * newName __attribute__ ((packed));
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_RENAMEFILE_PAR;



typedef struct
{
 IEC_STRING * pData __attribute__ ((packed));
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_WRITEFILE_PAR;



typedef struct
{
 unsigned char * pData __attribute__ ((packed));
 unsigned int ulPos __attribute__ ((packed));
 unsigned int ulLen __attribute__ ((packed));
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_ARRAYWRITEFILE_PAR;



typedef struct
{
 unsigned int ulRet __attribute__ ((packed));

} FA_FLUSH_PAR;



typedef struct
{
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_CLOSEFILE_PAR;



typedef struct
{
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_CREATEDIRECTORY_PAR;



typedef struct
{
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_DELETEFILE_PAR;



typedef struct
{
 FA_SYNC_FILE * file __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_EXISTSFILE_PAR;



typedef struct
{
 FA_SYNC_FILE * file __attribute__ ((packed));
 signed int * lpSize __attribute__ ((packed));

 unsigned char bRet __attribute__ ((packed)); unsigned char dummy_08_bRet __attribute__ ((packed)); unsigned short dummy_16_bRet __attribute__ ((packed));

} FA_SYNC_GETSIZE_PAR;



typedef struct
{
 IEC_STRING * sPath __attribute__ ((packed));
 signed int * lpFree __attribute__ ((packed));

 unsigned int ulRet __attribute__ ((packed));

} FA_GET_DISKFREESPACE_PAR;



typedef struct
{
 signed int lError __attribute__ ((packed));

 IEC_STRING * sError __attribute__ ((packed));

} FA_ERROR_STRING_PAR;



# 1 "../inc/osAlign.h" 1
# 39 "../inc/osAlign.h"
#pragma pack()
# 330 "libFile.h" 2
# 57 "fz.c" 2



# 1 "../inc/libMBus2.h" 1
# 61 "fz.c" 2



# 1 "../inc.bac/bacFun.h" 1
# 65 "fz.c" 2



# 1 "libMBRTU.h" 1
# 27 "libMBRTU.h"
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/termios.h" 1 3 4
# 36 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/termios.h" 3 4




# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/termios.h" 1 3 4
# 25 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/bits/termios.h" 3 4
typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;


struct termios
  {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_line;
    cc_t c_cc[32];
    speed_t c_ispeed;
    speed_t c_ospeed;


  };
# 41 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/termios.h" 2 3 4
# 49 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/termios.h" 3 4
extern speed_t cfgetospeed (__const struct termios *__termios_p) __attribute__ ((__nothrow__));


extern speed_t cfgetispeed (__const struct termios *__termios_p) __attribute__ ((__nothrow__));


extern int cfsetospeed (struct termios *__termios_p, speed_t __speed) __attribute__ ((__nothrow__));


extern int cfsetispeed (struct termios *__termios_p, speed_t __speed) __attribute__ ((__nothrow__));



extern int cfsetspeed (struct termios *__termios_p, speed_t __speed) __attribute__ ((__nothrow__));




extern int tcgetattr (int __fd, struct termios *__termios_p) __attribute__ ((__nothrow__));



extern int tcsetattr (int __fd, int __optional_actions,
        __const struct termios *__termios_p) __attribute__ ((__nothrow__));




extern void cfmakeraw (struct termios *__termios_p) __attribute__ ((__nothrow__));



extern int tcsendbreak (int __fd, int __duration) __attribute__ ((__nothrow__));





extern int tcdrain (int __fd);



extern int tcflush (int __fd, int __queue_selector) __attribute__ ((__nothrow__));



extern int tcflow (int __fd, int __action) __attribute__ ((__nothrow__));
# 105 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/termios.h" 3 4
# 1 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/sys/ttydefaults.h" 1 3 4
# 106 "/imx_mect/trunk/imx28/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/../arm-none-linux-gnueabi/sysroot/usr/include/termios.h" 2 3 4



# 28 "libMBRTU.h" 2



# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 32 "libMBRTU.h" 2
# 75 "libMBRTU.h"
typedef struct {
        unsigned char *data;
        int data_size;
        int data_start;
        int data_end;
        int max_data_start;
        } app_mbrtu_lb_buf_t;
# 121 "libMBRTU.h"
typedef struct {
        app_mbrtu_lb_buf_t data_buf;
        int found_frame_boundary;
        int frame_search_history;
        } app_mbrtu_recv_buf_t;
# 137 "libMBRTU.h"
typedef struct {
        int fd;
        struct timeval time_15_char;
        struct timeval time_35_char;
        app_mbrtu_recv_buf_t recv_buf;
        struct termios old_tty_settings;
 char device[15];
        int baud;
        int parity;
        int data_bits;
        int stop_bits;
 int ignore_echo;







         } app_mbrtu_master_node_t;




app_mbrtu_master_node_t *mbrtu_master;
# 177 "libMBRTU.h"
int app_mbrtu_init_cfg(int confbaud, int confdatabits, int confparity, int confstopbits, int ignore_echo);
int app_mbrtu_init( void );


int app_mbrtu_done(void);





int app_mbrtu_connect(app_mbrtu_master_node_t *node);
# 216 "libMBRTU.h"
unsigned short int fn_01_read_coil_status(unsigned short int slave, unsigned short int start_addr, unsigned short int count);

unsigned short int fn_02_read_input_status(unsigned short int slave, unsigned short int start_addr, unsigned short int count);

unsigned short int fn_03_read_holding_register(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned char byteorder);

unsigned short int fn_04_read_input_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned char byteorder);

unsigned short int fn_05_force_single_coil(unsigned short int slave, unsigned short int coil_addr, unsigned short int coil_state);

unsigned short int fn_06_preset_single_register(unsigned short int slave, unsigned short int reg_addr);

unsigned short int fn_07_read_exception_status(unsigned short int slave);

unsigned short int fn_0b_read_event_counter(unsigned short int slave);

unsigned short int fn_0c_read_event_log(unsigned short int slave);

unsigned short int fn_0f_force_multiple_coil(unsigned short int slave, unsigned short int coil_addr, unsigned short int coil_count);

unsigned short int fn_10_preset_multiple_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count);

unsigned short int fn_11_report_slave_id(unsigned short int slave);

unsigned short int fn_14_read_general_reference(unsigned short int slave );

unsigned short int fn_15_write_general_reference(unsigned short int slave, unsigned short int byte_count );

unsigned short int fn_16_mask_write_registers(unsigned short int slave, unsigned short int start_addr);

unsigned short int fn_17_read_write_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned short int reg_addr,unsigned short int reg_count, unsigned char byteorder);

unsigned short int fn_18_read_fifo_queue(unsigned short int slave, unsigned short int start_addr);

unsigned short int fn_08_diagnostics(unsigned short int slave, unsigned short int subfunction );



unsigned short int fn_response_byteorder_invert(unsigned short int count);


unsigned short int MB_byteorder(unsigned short int count);

short MODBUSstatus[3];
unsigned short MODBUSdata[257];



void MB_f01(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f02(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f03(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f04(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f05(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f06(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f07(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f08(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f0b(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f0c(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f0f(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f10(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f11(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f14(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f15(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f16(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f17(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_f18(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_data(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MB_get_status(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);




typedef struct
{
 unsigned short uLen __attribute__ ((packed));
 unsigned short pElem[257] __attribute__ ((packed));

}MB_DATA_ARRAY_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));
 unsigned short count __attribute__ ((packed)); unsigned short dummy_16_count __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F01_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));
 unsigned short count __attribute__ ((packed)); unsigned short dummy_16_count __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F02_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));
 unsigned short count __attribute__ ((packed)); unsigned short dummy_16_count __attribute__ ((packed));
 char byteorder __attribute__ ((packed)); unsigned char dummy_08_byteorder __attribute__ ((packed)); unsigned short dummy_16_byteorder __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F03_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));
 unsigned short count __attribute__ ((packed)); unsigned short dummy_16_count __attribute__ ((packed));
 char byteorder __attribute__ ((packed)); unsigned char dummy_08_byteorder __attribute__ ((packed)); unsigned short dummy_16_byteorder __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F04_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short coil_addr __attribute__ ((packed)); unsigned short dummy_16_coil_addr __attribute__ ((packed));
 unsigned short coil_state __attribute__ ((packed)); unsigned short dummy_16_coil_state __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F05_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short reg_addr __attribute__ ((packed)); unsigned short dummy_16_reg_addr __attribute__ ((packed));
 unsigned short value __attribute__ ((packed)); unsigned short dummy_16_value __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F06_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F07_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short subfunction __attribute__ ((packed)); unsigned short dummy_16_subfunction __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F08_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F0b_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F0c_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short coil_addr __attribute__ ((packed)); unsigned short dummy_16_coil_addr __attribute__ ((packed));
 unsigned short coil_count __attribute__ ((packed)); unsigned short dummy_16_coil_count __attribute__ ((packed));
 MB_DATA_ARRAY_PARAM * data __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F0f_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));
 unsigned short count __attribute__ ((packed)); unsigned short dummy_16_count __attribute__ ((packed));
 MB_DATA_ARRAY_PARAM * data __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F10_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F11_PARAM;

typedef struct
{
        unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
        MB_DATA_ARRAY_PARAM * data __attribute__ ((packed));

        unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F14_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short count __attribute__ ((packed)); unsigned short dummy_16_count __attribute__ ((packed));
 MB_DATA_ARRAY_PARAM * data __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F15_PARAM;

typedef struct
{
 unsigned short uLen __attribute__ ((packed));
 unsigned short pElem[2] __attribute__ ((packed));

}MB_DATA_ARRAY_PARAM_LEN_2;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));
 MB_DATA_ARRAY_PARAM_LEN_2 * data __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F16_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));
 unsigned short count __attribute__ ((packed)); unsigned short dummy_16_count __attribute__ ((packed));
 unsigned short reg_addr __attribute__ ((packed)); unsigned short dummy_16_reg_addr __attribute__ ((packed));
 unsigned short reg_count __attribute__ ((packed)); unsigned short dummy_16_reg_count __attribute__ ((packed));
 char byteorder __attribute__ ((packed)); unsigned char dummy_08_byteorder __attribute__ ((packed)); unsigned short dummy_16_byteorder __attribute__ ((packed));
 MB_DATA_ARRAY_PARAM * data __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F17_PARAM;

typedef struct
{
 unsigned short slave __attribute__ ((packed)); unsigned short dummy_16_slave __attribute__ ((packed));
 unsigned short start_addr __attribute__ ((packed)); unsigned short dummy_16_start_addr __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_F18_PARAM;

typedef struct
{
 MB_DATA_ARRAY_PARAM * data __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_DATA_PARAM;



typedef struct
{
 unsigned short uLen __attribute__ ((packed));
 signed short pElem[3] __attribute__ ((packed));

}MB_GET_STATUS_ARRAY_PARAM;

typedef struct
{
 MB_GET_STATUS_ARRAY_PARAM * status __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MB_GET_STATUS_PARAM;
# 69 "fz.c" 2



# 1 "libMect.h" 1
# 28 "libMect.h"
# 1 "../inc/osAlign.h" 1
# 33 "../inc/osAlign.h"
#pragma pack(1)
# 29 "libMect.h" 2



# 1 "../inc/libDef.h" 1
# 33 "libMect.h" 2



extern unsigned char app_mect_flag;
extern short app_mect_status;
extern float app_mect_enquiry;





int app_mect_init(void);
int app_mect_done(void);
short app_mect_enq(short id, char *command, int dd);
short app_mect_float_stx(short id, char *command, float value, int dd);
short app_mect_hex_stx(short id, char *command, unsigned short value, int dd);


extern unsigned char PLC_MECT_flag;
extern short PLC_MECT_status;
extern float PLC_MECT_enq;



void MECT_sread(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MECT_H_sread(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MECT_swrite(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MECT_H_swrite(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MECT_get_enquiry(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MECT_get_status(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void MECT_get_flag(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);



typedef struct
{
 unsigned short uLen __attribute__ ((packed));
 char pElem[2] __attribute__ ((packed));

}MECT_COMMAND_ARRAY_PARAM;

typedef struct
{
 signed short id __attribute__ ((packed)); unsigned short dummy_16_id __attribute__ ((packed));
 MECT_COMMAND_ARRAY_PARAM * command __attribute__ ((packed));
 signed int data __attribute__ ((packed));

 signed short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MECT_ENQUIRY_PARAM;

typedef struct
{
 signed short id __attribute__ ((packed)); unsigned short dummy_16_id __attribute__ ((packed));
 MECT_COMMAND_ARRAY_PARAM * command __attribute__ ((packed));
 float value __attribute__ ((packed));
 signed short data __attribute__ ((packed)); unsigned short dummy_16_data __attribute__ ((packed));

 signed short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MECT_FLOAT_STX_PARAM;

typedef struct
{
 signed short id __attribute__ ((packed)); unsigned short dummy_16_id __attribute__ ((packed));
 MECT_COMMAND_ARRAY_PARAM * command __attribute__ ((packed));
 unsigned short value __attribute__ ((packed)); unsigned short dummy_16_value __attribute__ ((packed));
 signed int data __attribute__ ((packed));

 signed short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}MECT_HEX_STX_PARAM;

typedef struct
{

 signed short status __attribute__ ((packed)); unsigned short dummy_16_status __attribute__ ((packed));

}MECT_GET_STATUS;

typedef struct
{

 char flag __attribute__ ((packed)); unsigned char dummy_08_flag __attribute__ ((packed)); unsigned short dummy_16_flag __attribute__ ((packed));

}MECT_GET_FLAG;

typedef struct
{

 float value __attribute__ ((packed));

}MECT_GET_ENQ_PARAM;
# 73 "fz.c" 2



# 1 "libUSB.h" 1
# 32 "libUSB.h"
typedef struct app_usb_mount_t {
 char dev[15];
 char dev1[15];
 char mpoint[15];
} app_usb_mount_s;



unsigned short app_usb_status[5 + 1];
app_usb_mount_s app_usb[5];
unsigned short app_usb_feedback[2];


short int app_usb_status_read(void);
void app_usb_init( void );
unsigned short app_usb_umount( unsigned short index );
unsigned short app_usb_mount( unsigned short index );
char * app_usb_mpoint_return( unsigned short index );



void Usb_on(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Usb_off(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Usb_copy(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Usb_diskcopy(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Usb_delete(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Usb_mkdir(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Usb_status(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Usb_feedback(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

typedef struct
{
 unsigned short index __attribute__ ((packed)); unsigned short dummy_16_index __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}USB_INDEX_PARAM;

typedef struct
{
 IEC_STRING const * srcfile __attribute__ ((packed));
 IEC_STRING const * dstfile __attribute__ ((packed));
 unsigned short src_index __attribute__ ((packed)); unsigned short dummy_16_src_index __attribute__ ((packed));
 unsigned short dst_index __attribute__ ((packed)); unsigned short dummy_16_dst_index __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));
}USB_COPY_PARAM;

typedef struct
{
 IEC_STRING const * srcfile __attribute__ ((packed));
 IEC_STRING const * dstfile __attribute__ ((packed));
 unsigned short src_index __attribute__ ((packed)); unsigned short dummy_16_src_index __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));
}USB_DISKCOPY_PARAM;

typedef struct
{
 IEC_STRING const * filename __attribute__ ((packed));
 unsigned short index __attribute__ ((packed)); unsigned short dummy_16_index __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));
}USB_FILE_PARAM;

typedef struct
{
 unsigned short uLen __attribute__ ((packed));
 signed short pElem[5] __attribute__ ((packed));

}USB_GET_STATUS_ARRAY_PARAM;

typedef struct
{
 USB_GET_STATUS_ARRAY_PARAM * status __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}USB_GET_STATUS_PARAM;

typedef struct
{
 unsigned short uLen __attribute__ ((packed));
 signed short pElem[2] __attribute__ ((packed));

}USB_GET_FEEDBACK_ARRAY_PARAM;

typedef struct
{
 USB_GET_FEEDBACK_ARRAY_PARAM * feedback __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}USB_GET_FEEDBACK_PARAM;
# 77 "fz.c" 2



# 1 "libDatalog.h" 1
# 31 "libDatalog.h"
typedef struct app_datalog_list_s {

 char label[20];
 unsigned short int occurence;
 FILE *fp;
 unsigned short int log_num;
 struct app_datalog_list_s *next;
}app_datalog_list_t;

typedef struct app_datalog_s {

 time_t milliseconds;
 unsigned int sys_time;
 int status;
 int dl_index;
 int lenght;
 unsigned short usbkey_index;
 char dir[40];
 app_datalog_list_t *list;

}app_datalog_t;





extern app_datalog_t datalog_data;


unsigned int *app_datalog_poweroff_mgmt;





int app_datalog_start(unsigned int buffer_lenght, unsigned short buffer_number, unsigned short index );
short app_datalog_capture(char *label, float value);
unsigned int app_datalog_stop(void);
short app_datalog_eject(unsigned short index);



void Datalog_start(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Datalog_get(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Datalog_stop(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Datalog_eject(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);



typedef struct
{
 unsigned int buffer_lenght __attribute__ ((packed));
 unsigned short buffer_number __attribute__ ((packed)); unsigned short dummy_16_buffer_number __attribute__ ((packed));
 unsigned short index __attribute__ ((packed)); unsigned short dummy_16_index __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}DATALOG_START_PARAM;

typedef struct
{

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}DATALOG_STOP_PARAM;

typedef struct
{
 IEC_STRING const * label __attribute__ ((packed));
        float value __attribute__ ((packed));

        unsigned int ret_value __attribute__ ((packed));
}DATALOG_GET_PARAM;

typedef struct
{
 unsigned short index __attribute__ ((packed)); unsigned short dummy_16_index __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}DATALOG_EJECT_PARAM;
# 81 "fz.c" 2



# 1 "libMectUserUtility.h" 1
# 30 "libMectUserUtility.h"
void Buzzer(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Date_add(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void Date_diff(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void DateTimeRead(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void DateTimeSync(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void DateTimeWrite(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void LCD_get_backlight(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);

void LCD_set_backlight(STaskInfoVM *pVM, unsigned char *pIP, unsigned char *pIN);



typedef struct
{
 unsigned short duration __attribute__ ((packed)); unsigned short dummy_16_duration __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}BUZZER_PARAM;

typedef struct
{
 IEC_STRING const * initDate __attribute__ ((packed));
 IEC_STRING const * format __attribute__ ((packed));
 signed int increment __attribute__ ((packed));
 IEC_STRING const * finalDate __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}DATA_ADD_PARAM;

typedef struct
{
 IEC_STRING const * initDate __attribute__ ((packed));
 IEC_STRING const * finalDate __attribute__ ((packed));
 IEC_STRING const * format __attribute__ ((packed));
 signed int diff __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed)); unsigned short dummy_16_ret_value __attribute__ ((packed));

}DATA_DIFF_PARAM;

typedef struct
{
 unsigned short uLen __attribute__ ((packed));
 unsigned short pElem[6] __attribute__ ((packed));
}DATE_TIME_ARRAY_PARAM;

typedef struct
{
 DATE_TIME_ARRAY_PARAM * DateTime __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed));
}RTC_DATE_TIME_PARAM;

typedef struct
{

 unsigned short ret_value __attribute__ ((packed));
}RTC_DATE_TIME_SYNC_PARAM;

typedef struct
{
 unsigned short level __attribute__ ((packed)); unsigned short dummy_16_level __attribute__ ((packed));

 unsigned short ret_value __attribute__ ((packed));

}LCD_BACKLIGHT_PARAM;
# 85 "fz.c" 2
# 100 "fz.c"
EXECUTE_FUN g_pLibraryFun[] =
{



# 1 "funIec.h" 1
# 26 "funIec.h"
         LeftString_int ,
         RightString_int ,
         MidString_int ,
         ConcatString ,
         InsertString_int ,
         DeleteString_int ,
         ReplaceString_int ,
         FindString_int ,
         LenString_int ,
# 47 "funIec.h"
         bool_to_string ,




         byte_to_string ,




         word_to_string ,




         dword_to_string ,




         int_to_string ,




         dint_to_string ,




         real_to_string ,




         lreal_to_string ,




         string_to_bool ,




         string_to_byte ,




         string_to_word ,




         string_to_dword ,




         string_to_int ,




         string_to_dint ,




         string_to_real ,




         string_to_lreal ,




         string_to_time ,




         iec_sin_real ,




         iec_sin_lreal ,




         iec_asin_real ,




         iec_asin_lreal ,




         iec_cos_real ,




         iec_cos_lreal ,




         iec_acos_real ,




         iec_acos_lreal ,




         iec_tan_real ,




         iec_tan_lreal ,




         iec_atan_real ,




         iec_atan_lreal ,




         iec_ln_real ,




         iec_ln_lreal ,




         iec_log_real ,




         iec_log_lreal ,




         iec_sqrt_real ,




         iec_sqrt_lreal ,




         iec_exp_real ,




         iec_exp_lreal ,




         time32_to_string ,




         mux_bit ,




         mux_byte ,




         mux_int ,




         mux_dint ,




         mux_string ,




         mux_64bit ,



         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),

        sfc_calcdotrans,
       sfc_calcdoact,
        sfc_finalise,






        trunc_real_dint,




       trunc_lreal_dint,



         ((void *)0),
         ((void *)0),
         ((void *)0),
# 106 "fz.c" 2




# 1 "funUtil.h" 1
# 28 "funUtil.h"
         copy_dword_from_byte_array ,
         copy_dword_to_byte_array ,
# 38 "funUtil.h"
         ((void *)0),
         ((void *)0),


         copy_dint_to_byte_array ,
         copy_dint_from_byte_array ,
# 52 "funUtil.h"
         ((void *)0),
         ((void *)0),


         copy_lreal_to_byte_array ,
         copy_lreal_from_byte_array ,





         copy_bool_to_byte_array ,
         copy_bool_from_byte_array ,





         copy_time_from_byte_array ,
         copy_time_to_byte_array ,





         copy_byte_from_byte_array ,
         copy_byte_to_byte_array ,





         copy_word_from_byte_array ,
         copy_word_to_byte_array ,





         copy_string_from_byte_array ,
         copy_string_to_byte_array ,





         copy_int_to_byte_array ,
         copy_int_from_byte_array ,





         copy_real_to_byte_array ,
         copy_real_from_byte_array ,





         byte_swap_word ,




         byte_swap_dword ,






         ((void *)0),


         hi_byte ,




         hi_word ,






         ((void *)0),


         lo_byte ,




         lo_word ,






         ((void *)0),


         make_word ,




         make_dword ,






         ((void *)0),


         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 111 "fz.c" 2




# 1 "funSys.h" 1
# 27 "funSys.h"
         ClearLocalTaskErrno ,
         ClearTaskErrno ,
         ((void *)0) ,
         GetLocalTaskErrno ,
         GetLocalTaskInfo ,
         GetLocalTaskState ,
         GetTaskErrno ,
         GetTimeSinceSystemBoot ,
         OutputDebugString ,
         StartLocalTask ,
         StopLocalTask ,
         ThrowException ,
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 116 "fz.c" 2




# 1 "funSys2.h" 1
# 27 "funSys2.h"
         EVT_GetException ,
         EVT_Set ,
         ((void *)0) ,
         MSG_SendToEng ,
         MSG_Trace ,
         ((void *)0) ,
         TIM_Get ,
         TSK_ClearError ,
         TSK_Exception ,
         TSK_GetCount ,
         TSK_GetError ,
         TSK_GetInfo ,
         TSK_GetMyNumber ,
         TSK_GetName ,
         TSK_GetState ,
         TSK_Start ,
         TSK_Stop ,
         WD_Disable ,
         WD_Enable ,
         ((void *)0) ,
         ((void *)0) ,
         EVT_GetEvent ,
         WD_IsEnabled ,
         TSK_GetMyName ,
         SYS_Coldstart ,
# 121 "fz.c" 2




# 1 "funFile.h" 1
# 27 "funFile.h"
         fa_sync_openFile ,
         fa_sync_readFile ,
         fa_sync_renameFile ,
         fa_sync_writeFile ,
         fa_sync_closeFile ,
         fa_sync_createDirectory ,
         fa_sync_deleteFile ,
         fa_sync_existsFile ,
         fa_sync_getSize ,
         fa_get_diskFreeSpace ,
         fa_error_string ,
         fa_sync_arrayReadFile ,
         fa_sync_arrayWriteFile ,
         fa_flush ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
# 126 "fz.c" 2




# 1 "funSys22.h" 1
# 27 "funSys22.h"
         SYS_Reboot ,
         SYS_RetainWrite ,
         SYS_Warmstart ,
         LD_Get ,
         WD_SetTrigger ,
         WD_GetTrigger ,
         TSK_GetStatistic ,
         TSK_ClearStatistic ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
# 131 "fz.c" 2
# 148 "fz.c"
# 1 "funMBRTU.h" 1
# 26 "funMBRTU.h"
         MB_f01,
         MB_f02,
         MB_f03,
         MB_f04,
         MB_f05,
         MB_f06,
         MB_f07,
         MB_f08,
         MB_f0b,
         MB_f0c,
         MB_f0f,
         MB_f10,
         MB_f11,
         MB_f14,
         MB_f15,
         MB_f16,
         MB_f17,
         MB_f18,
         MB_data,
         MB_get_status,
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 149 "fz.c" 2





# 1 "funMect.h" 1
# 26 "funMect.h"
         MECT_sread,
         MECT_H_sread,
         MECT_swrite,
         MECT_H_swrite,
         MECT_get_enquiry,
         MECT_get_flag,
         MECT_get_status,
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 155 "fz.c" 2





# 1 "funUSB.h" 1
# 24 "funUSB.h"
         Usb_off,
         Usb_copy,
         Usb_diskcopy,
         Usb_delete,
         Usb_mkdir,
         Usb_status,
         Usb_feedback,
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 161 "fz.c" 2





# 1 "funDatalog.h" 1
# 24 "funDatalog.h"
         Datalog_get,
         Datalog_stop,
         Datalog_eject,
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 167 "fz.c" 2





# 1 "funMectUserUtility.h" 1
# 24 "funMectUserUtility.h"
         Buzzer,
         Date_add,
         Date_diff,
         DateTimeRead,
         DateTimeSync,
         DateTimeWrite,
         LCD_get_backlight,
         LCD_set_backlight,
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 173 "fz.c" 2

};





EXECUTE_FB g_pLibraryFB [] =
{



# 1 "fbIec.h" 1
# 26 "fbIec.h"
         rs,




         sr,




         tp,




         ton,




         tof,




         ctu,




         ctd,




         ctud,




         r_trig,




         f_trig,



         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 186 "fz.c" 2




# 1 "fbUtil.h" 1
# 27 "fbUtil.h"
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 191 "fz.c" 2




# 1 "fbSys.h" 1
# 27 "fbSys.h"
         SignalError,
         ((void *)0),
         ((void *)0),
         ((void *)0),
         ((void *)0),
# 196 "fz.c" 2




# 1 "fbMBus2.h" 1
# 40 "fbMBus2.h"
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
         ((void *)0) ,
# 201 "fz.c" 2




# 1 "fbSys2.h" 1
# 206 "fz.c" 2




# 1 "fbFile.h" 1
# 211 "fz.c" 2

};
# 221 "fz.c"
unsigned short libGetFunCount()
{
 return sizeof(g_pLibraryFun) / sizeof(g_pLibraryFun[0]);
}
# 233 "fz.c"
unsigned short libGetFBCount()
{
 return sizeof(g_pLibraryFB) / sizeof(g_pLibraryFB[0]);
}
# 247 "fz.c"
unsigned short libSetError(STaskInfoVM *pVM, unsigned short uError)
{
 pVM->Local.pState->ulErrNo = uError;
 { unsigned short xxOK = 0x0000u; unsigned short xxWRN1 = 0x8001u; unsigned short xxWRN2 = 0x0102u; if ((unsigned short)(0x0000u) != xxOK && (unsigned short)(0x0000u) != xxWRN1 && (unsigned short)(0x0000u) != xxWRN2 ) { osTrace("ERR %s(%04ld): 0x%04x\r\n", "libDef.c", (unsigned int)250, (0x0000u)); } return(0x0000u); };
}
# 262 "fz.c"
unsigned short libSetException(STaskInfoVM *pVM, unsigned short uError)
{
 pVM->Local.Exception.uErrNo = uError;
 { unsigned short xxOK = 0x0000u; unsigned short xxWRN1 = 0x8001u; unsigned short xxWRN2 = 0x0102u; if ((unsigned short)(0x0000u) != xxOK && (unsigned short)(0x0000u) != xxWRN1 && (unsigned short)(0x0000u) != xxWRN2 ) { osTrace("ERR %s(%04ld): 0x%04x\r\n", "libDef.c", (unsigned int)265, (0x0000u)); } return(0x0000u); };
}
# 276 "fz.c"
unsigned int libSubstring(STaskInfoVM *pVM, IEC_STRING __attribute__((packed)) *in, IEC_STRING __attribute__((packed)) *out, unsigned int startPos, unsigned int endPos, unsigned char bAppend)
{
 unsigned int ulLen = 0;

 if (in == 0 || out == 0)
 {
  pVM->Local.Exception.uErrNo = 0x26u;
  return 0;
 }

 if(startPos > endPos)
 {
  unsigned int tmp = startPos;
  startPos = endPos;
  endPos = tmp;
 }

 if (bAppend == 0)
 {
  out->CurLen = 0;
 }

 if (startPos >= in->CurLen)
 {
  return out->CurLen;
 }

 ulLen = endPos - startPos > (unsigned int)(out->MaxLen - out->CurLen) ? (unsigned int)(out->MaxLen - out->CurLen) : (unsigned int)(endPos - startPos);
 ulLen = startPos + ulLen > in->CurLen ? in->CurLen - startPos : ulLen;

 if (ulLen != 0)
 {
  memcpy(out->Contents + out->CurLen, &in->Contents[startPos], ulLen);
  out->CurLen = (unsigned char)(out->CurLen + ulLen);
 }

 return out->CurLen;
}






unsigned short libGetTaskNo(STaskInfoVM *pVM, IEC_STRING __attribute__((packed)) *strTask, unsigned short *upTask)
{
 unsigned short i;
 STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

 for (i = 0; i < pVMM->Project.uTasks; i++)
 {
  if (strncasecmp(utilIecToAnsi(strTask, pVM->Local.pBuffer), (char *)pVMM->ppVM[i]->Task.szName, 33u) == 0)
  {
   break;
  }
 }

 if (i >= pVMM->Project.uTasks)
 {
  { unsigned short xxOK = 0x0000u; unsigned short xxWRN1 = 0x8001u; unsigned short xxWRN2 = 0x0102u; if ((unsigned short)(0x0201u) != xxOK && (unsigned short)(0x0201u) != xxWRN1 && (unsigned short)(0x0201u) != xxWRN2 ) { osTrace("ERR %s(%04ld): 0x%04x\r\n", "libDef.c", (unsigned int)335, (0x0201u)); } return(0x0201u); };
 }

 *upTask = i;

 { unsigned short xxOK = 0x0000u; unsigned short xxWRN1 = 0x8001u; unsigned short xxWRN2 = 0x0102u; if ((unsigned short)(0x0000u) != xxOK && (unsigned short)(0x0000u) != xxWRN1 && (unsigned short)(0x0000u) != xxWRN2 ) { osTrace("ERR %s(%04ld): 0x%04x\r\n", "libDef.c", (unsigned int)340, (0x0000u)); } return(0x0000u); };
}
