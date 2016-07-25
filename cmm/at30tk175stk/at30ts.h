/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief temperature sensor driver for AVR.
 *
 * This file defines a useful set of functions for the AT30TSE002B and AT30TS75x
 * devices.
 *
 * - Compiler:           IAR and GNU GCC for AVR
 * - Supported devices:  All AVR devices with an TWI module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
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


#ifndef AT30TS_H_
#define AT30TS_H_

#include <stdint.h>

//for kit support
#ifdef _AT30TK175_

	#include "i2c.h"

#elif defined _ATAVRTEMPSENSORX_

	#include <avr32/io.h>
	#include "compiler.h"
	#include "twi_master.h"
	#include "conf_twi_master.h"
	#include "conf_board.h"

#endif

#if 0
#ifdef  __AVR32_ABI_COMPILER__ 	// AVR32 mcu Big Endian	
 #define _BIG_ENDIAN_
#else
 #warning  Defaulting to LITTLE ENDIAN
 #define _LITTLE_ENDIAN_
#endif 
#else
#define _LITTLE_ENDIAN_
#endif

/* device type must be defined */
#if    ( !_AT30TS75_   && \
         !_AT30TS750_  && \
		 !_AT30TSE752_ && \
		 !_AT30TSE754_ && \
		 !_AT30TSE758_ && \
		 !_AT30TSE00_  && \
		 !_AT30TSE002B_  )
		 
		 #error No Digital Temperature Sensors device defined
#endif


// error codes
//! \todo Put all error codes into one header file (KIT_STATUS_SUCCESS, TWI_SUCCESS, etc.)
#define AT30TS_SUCCESS                  ( 0)
#define AT30TS_GENERAL_ERROR            (-1)
#define AT30TS_TWI_WRITE_NACK           (-2)
#define AT30TS_TWI_READ_NACK            (-3)
#define AT30TS_REG_LOCKED               (-4)
#define AT30TS_INVALID_EEPROM_ADDRESS   (-5)
#define AT30TS_INVALID_SIZE             (-6)
#define AT30TS_OUT_OF_BOUNDS            (-7)
#define AT30TS_NVRBSY_BUSY              (-8)


/**
 * \brief Configuration Reg Options 
 *        the for AT30TSE002B device
 */
typedef enum {

	MODE_CMP		= 0,	//!<
	MODE_INT		= 1,	//!<
	POL_LOW			= 0,	//!<
	POL_HIGH		= 1,	//!<
	CRIT_ALL_ALM	= 0,	//!<
	CRIT_ALM_ONLY	= 1,	//!<		
	EVTOUT_DIS		= 0,	//!<
	EVTOUT_EN		= 1,	//!<	
	EVTSTS_DIS		= 0,	//!<
	EVTSTS_EN		= 1,	//!<	
	EVTCLR_DIS		= 0,	//!<
	EVTCLR_EN		= 1,	//!<
	WINLOCK_DIS		= 0,	//!<
	WINLOCK_EN		= 1,	//!<		
	CRTALML_DIS		= 0,	//!<
	CRTALML_EN		= 0,	//!<
	SHTDWN_EN		= 0,	//!<
	SHTDWN_DIS		= 1,	//!<
	HYSTENB_00C		= 0,	//!<
	HYSTENB_15C		= 1,	//!<
	HYSTENB_30C		= 2,	//!<
	HYSTENB_60C		= 3,	//!<		
	
	} config_options_002b_t;
	
/**
 * \brief Configuration Options for 
		 the AT30TSE002B device
 */
typedef enum {

		EVTMOD,
		EVTPOL,
		CRITEVT,
		EVTOUT,
		EVTSTS,
		EVTCLR,
		WINLOCK,
		CRTALML,
		SHTDWN,
		HYSTENB
	}  opt_flag_002b_t;
		
	
/**
 * \brief Configuration Reg Options
 */
