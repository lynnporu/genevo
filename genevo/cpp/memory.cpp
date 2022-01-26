#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "memory.hpp"

VirtualMemory::VirtualMemory(
    const char* file_address, MemoryMode mode, const std::size_t size
) {

    this->file_descriptor = mode == MemoryMode::read
        ? open(file_address, O_RDONLY, 0)
        : open(file_address, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

    if (this->file_descriptor < 0)
        // throw error
        ;

    std::size_t file_size;
    if (size > 0) {
        VirtualMemory::resize_file(this->file_descriptor, size);
        this->size = size;
    } else {
        struct stat st;
        if (fstat(this->file_descriptor, &st) < 0)
            // throw error l
            ;
        this->size = st.st_size;
    }

    this->pointer = mode == MemoryMode::read
        ? (byte_t *)mmap(
             nullptr, file_size,
             PROT_READ, MAP_PRIVATE, file_descriptor,
             0 // offset
         )
        :  (byte_t *)mmap(
             nullptr, file_size,
             PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor,
             0 // offset
         );

    if (this->pointer == MAP_FAILED)
        // throw error
        ;

}

VirtualMemory::~VirtualMemory() {

    munmap(this->pointer, this->size);
    close(this->file_descriptor);

}

RAMemory::RAMemory(const std::size_t size) {

    this->pointer = new byte_t[size];

}

RAMemory::~RAMemory() {

    delete this->pointer;

}

const MemorySegment& Memory::take() {

    const auto segment = new MemorySegment(*this, 0);

    return *segment;

}

const MemorySegment& Memory::take(const std::size_t size) {

    const auto segment = new MemorySegment(*this, 0);

    return *segment;

}

const MemorySegment& Memory::take(
    const std::size_t start, const std::size_t stop
) {

    const auto segment = new MemorySegment(*this, 0);

    return *segment;

}

void Memory::to_file(const char* file_address) {

    ;

}

MemorySegment::MemorySegment(
    const Memory& memory, std::size_t size, std::size_t start
):
    source(memory)
{

    this->set_cursor(start);
    this->size = size;

}

void MemorySegment::set_cursor(const std::size_t position) {

    this->cursor = position;

}
