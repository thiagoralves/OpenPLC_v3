//-----------------------------------------------------------------------------
// Copyright 2015 Thiago Alves
//
// This file is part of the OpenPLC Runtime.
//
// OpenPLC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenPLC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// This file is the hardware layer for the OpenPLC. If you change the platform
// where it is running, you may only need to change this file. All the I/O
// related stuff is here. Basically it provides functions to read and write
// to the OpenPLC internal buffers in order to update I/O state.
// Thiago Alves, Dec 2015
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "ladder.h"
#include "custom_layer.h"

extern "C" int16_t getSwitchId();

#define I2C_DEVICE_PATH "/dev/i2c-1"
#define I2C_ADDRESS 0x1C
#define SWITCH_REG_1 0x03
#define SWITCH_REG_2 0x02
#define SWITCH_REG_3 0x00

int i2cHandle = -1;

int16_t initI2C()
{
    i2cHandle = open(I2C_DEVICE_PATH, O_RDWR);
    if (i2cHandle < 0)
    {
        perror("Error opening I2C device");
        return -1;
    }

    if (ioctl(i2cHandle, I2C_SLAVE, I2C_ADDRESS) < 0)
    {
        perror("Error setting I2C address");
        close(i2cHandle);
        return -1;
    }

    return 0;
}

void closeI2C()
{
    close(i2cHandle);
}

int16_t getSwitchId()
{
    if (i2cHandle == -1)
    {
        int ret = initI2C();
        if (ret < 0)
        {
            return -1;
        }
    }

    char buf[1];

    buf[0] = SWITCH_REG_1;
    buf[1] = 0xFF;
    if (write(i2cHandle, buf, 2) != 2)
    {
        perror("Error writing to register");
        return -1;
    }

    buf[0] = SWITCH_REG_2;
    buf[1] = 0x00;
    if (write(i2cHandle, buf, 2) != 2)
    {
        perror("Error writing to register");
        return -1;
    }

    buf[0] = SWITCH_REG_3;
    if (write(i2cHandle, buf, 1) != 1)
    {
        perror("Error writing to register");
        return -1;
    }

    if (read(i2cHandle, buf, 1) != 1)
    {
        perror("Error reading from register");
        return -1;
    }

    int8_t retData = buf[0];
    if (retData >= 0 && retData <= 9)
    {
        return retData;
    }

    return -1;
}


//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is finalizing.
// Resource clearing procedures should be here.
//-----------------------------------------------------------------------------
void finalizeHardware()
{
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
        pthread_mutex_lock(&bufferLock); //lock mutex

        /*********READING AND WRITING TO I/O**************

        *bool_input[0][0] = read_digital_input(0);
        write_digital_output(0, *bool_output[0][0]);

        *int_input[0] = read_analog_input(0);
        write_analog_output(0, *int_output[0]);

        **************************************************/

        pthread_mutex_unlock(&bufferLock); //unlock mutex
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
        pthread_mutex_lock(&bufferLock); //lock mutex

        /*********READING AND WRITING TO I/O**************

        *bool_input[0][0] = read_digital_input(0);
        write_digital_output(0, *bool_output[0][0]);

        *int_input[0] = read_analog_input(0);
        write_analog_output(0, *int_output[0]);

        **************************************************/

        pthread_mutex_unlock(&bufferLock); //unlock mutex
}
