/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief temperature sensor source file.
 *
 * - Compiler:           GCC and IAR for AVR
 * - Supported devices:  All AVR XMega devices with TWI module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

/* Copyright (c) 2010 Atmel Corporation. All rights reserved.
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


#include "string.h"
#include "at30ts.h"

//for kit support
#ifdef _AT30TK175_

	#include "i2c.h"

#elif defined _ATAVRTEMPSENSORX_

	#include "compiler.h"
	#include "twi_master.h"

#endif

const uint8_t copy_vol_nonvol_register[]	= { CPY_VOL_NVOL_REG };
const uint8_t copy_nonvol_vol_register[]	= { CPY_NVOL_VOL_REG };
const uint8_t nv_config[]					= { NV_CONFIG_REG };


uint8_t status;
uint8_t iobuf[3];

#define valtobuf(val,buf) {	buf[2] = val & 0xFF; \
						    buf[1] = (val >> 8) & 0xFF;}
							
							
#define buftoval(val,buf) {	val = iobuf[0]; \
						    val <<= 8;\
						    val |= iobuf[1];}						
							
									
// ------------------------ timing definitions -----------------------------------
/** \brief ACK_POLLING_INTERATION
 *
 * With an oscilloscope or logic analyzer, determine the number of NACK iteration required
 * for ts_ack_polling() to span 5ms.  5ms is the maximum time required for a nonvolatile
 * operation to complete.
 * 
 * For example: With the F_CPU running at 16MHz and an optimization of -O2,
 * it takes 165 iterations of ts_ack_polling to span 5ms.   
 */

  /* FOR ACK POLLING */
#ifdef _AT30TK175_
	#define ACK_POLLING_INTERATION  165	  

#elif defined _ATAVRTEMPSENSORX_
   #define ACK_POLLING_INTERATION  1000	
     
#endif   



/*!
 * \brief ACK polling function
 *
 * \param addr device address to ACK poll					
 * \return TWI_SUCCESS if all bytes were read, otherwise error code: AT30TS_TWI_WRITE_NACK
 */

uint8_t ts_ack_polling(uint8_t addr)
{
    
	uint16_t n = ACK_POLLING_INTERATION;

	do 
	{
	   
	   // Perform a write access & check write result
	   if( twi_probe_device(addr) == TWI_SUCCESS) 
		return TWI_SUCCESS;

	} while (--n);
	
	 return AT30TS_TWI_WRITE_NACK;

}

/*!
 * \brief attach_device, initialize epprom variable assicated with device
 *
 * \param device_addr  device address register offset address					
 * \param type		   device type
 * \return TWI_SUCCESS if all bytes were read
 */
uint8_t attach_device(uint8_t device_addr, device_type_t type)
{

	attached_device.ts_reg_addr = device_addr;
	attached_device.device_type	= type;
	
	switch(type)
	{
		
		case AT30TS75:
		case AT30TS750:
			 attached_device.eeprom_size =  0;
			 break;
			 
		case AT30TSE002B:
		case AT30TSE752:
			attached_device.eeprom_size =  256;
			break;
			
		case AT30TSE754:
			attached_device.eeprom_size	=  512;
			break;
				
		case AT30TSE758:
			attached_device.eeprom_size =  1024;
			break;	
		
		default:
		    attached_device.eeprom_size =  0;
			break;
		
	}

return 0;
}


	

/*!
 * \brief Write configuration register
 *
 * \param *obuf		Package information and data
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */
uint8_t ts_write_register(uint8_t *obuf)
{

	twi_package_t packet;
	uint8_t nonvolatile_regs  = ((obuf[0] == NV_CONFIG_REG) || (obuf[0] == NV_TLOLIM_REG) || (obuf[0] == NV_THILIM_REG));
	uint8_t nv_regs_supported = (attached_device.device_type == AT30TS750) || (attached_device.device_type == AT30TSE758);

	// before writing nonvolatile registers,
	// first check to see if NVRBSY is busy
	if (nv_regs_supported && nonvolatile_regs)
	{
		if((status = check_NVRBSY()) !=  AT30TS_SUCCESS)
			return status;
	}


	// TWI chip address to communicate with
	packet.chip         = attached_device.ts_reg_addr;
	// Internal address - pointer register
	packet.addr         = 0x00;
	packet.addr_length  = 0x00;
	// Data to be written
	packet.buffer       = obuf;
	// How many bytes do we want to write
	packet.length       = 0x03;

	// Perform a write access
	status = twi_write(&packet);
	
	if (status != TWI_SUCCESS)
		return AT30TS_TWI_WRITE_NACK;

	if (nv_regs_supported && nonvolatile_regs)
	{
		// wait max time for nonvol write to complete
		if (nonvolatile_regs)
		 {status |= check_NVRBSY();}
	}

	return status;
}


