//-----------------------------------------------------------------------------
// Copyright 2019 Thiago Alves
// This file is part of the OpenPLC Software Stack.
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
// This file is responsible for the persistent storage on the OpenPLC
// Thiago Alves, Jun 2019
//
// Added modifications from gexod to include %MD and %ML into the persistent file
// See https://openplc.discussion.community/post/variable-retain-function-9914880?highlight=retain&trail=30
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "ladder.h"

uint8_t pstorage_read = false;

//-----------------------------------------------------------------------------
// Main function for the thread. Should create a buffer for the persistent
// data, compare it with the actual data and write back to the persistent
// file if the data has changed
//-----------------------------------------------------------------------------
void startPstorage()
{
    //We can only start persistent storage after the persistent.file was read
    while (pstorage_read == false)
        sleepms(100);
    
    char log_msg[1000];
    	
	struct persistentBufferCell
	{
		IEC_INT int_cell;
		IEC_DINT dint_cell;
		IEC_LINT lint_cell;
	};

	persistentBufferCell persistentBuffer[BUFFER_SIZE];

    //Read initial buffers into persistent struct
    pthread_mutex_lock(&bufferLock); //lock mutex
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (int_memory[i] != NULL) persistentBuffer[i].int_cell = *int_memory[i];
		if (dint_memory[i] != NULL) persistentBuffer[i].dint_cell = *dint_memory[i];
		if (lint_memory[i] != NULL) persistentBuffer[i].lint_cell = *lint_memory[i];
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex
    
    //Perform the first write
    if (access("persistent.file", F_OK) == -1) 
    {
        sprintf(log_msg, "Creating Persistent Storage file\n");
        log(log_msg);
    }
    
    FILE *ps = fopen("persistent.file", "w"); //if file already exists, it will be overwritten
    if (ps == NULL)
    {
        sprintf(log_msg, "Persistent Storage: Error creating persistent memory file!\n");
        log(log_msg);
        return;
    }

    if (fwrite(persistentBuffer, sizeof(persistentBufferCell), BUFFER_SIZE, ps) < BUFFER_SIZE)
    {
        sprintf(log_msg, "Persistent Storage: Error writing to persistent memory file!\n");
        log(log_msg);
        return;
    }
    fclose(ps);
    
    //Run the main thread
    while (run_pstorage)
    {
        
        //Verify if persistent buffer is outdated
        bool bufferOutdated = false;
        pthread_mutex_lock(&bufferLock); //lock mutex
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (int_memory[i] != NULL)
            {
                if (persistentBuffer[i].int_cell != *int_memory[i])
                {
                    persistentBuffer[i].int_cell = *int_memory[i];
                    bufferOutdated = true;
                }
            }
            if (dint_memory[i] != NULL)
            {
                if ( persistentBuffer[i].dint_cell != *dint_memory[i])
                {
                    persistentBuffer[i].dint_cell = *dint_memory[i];
                    bufferOutdated = true;
                }
            }			
            if (lint_memory[i] != NULL)
            {
                if ( persistentBuffer[i].lint_cell != *lint_memory[i])
                {
                    persistentBuffer[i].lint_cell = *lint_memory[i];
                    bufferOutdated = true;
                }
            }			
        }
        pthread_mutex_unlock(&bufferLock); //unlock mutex

        //If buffer is outdated, write the changes back to the file
        if (bufferOutdated)
        {
            FILE *fd = fopen("persistent.file", "w"); //if file already exists, it will be overwritten
            if (fd == NULL)
            {
                sprintf(log_msg, "Persistent Storage: Error creating persistent memory file!\n");
                log(log_msg);
                return;
            }

            if (fwrite(persistentBuffer, sizeof(persistentBufferCell), BUFFER_SIZE, fd) < BUFFER_SIZE)
            {
                sprintf(log_msg, "Persistent Storage: Error writing to persistent memory file!\n");
                log(log_msg);
                return;
            }
            fclose(fd);
        }

        sleepms(pstorage_polling*1000);
    }
}

//-----------------------------------------------------------------------------
// This function reads the contents from persistent.file into OpenPLC internal
// buffers. Must be called when OpenPLC is initializing. If persistent storage
// is disabled, the persistent.file will not be found and the function will
// exit gracefully.
//-----------------------------------------------------------------------------
int readPersistentStorage()
{
    char log_msg[1000];
    FILE *fd = fopen("persistent.file", "r");
    if (fd == NULL)
    {
        sprintf(log_msg, "Warning: Persistent Storage file not found\n");
        log(log_msg);
        pstorage_read = true;
        return 0;
    }

	struct persistentBufferCell
	{
		IEC_INT int_cell;
		IEC_DINT dint_cell;
		IEC_LINT lint_cell;
	};

	persistentBufferCell persistentBuffer[BUFFER_SIZE];

    if (fread(persistentBuffer, sizeof(persistentBufferCell), BUFFER_SIZE, fd) < BUFFER_SIZE)
    {
        sprintf(log_msg, "Persistent Storage: Error while trying to read persistent.file!\n");
        log(log_msg);
        pstorage_read = true;
        return 0;
    }
    fclose(fd);
    
    sprintf(log_msg, "Persistent Storage: Reading persistent.file into local buffers\n");
    log(log_msg);
    
    pthread_mutex_lock(&bufferLock); //lock mutex
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (int_memory[i] != NULL) *int_memory[i] = persistentBuffer[i].int_cell;
		if (dint_memory[i] != NULL) *dint_memory[i] = persistentBuffer[i].dint_cell;
		if (lint_memory[i] != NULL) *lint_memory[i] = persistentBuffer[i].lint_cell;
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex
    
    pstorage_read = true;
}
