/*
 * I2C_Compass.c
 *
 * Created: 03/09/2022 11:59:57
 *  Author: higgy
 */ 

#include "I2C_Compass.h"

void compass_set(void);
void compass_calculate_offsets(void);

volatile uint8_t compass_tx_buffer[100];

//compass x,y,z data
volatile double x_data;
volatile double y_data;
volatile double z_data;

double offset[3] = {0,0,0};
	

/**
 * Initialise I2C on the board and set the slave address.
 */
void compass_init()
{
	// Initialise the I2C I/O Communications
	i2c_m_sync_get_io_descriptor(&cmps_i2c, &cmps_io);
	i2c_m_sync_enable(&cmps_i2c);
	i2c_m_sync_set_slaveaddr(&cmps_i2c, CMPS_ADDR, I2C_M_SEVEN);
	
	compass_set_resolution();
	
	compass_calculate_offsets();
}

/*
* set the resolution on the compass
*/
void compass_set_resolution()
{
	uint8_t icmd[2] = { 0x00 };
	
	// set resolution
	icmd[0] = CMPS_CMD_INT_CTRL_1;
	icmd[1] = CMPS_MASK_SET_RES;
	io_write(cmps_io, icmd, 2); // <----- check this is right, should probably be 2
}

/*
* set the compass polarisation with a set pulse
*/
void compass_set()
{
	uint8_t icmd[2] = { 0x00 };
	
	// write to control register 0 to address it
	icmd[0] = CMPS_CMD_INT_CTRL_0;
	icmd[1] = CMPS_MASK_FILL_CAP;
	io_write(cmps_io, icmd, 2);
	
	// wait at least 50ms for cap to fill
	delay_ms(60);
	
	// write to control register 0 to address it
	icmd[0] = CMPS_CMD_INT_CTRL_0;
	icmd[1] = CMPS_MASK_SET_CMPS;
	io_write(cmps_io, icmd, 2);
}

/*
* reset the compass polarisation with a reset pulse
*/
void compass_reset()
{
	uint8_t icmd[2] = { 0x00 };
	
	// write to control register 0 to address it
	icmd[0] = CMPS_CMD_INT_CTRL_0;
	icmd[1] = CMPS_MASK_FILL_CAP;
	io_write(cmps_io, icmd, 2);
	
	// wait at least 50ms for cap to fill
	delay_ms(60);
	
	// write to control register 0 to address it
	icmd[0] = CMPS_CMD_INT_CTRL_0;
	icmd[1] = CMPS_MASK_RESET_CMPS;
	io_write(cmps_io, icmd, 2);
}

/*
* use the set and reset pulses to calculate any offsets in the compass direction including metal objects nearby
*/
void compass_calculate_offsets()
{
	double ds1[3] = {0, 0, 0};
	double ds2[3] = {0, 0, 0};
		
	// set the compass polarisation and store the xyz data
	compass_set();
	delay_ms(10);
	compass_read_xyz();
	ds1[0] = x_data;
	ds1[1] = y_data;
	ds1[2] = z_data;
	
	// reset compass polarisatin and store xyz data
	compass_reset();
	delay_ms(10);
	compass_read_xyz();
	ds2[0] = x_data;
	ds2[1] = y_data;
	ds2[2] = z_data;	
	
	// saves divide by 0 error if no reading came back
	if (x_data == 0 && y_data == 0)
	{
		offset[0] = 0.0;
		offset[1] = 0.0;
		offset[2] = 0.0;
	}
	else
	{
		for (uint8_t i=0; i<3; i++)
		{
			// values should be equal and opposite, thus zero
			// when they are not, take the average
			offset[i] = (ds1[i]+ds2[i])/2;
		}
	}
	 // put compass round the right way again
	compass_set();
}

/*
* software reset the compass
*/
void compass_sw_reset()
{
	uint8_t icmd[2];
	icmd[0] = CMPS_CMD_INT_CTRL_1;
	icmd[1] = CMPS_MASK_SW_RESET;
	io_write(cmps_io, icmd, 2);
}

