#include "files.h"

file_map_t * open_file(const char *address, map_mode_t mode, size_t trunc_to_size) {

    int descriptor = mode == OPEN_MODE_READ
        ? open(address, O_RDONLY, 0)
        : open(address, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (descriptor < 0) {
        ERROR_LEVEL = ERR_FILE_CANNOT_OPEN;
        return NULL;
    }

    size_t file_size;
    if (trunc_to_size > 0) {

        if (lseek(descriptor, trunc_to_size, SEEK_SET) == -1) {
            ERROR_LEVEL = ERR_FILE_CANNOT_LSEEK;
            close(descriptor);
            return NULL;
        }

        if (write(descriptor, "", 1) != 1) {
            ERROR_LEVEL = ERR_FILE_CANNOT_WRITE;
            close(descriptor);
            return NULL;
        }

        file_size = trunc_to_size;

    } else {

        struct stat st;
        if (fstat(descriptor, &st) < 0) {
            ERROR_LEVEL = ERR_FILE_CANNOT_FSTAT;
            close(descriptor);
            return NULL;
        }

        file_size = st.st_size;

    }

    void *data = mode == OPEN_MODE_READ
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

    if (data == MAP_FAILED) {
        ERROR_LEVEL = ERR_FILE_CANNOT_MMAP;
        close(descriptor);
        return NULL;
    }

    file_map_t *mapping = malloc(sizeof(file_map_t));
    if (mapping == NULL) {
        ERROR_LEVEL = ERR_CANNOT_MALLOC;
        close(descriptor);
        munmap(data, file_size);
        return NULL;
    }

    mapping->descriptor = descriptor;
    mapping->size       = file_size;
    mapping->data       = data;

    return mapping;

}

void close_file(file_map_t *mapping) {

    munmap(mapping->data, mapping->size);
    close(mapping->descriptor);
    free(mapping);

}
