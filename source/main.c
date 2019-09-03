/* ==========================================================
 *    ___             _   _     _
 *   / _ \ _ __   ___| |_| |__ (_)_ __ __  __
 *  | | | | '_ \ / _ \ __| '_ \| | '_ \\ \/ /
 *  | |_| | | | |  __/ |_| | | | | | | |>  <
 *   \___/|_| |_|\___|\__|_| |_|_|_| |_/_/\_\
 *
 * Copyright Onethinx, 2019
 * All Rights Reserved
 *
 * UNPUBLISHED, LICENSED SOFTWARE.
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Onethinx BV
 *
 * ==========================================================
*/

#include "OnethinxCore01.h"
#include "ATinterface.h"
#include "DemoKit01.h"
#include "cy_pdl.h"
#include "cycfg.h"

#include "stdio.h"



/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*  Sends a "Hello World" message using LoRaWAN.
* Details:
*  Go to ../OnethinxCore/LoRaWAN_keys.h and fill in the fields of the TTN_OTAAkeys structure
*  Read the quickstart guide on how to create keys.
*******************************************************************************/


int main(void) // LP
{
	CyDelay(1000);
	/* initialize hardware configuration */
	init_cycfg_all();

	
	/* enable global interrupts */
	__enable_irq();

	Cy_SCB_UART_Init( UART_HW, &UART_config, NULL);
	Cy_SCB_UART_Enable( UART_HW );

	while (1) {
		ATcomm();
		
	}
	
	// /* Initialise LoRaWAN Stack */
	// statusLoRaWAN = LoRaWAN_Init(&coreConfig);


	// statusLoRaWAN = LoRaWAN_GetStatus();


	// LoRaWAN_GetInfo(&coreInfo);
	
	// /* Join LoRaWAN network */
	// statusLoRaWAN = LoRaWAN_Join(true);
	
	// LED_B_SET(LED_ON);
	
	// /* checking if it has joined the LoRaWAN network, if not, RED LED will flash at 1Hz */
	// if (!statusLoRaWAN.mac.isJoined)
	// 	while (1)
	// 	{
	// 		LED_R_SET(LED_ON);
	// 		CyDelay(1000);
	// 		LED_R_SET(LED_OFF);
	// 		CyDelay(1000);
	// 	}
	// LED_B_SET(LED_ON);
	// CyDelay(4000);
	// LED_B_SET(LED_OFF);
	//  


}


/* [] END OF FILE */