/*!
 * \brief Read at30ts75xx configuration register
 *
 * \param pointer_reg register offset address					
 * \param *buf			store temperature data
 * \return TWI_SUCCESS if all bytes were read, error code otherwise
 */
uint8_t ts_read_register(const uint8_t pointer_reg, uint8_t *buf)
{
	twi_package_t packet;

	// TWI chip address to communicate with
	packet.chip = attached_device.ts_reg_addr;
	// Internal address - pointer register
	packet.addr = 0x00;
	packet.addr_length = 0x00;

	// to prevent warning pointer_reg being const
	uint8_t reg		= pointer_reg;
	packet.buffer	= &reg;

	// How many data bytes to write
	packet.length = 0x01;

	// Perform a write access & check result
	if (twi_write(&packet) != TWI_SUCCESS)
		return AT30TS_TWI_WRITE_NACK;
	
	// How many bytes to read
	packet.length = 0x02;	
	// Where to put the read data
	packet.buffer = buf;

	// Perform a read access & check result
	if (twi_read(&packet) != TWI_SUCCESS)
		return AT30TS_TWI_READ_NACK;

	return TWI_SUCCESS;
}

/*!
 * \brief Write EEPROM
 *
 * \param address      Data word address
 * \param size		   Size of the data to write
 * \param *buf		   Data to be written
 *
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */
uint8_t ts_write_eeprom(uint16_t address, uint16_t size, uint8_t *buf)
{
  twi_package_t packet;
  uint8_t data[17]; // page size is 16 plus one address byte

  // check for valid eeprom address
  if ((address + size) > attached_device.eeprom_size)
    return AT30TS_INVALID_EEPROM_ADDRESS;

  if (size > PAGE_SIZE)
    return AT30TS_INVALID_SIZE;

  ts_construct_eeprom_address(&packet, address);

  // data word address
  data[0] = address & 0xFF;
  memcpy (data + 1, buf, size);

  // Word address
  packet.addr 			= 0x00;
  packet.addr_length 	= 0x00;
  // Data to be written
  packet.buffer = data;
  // How many bytes do we want to write + 1 byte for the word address
  packet.length = size + 1;

  // Perform a write access
  status = twi_write(&packet);
  if (status != TWI_SUCCESS)
    return AT30TS_TWI_WRITE_NACK;

  // wait for write cycle to complete (max 5ms)
  status |= ts_ack_polling(packet.chip);
 
  return status;
}


/*!
 * \brief Read EEPROM
 *
 * \param address     Data word address
 * \param size		  Size of the data to read
 * \param *buf		  Where to store the data
 * \return TWI_SUCCESS if all bytes were read, error code otherwise
 */
uint8_t ts_read_eeprom(uint16_t address, uint16_t size, uint8_t *buf)
{
  twi_package_t packet;

  // check for valid eeprom address
  if ((address + size) > attached_device.eeprom_size)
    return AT30TS_INVALID_EEPROM_ADDRESS;

  ts_construct_eeprom_address(&packet, address);

  // Option settings
  packet.addr = address & 0xFF;
  packet.addr_length = 0x01;
  packet.length = 0x00;

  // Perform a write access & check result
  status = twi_write(&packet);
  if (status != TWI_SUCCESS)
    return AT30TS_TWI_WRITE_NACK;

  // How many bytes to read
  packet.length = size;
  // Where to put the read data
  packet.buffer = buf;
  
  // Perform a read access & check result
  status = twi_read(&packet);
  if (status != TWI_SUCCESS)
    return AT30TS_TWI_READ_NACK;

  return TWI_SUCCESS;
}


 /*!
  * \brief Copy volatile registers to nonvolatile registers
  *
  * \return TWI_SUCCESS if copy was successful, error code otherwise
  */
