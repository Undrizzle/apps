/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Example usage of the AT30TS75 Temperature Sensor.
 *
 * - Compiler:           GCC and IAR for AVR
 * - Supported devices:  All AVR devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/*! \page License
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the TWI master mode basic services example.
 *
 * \section files Main Files
 * - at30ts75_example.c: example application.
 * - at30tS75.c: API.
 * - conf_board.h: board configuration
 * - conf_twi_master.h: TWI master configuration used in this example
 * - twi_master.h: Part Specific TWI Master Mode Implementation
 *
 * \section at30tsinfo components/tempsensor/at30ts75 API
 * The AT30T75 APIs can be found \ref at30tS75.h "here".
 
 * \section twiapiinfo services/basic/twi API
 * The TWI API can be found \ref twi_master.h "here".
 *
 * \section deviceinfo Device Info
 * All AVR devices can be used. This example has been tested
 * with the following setup:
 *   - access to the TWI signals.
 *
 * \section exampledescription Description of the example
 * The given example uses the kit as a TWI master:
 *	  - to setup the configuration register
 *	  - to setup the high, and low alarm trip registers
 *	  - to read registers
 *	  - to write and read EEPROM
 *    - the TWI master performs a write access to the TWI slave,
 *    - the TWI master performs a read access of what it just wrote to the TWI slave,
 *    - the read data are compared with the originally written data.
 *
 * LED0 gives the result of the EEPROM test:
 *  If FAILED: LED_RGB_RED is off.
 *  If PASS: LED_RGB_GREEN is on at the end of the test.
 * 
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 * Support and FAQ: http://support.atmel.no/
 */


//#include <avr/io.h>
#include "at30ts.h"
#include "i2c.h"
//#include <util/delay.h>
#include <string.h>
#include "at30ts75.h"

#define PATTERN_TEST_LENGTH  16

/* development kitS */
#ifdef _AT30TK175_

	/* temperature register address */
#define	AT30TSE002B_ADDR			0x1B		//!< 002B 7-bit temperature address on AT30TK175 daughter board	
#define AT30TS75_ADDR				0x48		//!< TS75 7-bit temperature address on AT30TK175 daughter board	
#define	AT30TS750_ADDR				0x49		//!< TS750 7-bit temperature address on AT30TK175 daughter board	
#define	AT30TSE758_ADDR				0x4A		//!< TSSE758 7-bit temperature address on AT30TK175 daughter board	

	/* EEPROM Address */
#define	AT30TSE002B_EEPROM_ADDR		0x53		//!< 002B 7-bit EEPROM address on AT30TK175 daughter board						
#define	AT30TSE758_EEPROM_ADDR		0x52		//!< 002B 7-bit EEPROM address on AT30TK175 daughter board	
	

#elif defined _ATAVRTEMPSENSORX_

	//local (main board): temperature register address
#define	AT30TSE002B_ADDR			0x1B		//!< 002B 7-bit temperature address on AT30TK175 daughter board	
#define	AT30TSE758_ADDR				0x48		//!< TS75 7-bit temperature address on AT30TK175 daughter board	

	/* breakaway boards: temperature register address */
#define	AT30TSE002B_ADDR			0x1B		//!< 002B 7-bit temperature address on AT30TK175 daughter board	
#define AT30TS75_ADDR				0x4C		//!< TS75 7-bit temperature address on AT30TK175 daughter board	
#define	AT30TS750_ADDR				0x4C		//!< TS750 7-bit temperature address on AT30TK175 daughter board	
#define	AT30TSE758_ADDR				0x4C		//!< TSE758 7-bit temperature address on AT30TK175 daughter board	

	/* local (main board): EEPROM Address */
#define	AT30TSE002B_EEPROM_ADDR		0x50		//!< 002B 7-bit EEPROM address on AT30TK175 daughter board						
#define	AT30TSE758_EEPROM_ADDR		0x50		//!< TSE758 7-bit EEPROM address on AT30TK175 daughter board	

	/* breakaway boards: EEPROM Address */
#define	AT30TSE002B_EEPROM_ADDR		0x54		//!< 002B 7-bit EEPROM address on AT30TK175 daughter board						
#define	AT30TSE758_EEPROM_ADDR		0x54		//!< TSE758 7-bit EEPROM address on AT30TK175 daughter board	


#endif

int g_i2c_fd = 0;

int g_at30ts75_init_status = TWI_START_ERROR;

uint8_t at30ts75_read_temperature(st_temperature *temp_data)
{
	uint8_t ret = 0;
	sensor_data_t sensor_data;	
	
	ret = read_temperature(&sensor_data);
	if( TWI_SUCCESS == ret )
	{
		temp_data->itemp = sensor_data.temperature.itemp;
		temp_data->ftemp = sensor_data.temperature.ftemp;
		temp_data->sign = sensor_data.temperature.sign;
		temp_data->raw_value = sensor_data.temperature.raw_value;
	}
	return ret;
}

int get_at30ts75_init_status(void)
{
	return g_at30ts75_init_status;
}

