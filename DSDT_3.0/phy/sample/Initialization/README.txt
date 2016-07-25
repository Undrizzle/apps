========================================================================
       Marvell Alaska Driver (MAD) Initialization called by BSP
========================================================================

Initialization Sample Program will show how to initialize the Marvell
Alaska Device Driver.

This Sample includes the following files:
    - madApiInit.c
    - bspSpecificMii.c
    - madOsSem.c

madApiInit.c
    madStart is the main function of this Sample and does the followings:
    1) Load MAD driver (mdLoadDriver API)
        1.1) allocate MAD driver sturcture and zero it out.
        1.2) register the required functions.
            readMii - BSP specific MII read function 
                      (provided by BSP and required by all MAD API)
            writeMii - BSP specific MII write function 
                      (provided by BSP and required by all MAD API)
            semCreate - OS specific semaphore create function.
                      (optionally provided by BSP, if semaphore is required
                      to access MII in paged mode)
            semDelete - OS specific semaphore delete function.
                      (optionally provided by BSP, if semaphore is required
                      to access MII in paged mode)
            semTake - OS specific semaphore take function.
                      (optionally provided by BSP, if semaphore is required
                      to access MII in paged mode)
            semGive - OS specific semaphore give function.
                      (optionally provided by BSP, if semaphore is required
                      to access MII in paged mode)

    		Notes) The given example uses DB-88E6218 BSP.

        1.3) notify SMI address used by the Marvell Alaska Phy.
        1.4) call madLoadDriver function to load MAD driver.

    2) Enable all 8 ports in the Marvell Alaska Phy by calling
       mdSysSetPhyEnable API for each port.

bspSpecificMii.c
    Provides DB-88E6218 Board specific MII access functions.

    Exported routines are :
        bspSpecificReadMii 	for reading MII registers, and
        bspSpecificWriteMii	for writing to MII registers.

madOsSem.c
    Provides OS specific Semapore Functions.

    Exported routines are :
        madOsSemCreate  for semaphore creation
        madOsSemDelete  for semaphore deletion
        madOsSemWait    for taking semaphore
        madOsSemSignal  for releasing semaphore
