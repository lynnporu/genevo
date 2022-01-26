#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

typedef std::uint8_t byte_t;

class Memory;
class VirtualMemory;
class CPUMemory;
class MemorySegment;

/* Represents some source of bytes that are physically stored somewhere. Is an
 * abstract class. This memory is randomly-defined by default. You can allocate
 * here segments for your needs. This memory can be writed only through the
 * segments.
 */

enum class MemoryMode {
	read  = 1 << 0,
	write = 1 << 1
};

class Memory {

public:
	~Memory();
	void resize(const std::size_t);
	/* takes the whole memory. */
	const MemorySegment& take();
	/* takes a segment of a defined size somewhere in the memory. */
	const MemorySegment& take(const std::size_t);
	/* takes defined range of the memory. */
	const MemorySegment& take(const std::size_t, const std::size_t);
	void to_file(const char*);

protected:
	// forbids instantiating this class
	Memory();
	byte_t*       pointer;
	std::size_t size;

};

/* Represents bytes that are stored in the memory-mapped file.
 */
class VirtualMemory : public virtual Memory {

public:
	VirtualMemory(const char*, MemoryMode, const std::size_t = 0);
	~VirtualMemory();

protected:
	int file_descriptor;
	static void resize_file(int, std::size_t);

};

/* Represents bytes that are stored in the RAM.
 */
class RAMemory : public virtual Memory {

public:
	RAMemory(const std::size_t);
	~RAMemory();

};

/* Represents a segment in the Memory.
 */
class MemorySegment {

public:
	MemorySegment(const Memory&, std::size_t size, std::size_t start);
	~MemorySegment();
	void reset_cursor();
	void set_cursor(const std::size_t);
	void set_every_byte(const byte_t);
	/* copy from the source in range [start; stop]. stop = -1 means copying to
	 * the end. */
	void copy_from(
		const MemorySegment,
		const std::size_t start = 0, const std::size_t stop = -1);
	/* enlarge size of the segment. */
	void ask_more(const std::size_t);
	constexpr byte_t& operator[](const std::size_t) const;

private:
	const Memory& source;
	std::size_t cursor;
	std::size_t size;

};