uint8_t ts75_copy_vol_nonvol_register()
{

	twi_package_t packet;

	// TWI chip address to communicate with
	packet.chip = attached_device.ts_reg_addr;
	// Internal address - pointer register
	packet.addr = 0x00;
	packet.addr_length = 0x00;
	// Data to be written
	packet.buffer = (void *) copy_vol_nonvol_register;
	// How many bytes do we want to write
	packet.length = 0x01;

	// before writing nonvolatile registers,
	// first check to see if NVRBSY is busy
	if((status = check_NVRBSY()) !=  AT30TS_SUCCESS)
		return status;

	// Perform a write access
	status = twi_write(&packet);
	if (status != TWI_SUCCESS)
		return AT30TS_TWI_WRITE_NACK;

	status |= check_NVRBSY();
			
	return  status;
}


 /*!
  * \brief Copy nonvolatile registers to volatile registers
  *
  * \return TWI_SUCCESS if copy was successful, error code otherwise
  */
uint8_t ts75_copy_nonvol_vol_register()
{
	twi_package_t packet;

	// TWI chip address to communicate with
	packet.chip = attached_device.ts_reg_addr;
	// Internal address - pointer register
	packet.addr = 0x00;
	packet.addr_length = 0x00;
	// Data to be written
	packet.buffer = (void *) copy_nonvol_vol_register;
	// How many bytes do we want to write
	packet.length = 0x01;

	// before writing nonvolatile registers,
	// first check to see if NVRBSY is busy
	if((status = check_NVRBSY()) !=  AT30TS_SUCCESS)
		return status;

	// Perform a write access
	status = twi_write(&packet);
	if (status != TWI_SUCCESS)
		return AT30TS_TWI_WRITE_NACK;

	return status;
}


/** \brief Constructs an eeprom address.
 *
 * \param[in, out] packet pointer to packet structure
 * \param[in] address eeprom address
 */
void ts_construct_eeprom_address(twi_package_t *packet, uint16_t address)
{
    /* serial eeprom upper nibble */
    packet->chip = 0x50;

	if ((attached_device.device_type == AT30TSE002B) | (attached_device.device_type == AT30TSE752)) // 256
	  packet->chip |= (attached_device.ts_reg_addr & 0x07);
	
	else if (attached_device.device_type == AT30TSE754) // 512
	{	  
	  // obtain A1 and A2 bits
	  packet->chip |= (attached_device.ts_reg_addr & 0x06);
	  // obtain the page bit (P0): [101] [0 A2 A1 P0]
	  packet->chip |= ((address >> 8) & 0x01);
	}
	else if (attached_device.device_type == AT30TSE758) // 1024
	{
	  // obtain A2 bit
	  packet->chip |= (attached_device.ts_reg_addr & 0x04);
	  // obtain the page bits (P1, P0): [101] [0 A2 P1 P0]
	  packet->chip |= ((address >> 8) & 0x03);
	  
	}
   
}



/*!
 * \brief Probe at30ts75 nonvolatile configuration register
 *
 * \param device_addr   at30ts75 device address
 * \return TWI_SUCCESS if device ACK's, error code otherwise
 */
uint8_t ts75_probe_nonvol_register(uint8_t device_addr)
{

  twi_package_t packet;

  // TWI chip address to communicate with
  packet.chip = device_addr;
  // Internal address - pointer register
  packet.addr = 0x00;
  packet.addr_length = 0x00;

  packet.buffer = (void *) nv_config;

  // How many data bytes write
  packet.length = 0x01;

  // Perform a write access & check write result
  status = twi_write(&packet);
  if (status != TWI_SUCCESS)
    return AT30TS_TWI_WRITE_NACK;

  return TWI_SUCCESS;
}

