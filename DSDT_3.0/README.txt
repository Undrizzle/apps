
                    DSDT Release version 3.0
                   ==========================


DSDT Release version 3.0 GPL 2.0 Compliant.

Copyright (C) 2010 (or other year developed and each year modified and/or added to) 
Marvell International Ltd.
 
This program is free software; you can redistribute it and/or modify it under the terms 
of the GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License along with this program;
if not, write to the Free Software Foundation, Inc., 
675 Mass Ave, Cambridge, MA 02139, USA.


Marvell Semiconductor, Inc.
5488 Marvell Lane
Santa Clara, CA 95054, USA

Tel: 1.408.222.2500
Fax: 1.408.988.8279

www.marvell.com


Table of Content:
-----------------
1) Release History 
2) Source Code Organization
3) General Introduction
4) How to - build Marvell drivers
5) How to - build Marvell sw drivers (add by frank)
6) Example of global building variables

1) Release History 
------------------
DSDT3.0.zip - May 26. 2010. 
         Merged and developed from Marvell SOHO Switch driver DSDT_2.8a, 
	 and Marvell Phy driver MAD_2.5.
         Supports switch chips, 88EC0XX and 88E6250.
         Update phy macsec to support 1540 y0.
         Fixed bugs.

2) Source Code Organization
--------------------------
    2.1) Switch 
        Marvell SOHO Switch Driver. See README_SW.

    2.2) Phy 
        Marvell Phy Driver.	See README_phy

3) General Introduction
-----------------------

The DSDT 3.xx supports Marvell SOHO Switch and Phy products.
The DSDT 3.xx starts from original DSDT 2.8a and MAD 2.5.
The DSDT 3.xx follows original DSDT 2.x and MAD 2.x. 
The customers, who use DSDT 2.x (qdDrv), use DSDT_3.x/switch to get support of switch driver.
The customers, who use MAD 2.x (madDrv), use DSDT_3.x/phy to get support of phy driver.
The DSDT 3.xx provided more new functions to support Marvell Switch and Phy products.
The complete DSDT driver suite includes
  Marvell SOHO Switch driver (switch), 
  Marvell phy driver (phy), 
  ...

The DSDT driver might not include the complete driver, that is based on customer's application.
Phy customer might include DSDT_x.y/phy part only.
SOHO switch customer might include DSDT_x.y/switch part only.

In more detail see README_SW and README_PHY in each driver.

4) How to - build Marvell drivers
---------------------------------
    In Linux system,
    4.1) Build Marvell SOHO Switch driver only (qdDrv)
        <make SW> in root directory.
    4.2) Build Marvell Phy driver (madDrv)
        <make PHY> in root directory.
    4.3) Build Marvell SOHO Switch and Phy drivers
        <make> in root directory.
    4.4) Build Marvell SOHO Switch driver on madDrv 
        <make DSDT_USE_MAD=TRUE> in root directory.

5) How to - build Marvell sw drivers (add by frank)
---------------------------------
    In Linux system,
    5.1) Build Marvell SOHO Switch driver only (qdDrv)
        <cd switch/tools> (need modify your src home path first in switch/tools/setenv file in 
        dir first)
        <source setenv>
        <cd switch/src>
        <make> in switch/src directory. 
        <make clean> in switch/src directory. 

6) Example of global building variables		
---------------------------------
  <setenv> includes example of building global variables in Linux environment.
