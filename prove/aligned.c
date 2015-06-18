unsigned short aligned_peek(unsigned short *p)
{
	unsigned short aligned;

	asm(";qui\n");
	aligned = *p;

    return aligned;
}
