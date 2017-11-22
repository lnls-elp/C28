/*
 * 		FILE: 		.h
 * 		PROJECT: 	DRS v2.0
 * 		CREATION:	09/28/2015
 * 		MODIFIED:	10/01/2015
 *
 * 		AUTHOR: 	Gabriel O. Brunheira  (LNLS/ELP)
 *
 * 		DESCRIPTION:
 *		Definition of constants, structs, enumarates and prototypes statements
 *		for variables and functions	from source code of Digital Power Framework.
 *
 *		TODO:
 */

#ifndef F28M36x_ELP_LIBS
#define F28M36x_ELP_LIBS

typedef enum
{
	Buffer_Idle,
	Buffer_All,
	Buffer_Block0,
	Buffer_block1
} eBlockBusy;

typedef volatile struct
{
	volatile float		*PtrBufferStart;
	volatile float		*PtrBufferEnd;
	volatile float		*PtrBufferK;
	eBlockBusy	BufferBusy;
} tBuffer;

/*
 * Especifica versao do DRS
 */
#define UDC_V2_0	0
#define UDC_V2_1	1

#define HRADC_v2_0  0
#define HRADC_v2_1  1

/*
 * Especificacoes do core C28 do Concerto
 */

#define C28_FREQ_MHZ	150
#define LSPCLK_DV		1   	// Divide o LSCLOCK (servido a McBsp e SCI) por 2
#define COUNT_UP

/*
 * Macro defines
 */

#define BUZZER_PITCH_FREQ		4000.0
#define BUZZER_MOD_FREQ			16.0

#define BUZZER_MOD_CLKDIV		0x7		// CLKDIV    : /128
#define BUZZER_MOD_HSPCLKDIV	0x5		// HSPCLKDIV : /10
#define BUZZER_MOD_PERIOD		7323	// PERIOD    : int(150 MHz / (FREQ * CLKDIV * HSPCLKDIV)) - 1

#define INIT_DEBUG_GPIO0 		GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 0;	\
								GpioDataRegs.GPBCLEAR.bit.GPIO46 = 1;	\
								GpioCtrlRegs.GPBDIR.bit.GPIO46 = 1;

#define INIT_DEBUG_GPIO1		GpioCtrlRegs.GPDMUX1.bit.GPIO111 = 0;	\
								GpioDataRegs.GPDCLEAR.bit.GPIO111 = 1;	\
								GpioCtrlRegs.GPDDIR.bit.GPIO111 = 1;

#define SET_DEBUG_GPIO0			GpioDataRegs.GPBSET.bit.GPIO46 = 1;
#define SET_DEBUG_GPIO1			GpioDataRegs.GPDSET.bit.GPIO111 = 1;
#define CLEAR_DEBUG_GPIO0		GpioDataRegs.GPBCLEAR.bit.GPIO46 = 1;
#define CLEAR_DEBUG_GPIO1		GpioDataRegs.GPDCLEAR.bit.GPIO111 = 1;



#include <math.h>
#include <float.h>
#include "DSP28x_Project.h"
//#include "../C28 Project/config.h"

#include "PS_modules.h"

#include "HRADC_Boards.h"
#include "DMA_SPI_Interface.h"
#include "McBSP_SPI_Interface.h"

#include "PWM_modules.h"

#include "ELP_DCL.h"
#include "SigGen.h"
#include "TimeSlicer.h"
#include "DP_framework.h"

#include "IPC_modules.h"

extern void InitBuffer(tBuffer *ptrBuff, volatile float *ptrStart, Uint16 size);
extern void ResetBuffer(tBuffer *ptrBuff);
extern Uint16 SizeBuffer(tBuffer *ptrBuff);
extern Uint16 IndexBuffer(tBuffer *ptrBuff);
extern Uint16 WriteBuffer(tBuffer *ptrBuff, float data);
extern Uint16 Test_SamplesLimit(tBuffer *ptrBuff, float value, float tol);

#endif