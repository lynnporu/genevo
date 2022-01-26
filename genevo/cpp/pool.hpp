#pragma once

#include <string>
#include <iterator>

#include "memory.hpp"
#include "gene_unit.hpp"

class GenomeFlask;
class Genome;

class GenomeFlask {

public:

	GenomeFlask(const MemorySegment&);
	~GenomeFlask();
	std::string metadata;
	// iterator on genes
	// mutate method
	// copulate_with method

private:
	MemorySegment& data;

};

class Genome : public GenomeFlask {

public:
	const GeneStructure& structure;
	Genome(GeneStructure&, MemorySegment&);
	Genome(ConcreteGene*);
	Genome(ConcreteGene*, MemorySegment&);
	~Genome();
	// redefine iterator on genomes
	// redefine mutate method
	// redefine copulate_with method
	
};

typedef Genome Organism;

class PoolFlask {

public:
	PoolFlask(MemorySegment&);
	~PoolFlask();
	std::string metadata;
	// iterator on genomes
	// reproduce method

private:
	MemorySegment& data;

};

class Pool {

public:
	const GeneStructure& structure;
	size_t length(void);
	Pool(GeneStructure&, MemorySegment&);
	~Pool();
	// redefine iterator on genomes
	// redefine reproduce method

};

typedef Pool Population;
