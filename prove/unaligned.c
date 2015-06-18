#define get_unaligned(ptr) \
	({ \
	     struct __attribute__((packed)) { \
	         typeof(*(ptr)) __v; \
	     } *__p = (void *) (ptr); \
	 __p->__v; \
	 })

#define put_unaligned(val, ptr) \
	do { \
		struct __attribute__((packed)) { \
			typeof(*(ptr)) __v; \
		} *__p = (void *) (ptr); \
		__p->__v = (val); \
	} while(0)

#define copy_unaligned(dst, src) \
	do { \
		struct __attribute__((packed)) { \
			typeof(*(src)) __v; \
		} *__s = (void *) (src); \
		struct __attribute__((packed)) { \
			typeof(*(dst)) __v; \
		} *__d = (void *) (dst); \
		__d->__v = __s->__v; \
	} while(0)

struct __attribute__((packed)) {
	char trebyte[3];
	unsigned short instr;
} data;

unsigned short unaligned_peek(unsigned short *p)
{
	asm(";qui\n");
    return get_unaligned((signed short *)p);
}

void unaligned_poke(unsigned short v, unsigned short *p)
{
	asm(";qui\n");
    put_unaligned((signed short)v, (signed short *)p);
}

void unaligned_copy(unsigned short *s, unsigned short *d)
{
	asm(";qui\n");
    copy_unaligned((signed short *)s, (signed short *)d);
}

main()
{
	unsigned short instr = 0x0504;
	data.trebyte[2] = '2';
	data.instr = 0x0908;
	unaligned_peek(&data.instr);
	unaligned_poke(0x0706,&data.instr);
	unaligned_copy(&instr,&data.instr);
}
