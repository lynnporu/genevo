#include "files.h"

/*

Stretch file to given size. Assumes its descriptor were opened with O_TRUNC
flag.

*/
void set_file_size(int descriptor, size_t new_size) {

    if (lseek(descriptor, new_size, SEEK_SET) == -1) {
        ERROR_LEVEL = ERR_FILE_CANNOT_LSEEK;
        return;
    }

    if (write(descriptor, "", 1) != 1) {
        ERROR_LEVEL = ERR_FILE_CANNOT_WRITE;
        return;
    }

}

file_map_t * open_file(const char *address, map_mode_t mode, size_t trunc_to_size) {

    ERROR_LEVEL = 0;

    if (mode == OPEN_MODE_READ && trunc_to_size > 0) {
        ERROR_LEVEL = ERR_FILE_CANNOT_STRETCH_READ;
        return NULL;
    }

    int descriptor = mode == OPEN_MODE_READ
        ? open(address, O_RDONLY, 0)
        : open(address, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

    if (descriptor < 0) {
        ERROR_LEVEL = ERR_FILE_CANNOT_OPEN;
        return NULL;
    }

    #define CLOSE_AND_RETURN { close(descriptor); return NULL; }
    #define CLOSE_AND_RETURN_WITH_STATUS(_STATUS) \
        { ERROR_LEVEL = _STATUS; CLOSE_AND_RETURN; }

    size_t file_size;
    if (trunc_to_size > 0) {

        set_file_size(descriptor, trunc_to_size);
        if (ERROR_LEVEL != ERR_OK) CLOSE_AND_RETURN;

        file_size = trunc_to_size;

    } else {

        struct stat st;
        if (fstat(descriptor, &st) < 0)
            CLOSE_AND_RETURN_WITH_STATUS(ERR_FILE_CANNOT_FSTAT);

        file_size = st.st_size;

    }

    void * const data = mode == OPEN_MODE_READ
        ? mmap(
            NULL, file_size,
            PROT_READ,
            MAP_PRIVATE,
            descriptor,
            0) // offset
        : mmap(
            NULL, file_size,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            descriptor,
            0);

    if (data == MAP_FAILED)
        CLOSE_AND_RETURN_WITH_STATUS(ERR_FILE_CANNOT_MMAP);

    file_map_t * const mapping = malloc(sizeof(file_map_t));
    if (mapping == NULL) {
        munmap(data, file_size);
        CLOSE_AND_RETURN_WITH_STATUS(ERR_CANNOT_MALLOC);
    }

    mapping->descriptor = descriptor;
    mapping->size       = file_size;
    mapping->data       = data;

    #undef CLOSE_AND_RETURN
    #undef CLOSE_AND_RETURN_WITH_STATUS

    return mapping;

}

void close_file(file_map_t * const mapping) {

    munmap(mapping->data, mapping->size);
    close(mapping->descriptor);
    free(mapping);

}
