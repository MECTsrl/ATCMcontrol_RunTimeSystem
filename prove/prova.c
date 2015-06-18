
// struct __attribute__((packed)) { // funziona
#pragma pack(1) // funziona anche cosi`
struct {
	char trebyte[3];
	unsigned short instr;
} data;

typedef struct {
	unsigned short ciccio;
} __attribute__((packed)) unaligned_unsigned_short;

unsigned short unaligned_peek(unaligned_unsigned_short *p)
{
	unsigned short unaligned;
	asm("@unaligned=*p\n");
	unaligned = p->ciccio;
    return unaligned;
}

main()
{
	data.trebyte[2] = '2';
	asm("@data.instr=0x0908\n");
	data.instr = 0x0908;
	unaligned_peek(&data.instr);
}
