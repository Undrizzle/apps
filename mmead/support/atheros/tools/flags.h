/*====================================================================*
 *
 *   flags.h - bitmap flagword definitions and declarations;
 *
 *.  published 2005 by charles maier associates limited for internal use;
 *:  compiled for debian gnu/linux environment using gcc 2.95 compiler;
 *;  licensed according to the GNU Public License version two;
 *
 *--------------------------------------------------------------------*/

#ifndef FLAGS_HEADER
#define FLAGS_HEADER
 
/*====================================================================*
 *   define macros for common bitmap operations; 
 *--------------------------------------------------------------------*/

#define _getbits(map,pos,cnt) (((map)>>((pos)-(cnt)+1))&~(~(0)<<(cnt)))

#define _bitmask(cnt) (1 << (cnt)) - 1

#define _setbits(flag,mask) flag |=  (mask)
#define _clrbits(flag,mask) flag &= ~(mask)
#define _toggle(flag,mask)  flag = ~flag & ~(mask)

#define _anybits(flag,mask) (flag & (mask)) != (0)
#define _allbits(flag,mask) (flag & (mask)) == (mask)

#define _anyset(flag,mask) (flag & (mask)) != (0)
#define _anyclr(flag,mask) (flag & (mask)) != (mask)
#define _allset(flag,mask) (flag & (mask)) == (mask)
#define _allclr(flag,mask) (flag & (mask)) == (0)

#define _clean(flag,mask) (flag & ~(mask)) == (0)
#define _dirty(flag,mask) (flag & ~(mask)) != (0)
 
/*====================================================================*
 *   end definitions and declarations;
 *--------------------------------------------------------------------*/

#endif
 