void uninit_at30ts75(void)
{
	twi_master_destory();
}

int init_at30ts75(void)
{
	/* variables declarations */  
	static uint8_t ret = 0; 
	uint8_t i = 0; 
	uint8_t ibuf[16];
	sensor_data_t sensor_data;
	static uint8_t test_pattern[ PATTERN_TEST_LENGTH];
	
	/* initialize hardware components,... set speed */
	twi_master_init(TWBR_VAL);

	/* attach device and initialize parameters */
	attach_device(AT30TS75_ADDR, AT30TS75);
	//attach_device(AT30TSE002B_ADDR, AT30TSE002B);
	
	/* clear variable */
	sensor_data.config_reg.value =0;

	/* set configuration register to 12-bis resolution */
	sensor_data.config_reg.option.RES  = RES12;
	if(write_config(sensor_data.config_reg.value)!= TWI_SUCCESS)
		goto failed;

	/* only set polarity to low */ 
	if(set_config_option(&sensor_data, POL, POL_ACTIVE_LOW) != TWI_SUCCESS)
		goto failed;

	/* read the configuration register */ 
	if(read_config(&sensor_data) != TWI_SUCCESS)
		goto failed;
   
#if defined _AT30TS00_ || defined _AT30TSE002B_		
	/* set t_high limit register to +75.0000C */
	if(write_tcrit(pos, 75, 0000)!= TWI_SUCCESS)
		goto failed;
#endif  
    
	/* set t_high limit register to +60.2500C */
	if(write_thigh(pos, 60, 2500)!= TWI_SUCCESS)
		goto failed;
	
	/* set t_low limit register to -20.2500C */
	if(write_tlow(neg, 20, 2500)!= TWI_SUCCESS)
		goto failed;
	
#if defined _AT30TS00_ || defined _AT30TSE002B_	
	/* read t_crit register register */
	if(read_tcrit(&sensor_data) != TWI_SUCCESS)
		goto failed;
#endif

	/* read t_high limit register */
	if(read_thigh(&sensor_data) != TWI_SUCCESS)
		goto failed;

	/* read t_low register register */
	if(read_tlow(&sensor_data) != TWI_SUCCESS)
		goto failed;

	/* non volatile register functionality */
#if defined _AT30TS750_  || defined _AT30TSE752_ || \
    defined _AT30TSE754_ || defined _AT30TSE758_
	
	/* copy volatile registers to nonvolatile registers 
	 *
	 * vol configuration register  -> nonvol configuration register
	 * vol t_high register		   -> nonvol t_high register
	 * vol t_low  register         -> nonvol t_low register
	 *	 	   
	*/
	
	if((ret = ts75_copy_vol_nonvol_register ()) != TWI_SUCCESS)
		goto failed;
  
	/* read the nonvol configuration register */ 
	if( read_nvconfig(&sensor_data) != TWI_SUCCESS)
		goto failed;
	
	/* read the nonvol t_high register */ 
	if( read_nvthigh(&sensor_data) != TWI_SUCCESS)
		goto failed;  
  
	/* read the nonvol t_low register */ 
	if( read_nvtlow(&sensor_data) != TWI_SUCCESS)
		goto failed;
	
	/* clear vol configuration register */
	if(write_config(0x0000)!= TWI_SUCCESS)
		goto failed;
  
	/* read the vol configuration register */ 
	if(read_config(&sensor_data) != TWI_SUCCESS)
		goto failed;
  
	/* copy nonvolatile registers to volatile registers */
	if(ts75_copy_nonvol_vol_register () != TWI_SUCCESS)
		goto failed;
  
	/* read the configuration register */ 
	if(read_config(&sensor_data) != TWI_SUCCESS)
		goto failed;

#endif

	/* EEPROM memory Test */
#if defined _AT30TSE002B_  || defined _AT30TSE752_ || \
    defined _AT30TSE754_   || defined _AT30TSE758_

	/* Generate Test Pattern */
	for(i=0; i < PATTERN_TEST_LENGTH; i++)
		test_pattern[i]= i;
  
	/* Write data to the EEPROM */
	/* Perform a write access & check write result */
	if ((ret = ts_write_memory(0x00, PATTERN_TEST_LENGTH, (void *) test_pattern)) != TWI_SUCCESS)
		goto failed;
	
	/* clear test_pattern */
	memset(ibuf,0,sizeof(ibuf));

	/* perform a read access & check read result */
	if(ts_read_eeprom (0x00, PATTERN_TEST_LENGTH, ibuf) != TWI_SUCCESS)
	{
		asm("nop");
		goto failed;
	}	

	/* check received data against sent data */
	for (i = 0; i < PATTERN_TEST_LENGTH; i++)
	{
		if (ibuf[i] != test_pattern[i])
			goto failed;
	}
	
#endif
	/* set g_at30ts75_init_status to TWI_SUCCESS */
	g_at30ts75_init_status = TWI_SUCCESS;
	return TWI_SUCCESS;

failed:
	return TWI_START_ERROR;
}