/*!
 * \brief Checks at30ts75 NVRBSY, RLCKDWN and RCLK bits
 *
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t check_NVRBSY(void)
{
	uint8_t buf[2];
	uint8_t n = 80;
	
	do
	{

		/* read vol configuration register */
		ts_read_register(CONFIG_REG, buf);

		//do n-attempts, then abort 
		if(!n)
         return AT30TS_NVRBSY_BUSY;   
		
		n--;

	}while (buf[1] & NVRBSY);


	status = ts_read_register(NV_CONFIG_REG, buf);

	if (status != TWI_SUCCESS)
	return status;

	if (buf[1] & RLCKDWN_MSK || buf[1] & RLCK_MSK)
	return AT30TS_REG_LOCKED; 

	return AT30TS_SUCCESS;
}



/*!
 * \brief Read tLOW limit register
 *
 * \param *sensor_data	Where to store the retrieved data
 * \return				TWI_SUCCESS if all bytes were read,
 *						error code otherwise
 */
uint8_t read_tlow(sensor_data_t *sensor_data)
{

#if defined _AT30TS00_ || defined _AT30TSE002B_	
	status = ts_read_register(TRIP_LWR_REG, iobuf);
	buftoval(sensor_data->limits.low, iobuf);
#else
	status = ts_read_register(TLOLIM_REG, iobuf);
	buftoval(sensor_data->limits.low, iobuf);
#endif

	return status;
}


/*!
 * \brief Read tHIGH limit register
 *
 * \param *sensor_data	Where to store the retrieved data
 * \return				TWI_SUCCESS if all bytes were read,
 *						error code otherwise
 */
uint8_t read_thigh(sensor_data_t *sensor_data)
{


#if defined _AT30TS00_ || defined _AT30TSE002B_	
	status = ts_read_register(TRIP_UPR_REG, iobuf);
	buftoval(sensor_data->limits.high, iobuf);
#else
	status = ts_read_register(THILIM_REG, iobuf);
	buftoval(sensor_data->limits.high, iobuf);
#endif

	return status;	
	
}


/*!
 * \brief Read nonvolatile config register
 *
 * \param *sensor_data	Where to store the retrieved data
 * \return				TWI_SUCCESS if all bytes were read,
 *						error code otherwise
 */
uint8_t read_config(sensor_data_t *sensor_data)
{

#if defined _AT30TS00_ || defined _AT30TSE002B_
	status =  ts_read_register(CONFIGUR_REG, iobuf);
#else
	status =  ts_read_register(CONFIG_REG, iobuf);
#endif
	
	buftoval(sensor_data->config_reg.value, iobuf);
	return status;
}


/*!
 * \brief Write tLOW limit register
 *
 * \param sign		   temperature sign, used for conversion
 * \param itemp		   Integer data part to be written
 * \param ftemp		   Fractional data to be written
 *
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */

uint8_t write_tlow(uint8_t sign, uint8_t itemp, uint16_t ftemp)
{

	uint16_t val;
	
#if defined _AT30TS00_ || defined _AT30TSE002B_	
	val = (itemp << 4) | ((ftemp/2500) << 2);
	// set pointer register
	iobuf[0] = TRIP_LWR_REG;
#else	
	val = (itemp << 8) | ((ftemp/625) << 4);
	// set pointer register
	iobuf[0] = TLOLIM_REG;
#endif	

	/* 2's compliment */ 
	if(sign == neg)
	{
	  val = ~val;
	  val += 1;
	}	

    // load val into buf
	valtobuf(val,iobuf);
	
	// write the tLOW limit register
    return ts_write_register(iobuf);

}


/*!
 * \brief Write tHIGH limit register
 *
 * \param sign		   temperature sign, used for conversion
 * \param itemp		   Integer data part to be written
 * \param ftemp		   Fractional data to be written
 *
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */


uint8_t write_thigh(uint8_t sign, uint8_t itemp, uint16_t ftemp)
{
	
	uint16_t val;
	
#if defined _AT30TS00_ || defined _AT30TSE002B_	
	val = (itemp << 4) | ((ftemp/2500) << 2);
	// set pointer register
	iobuf[0] = TRIP_UPR_REG;
#else	
	val = (itemp << 8) | ((ftemp/625) << 4);
	// set pointer register
	iobuf[0] = THILIM_REG;
#endif	
 
	/* 2's compliment */ 
	if(sign == neg)
	{
	  val = ~val;
	  val += 1;
	}	 

    // load val into buf
	valtobuf(val,iobuf);
	
	// write the tHIGH limit register
    return ts_write_register(iobuf);
}



