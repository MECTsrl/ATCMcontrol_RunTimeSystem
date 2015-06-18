
// struct __attribute__((packed)) { // funziona
#pragma pack(1) // funziona anche cosi`
struct {
	char trebyte[3];
	unsigned short instr;
} data;

unsigned short unaligned_peek(unsigned short *p)
{
	unsigned short unaligned;
	asm("@unaligned=*p\n");
	unaligned = *(__attribute__((packed)) unsigned short *)p; // NON funziona
    return unaligned;
}

main()
{
	data.trebyte[2] = '2';
	asm("@data.instr=0x0908\n");
	data.instr = 0x0908;
	unaligned_peek(&data.instr);
}