typedef enum {

	SD_DISABLE		= 0,	//!<
	SD_ENABLE		= 1,	//!<
	CMP				= 0,	//!<
	INT				= 1,	//!<	
	POL_ACTIVE_LOW	= 0,	//!<
	POL_ACTIVE_HIGH	= 1,	//!<
	FAULT1			= 0,	//!<
	FAULT2			= 1,	//!<
	FAULT4			= 2,	//!<
	FAULT6			= 3,	//!<
    RES09			= 0,	//!<				
    RES10			= 1,	//!<			
    RES11			= 2,	//!<			
    RES12			= 3,	//!<
    OS_DISABLE	    = 0,	//!<
	OS_ENABLE	    = 1		//!<
	
	} config_options_t;	
	
	
/**
 * \brief Nonvolatile Configuration Reg Options
 */
typedef enum {

	RLCK_UNLOCK			= 0,	//!<
    RLCK_LOCK			= 1,	//!<		
	RLCKDWN_UNLOCK		= 0,	//!<	
	RLCKDWN_LOCK		= 1,	//!<
	NVSD_DISABLE		= 0,	//!<
	NVSD_ENABLE			= 1,	//!<	
	NVCMP				= 0,	//!<
	NVINT				= 1,	//!<		
	NVPOL_ACTIVE_LOW	= 0,	//!<
	NVPOL_ACTIVE_HIGH	= 1,	//!<	
	NVFAULT1			= 0,	//!<
	NVFAULT2			= 1,	//!<
	NVFAULT4			= 2,	//!<
	NVFAULT6			= 3,	//!<	
    NVRES09				= 0,	//!<				
    NVRES10				= 1,	//!<			
    NVRES11				= 2,	//!<			
    NVRES12				= 3		//!<

	} nvconfig_options_t;

/**
 * \brief Configuration Options
 */
typedef enum {
		RES,
		FAULT,
		POL,
		CMP_INT_MODE,	
		SD,
		OS,
		RLCK,
		RLCKDWN
	}  opt_flag_t;
	

	
	
/**
 * \brief AT30TS Family device types
 */
 typedef enum {	 
	 
		AT30TSE002B,
		AT30TS75, 
		AT30TS750, 
		AT30TSE752,
		AT30TSE754, 
		AT30TSE758
	} device_type_t;

/**
 * \brief Information about a AT30TS device
 */	
typedef enum {
		pos, 
		neg, 
		cel, 
		farh, 
		raw
	}  data_types;
	
		
/**
 * \brief Information about a AT30TS device
 */	
typedef enum {
		vol, 
		nonvol
	} reg_type;	
	
	

 //! \brief Information about a AT30TS device
 // @{
