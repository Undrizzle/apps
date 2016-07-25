#include <madCopyright.h>

/********************************************************************************
* madHwCntl.h
*
* DESCRIPTION:
*       Function declarations for Hw accessing Marvell phy registers.
*       global registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __madHwCntlh
#define __madHwCntlh

#include <madApi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 *  This macro is used to calculate the register's SMI device address, 
 *  according to the baseRegAddr field in the device driver struct.
*/
#define MAD_CALC_SMI_DEV_ADDR(_dev, _portNum)  \
                ((_dev)->baseRegAddr + (_portNum))

/* 
 *  This macro is used to get the correct page address from the given register.
*/
#define MAD_GET_PAGE_ADDR(_dev, _regOffset)  \
                ((_regOffset) < 29 ? 22 : 29)

#define MAD_UPDATE_PAGE_NUM(_pageAddr, _org)  \
                ((_pageAddr == 22) ? ((_org) & 0xFF00) : (_org))

/* 
    This macro calculates the mask for partial read/write of register's data.
*/
#define MAD_CALC_MASK(fieldOffset,fieldLen,mask)        \
            if((fieldLen + fieldOffset) >= 16)      \
                mask = (0 - (1 << fieldOffset));    \
            else                                    \
                mask = (((1 << (fieldLen + fieldOffset))) - (1 << fieldOffset))

#define MAD_IS_ANY_PAGE(_dev,_portNum)      \
    ((_dev->phyInfo.featureSet&MAD_PHY_FULL_PAGE_ADDR)? 0: \
     ((_dev->phyInfo.featureSet&MAD_PHY_30XX_MODE)? 1:(0xFFFF & (0x1 << _portNum))) \
    )


/****************************************************************************/
/* Phy registers access functions.                                          */
/****************************************************************************/

/*******************************************************************************
* madHwReadPhyReg
*
* DESCRIPTION:
*       This function reads a switch's port phy register.
*
* INPUTS:
*       portNum - Port number to read the register for.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwReadPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    OUT MAD_U16    *data
);


/*******************************************************************************
* madHwWritePhyReg
*
* DESCRIPTION:
*       This function writes to a switch's port phy register.
*
* INPUTS:
*       portNum - Port number to write the register for.
*       regAddr - The register's address.
*       data    - The data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwWritePhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U16    data
);


/*******************************************************************************
* madHwGetPhyRegField
*
* DESCRIPTION:
*       This function reads a specified field from a switch's port phy register.
*
* INPUTS:
*       portNum     - Port number to read the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLenmadh - Number of bits to read.
*
* OUTPUTS:
*       data        - The read register field.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwGetPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    OUT MAD_U16    *data
);


/*******************************************************************************
* madHwSetPhyRegField
*
* DESCRIPTION:
*       This function writes to specified field in a switch's port phy register.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwSetPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    IN  MAD_U16    data
);


/****************************************************************************/
/* Page Mode access                                                            */
/****************************************************************************/

/*******************************************************************************
* madHwReadPagedPhyReg
*
* DESCRIPTION:
*       This function reads a phy register in paged mode.
*
* INPUTS:
*       portNum - Port number to read the register for.
*       pageNum - Page number to be accessed.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwReadPagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    OUT MAD_U16    *data
);


/*******************************************************************************
* madHwWritePagedPhyReg
*
* DESCRIPTION:
*       This function writes to a phy register in paged mode.
*
* INPUTS:
*       portNum - Port number to write the register for.
*       pageNum - Page number to be accessed.
*       regAddr - The register's address.
*       data    - The data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwWritePagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U16    data
);


/*******************************************************************************
* madHwGetPagedPhyRegField
*
* DESCRIPTION:
*       This function reads a specified field from a phy register in paged mode.
*
* INPUTS:
*       portNum     - Port number to read the register for.
*       pageNum - Page number to be accessed.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLenmadh - Number of bits to read.
*
* OUTPUTS:
*       data        - The read register field.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwGetPagedPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    OUT MAD_U16    *data
);


/*******************************************************************************
* madHwSetPagedPhyRegField
*
* DESCRIPTION:
*       This function writes to specified field in a phy register in paged mode.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       pageNum     - Page number to be accessed.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwSetPagedPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    IN  MAD_U16    data
);

/*******************************************************************************
* madHwPagedSetCtrlPara
*
* DESCRIPTION:
*       This function set control parameter through soft reset.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       pageNum     - Page number to be accessed.
*        ctrlData    - Control parameter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS madHwPagedSetCtrlPara
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN    MAD_U16       ctrlData
);

/*******************************************************************************
* madHwPagedReset
*
* DESCRIPTION:
*       This function performs softreset and waits until reset completion.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       pageNum     - Page number to be accessed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS madHwPagedReset
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum
);


#ifdef __cplusplus
}
#endif
#endif /* __madHwCntlh */