/*!
 * \brief Write configuration register
 *
 * \param val		   Data to be written
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */
uint8_t write_config(uint16_t val)
{
	/* set pointer register */
#if defined _AT30TS00_ || defined _AT30TSE002B_
	iobuf[0] = CONFIGUR_REG;	
#else
	iobuf[0] = CONFIG_REG;
#endif
	
    /* load val into buf */
	valtobuf(val,iobuf);
	
	/* write the configuration register */
    return ts_write_register(iobuf);

}


/*!
 * \brief Set Configuration Options
 * \param data		   Sensor data type, holds sensor information
 * \param flag		   Defines which option to change
 * \param opt		   Option(s) settings
 
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t set_config_option(sensor_data_t *data, opt_flag_t flag, config_options_t opt)
{
	
#if defined _AT30TS00_ || defined _AT30TSE002B_	

	 /* read current configuration setting */ 
	status =  ts_read_register(CONFIGUR_REG, iobuf);
	
	/* save current configuration setting */
    buftoval(data->config_reg_002b.value,iobuf);
	
	switch(flag)
	{
	   
		case EVTMOD:
			/* set configuration resolution parameter */
			data->config_reg_002b.option.EVTMOD =  opt;	   
			break;   
		
		case EVTPOL:
			/* set configuration register fault parameter */
			data->config_reg_002b.option.EVTPOL =  opt;	   
			break;  
			
		case CRITEVT:
			/* set configuration register polarity parameter */
			data->config_reg_002b.option.CRITEVT=  opt;	   
			break;  
			
		case EVTOUT:
			/* set configuration register comparator/interrupt parameter */
			data->config_reg_002b.option.EVTOUT  =  opt;	   
			break; 
									
		case EVTSTS:
			/* set configuration register shutdown parameter */
			data->config_reg_002b.option.EVTSTS  =  opt;	   
			break; 		  
			
		case EVTCLR:
			/* set configuration register one-shot parameter */
			data->config_reg_002b.option.EVTCLR  =  opt;	   
			break; 		  
			
		case WINLOCK:
			/* set configuration register one-shot parameter */
			data->config_reg_002b.option.WINLOCK  =  opt;	   
			break;
			
		case CRTALML:
			/* set configuration register one-shot parameter */
			data->config_reg_002b.option.CRTALML  =  opt;	   
			break;
			
		case SHTDWN:
			/* set configuration register one-shot parameter */
			data->config_reg_002b.option.SHTDWN  =  opt;	   
			break;			
				
		case HYSTENB:
			/* set configuration register one-shot parameter */
			data->config_reg_002b.option.HYSTENB  =  opt;	   
			break;							 
	   
	}	   

	return write_config(data->config_reg_002b.value);
	
#else

	 /* read current configuration setting */ 
	ts_read_register(CONFIG_REG, iobuf);
	 
	/* save current configuration setting */
    buftoval(data->config_reg.value,iobuf);
	
	switch(flag)
	{
	   
		case RES:
			/* set configuration resolution parameter */
			data->config_reg.option.RES =  opt;	   
			break;   
		
		case FAULT:
			/* set configuration register fault parameter */
			data->config_reg.option.FAULT =  opt;	   
			break;  
			
		case POL:
			/* set configuration register polarity parameter */
			data->config_reg.option.POL =  opt;	   
			break;  
			
		case CMP_INT_MODE:
			/* set configuration register comparator/interrupt parameter */
			data->config_reg.option.CMP_INT  =  opt;	   
			break; 
									
		case SD:
			/* set configuration register shutdown parameter */
			data->config_reg.option.SD  =  opt;	   
			break; 		  
			
		case OS:
			/* set configuration register one-shot parameter */
			data->config_reg.option.OS  =  opt;	   
			break; 		  
			
			/* parameters are not valid for volatile configuration register */
		case RLCK:
		case RLCKDWN:
			break;				 
	   
	}	   

	return write_config(data->config_reg.value);

 #endif
 
}

/*!
 * \brief Read temperature register
 *
 * \param *sensor_data	Where to store the retrieved data
 * \return				TWI_SUCCESS if all bytes were read,
 *						error code otherwise
 */