typedef struct _sensor_data{
	
	// volatile configuration register 
	//! \brief volatile configuration register
	union{
	   uint16_t value;
	   struct{  
		   
#ifdef  _BIG_ENDIAN_ 	// AVR32 mcu Big Endian	
		
		uint8_t OS         :1   ;//!< OS		
		uint8_t RES        :2   ;//!< Conversion Resolution			
		uint8_t FAULT      :2   ;//!< Fault Tolerance Queue			
		uint8_t POL        :1   ;//!< ALERT Pin Polarity			
		uint8_t CMP_INT    :1   ;//!< Alarm Thermostat Mode		
		uint8_t SD         :1   ;//!< Shutdown Mode			
		uint8_t RFU        :7   ;//!< RFU	
		uint8_t NVRBSY	   :1   ;//!< Nonvolatile Registers Busy	
			
#else				 // AVR8 mcu Little Endian
		uint8_t NVRBSY	   :1   ;//!< Nonvolatile Registers Busy	
		uint8_t RFU        :7   ;//!< RFU		
		uint8_t SD         :1   ;//!< Shutdown Mode		
		uint8_t CMP_INT    :1   ;//!< Alarm Thermostat Mode
		uint8_t POL        :1   ;//!< ALERT Pin Polarity	
		uint8_t FAULT      :2   ;//!< Fault Tolerance Queue		
		uint8_t RES        :2   ;//!< Conversion Resolution						
		uint8_t OS         :1   ;//!< OS	
#endif		

	   } option;  	
	 }config_reg ;	
    
	// volatile configuration register 
	//! \brief volatile configuration register
	union{
	   uint16_t value;
	   struct{  
		   
#ifdef  _BIG_ENDIAN_	// AVR32 mcu Big Endian

		uint8_t RFU		   :5   ;//!< RFU (Reserved for Future Use)	 		   
		uint8_t HYSTENB	   :2   ;//!< Hysteresis Enable		   
		uint8_t SHTDWN     :1   ;//!< Shutdown Mode		   
		uint8_t CRTALML    :1   ;//!< Crit_Alarm Trip Lock bit	
		uint8_t WINLOCK    :1   ;//!< Alarm Window Lock bit		   
		uint8_t EVTCLR     :1   ;//!< EVENT Clear(active low)	
		uint8_t EVTSTS     :1   ;//!< Pin Output Status (active low)		
		uint8_t EVTOUT     :1   ;//!< Output Control	
		uint8_t CRITEVT    :1   ;//!< Critical Temperature only		
		uint8_t EVTPOL     :1   ;//!< Polarity 				   
		uint8_t EVTMOD     :1   ;//!< Mode
	 
#else				// AVR8 mcu Little Endian

		uint8_t EVTMOD     :1   ;//!< Mode
		uint8_t EVTPOL     :1   ;//!< Polarity 
		uint8_t CRITEVT    :1   ;//!< Critical Temperature only
		uint8_t EVTOUT     :1   ;//!< Output Control
		uint8_t EVTSTS     :1   ;//!< Pin Output Status (active low)
		uint8_t EVTCLR     :1   ;//!< EVENT Clear(active low)
		uint8_t WINLOCK    :1   ;//!< Alarm Window Lock bit
		uint8_t CRTALML    :1   ;//!< Crit_Alarm Trip Lock bit	
		uint8_t SHTDWN     :1   ;//!< Shutdown Mode
		uint8_t HYSTENB	   :2   ;//!< Hysteresis Enable
		uint8_t RFU		   :5   ;//!< RFU (Reserved for Future Use)	  

#endif


	   } option;  	
	 } config_reg_002b ;	
	
	
	//! \brief nonvolatile configuration register 
	 union{
	   uint16_t value;
	    
	   struct{  
		   
#ifdef  _BIG_ENDIAN_ 	// AVR32 mcu Big Endian
	   
		uint8_t NU         :1   ;//!< Nonvolatile Not used		
		uint8_t NVRES      :2   ;//!< Nonvolatile Conversion Resolution	
		uint8_t NVFT       :2   ;//!< Nonvolatile Fault Tolerance Queue
		uint8_t NVPOL      :1   ;//!< Nonvolatile ALERT Pin Polarity
		uint8_t NVCMP_INT  :1   ;//!< Nonvolatile Alarm Thermostat Mode	
		uint8_t NVSD       :1   ;//!< Nonvolatile Shutdown Mode 
		uint8_t	RFU_       :5   ;//!< bits 7:3 reserved for future use	
		uint8_t RLCKDWN	   :1   ;//!< Nonvolatile Register Lockdown	
		uint8_t RLCK       :1   ;//!< Nonvolatile Register Lock											   
		uint8_t RFU        :1   ;//!< RFU
		
#else				// AVR8 mcu Little Endian

		uint8_t RFU        :1   ;//!< RFU
		uint8_t RLCK       :1   ;//!< Nonvolatile Register Lock
		uint8_t RLCKDWN	   :1   ;//!< Nonvolatile Register Lockdown
		uint8_t	RFU_       :5   ;//!< bits 7:3 reserved for future use		
		uint8_t NVSD       :1   ;//!< Nonvolatile Shutdown Mode 
		uint8_t NVCMP_INT  :1   ;//!< Nonvolatile Alarm Thermostat Mode
		uint8_t NVPOL      :1   ;//!< Nonvolatile ALERT Pin Polarity
		uint8_t NVFT       :2   ;//!< Nonvolatile Fault Tolerance Queue
		uint8_t NVRES      :2   ;//!< Nonvolatile Conversion Resolution
		uint8_t NU         :1   ;//!< Nonvolatile Not used	
		
#endif		

	   } option;  	
	 }nvconfig_reg ;	

	//! \brief Information temperature	
	struct _temperature{
		  
		uint8_t  itemp;			//!< integer part of the temperature data
		uint16_t ftemp;			//!< fractional part of the temperature data
		uint8_t  sign;			//!< sign indicator		  
		uint16_t raw_value;		//!< raw data		   
		   
	} temperature;

	struct _limits{
		  
		uint16_t  low;			//!< low limit
		uint16_t  high;			//!< high limit
		uint16_t  critical;		//!< critical limit, AT30TSE002B device only				  
	} limits;

	struct _nvlimits{
		  
		uint16_t  low;			//!< low limit
		uint16_t  high;			//!< high limit			  
	} nv_limits;


	struct{
		  
		uint16_t  smbus;		//!< 002b smbus 
		uint16_t  device_id;	//!< 002b device id
		uint16_t  manf_id;		//!< 002b manufaucter id
		uint16_t  capability;	//!< 002b capability 	  
	} _002b;



} __attribute__ ((packed)) sensor_data_t;

