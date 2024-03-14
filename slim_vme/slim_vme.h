//=============================================================================
// Description : interface for slim vme files player
//
//-----------------------------------------------------------------------------
// Usage:     Customize the hardware.c function implementations to establish
//            the correct protocol for communicating with your JTAG ports
//            FINALLY - Call ispEntryPoint().
//=============================================================================

#ifndef __SLIM_VME_H__
#define __SLIM_VME_H__

// #define CPLD_PROGRAM(fwData,fwSize)     ispEntryPoint(fwData,fwSize)

int ispEntryPoint(const unsigned char *fwData, int fwSize, const unsigned char *algoData, int algoSize);

#endif  // __SLIM_VME_H__