uint8_t read_temperature(sensor_data_t *sensor_data)
{
   uint16_t val;
   uint8_t buf[2];

   /* get temperature data */
#if defined _AT30TS00_ || defined _AT30TSE002B_

   status = ts_read_register(TEMP_DATA_REG, buf);
   
   if(status == TWI_SUCCESS)
   {
	      
	   val = buf[0];  // MSB
	   val <<= 8;    
	   val |= buf[1]; // LSB
       
	   /* bits 15 - 0 */
	   sensor_data->temperature.raw_value = val;
	   sensor_data->temperature.sign = val >> 12;
   
	   /* neg value */
	   if(sensor_data->temperature.sign)
	   {
			/* 2's compliment */ 
			val = ~val;
			val += 1;
		
			sensor_data->temperature.ftemp = ((val >> 2) & 0x0F) * 2500; 
			sensor_data->temperature.itemp = (val  >> 4) & 0xFF;  
			
	   }else
	   {
			/* pos value */
			sensor_data->temperature.itemp = (val >> 4) ; 
			sensor_data->temperature.ftemp = (val>>1 & 0x07) * 1250; 
	   }  
	   
	   return TWI_SUCCESS;

   }

#else

   status = ts_read_register(TEMPER_REG, buf);
   
   if(status == TWI_SUCCESS)
   {
   
	   val = buf[0];
	   val <<= 8;
	   val |= buf[1];
   
	   sensor_data->temperature.raw_value = val;
	   
	   // KMJ, verify this
	   sensor_data->temperature.sign = (val & 0x8000)>>8;
   
	   /* neg value */
	   if(sensor_data->temperature.sign)
	   {
			/* 2's compliment */ 
			val = ~val;
			val += 1;
		
			sensor_data->temperature.ftemp = ((val & 0xff)>>4)*625; 
			sensor_data->temperature.itemp = (val>>8) & 0xff;   
	   }else
	   {
			/* pos value */
			sensor_data->temperature.itemp = buf[0]; 
			sensor_data->temperature.ftemp = (buf[1]>>4)*625;
	   }  
	   
	   return TWI_SUCCESS;

   }
   
 #endif  
   else
    return AT30TS_TWI_WRITE_NACK;

}

#if defined _AT30TS750_  || defined _AT30TSE752_ || \
    defined _AT30TSE754_ || defined _AT30TSE758_
	
	
/*!
 * \brief Read nonvolatile tLOW limit register
 *
 * \param *sensor_data	Where to store the retrieved data
 * \return				TWI_SUCCESS if all bytes were read,
 *						error code otherwise
 */
uint8_t read_nvtlow(sensor_data_t *sensor_data)
{

	status = ts_read_register(NV_TLOLIM_REG, iobuf);
	buftoval(sensor_data->nv_limits.low, iobuf);
	return status;
}


/*!
 * \brief Read nonvolatile tHIGH limit register
 *
 * \param *sensor_data	Where to store the retrieved data
 * \return TWI_SUCCESS if all bytes were read, error code otherwise
 */
uint8_t read_nvthigh(sensor_data_t *sensor_data)
{
	status = ts_read_register(NV_THILIM_REG, iobuf);
	buftoval(sensor_data->nv_limits.high, iobuf);

	return status;	
	
}


/*!
 * \brief Read nonvolatile config register
 *
 * \param *sensor_data  Where to store the retrieved data
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t read_nvconfig(sensor_data_t *sensor_data)
{
	 
	status =  ts_read_register(NV_CONFIG_REG, iobuf);
	buftoval(sensor_data->nvconfig_reg.value, iobuf);
	return status;
}	
	
	
/*!
 * \brief Write nonvolatile tLOW limit register
 *
 * \param val		   Data to be written
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */

uint8_t write_nvtlow(uint8_t sign, uint8_t itemp, uint16_t ftemp)
{

	uint16_t val;
	val = (itemp << 8) | ((ftemp/625) << 4);

	/* 2's compliment */ 
	if(sign == neg)
	{
	  val = ~val;
	  val += 1;
	}	
	 
	// set pointer register
	iobuf[0] = NV_TLOLIM_REG;
	
    // load val into buf
	valtobuf(val,iobuf);
	
	// write the tLOW limit register
    return ts_write_register(iobuf);

}

