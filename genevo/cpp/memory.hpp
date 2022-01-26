#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "statuses.hpp"

class Memory;
class VirtualMemory;
class CPUMemory;
class MemorySegment;

/* Represents some source of bytes that are physically stored somewhere. Is an
 * abstract class. This memory is randomly-defined by default. You can allocate
 * here segments for your needs. This memory can be writed only through the
 * segments.
 */
class Memory {

public:
	Memory();
	~Memory();
	virtual void resize(size_t) = 0;
	/* takes the whole memory. */
	virtual MemorySegment take() = 0;
	/* takes a segment of a defined size somewhere in the memory. */
	virtual MemorySegment take(size_t) = 0;
	/* takes defined range of the memory. */
	virtual MemorySegment take(size_t, size_t) = 0;
	virtual void to_file(const char*) = 0;

private:
	void*       start;
	std::size_t size;

};

/* Represents bytes that are stored in the memory-mapped file.
 */
class VirtualMemory : public virtual Memory {

public:
	VirtualMemory(const char*, size_t);
	~VirtualMemory();

};

/* Represents bytes that are stored in the RAM.
 */
class RAMemory : public virtual Memory {

public:
	RAMemory(size_t);
	~RAMemory();

};

typedef std::uint8_t byte_t;

/* Represents a segment in the Memory.
 */
class MemorySegment {

public:
	MemorySegment();
	~MemorySegment();
	void reset_cursor();
	void set_cursor(size_t);
	void set_every_byte(byte_t);
	/* copy from the source in range [start; stop]. stop = -1 means copying to
	 * the end. */
	void copy_from(MemorySegment, size_t start = 0, size_t stop = -1);
	/* enlarge size of the segment. */
	void ask_more(size_t);
	constexpr byte_t& operator[](size_t) const;

private:
	Memory& source;
	size_t cursor;
	size_t size;

};
