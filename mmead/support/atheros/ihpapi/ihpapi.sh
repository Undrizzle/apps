#!/bin/bash
# file: ihpapi/ihpapi.sh

# ====================================================================
#
# --------------------------------------------------------------------

gcc -c DecodeEthernetHeader.c
gcc -c DecodeEthernetPHYSettings.c
gcc -c DecodeFlashNVM.c
gcc -c DecodeGetNVM.c
gcc -c DecodeGetRxToneMap.c
gcc -c DecodeGetToneMap.c
gcc -c DecodeGetVersion.c
gcc -c DecodeHostAction.c
gcc -c DecodeLinkStats.c
gcc -c DecodeMfgString.c
gcc -c DecodeNetworkInfo.c
gcc -c DecodeReadModule.c
gcc -c DecodeResetDevice.c
gcc -c DecodeResetToFactoryDefaults.c
gcc -c DecodeSetKey.c
gcc -c DecodeSetSDRAMCfg.c
gcc -c DecodeStartMAC.c
gcc -c DecodeWriteMemory.c
gcc -c DecodeMdioCommand.c
gcc -c DecodeWriteModule.c
gcc -c EncodeEthernetHeader.c
gcc -c EncodeIntellonHeader.c
gcc -c EncodeSetSDRAMCfg.c
gcc -c FindFWHeader.c
gcc -c GetConnectionInfo.c
gcc -c GetManufacturerInfo.c
gcc -c GetNetworkInfo.c
gcc -c GetRxToneMap.c
gcc -c GetToneMap.c
gcc -c GetVersionInfo.c
gcc -c InitializeDevice.c
gcc -c ManageEnetPHY.c
gcc -c ResetDevice.c
gcc -c ResetToFactoryDefaults.c
gcc -c RxFrame.c
gcc -c SequenceInProgress.c
gcc -c SequenceReadModule.c
gcc -c SequenceTerminate.c
gcc -c SequenceWriteMemory.c
gcc -c SequenceWriteModule.c
gcc -c SetKey.c
gcc -c SetSDRAMConfig.c
gcc -c TxFrame.c
gcc -c UpdateDevice.c
gcc -c ihpapi.c

