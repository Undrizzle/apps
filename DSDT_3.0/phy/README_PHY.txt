

            Phy Driver (MAD) in DSDT Release version 3.0
           ==============================================


Table of Content:
-----------------
1) Release History 
2) Source Code Organization
3) General Introduction
4) HOW TO - Build madDrv.o for vxWorks
5) HOW TO - Build madDrv.o for Linux
6) Changes from previous release
7) Supported APIs


1) Release History 
------------------
DSDT_3.0/phy - May 27. 2010, 
    based on MAD_2.5.
    Merged into Marvell DSDT driver.
    Added to support SyncE of E1340S.
    Fixed control register 2 and 3 confusing between old version and newer version.
    Modified Extended VCT functions.
    Added to support the Phys in Marvell switch chips.
    Added to support the E1540 and E1540-y0.
MAD_2.5.zip - Feb. 2009, Added to support PTP of E1340S.
    Added to support offset and sample point in advanced VCT.
MAD_2.4.zip - Dec. 2008, Added to support E1340M, E1310, E1119R, and E3019.
MAD_2.3.zip - Nov. 2008, Added to support E1340 full functions.
    supports Copper, Fiber, SGMII, and QSGMII media interface.
    supports pages for all registers. (before page for above register 16).
    Added APIs for 9 new features of E1340.
    Added and modified APIs for functions.
MAD_2.2.zip - Oct. 2008, Added to support E1340 MacSec.
MAD_2.1.zip - Jan. 2007, Added to support E1121R, E1240, E1149R, E1116R.
MAD_2.04.zip - May. 2006, Added to support E3082.
    Merged User layer API's.
    Use new definitions of MAD_STATUS.
    Fixed several bugs.
MAD_2.03.zip - May. 2006, Added to support E1121.
MAD_2.01.zip - April. 2006, Added to support E3016.

MAD_2.0.zip - Dec. 2005, First release of General MAD Driver.
    The release is for general drivers. It supports Marvell Transceiver products, E1181, 
    E1149, E1141/E1143/E1145, E1112, E1111, E1116, and E1041/E1042/E104 3. 
    The driver is tested on related Marvell evaluation boards with Marvell 
    Firefox board in vXWorks environment. Also, as an example, the driver is
    compiled in Linux/PC environment.
                   
MAD_2.01pre.zip - Nov. 2005, Added to support 88E114x, 88E1112 and 88E1111 devices.
                   
MAD_2.0pre.zip - Oct. 2005, Supports 88E1149 and 88E1116 devices.
    Bug fix, semaphore for multi-thread safe and new APIs are added.
MAD_1.1.zip - Sep. 2005, New API, mdDiagGetAdvCableStatus, are added.
    DSP based cable length is reported in the API,
    mdDiagGet1000BTExtendedStatus, not in the VCT APIs.
    Tested on 88E1181 Rev.E device with 88E6218 System board.
MAD_1.0.zip - Jun. 2005, First official release version of MAD driver.
    Tested on 88E1181 device with 88E6218 System board.
MAD_0. 3. zip - Jun. 2005, All the APIs are implemented, but not yet tested.
    Some of the arguments of APIs and definitions in header
    files are modified.
MAD_0.2.zip - Jun. 2005, API List to be supported is included.
MAD_0.1.zip - Jun. 2005, Initial MAD Release


2) Source Code Organization
--------------------------
  2.1) src
    MAD Source Code.

  2.2) Include directory
    MAD Header files and Prototype files

  2.3) Library
    Object files for MAD

  2.4) Sample
    Sample Code that shows how to use APIs.
    Test programs to test MAD APIs

  * The MAD Source Codes are OS independent, and fully supported by Marvell.
  * The Sample Code is not part of MAD. It is included here to demonstrate
      how MAD can be used.


3) General Introduction
-----------------------

The MAD is standalone program, which is independent of both OS and Platform. 
As such, applications of MAD APIs need to register platform specific functions. 
This is done by calling mdLoadDriver function. This function fills up the MAD 
Driver Structure (*dev) with the device and platform information. 
Thereafter, "*dev" is used for each MAD API call to identify a specific PHY.
Consequently, multiple PHYs can be supported by one MAD.

