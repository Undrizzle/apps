#include <dbsUtils.h>

#define NIBBLE_MASK 0x0F
#define DIGITS_HEX "0123456789ABCDEF" 
 
static void hexword (unsigned value, size_t field, FILE *fp) 

{
	if (field > 0) 
	{
		hexword (value >> 4, --field, fp);
		putc (DIGITS_HEX [value & NIBBLE_MASK], fp);
		return;
	}
	return;
}

void hexdump (const unsigned char memory [], size_t length, FILE *fp) 

{
	size_t first = 0;
	size_t final = 0x10;
	size_t block = 0x10;
	size_t group = 0x10;
	size_t field = sizeof (unsigned);
	size_t offset = length;

#if AUTOWIDTH
 
	for (field = 0; offset > 0; field++) 
	{
		offset /= 0x10;
	}

#endif
 
	while (first < length) 
	{
		hexword (first, field, fp);
		for (offset = first; offset < final; offset++) 
		{
			putc (' ', fp);
			if ((offset % group) == 0) 
			{
				putc (' ', fp);
			}
			if (offset < length) 
			{
				putc (DIGITS_HEX [(memory [offset] >> 4) & NIBBLE_MASK], fp);
				putc (DIGITS_HEX [(memory [offset] >> 0) & NIBBLE_MASK], fp);
			}
			else 
			{
				putc (' ', fp);
				putc (' ', fp);
			}
		}
		putc (' ', fp);
		for (offset = first; offset < final; offset++) 
		{
			unsigned c = memory [offset];
			if ((c < 0x20) || (c > 0x7E)) 
			{
				c = '.';
			}
			if ((offset % group) == 0) 
			{
				putc (' ', fp);
			}
			if (offset < length) 
			{
				putc (c, fp);
			}
			else 
			{
				putc (' ', fp);
			}
		}
		putc ('\n', fp);
		first += block;
		final += block;
	}
	fflush (fp);
	return;
}

 

