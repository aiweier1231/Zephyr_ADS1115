/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ads1115.h"

/**
 * @file Sample app using the Fujitsu MB85RC256V FRAM through I2C.
 */

#define ADS1115_I2C_ADDR	0x48

#define CHANNEL_1 0x40
#define CHANNEL_2 0x50
#define CHANNEL_3 0x60
#define CHANNEL_4 0x70


static int write_bytes(const struct device *i2c_dev, uint16_t addr,
		       uint8_t *data, uint32_t num_bytes)
{
	uint8_t wr_addr[2];
	struct i2c_msg msgs[2];

	/* FRAM address */
	wr_addr[0] = addr;
	/* Setup I2C messages */

	/* Send the address to write to */
	msgs[0].buf = wr_addr;
	msgs[0].len = 1U;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Data to be written, and STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, ADS1115_I2C_ADDR);
}

static int read_bytes(const struct device *i2c_dev, uint16_t addr,
		      uint8_t *data, uint32_t num_bytes)
{
	uint8_t wr_addr[2];
	struct i2c_msg msgs[2];
	int ret;

	/* Now try to read back from FRAM */

	/* FRAM address */
	wr_addr[0] = addr;

	/* Setup I2C messages */

	/* Send the address to read from */
	msgs[0].buf = wr_addr;
	msgs[0].len = 1U;
	msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	// ret = i2c_transfer(i2c_dev, &msgs[0], 1, ADS1115_I2C_ADDR);
	// if(ret)
	// {
	// 	printk("%s %d fail!!!\n", __FUNCTION__, __LINE__ );
	// 	return ret;
	// }

	/* Read from device. STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, ADS1115_I2C_ADDR);
}

void main(void)
{
	//uint8_t channelSelect[] = {CHANNEL_1, CHANNEL_2, CHANNEL_3, CHANNEL_4};
	const uint16_t SELECT_CHANNEL[] = {
		single_0,          // Select channel 0 in multiplexer
		single_1,          // Select channel 1 in multiplexer
		single_2,          // Select channel 2 in multiplexer
		single_3           // Select channel 3 in multiplexer
	};
	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
	uint8_t configData[16];
	uint8_t data[2];
	double samples[4] = {0};
    uint16_t values[4] = {0};
	uint16_t value = 0x0000;
	uint8_t *test;
	int ret;
	const float V_FS = (6144 * 2); //full scale voltage of ADC. 
  	const float MAX_ADC_BIN_VAL = 0xffff; //number of ADC bits
	float voltage = 0;
	const int RETRY = 5;
	int retries = 0;

	uint16_t configuration =
        convert |
        pga_6_144V | // Also need to change V_FS value if this is changed
        single |
        sps860 |
        traditional |
        nonLatching |
        none |
        activeLow;

	if (!device_is_ready(i2c_dev)) 
	{
		printk("I2C: Device is not ready.\n");
		return;
	}
	printk("Init Configuration %x\n", configuration);

	while(1)
	{
		uint16_t configChannel = 0x0000;
		for(int channel = 0; channel < 4; channel++)
		{
			configChannel = configuration | SELECT_CHANNEL[channel];
			test = (uint8_t*)(&configChannel);
			test[2] = test[0];
			printk("test[1] (%x) test[2] (%x)\n", test[1], test[2]);
			ret = write_bytes(i2c_dev, 0x01, &test[1], 2);
			if(ret)
			{
				k_sleep(K_MSEC(1000));
				printk("Error: write config register fail (%d)\n", ret);
				return;
			}
			else
			{
				do
				{
					//k_sleep(K_MSEC(1));
					if(read_bytes(i2c_dev, 0x01, &data[0], 2))
					{
						printk("failed to read from config register\n");
						break;
					}
					configChannel = data[0] << 8 | data[1];
					printk("configChannel data0 %x data1 %x\n", data[0], data[1]);
				}
				while(configChannel & ready == 0 && ((configChannel >> 12) & 0b111) != (SELECT_CHANNEL[channel] >> 12)); //Still not ready

				if(read_bytes(i2c_dev, 0x00, &data[0], 2))
				{
					printk("Error: read from ads error code (%d)\n", ret);
				}
				else
				{
					value = data[0];
					value <<= 8;
					value &= 0xFF00;
					value |= data[1];
					//voltage = ((float)value) / MAX_ADC_BIN_VAL * V_FS;
					// float temp = value * 0.1875;
					// printk("read adc ok (%d)\n", temp);
					// values[channel] = data[0];
					// values[channel] <<= 8;
					// values[channel] &= 0xFF00;
					// values[channel] |= data[1];
					// samples[channel] = (values[channel]) / MAX_ADC_BIN_VAL * V_FS;
					printk("value %d \n", value);
				}
			}
		}
		k_sleep(K_MSEC(1));
	}
}
