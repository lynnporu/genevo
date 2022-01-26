#pragma once

#include "pool.hpp"
#include "memory.hpp"

class Fridge {

public:
	Fridge(const char*);
	~Fridge();
	void freeze(const Pool&);
	const Pool& unfreeze(void);

private:
	const MemorySegment& location;

};
