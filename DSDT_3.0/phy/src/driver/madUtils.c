#include <madCopyright.h>

/********************************************************************************
* madUtils.c
*
* DESCRIPTION:
*       Collection of Utility functions
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#include <madApi.h>

/*******************************************************************************
* madMemSet
*
* DESCRIPTION:
*       Set a block of memory
*
* INPUTS:
*       start  - start address of memory block for setting
*       simbol - character to store, converted to an unsigned char
*       size   - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to set memory block
*
* COMMENTS:
*       None
*
*******************************************************************************/
void * madMemSet
(
    IN void * start,
    IN int    symbol,
    IN MAD_U32 size
)
{
    MAD_U32 i;
    char* buf;
    
    buf = (char*)start;
        
    for(i=0; i<size; i++)
    {
        *buf++ = (char)symbol;
    }

    return start;
}

/*******************************************************************************
* madMemCpy
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. If copying takes place between
*       objects that overlap, the behavior is undefined.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
void * madMemCpy
(
    IN void *       destination,
    IN const void * source,
    IN MAD_U32       size
)
{
    MAD_U32 i;
    char* buf;
    char* src;
    
    buf = (char*)destination;
    src = (char*)source;
        
    for(i=0; i<size; i++)
    {
        *buf++ = *src++;
    }

    return destination;
}

/*******************************************************************************
* madMemCmp
*
* DESCRIPTION:
*       Compares given memories.
*
* INPUTS:
*       src1 - source 1
*       src2 - source 2
*       size - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0, if equal.
*        negative number, if src1 < src2.
*        positive number, if src1 > src2.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int madMemCmp
(
    IN char src1[],
    IN char src2[],
    IN MAD_U32 size
)
{
    MAD_U32 i;
    int value;

    for(i=0; i<size; i++)
    {
        if((value = (int)(src1[i] - src2[i])) != 0)
            return value; 
    }

    return 0;
}

/*******************************************************************************
* madStrlen
*
* DESCRIPTION:
*       Determine the length of a string
* INPUTS:
*       source  - string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       size    - number of characters in string, not including EOS.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_U32 madStrlen
(
    IN const void * source
)
{
    MAD_U32 i = 0;
    char* src;
    
    src = (char*)source;
        
    while(*src++) i++;

    return i;
}


