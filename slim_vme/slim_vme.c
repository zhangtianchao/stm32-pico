/**************************************************************
*
* Revision History of slim_vme.c
*
*
* 09/11/07 NN Updated to support version 1.3
* This version supported new POLING STATUS LOOP opcodes (LOOP and ENDLOOP)
* for Flash programming of the Lattice FPGA devices
* 09/11/07 NN Added Global variables initialization
* 09/11/07 NN type cast all mismatch variables
***************************************************************/

#include <stdio.h>
#include "opcode.h"


/*************************************************************
*                                                            *
* EXTERNAL FUNCTIONS                                         *
*                                                            *
*************************************************************/

extern unsigned char GetByte( int a_iCurrentIndex, char a_cAlgo );
extern int ispProcessVME();
extern void EnableHardware();
extern void DisableHardware();

/***************************************************************
*
* Supported VME versions.
*
***************************************************************/
char *g_szSupportedVersions[] = { "_SVME1.1", "_SVME1.2", "_SVME1.3", 0 };

/*************************************************************
*                                                            *
* EXTERNAL VARIABLES                                         *
*                                                            *
*     If the algorithm does not require the data, then       *
*     declare the variables g_pucDataArray and g_iDataSize   *
*     as local variables and set them to NULL and 0,         *
*     respectively.                                          *
*                                                            *
*     Example:                                               *
*          xdata unsigned char * g_pucDataArray = NULL;      *
*          xdata int g_iDataSize = 0;                        *
*                                                            *
*************************************************************/

extern int g_iMovingAlgoIndex;
extern int g_iMovingDataIndex;
extern unsigned int g_usDataType;

const unsigned char * g_pucDataArray = NULL;
int g_iDataSize = 0;
const unsigned char *g_pucAlgoArray = NULL;
int g_iAlgoSize = 0;

/*************************************************************
*                                                            *
* ISPVMENTRYPOINT                                            *
*                                                            *
* INPUT:                                                     *
*     a_pszAlgoFile: this is the name of the algorithm file. *
*                                                            *
*     a_pszDataFile: this is the name of the data file.      *
*     Note that this argument may be empty if the algorithm  *
*     does not require a data file.                          *
*                                                            *
* RETURN:                                                    *
*     The return value will be a negative number if an error *
*     occurred, or 0 if everything was successful            *
*                                                            *
* DESCRIPTION:                                               *
*     This function opens the file pointers to the algo and  *
*     data file.  It intializes global variables to their    *
*     default values and enters the processor.               *
*                                                            *
*************************************************************/

int ispEntryPoint(const unsigned char *fwData, int fwSize, const unsigned char *algoData, int algoSize)
{
    char szFileVersion[ 9 ] = { 0 };
    int siRetCode     = 0;
    int iIndex        = 0;
    int cVersionIndex = 0;

    /*************************************************************
    *                                                            *
    * VARIABLES INITIALIZATION                                   *
    *                                                            *
    *************************************************************/
    g_iDataSize = fwSize;
    g_pucDataArray = fwData;
    g_iAlgoSize = algoSize;
    g_pucAlgoArray = algoData;
    
    g_usDataType       = 0;
    g_iMovingAlgoIndex = 0;
    g_iMovingDataIndex = 0;

    if ( g_iDataSize > 0 && g_iDataSize < 0x00100000) {
        if ( GetByte( g_iMovingDataIndex++, 0 ) ) {
            g_usDataType |= COMPRESS;
        }
    } else {
        return ERR_OUT_OF_MEMORY;
    }

    
    /***************************************************************
    *
    * Read and store the version of the VME file.
    *
    ***************************************************************/
    for ( iIndex = 0; iIndex < 8; iIndex++ ) {
        szFileVersion[ iIndex ] = GetByte( g_iMovingAlgoIndex++, 1);
    }
    
    
    /***************************************************************
    *
    * Compare the VME file version against the supported version.
    *
    ***************************************************************/
    for ( cVersionIndex = 0; g_szSupportedVersions[ cVersionIndex ] != 0; cVersionIndex++ ) {
        for ( iIndex = 0; iIndex < 8; iIndex++ ) {
            if ( szFileVersion[ iIndex ] != g_szSupportedVersions[ cVersionIndex ][ iIndex ] ) {
                siRetCode = ERR_WRONG_VERSION;
                break;
            }
            siRetCode = 0;
        }

        if ( siRetCode == 0 ) {
            //Found matching version
            DPRINT(LOG_DEBUG,"%s: algo_ver=%s\r\n", __func__,g_szSupportedVersions[cVersionIndex]);
            break;
        }
    }

    if ( siRetCode < 0 ) {
        //VME file version failed to match the supported versions.
        return ERR_WRONG_VERSION;
    }

    // Start the hardware.
    EnableHardware();
    // Begin processing algorithm and data file.
    siRetCode = ispProcessVME();
    // Stop the hardware.
    DisableHardware();

    //Return the return code.
    return ( siRetCode );
}

