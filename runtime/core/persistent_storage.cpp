//-----------------------------------------------------------------------------
// Copyright 2019 Thiago Alves
// This file is part of the OpenPLC Software Stack.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
// This file is responsible for the persistent storage on the OpenPLC
// Thiago Alves, Jun 2019
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <mutex>

#include <spdlog/spdlog.h>

#include "ladder.h"

/** \addtogroup openplc_runtime
 *  @{
 */

//-----------------------------------------------------------------------------
// Main function for the thread. Should create a buffer for the persistent
// data, compare it with the actual data and write back to the persistent
// file if the data has changed
//-----------------------------------------------------------------------------
void startPstorage()
{
    unsigned char log_msg[1000];
	IEC_UINT persistentBuffer[BUFFER_SIZE];

    //Read initial buffers into persistent struct
	{
		std::lock_guard<std::mutex> guard(bufferLock);
		for (int i = 0; i < BUFFER_SIZE; i++)
		{
			if (int_memory[i] != NULL) persistentBuffer[i] = *int_memory[i];
		}
	}
    
    //Perform the first write
    if (access("persistent.file", F_OK) == -1) 
    {
		spdlog::info("Creating Persistent Storage file");
    }
    
    FILE *ps = fopen("persistent.file", "w"); //if file already exists, it will be overwritten
    if (ps == NULL)
    {
		spdlog::error("Persistent Storage: Error creating persistent memory file!");
        return;
    }

    if (fwrite(persistentBuffer, sizeof(IEC_INT), BUFFER_SIZE, ps) < BUFFER_SIZE)
    {
		spdlog::error("Persistent Storage: Error writing to persistent memory file!");
        return;
    }
    fclose(ps);
    
    //Run the main thread
	while (run_pstorage)
	{
        
        //Verify if persistent buffer is outdated
		bool bufferOutdated = false;
		{
			for (int i = 0; i < BUFFER_SIZE; i++)
			{
				std::lock_guard<std::mutex> guard(bufferLock);
				if (int_memory[i] != NULL)
				{
					if (persistentBuffer[i] != *int_memory[i])
					{
						persistentBuffer[i] = *int_memory[i];
						bufferOutdated = true;
					}
				}
			}
		}

        //If buffer is outdated, write the changes back to the file
		if (bufferOutdated)
		{
			FILE *fd = fopen("persistent.file", "w"); //if file already exists, it will be overwritten
			if (fd == NULL)
			{
				spdlog::error("Persistent Storage: Error creating persistent memory file!");
				return;
			}

			if (fwrite(persistentBuffer, sizeof(IEC_INT), BUFFER_SIZE, fd) < BUFFER_SIZE)
			{
				spdlog::error("Persistent Storage: Error writing to persistent memory file!");
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
    unsigned char log_msg[1000];
	FILE *fd = fopen("persistent.file", "r");
	if (fd == NULL)
	{
		spdlog::warn("Warning: Persistent Storage file not found");
		return 0;
	}

	IEC_INT persistentBuffer[BUFFER_SIZE];

	if (fread(persistentBuffer, sizeof(IEC_INT), BUFFER_SIZE, fd) < BUFFER_SIZE)
	{
		spdlog::error("Persistent Storage: Error while trying to read persistent.file!");
		return 0;
	}
	fclose(fd);
    
	spdlog::info("Persistent Storage: Reading persistent.file into local buffers");
    
	{
		std::lock_guard<std::mutex> guard(bufferLock);
		for (int i = 0; i < BUFFER_SIZE; i++)
		{
			if (int_memory[i] != NULL) *int_memory[i] = persistentBuffer[i];
		}
	}
	return 0;
}

/** @}*/