// @}

/**
 * \brief Information about a AT30TS device
 */
typedef struct _ts_device
{
  //! device address
  uint8_t ts_reg_addr;

  //! device id
  uint8_t device_type;
  
  //! eeprom size
  uint16_t eeprom_size;

}
__attribute__ ((packed)) ts_device_t;



ts_device_t device_info[4];
ts_device_t attached_device;

/*! \name AT30TS75 registers
 */
//! @{

#define tPROG						5		//!< Nonvolatile Register Program Time (ms), max
#define tCOPYW						5	    //!< Volatile to Nonvolatile Register Copy Time (ms), max
#define tCOPYR						200	    //!< Nonvolatile to Volatile Register Copy Time (us), max
#define EEPROM_WP_ADDRESS			0x60	//!< EEPROM's WP TWI-address (bus val: 0x60)
#define PAGE_SIZE                   0x10	//!< EEPROM page size

/* AT30TS75x Pointer register address */
#define TEMPER_REG					0x00	//!< Temperature Data   (R)   Default =   0x0000
#define CONFIG_REG					0x01	//!< Vol Config			(R/W) Default =	  N/A
#define TLOLIM_REG					0x02	//!< Vol Lo Lim			(R/W) Default =   N/A
#define THILIM_REG					0x03	//!< Vol Hi Lim			(R/W) Default =   N/A
#define NV_CONFIG_REG				0x11	//!< NonVol Config		(R/W) Default =   0x0000
#define NV_TLOLIM_REG				0x12	//!< NonVol Lo Lim		(R/W) Default =   0x04B0
#define NV_THILIM_REG				0x13	//!< NonVol Hi Lim		(R/W) Default =   0x0500

/* AT30TSE002B Pointer register address */
#define CAPABIL_REG					0x00	//!< Capability       (R)   Default = 0x00D7
#define CONFIGUR_REG				0x01	//!< Configuration    (R/W) Default = 0x0000
#define TRIP_UPR_REG				0x02	//!< Upper Alarm      (R/W) Default = 0x0000
#define TRIP_LWR_REG				0x03	//!< Lower Alarm      (R/W) Default = 0x0000
#define TRIP_CRT_REG				0x04	//!< Critical Alarm   (R/W) Default = 0x0000
#define TEMP_DATA_REG				0x05	//!< Temperature Data (R)   Default = N/A
#define MANF_ID_REG					0x06	//!< Manufacturer ID  (R)   Default = 0x001F
#define DEV_ID_REG					0x07	//!< Dev ID/Revision  (R)   Default = 0x8201
#define SMBUS_TO_REG				0x22	//!< SMBus Timeout    (R/W) Default = 0x0000

