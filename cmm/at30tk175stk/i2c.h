
#ifndef TWI_H
#define TWI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
//#include <avr/interrupt.h>
//#include <avr/io.h>
//#include <util/twi.h>
#include <sys/ioctl.h>
	
	 /**
	 * \brief Information concerning the data transmission
	 */
	typedef struct
	{
		//! TWI chip address to communicate with.
		uint8_t chip;
		//! TWI address/commands to issue to the other chip (node).
		uint32_t addr;
		//! Length of the TWI data address segment (1-3 bytes).
		uint8_t addr_length;
		//! Where to find the data to be written.
		void *buffer;
		//! How many bytes do we want to write.
		uint32_t length;
	}
	__attribute__ ((packed)) twi_package_t;

	struct i2c_msg
	{
		uint16_t addr;	/* slave address			*/
		uint16_t flags;
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_RD		0x0001	/* read data, from slave to master */
#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
		uint16_t len;		/* msg length				*/
		uint8_t *buf;		/* pointer to msg data			*/
	};
	
	/* This is the structure as used in the I2C_RDWR ioctl call */
	struct i2c_rdwr_ioctl_data
	{
		struct i2c_msg *msgs;	/* pointers to i2c_msgs */
		uint32_t nmsgs;			/* number of i2c_msgs */
	};




	/****************************************************************************************************/
	// TYPES
	/****************************************************************************************************/
	#define vuint8_t					   volatile uint8_t
	#define vuint16_t					   volatile uint16_t

	/****************************************************************************************************/
	// MACROS
	/****************************************************************************************************/
	#define TWSR_status				 (TWSR & 0xF8)
	#define TWSR_status_is_not(cond) (TWSR_status != cond)		//< TWI TWSR status query
    #define TWI_MEM_ADDR_LEN_MAX     (3)

	/****************************************************************************************************/
	// RETURN CODES
	/****************************************************************************************************/
	typedef enum {
	  TWI_SUCCESS = (vuint8_t)0x0,		//< Function execution completed successfully
	  TWI_BUS_ERROR,				    //< TWI communications error
      TWI_START_ERROR,
      TWI_ADDR_LEN_INVALID
	} RETURN_CODE;


	/****************************************************************************************************/
	// AVR Microcontroller Hardware configuration
	/****************************************************************************************************/
	#define TWBR_VAL					0x0C	//< Value to program in the TWI baudrate register


	/****************************************************************************************************/
	// FUNCTION PROTOTYPES
	/****************************************************************************************************/
	#define I2C_RETRIES	0x0701	/* number of times a device address should
				   be polled when not acknowledging */
	#define I2C_TIMEOUT	0x0702	/* set timeout in units of 10 ms */
	#define I2C_RDWR 0x0707
	extern int g_i2c_fd;

	void twi_master_destory(void);
	RETURN_CODE twi_master_init(uint8_t);
	RETURN_CODE twi_master_write(const twi_package_t *packet);   
	RETURN_CODE twi_master_read(const twi_package_t *packet);
	RETURN_CODE twi_probe(uint8_t device_addr);


#endif


