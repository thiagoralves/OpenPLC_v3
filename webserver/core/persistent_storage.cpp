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
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "ladder.h"

#define FILE_PATH "persistent.file"

uint8_t pstorage_read = false;

uint32_t combined_checksum = 0; // Stored checksum for detecting changes

// Function to calculate a simple XOR-based checksum for all three arrays together
uint32_t calculate_combined_checksum() 
{
    uint32_t checksum = 0;

    // Calculate checksum for int_memory
    for (size_t i = 0; i < BUFFER_SIZE; i++) 
    {
        if (int_memory[i] != NULL)
        {
            checksum ^= *int_memory[i];
        }
    }

    // Calculate checksum for dint_memory
    for (size_t i = 0; i < BUFFER_SIZE; i++) 
    {
        if (dint_memory[i] != NULL)
        {
            checksum ^= *dint_memory[i];
        }
    }

    // Calculate checksum for lint_memory
    for (size_t i = 0; i < BUFFER_SIZE; i++) 
    {
        if (lint_memory[i] != NULL)
        {
            checksum ^= (uint32_t)(*lint_memory[i] ^ (*lint_memory[i] >> 32));
        }
    }

    return checksum;
}

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
    sprintf(log_msg, "Starting Persistent Storage thread\n");
    log(log_msg);

    //Run the main thread
    while (run_pstorage)
    {
        // Calculate the combined checksum
        uint32_t new_checksum = calculate_combined_checksum();
        if (new_checksum == combined_checksum) 
        {
            // No changes detected, skip saving
            sleepms(pstorage_polling*1000);
            continue;
        }

        // Open persisten.file for writing
        FILE *file = fopen(FILE_PATH, "wb");
        if (file == NULL) 
        {
            sprintf(log_msg, "Persistent Storage: Error creating persistent memory file!\n");
            log(log_msg);
            return;
        }
        // Update the stored checksum
        combined_checksum = new_checksum;

        // Write the contents of int_memory
        for (size_t i = 0; i < BUFFER_SIZE; i++) 
        {
            uint16_t value = (int_memory[i] != NULL) ? *int_memory[i] : 0;
            if (fwrite(&value, sizeof(uint16_t), 1, file) != 1)
            {
                sprintf(log_msg, "Persistent Storage: Error writing int_memory to file\n");
                log(log_msg);
            }
        }
        // Write the contents of dint_memory
        for (size_t i = 0; i < BUFFER_SIZE; i++) 
        {
            uint32_t value = (dint_memory[i] != NULL) ? *dint_memory[i] : 0;
            if (fwrite(&value, sizeof(uint32_t), 1, file) != 1) 
            {
                sprintf(log_msg, "Persistent Storage: Error writing dint_memory to file\n");
                log(log_msg);
            }
        }

        // Write the contents of lint_memory
        for (size_t i = 0; i < BUFFER_SIZE; i++) 
        {
            uint64_t value = (lint_memory[i] != NULL) ? *lint_memory[i] : 0;
            if (fwrite(&value, sizeof(uint64_t), 1, file) != 1) 
            {
                sprintf(log_msg, "Persistent Storage: Error writing lint_memory to file\n");
                log(log_msg);
            }
        }

        // Flush and sync to ensure data is written to disk
        /*
        fflush(file);
        int fd = fileno(file);
        fsync(fd);  // Ensure data reaches the disk
        */

        fclose(file);

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
    FILE *file = fopen(FILE_PATH, "rb");
    if (file == NULL) 
    {
        sprintf(log_msg, "Persistent Storage is empty\n");
        log(log_msg);
        pstorage_read = true;
        return 0;
    }
    
    pthread_mutex_lock(&bufferLock); //lock mutex
    // Read the contents into int_memory
    for (size_t i = 0; i < BUFFER_SIZE; i++) 
    {
        uint16_t value;
        // Read each value individually
        if (fread(&value, sizeof(uint16_t), 1, file) != 1) 
        {
            if (feof(file)) break; // Stop on end of file
            sprintf(log_msg, "Error reading int_memory from file");
            log(log_msg);
            pthread_mutex_unlock(&bufferLock); //unlock mutex
            fclose(file);
            pstorage_read = true;
            return;
        }
        
        // Only assign if value is non-zero
        if (value != 0)
        {
            // Allocate memory if the pointer is NULL
            if (int_memory[i] == NULL) 
            {
                int_memory[i] = malloc(sizeof(uint16_t));
                if (int_memory[i] == NULL) 
                {
                    sprintf(log_msg, "Error allocating memory for int_memory[%d]", i);
                    log(log_msg);
                    continue;
                }
            }
            *int_memory[i] = value; // Store the value
        }
    }

    // Read the contents into dint_memory
    for (size_t i = 0; i < BUFFER_SIZE; i++) 
    {
        uint32_t value;
        // Read each value individually
        if (fread(&value, sizeof(uint32_t), 1, file) != 1) 
        {
            if (feof(file)) break; // Stop on end of file
            sprintf(log_msg, "Error reading dint_memory from file");
            log(log_msg);
            pthread_mutex_unlock(&bufferLock); //unlock mutex
            fclose(file);
            pstorage_read = true;
            return;
        }
        
        // Only assign if value is non-zero
        if (value != 0)
        {
            // Allocate memory if the pointer is NULL
            if (dint_memory[i] == NULL) 
            {
                dint_memory[i] = malloc(sizeof(uint32_t));
                if (dint_memory[i] == NULL) 
                {
                    sprintf(log_msg, "Error allocating memory for dint_memory[%d]", i);
                    log(log_msg);
                    continue;
                }
            }
            *dint_memory[i] = value; // Store the value
        }
    }

    // Read the contents into lint_memory
    for (size_t i = 0; i < BUFFER_SIZE; i++) 
    {
        uint64_t value;
        // Read each value individually
        if (fread(&value, sizeof(uint64_t), 1, file) != 1) 
        {
            if (feof(file)) break; // Stop on end of file
            sprintf(log_msg, "Error reading lint_memory from file");
            log(log_msg);
            pthread_mutex_unlock(&bufferLock); //unlock mutex
            fclose(file);
            pstorage_read = true;
            return;
        }
        
        // Only assign if value is non-zero
        if (value != 0)
        {
            // Allocate memory if the pointer is NULL
            if (lint_memory[i] == NULL) 
            {
                lint_memory[i] = malloc(sizeof(uint64_t));
                if (lint_memory[i] == NULL) 
                {
                    sprintf(log_msg, "Error allocating memory for lint_memory[%d]", i);
                    log(log_msg);
                    continue;
                }
            }
            *lint_memory[i] = value; // Store the value
        }
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex

    fclose(file);

    sprintf(log_msg, "Persistent Storage: Finished reading persistent memory\n");
    log(log_msg);
    pstorage_read = true;
    return 0;
}