/* register commands */
#define CPY_VOL_NVOL_REG			0x48	//!< Copy Volatile Registers to Nonvolatile Registers
#define CPY_NVOL_VOL_REG			0xB8	//!< Copy Nonvolatile Registers to Volatile Registers

/* register configuration bits */
#define NVRBSY						0x01	//!< Nonvolatile Registers Busy
#define RLCK_MSK					0x02    //!< Nonvolatile Register Lock
#define RLCKDWN_MSK					0x04    //!< Nonvolatile Register Lockdown
//! @}

extern uint8_t ts_read_register (const uint8_t pointer_reg, uint8_t * ibuf);
extern uint8_t ts_write_register (uint8_t * obuf);
extern uint8_t ts_read_eeprom (uint16_t address, uint16_t size, uint8_t * buf);
extern uint8_t ts_write_eeprom (uint16_t address, uint16_t size,uint8_t * buf);
extern uint8_t ts75_copy_vol_nonvol_register (void);
extern uint8_t ts75_copy_nonvol_vol_register (void);
extern uint8_t ts75_probe_nonvol_register (uint8_t device_addr);

extern uint8_t write_config(uint16_t val);
extern uint8_t write_tlow(uint8_t sign, uint8_t itemp, uint16_t ftemp);
extern uint8_t write_thigh(uint8_t sign, uint8_t itemp, uint16_t ftemp);

extern uint8_t read_thigh(sensor_data_t *data);
extern uint8_t read_tlow(sensor_data_t *data);
extern uint8_t read_config(sensor_data_t *data);
extern uint8_t read_temperature(sensor_data_t *data);
extern uint8_t set_config_option(sensor_data_t *data, opt_flag_t flag, config_options_t opt);

extern uint8_t write_nvconfig(uint16_t val);
extern uint8_t write_nvtlow(uint8_t sign, uint8_t itemp, uint16_t ftemp);
extern uint8_t write_nvthigh(uint8_t sign, uint8_t itemp, uint16_t ftemp);
extern uint8_t read_nvthigh(sensor_data_t *data);
extern uint8_t read_nvtlow(sensor_data_t *data);
extern uint8_t read_nvconfig(sensor_data_t *data);
extern uint8_t set_nvconfig_option(sensor_data_t *data, opt_flag_t flag, nvconfig_options_t opt);

extern uint8_t ts_ack_polling(uint8_t addr);
extern void parse_temp_reading(sensor_data_t *sensor_data);
extern uint8_t check_NVRBSY(void);
extern uint8_t attach_device(uint8_t device_addr, device_type_t type);
extern uint8_t ts_write_memory(uint16_t address, uint16_t size, uint8_t *buf);
extern void ts_construct_eeprom_address(twi_package_t *packet, uint16_t address);
extern char *ValidateSize(char *command, uint16_t *address, uint16_t *size);


// use for compatibility
extern uint8_t twi_write(twi_package_t *packet);	
extern uint8_t twi_read(twi_package_t *packet);	
extern uint8_t twi_probe_device(uint8_t addr);

/* at30tse002b device only */
uint8_t read_smbus(sensor_data_t *sensor_data);
uint8_t read_devid(sensor_data_t *sensor_data);
uint8_t read_manfid(sensor_data_t *sensor_data);
uint8_t read_capability(sensor_data_t *sensor_data);
uint8_t read_tcrit(sensor_data_t *sensor_data);
uint8_t write_tcrit(uint8_t sign, uint8_t itemp, uint16_t ftemp);

#endif	/* AT30TS_H_ */
