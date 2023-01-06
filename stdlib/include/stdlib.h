#ifndef DDRAIG_OS_STDLIB_H
#define DDRAIG_OS_STDLIB_H

#ifndef NULL
#define NULL ((void*) 0)
#endif

#include <klibc/extern.h>
#include <klibc/inline.h>
#include <stddef.h>

__extern_inline int abs(int __n)
{
	return (__n < 0) ? -__n : __n;
}

__extern int atoi(const char *);
__extern long atol(const char *);
__extern long long atoll(const char *);

__extern_inline long labs(long __n)
{
	return (__n < 0L) ? -__n : __n;
}
__extern_inline long long llabs(long long __n)
{
	return (__n < 0LL) ? -__n : __n;
}

__extern long strtol(const char *, char **, int);
__extern long long strtoll(const char *, char **, int);
__extern unsigned long strtoul(const char *, char **, int);
__extern unsigned long long strtoull(const char *, char **, int);

typedef int (*__comparefunc_t) (const void *, const void *);
__extern void *bsearch(const void *, const void *, size_t, size_t, __comparefunc_t);
__extern void qsort(void *, size_t, size_t, __comparefunc_t);

__extern long jrand48(unsigned short *);
__extern long mrand48(void);
__extern long nrand48(unsigned short *);
__extern long lrand48(void);
__extern unsigned short *seed48(const unsigned short *);
__extern void srand48(long);

#define RAND_MAX 0x7fffffff
__extern_inline int rand(void)
{
	return (int)lrand48();
}
__extern_inline void srand(unsigned int __s)
{
	srand48(__s);
}
__extern_inline long random(void)
{
	return lrand48();
}
__extern_inline void srandom(unsigned int __s)
{
	srand48(__s);
}

#endif
