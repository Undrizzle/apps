
#include "i2c.h"

void twi_master_destory(void)
{
	if( g_i2c_fd != 0 )
	{
		close(g_i2c_fd);
		g_i2c_fd = 0;
	}	
}


RETURN_CODE twi_master_init(uint8_t twbr_val)
{
	g_i2c_fd = open("/dev/i2c-0",O_RDWR);
	if( g_i2c_fd <= 0 )
	{
		perror("\ntwi_master_init: open /dev/i2c-0 error\n");
		return TWI_BUS_ERROR;
	}
	/*超时时间*/
	if( ioctl(g_i2c_fd, I2C_TIMEOUT, 3) < 0 )
	{
		perror("twi_master_write: ioctl(I2C_TIMEOUT) error\n");
		return TWI_BUS_ERROR;
	}
	
       /*重复次数*/
	if( ioctl(g_i2c_fd, I2C_RETRIES,2) < 0 )
	{
		perror("twi_master_write: ioctl(I2C_RETRIES) error\n");
		return TWI_BUS_ERROR;
	}
	return TWI_SUCCESS;
}

RETURN_CODE twi_master_write(const twi_package_t *packet)
{
	uint8_t buf[3] = {0};
	struct i2c_msg e2prom_msg;
	struct i2c_rdwr_ioctl_data e2prom_data;

	e2prom_msg.buf = buf;
	e2prom_data.msgs = &e2prom_msg;

	
	/***write data to e2prom**/
	e2prom_data.nmsgs = 1;
	
	e2prom_data.msgs->addr = packet->chip;
	e2prom_data.msgs->flags = 0; //write
	e2prom_data.msgs->len = packet->length;
	memcpy(e2prom_data.msgs->buf, packet->buffer, packet->length);
	
	if( ioctl(g_i2c_fd, I2C_RDWR, (unsigned long)&e2prom_data) < 0 )
	{
		perror("\ntwi_master_write: ioctl error\n");
		return TWI_BUS_ERROR;
	}
	return TWI_SUCCESS;
}


