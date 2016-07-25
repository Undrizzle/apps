//**************************************************************************************
// File Name  : upgrade.h
//
// Description: add tag with validation system to the firmware image file to be upgraded
//              via http
//
// Created    : 05/18/2015  Burning
//**************************************************************************************

#ifndef _UPGRADE_H_
#define _UPGRADE_H_

#endif

int do_upgrade_pre1(FILE * stream, int upgrade_len, int upgrade_type);
int do_upgrade_pre2(FILE * stream, int upgrade_len, int upgrade_type);
int do_upgrade_pre3(FILE * stream, int upgrade_len, int upgrade_type);

