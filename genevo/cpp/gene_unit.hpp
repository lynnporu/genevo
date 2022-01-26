#pragma once

#include "gene/structure.hpp"
#include "memory.hpp"

class GeneOnDish {

public:
	GeneOnDish(const MemorySegment&);
	~GeneOnDish();
	byte_t* to_numbers(const GeneStructure&);

private:
	const MemorySegment& data;
	
};

class ConcreteGene : public GeneOnDish {

public:
	const GeneStructure& structure;
	ConcreteGene(const GeneStructure&, const MemorySegment&);
	~ConcreteGene();
	byte_t* to_numbers(void);
	
};