/*!
 * \brief Write nonvolatile tHIGH limit register
 *
 * \param val		   Data to be written
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */

uint8_t write_nvthigh(uint8_t sign, uint8_t itemp, uint16_t ftemp)
{
	
	uint16_t val;
	val = (itemp << 8) | ((ftemp/625) << 4);
	 
	/* 2's compliment */ 
	if(sign == neg)
	{
	  val = ~val;
	  val += 1;
	}	 
	 
	// set pointer register
	iobuf[0] = NV_THILIM_REG;
	
    // load val into buf
	valtobuf(val,iobuf);
	
	// write the tHIGH limit register
    return ts_write_register(iobuf);
}

/*!
 * \brief Write nonvolatile configuration register
 *
 * \param val		   Data to be written
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */
uint8_t write_nvconfig(uint16_t val)
{
	/* set pointer register */
	iobuf[0] = NV_CONFIG_REG;
	
    /* load val into buf */
	valtobuf(val,iobuf);
	
	/* write the configuration register */
    return ts_write_register(iobuf);

}




/*!
 * \brief Set bit(s) in nonvolatile configuration register
 *
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t set_nvconfig_option(sensor_data_t *data, opt_flag_t flag, nvconfig_options_t opt)
{
	
	 	/* read current configuration setting */ 
	ts_read_register(NV_CONFIG_REG, iobuf);
	 
	/* save current configuration setting */
    buftoval(data->nvconfig_reg.value,iobuf);
	
	switch(flag)
	{
	   
		case RES:
			/* set configuration resolution parameter */
			data->nvconfig_reg.option.NVRES =  opt;	   
			break;   
		
		case FAULT:
			/* set configuration register fault parameter */
			data->nvconfig_reg.option.NVFT =  opt;	   
			break;  
			
		case POL:
			/* set configuration register polarity parameter */
			data->nvconfig_reg.option.NVPOL =  opt;	   
			break;  
			
		case CMP_INT_MODE:
			/* set configuration register comparator/interrupt parameter */
			data->nvconfig_reg.option.NVCMP_INT =  opt;	   
			break; 
									
		case SD:
			/* set configuration register shutdown parameter */
			data->nvconfig_reg.option.NVSD  =  opt;	   
			break; 	
			
		case RLCK:
			/* set configuration register lock parameter */
			data->nvconfig_reg.option.RLCK  =  opt;	   
			break; 				
				   
		case RLCKDWN:
			/* set configuration register lockdown parameter */
			data->nvconfig_reg.option.RLCKDWN  =  opt;	   
			break; 		   
			
		case OS:
			/* parameter not valid for nonvolatile register */   
			break; 					
			
	}	   

	return write_config(data->nvconfig_reg.value);

}

#endif

/*!
 * \brief Write memory, this function manages writing
 *        data beyond a pages size (16 bytes)
 *
 * \param address      Data word address
 * \param size		   Size of the data to write
 * \param *buf		   Data to be written
 *
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */

// write memory
uint8_t ts_write_memory(uint16_t address, uint16_t size, uint8_t *buf)
{
		uint8_t write_size 			= 0;
		uint8_t n 					= 0;
		uint16_t start_page_addr 	= 0;
		uint16_t end_page_addr 		= 0;
		int status 					= 0;
        uint8_t *pToken				= 0;
	 
		pToken = buf;
		
		do
		{
		  n = (uint16_t) (address / PAGE_SIZE);
		  start_page_addr = n * PAGE_SIZE;
		  end_page_addr   = ((n + 1) * PAGE_SIZE);

		  if ((address + size) > end_page_addr)
			// response_data doesn't fit, do a partial write
			write_size = end_page_addr - address;
		  else
			write_size = size;

		  status = ts_write_eeprom(address, write_size, pToken);
		  
		  if (status != TWI_SUCCESS)
			  return status;
		  size -= write_size;
		  address += write_size;
		  pToken += write_size;
		}
		while (size);

	return TWI_SUCCESS;
		
}

