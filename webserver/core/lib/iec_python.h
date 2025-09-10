#ifdef __linux__
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

extern "C" void openplc_log(char *logmsg);
extern "C" int create_shm_name(char *buf, size_t size);
extern "C" int python_block_loader(const char *script_name, const char *script_content, char *shm_name, size_t shm_in_size, size_t shm_out_size, void **shm_in_ptr, void **shm_out_ptr, pid_t pid);