#ifndef __MD5_LITTLE_ENDIAN_H__
#define __MD5_LITTLE_ENDIAN_H__
 
#ifdef __alpha
typedef unsigned int uint32;
#else
typedef unsigned long uint32;
#endif
 
struct MD5Context {
        uint32 buf[4];
        uint32 bits[2];
        unsigned char in[64];
};
 
extern void MD5Init();
extern void MD5Update();
extern void MD5Final();
extern void MD5Transform();
 
/*
* This is needed to make RSAREF happy on some MS-DOS compilers.
*/
typedef struct MD5Context MD5_CTX;
 
#endif /* !__MD5_LITTLE_ENDIAN_H__ */