void parse_temp_reading(sensor_data_t *sensor_data)
{

uint16_t raw_temp_reading = sensor_data->temperature.raw_value;

#if defined _AT30TS00_ || defined _AT30TSE002B_

	sensor_data->temperature.itemp = (raw_temp_reading  >> 4) && 0xFF;
	sensor_data->temperature.ftemp = ((raw_temp_reading  >> 2) && 0x0F) * 2500;
	sensor_data->temperature.sign  = raw_temp_reading  && 0x1000;

#else

	sensor_data->temperature.itemp = (raw_temp_reading  >> 8) && 0xFF;
	sensor_data->temperature.ftemp = ((raw_temp_reading  >> 4) && 0x0F) * 625;
	sensor_data->temperature.sign  = raw_temp_reading  && 0x8000;
	
#endif	
	
}


#if defined _AT30TS00_ || defined _AT30TSE002B_

/*!
 * \brief Read capability register
 *
 * \param *buf		  Where to store the retrieved data
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t read_capability(sensor_data_t *sensor_data)
{
	 
	status =  ts_read_register(CAPABIL_REG, iobuf);
	buftoval(sensor_data->_002b.capability , iobuf);
	return status;
}	


/*!
 * \brief Read manufacture ID register
 *
 * \param *buf		  Where to store the retrieved data
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t read_manfid(sensor_data_t *sensor_data)
{
	 
	status =  ts_read_register(MANF_ID_REG, iobuf);
	buftoval(sensor_data->_002b.manf_id , iobuf);
	return status;
}	


/*!
 * \brief Read device ID register
 *
 * \param *buf		  Where to store the retrieved data
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t read_devid(sensor_data_t *sensor_data)
{
	 
	status =  ts_read_register(DEV_ID_REG, iobuf);
	buftoval(sensor_data->_002b.device_id , iobuf);
	return status;
}	


/*!
 * \brief Read SMBUS register
 *
 * \param *buf		  Where to store the retrieved data
 * \return AT30TS_SUCCESS if all bits = 0, error code otherwise
 */
uint8_t read_smbus(sensor_data_t *sensor_data)
{
	 
	status =  ts_read_register(SMBUS_TO_REG, iobuf);
	buftoval(sensor_data->_002b.smbus , iobuf);
	return status;
}	

/*!
 * \brief Read tCRIT limit register
 *
 * \param *buf		  Where to store the retrieved data
 * \return TWI_SUCCESS if all bytes were read, error code otherwise
 */
uint8_t read_tcrit(sensor_data_t *sensor_data)
{
	status = ts_read_register(TRIP_CRT_REG, iobuf);
	buftoval(sensor_data->limits.critical, iobuf);
	return status;	
	
}

/*!
 * \brief Write tCRIT limit register
 *
 * \param val		   Data to be written
 * \return TWI_SUCCESS if all bytes were written, error code otherwise
 */

uint8_t write_tcrit(uint8_t sign, uint8_t itemp, uint16_t ftemp)
{
	
	uint16_t val;
	val = (itemp << 4) | ((ftemp/2500) << 2);
	 
	/* 2's compliment */ 
	if(sign == neg)
	{
	  val = ~val;
	  val += 1;
	}	 
	 
	// set pointer register
	iobuf[0] = TRIP_CRT_REG;
	
    // load val into buf
	valtobuf(val,iobuf);
	
	// write the tCRIT limit register
    return ts_write_register(iobuf);
}
#endif

uint8_t twi_write(twi_package_t *packet)
{
  /* Perform a write access */
#ifdef _AT30TK175_
	return twi_master_write (packet);  

#elif defined _ATAVRTEMPSENSORX_
   return twi_master_write (TWI_MODULE,packet); 
    
#endif   

}
  
uint8_t twi_read(twi_package_t *packet)
{
  
  /* Perform a read access */
#ifdef _AT30TK175_
	return twi_master_read (packet);  

#elif defined _ATAVRTEMPSENSORX_
   return twi_master_read (TWI_MODULE, packet);
     
#endif   

}

uint8_t twi_probe_device(uint8_t addr)
{
  
  /* Perform a read access */
#ifdef _AT30TK175_
	return twi_probe(addr);  

#elif defined _ATAVRTEMPSENSORX_
   return twi_probe(TWI_MODULE, addr);
     
#endif   
}


  
/*
uint8_t read_smbus()
{
	//reset
}
*/