/*
* read out the xyz compass data
*/
void compass_read_xyz()
{
	uint8_t count = 0;
	uint8_t icmd[2];
	uint8_t status[1] = {0x00};
	uint8_t data[6];
	
	icmd[0] = CMPS_CMD_INT_CTRL_0;
	icmd[1] = CMPS_MASK_TAKE_MEAS;
	io_write(cmps_io, icmd, 2);

	icmd[0] = CMPS_CMD_STATUS;
	io_write(cmps_io, icmd, 1);
	io_read(cmps_io, status, 1);
	
	// bomb out if too many counts to get measurement
	while ((status[0] & CMPS_MASK_MEAS_DONE) == 0 || count > 20 )
	{
		delay_ms(1);
		io_write(cmps_io, icmd, 1);
		io_read(cmps_io, status, 1);
		count +=1;
	}
	
	uint16_t measured_data[3] ={0x00};
	
	
	if (count < 20)
	{
		// start at address x low register
		icmd[0] = CMPS_CMD_XOUT_LOW;
		io_write(cmps_io, icmd, 1);	
		// read the next 6 bytes should be x_low, x_high, y_low, y_high, z_low, z_high
		io_read(cmps_io, data, 6);
		
		measured_data[0] = data[1] <<8 | data[0];
		measured_data[1] = data[3]<< 8 | data[2];
		measured_data[2] = data[5]<< 8 | data[4];
	}
	
	x_data = measured_data[0] * MGAUSS_CONVERSION - offset[0];
	y_data = measured_data[1] * MGAUSS_CONVERSION - offset[1];
	z_data = measured_data[2] * MGAUSS_CONVERSION - offset[2];
}

/*
* check it can read the id ok
*/
void compass_read_id(char *id)
{
	uint8_t icmd[1] = { CMPS_CMD_ID };
	io_write(cmps_io, icmd, 1);
	
	delay_ms(2);
	
	io_read(cmps_io, &id[0], 1);
}

/*
* get the bearing
*/
double compass_get_bearing()
{
	volatile double temp0 = 0; // partial result 0
	volatile double temp1 = 0; // partial result 1
	volatile double bearing = 0;   // final result
	
	compass_read_xyz();
	
	// atan2 hack to make sure it is in the right quadrant 
	// otherwise atan will only return between -PI()/2 & PI()/2 and lose direction information
	if (x_data < 0) 
	{
		if (y_data > 0) 
		{ //Quadrant 1
			temp0 = y_data;
			temp1 = -x_data;
			bearing = atan(temp0 / temp1) * (180 / M_PI);
			bearing = 90 - bearing;
		}
		else 
		{ //Quadrant 2
			temp0 = -y_data;
			temp1 = -x_data;
			bearing = atan(temp0 / temp1) * (180 / M_PI);
			bearing += 90;
		}
	}
	else 
	{
		if (y_data < 0) 
		{ //Quadrant 3
			temp0 = -y_data;
			temp1 = x_data;
			//bearing = 270 - atan(temp0 / temp1) * (180 / M_PI);
			// for some reason it was not subtracting it from 270, so this is a hack to get it to work
			bearing = atan(temp0 / temp1) * (180 / M_PI);
			bearing = 90 - bearing;
			bearing = 180 + bearing;
		}
		else 
		{ //Quadrant 4
			temp0 = y_data;
			temp1 = x_data;
			bearing = atan(temp0 / temp1) * (180 / M_PI);
			bearing += 270;
		}
	}
	
	return bearing;
}

uint8_t convert_bearing_to_compass_point(double bearing)
{
	uint8_t output_direction = 0;
	
	if (bearing > 337.5 || bearing < 22.5)
	{
		output_direction = 1; //north
	}
	else if (bearing > 292.5 && bearing < 337.25)
	{
		output_direction = 8; // north east
	}
	else if (bearing > 247.5 && bearing < 292.5)
	{
		output_direction = 7; // east
	}
	else if (bearing > 202.5 && bearing < 247.5)
	{
		output_direction = 6; // south east
	}
	else if (bearing > 157.5 && bearing < 202.5)
	{
		output_direction = 5; // south
	}
	else if (bearing > 112.5 && bearing < 157.5)
	{
		output_direction = 4; // south west
	}
	else if (bearing > 67.5 && bearing < 112.5)
	{
		output_direction = 3; // west
	}
	else if (bearing > 22.5 && bearing < 67.5)
	{
		output_direction = 2; //north west
	}
	else
	{
		output_direction = 0; // direction error
	}

	return output_direction;
}

void check_compass()
{
	char x[10];
	compass_read_xyz();
	
	double bearing = compass_get_bearing();
	uint8_t dir = convert_bearing_to_compass_point(bearing);
	
	char compass_out[10];
	
	x[0] = dir + 0x30;
	x[1] = '\r';
	x[2] = '\n';
	
	memcpy(&compass_out[0], x, 3);
	
	//compass_out[2] = '\r';
	//compass_out[3] = '\n';
	memcpy(&compass_tx_buffer[0], &compass_out[0], ARRAY_SIZE(compass_out));
	io_write(&Debug_io.io, compass_tx_buffer, 3);
}

