//-----------------------------------------------------------------------------
// Copyright 2025 Thiago Alves
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
// This file is responsible for loading function blocks written in Python
// Thiago Alves, Ago 2025
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>


#include "ladder.h"

void *runner_thread(void *arg)
{
    char log_msg[1024];
    const char *cmd = (const char *)arg;
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        snprintf(log_msg, sizeof(log_msg), "Failed to start process: %s\n", cmd);
        openplc_log(log_msg);
        return NULL;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        snprintf(log_msg, sizeof(log_msg), "[Python] %s", buffer);
        openplc_log(log_msg);
    }

    pclose(fp);
    return NULL;
}

int create_shm_name(char *buf, size_t size)
{
    char log_msg[1024];
    char shm_mask[] = "/tmp/shmXXXXXXXXXXXX";
    int fd = mkstemp(shm_mask);
    if (fd == -1)
    {
        snprintf(log_msg, sizeof(log_msg), "mkstemp failed: %s\n", strerror(errno));
        openplc_log(log_msg);
        return -1;
    }
    close(fd);

    snprintf(buf, size, "/%s", strrchr(shm_mask, '/') + 1);
    unlink(shm_mask);

    return 0;
}

int python_block_loader(const char *script_name, const char *script_content, char *shm_name, size_t shm_in_size, size_t shm_out_size, void **shm_in_ptr, void **shm_out_ptr, pid_t pid)
{
    char log_msg[1024];
    char shm_in_name[1024];
    char shm_out_name[1024];
    
    FILE *fp = fopen(script_name, "w");
    if (!fp)
    {
        snprintf(log_msg, sizeof(log_msg), "[Python loader] Failed to write Python script: %s\n", strerror(errno));
        openplc_log(log_msg);
        return -1;
    }
    chmod(script_name, 0640);

    snprintf(log_msg, sizeof(log_msg), "Random shared memory location: %s\n", shm_name);
    openplc_log(log_msg);

    snprintf(shm_in_name, sizeof(shm_in_name), "%s_in", shm_name);
    snprintf(shm_out_name, sizeof(shm_out_name), "%s_out", shm_name);

    fprintf(fp, script_content, pid, shm_name, shm_name);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);

    // Map shared memory (inputs)
    int shm_in_fd = shm_open(shm_in_name, O_CREAT | O_RDWR, 0660);
    if (shm_in_fd < 0)
    {
        snprintf(log_msg, sizeof(log_msg), "shm_open error: %s", strerror(errno));
        openplc_log(log_msg);
        return -1;
    }
    if (ftruncate(shm_in_fd, shm_in_size) == -1)
    {
        snprintf(log_msg, sizeof(log_msg), "ftruncate error: %s", strerror(errno));
        openplc_log(log_msg);
        close(shm_in_fd);
        return -1;
    }
    *shm_in_ptr = mmap(NULL, shm_in_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_in_fd, 0);
    if (*shm_in_ptr == MAP_FAILED)
    {
        snprintf(log_msg, sizeof(log_msg), "mmap error: %s", strerror(errno));
        openplc_log(log_msg);
        close(shm_in_fd);
        return -1;
    }

    // Map shared memory (outputs)
    int shm_out_fd = shm_open(shm_out_name, O_CREAT | O_RDWR, 0660);
    if (shm_out_fd < 0)
    {
        snprintf(log_msg, sizeof(log_msg), "shm_open error: %s", strerror(errno));
        openplc_log(log_msg);
        close(shm_in_fd);
        munmap(*shm_in_ptr, shm_in_size);
        shm_unlink(shm_in_name);
        return -1;
    }
    if (ftruncate(shm_out_fd, shm_out_size) == -1)
    {
        snprintf(log_msg, sizeof(log_msg), "ftruncate error: %s", strerror(errno));
        openplc_log(log_msg);
        close(shm_out_fd);
        close(shm_in_fd);
        munmap(*shm_in_ptr, shm_in_size);
        shm_unlink(shm_in_name);
        return -1;
    }
    *shm_out_ptr = mmap(NULL, shm_out_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_out_fd, 0);
    if (*shm_out_ptr == MAP_FAILED)
    {
        snprintf(log_msg, sizeof(log_msg), "mmap error: %s", strerror(errno));
        openplc_log(log_msg);
        close(shm_out_fd);
        close(shm_in_fd);
        munmap(*shm_in_ptr, shm_in_size);
        shm_unlink(shm_in_name);
        return -1;
    }

    close(shm_in_fd);
    close(shm_out_fd);

    pthread_t tid;
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "python3 -u %s 2>&1", script_name);
    pthread_create(&tid, NULL, runner_thread, (void *)cmd);
    pthread_detach(tid);

    return 0;
}