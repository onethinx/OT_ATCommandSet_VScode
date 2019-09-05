/********************************************************************************
 *    ___             _   _     _			
 *   / _ \ _ __   ___| |_| |__ (_)_ __ __  __
 *  | | | | '_ \ / _ \ __| '_ \| | '_ \\ \/ /
 *  | |_| | | | |  __/ |_| | | | | | | |>  < 
 *   \___/|_| |_|\___|\__|_| |_|_|_| |_/_/\_\
 *
 ********************************************************************************
 *
 * Copyright (c) 2019 Onethinx BV <info@onethinx.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ********************************************************************************
 *
 * Created by: Rolf Nooteboom on 2019-08-03
 *
 * Library to use AT commands with the Onethinx Core LoRaWAN module
 * 
 * For a description please see:
 * https://github.com/onethinx/OT_ATCommandSet_VScode/blob/master/README.md
 *
 ********************************************************************************/

#include "OnethinxCore01.h"
#include "ATinterface.h"
#include "DemoKit01.h"
#include "cycfg.h"
#include "stdio.h"

int main(void)
{
	CyDelay(1000);	// Use only for debugging otherwise testmode acquire will not work. Testmode acquire is the preffered method. 
					// To acquire in non-testmode use 'set ENABLE_ACQUIRE 0' in OpenOCD settings (CM4_kitprog.cfg)

	/* initialize hardware configuration */
	init_cycfg_all();
	
	/* enable global interrupts */
	__enable_irq();

	Cy_SCB_UART_Init( UART_HW, &UART_config, NULL);
	Cy_SCB_UART_Enable( UART_HW );

	while (1) ATcomm();
}


/* [] END OF FILE */
