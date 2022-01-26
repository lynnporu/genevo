#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstdio>
#include <cstring>

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

    const auto segment = new MemorySegment(
        *this, this->size, 0 /* start */);
    return *segment;

}

const MemorySegment& Memory::take(const std::size_t size) {

    throw "This function is not implemented yet.";
    const auto segment = new MemorySegment(*this, 0, 0);
    return *segment;

}

const MemorySegment& Memory::take(
    const std::size_t start, const std::size_t stop
) {

    const auto segment = new MemorySegment(
        *this, stop - start + 1 /* size */, start);
    return *segment;

}

void Memory::to_file(const char* file_address) {

    FILE* fp;
    std::fopen(file_address, "a+");
    size_t written = std::fwrite(this->pointer, 1, this->size, fp);
    std::fclose(fp);
    if (written < this->size)
        // throw error
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

inline void MemorySegment::set_every_byte(const byte_t byte) {
    std::memset(
        this->source.pointer + this->start, byte, this->size);
}

void MemorySegment::copy_from(
    const MemorySegment& from, const std::size_t start, std::size_t stop
) {

    if (stop - start <= this->size)
        throw "Tried to fill the segment with more stuff than its size";

    std::memcpy(
        this->source.pointer + this->start,
        from.source.pointer + start,
        stop - start);

}

void MemorySegment::copy_from(
    byte_t *const source, const std::size_t start, const std::size_t stop
) {

    if (stop - start <= this->size)
        throw "Tried to fill the segment with more stuff than its size";

    std::memcpy(
        this->source.pointer + this->start, source + start, stop - start);

}

constexpr byte_t& MemorySegment::operator[](const std::size_t index) const {

    return *(byte_t *)(this->source.pointer + this->start + index);

}
