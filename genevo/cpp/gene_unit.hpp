#pragma once

#include "gene/structure.hpp"
#include "memory.hpp"

class GeneOnDish {

private:
	MemorySegment& data;

public:
	GeneOnDish();
	~GeneOnDish();
	byte_t* to_numbers(GeneStructure&);
	
};

class ConcreteGene {

public:
	GeneStructure& structure;
	ConcreteGene();
	~ConcreteGene();
	byte_t* to_numbers(void);
	
};
