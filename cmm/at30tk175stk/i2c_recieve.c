
#include "i2c.h"

//! \internal Pointer to the applicative TWI receive buffer.
//static volatile uint8_t *twim_rx_data = NULL;

RETURN_CODE twi_master_read(const twi_package_t *package)
{ 
	uint8_t buf[3] = {0};
	struct i2c_msg e2prom_msg;
	struct i2c_rdwr_ioctl_data e2prom_data;

	e2prom_msg.buf = buf;
	e2prom_data.msgs = &e2prom_msg;

	
	/***write data to e2prom**/
	e2prom_data.nmsgs = 1;
	
	e2prom_data.msgs->addr = package->chip;
	e2prom_data.msgs->flags = I2C_M_RD; //read
	e2prom_data.msgs->len = package->length;
	
	if( ioctl(g_i2c_fd, I2C_RDWR, (unsigned long)&e2prom_data) < 0 )
	{
		perror("twi_master_write: ioctl error\n");
		return TWI_BUS_ERROR;
	}
	memcpy(package->buffer, e2prom_data.msgs->buf, package->length);
	return TWI_SUCCESS;
}



RETURN_CODE twi_probe(uint8_t device_addr)
{ 
	return TWI_SUCCESS;
}