madApiInit.c file in Sample\Initialization directory demonstrates how to 
register the required functions.

madApiInit.c
  madStart is the main function of this Sample and does the followings:
  [1] Load MAD driver (mdLoadDriver API)
    [1.1] allocates MAD driver structure and zeros it out.
    [1.2] register the required functions.
      readMii   - BSP specific MII read function 
                  (provided by BSP and required by all MAD API)
      writeMii  - BSP specific MII write function 
                  (provided by BSP and required by all MAD API)
      semCreate - OS specific semaphore create function.
                  (optionally provided by BSP, if semaphore is required
                  to access MII in paged mode)
      semDelete - OS specific semaphore delete function.
                  (optionally provided by BSP, if semaphore is required
                      to access MII in paged mode)
      semTake   - OS specific semaphore take function.
                  (optionally provided by BSP, if semaphore is required
                  to access MII in paged mode)
      semGive   - OS specific semaphore give function.
                  (optionally provided by BSP, if semaphore is required
                  to access MII in paged mode)

      Notes) The given example uses Marvell Firebat or DB-88E6218 BSP.

    [1.3] notify SMI address used by the Marvell Alaska Phy.
    [1.4] call mdLoadDriver function to load MAD driver.

  [2] Enable all ports in the Marvell Alaska Phy by calling
    mdSysSetPhyEnable API for each port.

Image size of the current MAD Driver:
  XXXX Bytes with vxWorks compiler for ARM
  YYYY Bytes with gcc version 2.96 for i386


4) HOW TO - Build madDrv.o for vxWorks
-------------------------------------

1. Extract the given ZIP file into c:\DSDT_3.x\phy directory (3.x is the version)
   You may change the directory name to your choice, and change the environment 
   variable below accordingly.
2. Edit setenv.bat file in c:\DSDT_3.x\phy\tools
3. Modify the following variables according to your setup.

   set MAD_USER_BASE=C:\DSDT_3.x\phy
   set MAD_PROJ_NAME=madDrv
   set WIND_BASE=C:\Tornado
   set TARGETCPU=ARM            ;MIPS for MIPS Cpu
   set WIND_HOST_TYPE=x86-win32
   set VCT_PENDING=TRUE ; if using pending to run VCT or Advanced VCT

4. run "setenv"
5. Change directory to c:\DSDT_3.x\phy\src
6. run "make"
7. madDrv.o and madDrv.map will be created in c:\DSDT_3.x\phy\Library.


5) HOW TO - Build madDrv.o for Linux
-----------------------------------

1. Extract the given ZIP file into $HOME/DSDT_3.x/phy directory(directory can be changed)
   in Linux system (verified with RedHat 7.3)
2. Edit setenv file in $HOME/DSDT_3.x/phy/tools
3. Modify the following variables according to your setup.

   declare -x MAD_USER_BASE=$HOME/DSDT_3.x/phy
   declare -x MAD_PROJ_NAME=madDrv
   declare -x VCT_PENDING=TRUE ; if using pending to run VCT or Advanced VCT

4. run "source setenv"
5. Change directory to $HOME/DSDT_3.x/phy/src
6. run "make"
7. madDrv.o and madDrv.map will be created in $HOME/DSDT_3.x/phy/Library.


6) Changes from previous release
--------------------------------
6.1) Merged into Marvell DSDT driver.
    Based on MAD_2.5, it is merged into DSDT_3.0, and as DSDT_X.x/phy.
6.2) Support 88E1340
  Added PTP features: PTP configuration, PTP control, and PTP time stamp
  Added to support SyncE of E1340S.
6.3) Modified features:
   Sample point and offset of Advanced VCT
6.4) Fixed control register 2 and 3 confusing between old version and newer version.
6.5) Modified Extended VCT functions.
6.6) Added to support the E1540 and E1540-y0.
6.7) Fixed some small bugs.

