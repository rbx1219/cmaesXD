#include <iostream>
#include <iomanip>
#include <cassert>

#include "random.hpp"
#include "chromosome.hpp"
#include "gene.hpp"
#include "parameter.hpp"
#include "objfunc.hpp"
using namespace std;

extern randomG RANDOM;

// initializes a chromosome. alleles are not initialized.
chromosome::chromosome( int ell )
{
	Length = ell;
	genes = new gene[ Length ]; // allocate memory
	Fitness = 0;
}

// copy constructor
chromosome::chromosome(const chromosome &chrom)
{
	genes = new gene[ chrom.Length ];  // allocate memory

	for( int i=0; i< chrom.Length; i++ )     // copy the genes
	{
		genes[i] = chrom.genes[i];
	}

	Length = chrom.Length;
	Fitness = chrom.Fitness;
}

// destructor
chromosome::~chromosome()
{
	delete [] genes;  // free memory
}

// set the genes of the chromosome with random alleles
void chromosome::random()
{
	for( int i=0; i< Length; i++ )
		genes[i].random();
}

// assignment operator
chromosome & chromosome::operator=(const chromosome &chrom)
{
	if (this == &chrom)
		return *this;

	for( int i=0; i< chrom.Length; i++ )     // copy the genes
	{
		genes[i] = chrom.genes[i];
	}

	Length = chrom.Length;
	Fitness = chrom.Fitness;

	return *this;
}

//
// copy 'n' genes specified by 'locus' of chromosome 'C' into this chromosome
//
void chromosome::copyGenes( chromosome &C, int *locus, int n )
{
	for( int i=0; i< n; i++ )
	{
		genes[ locus[i] ] = C.genes[ locus[i] ];
	}
}

// print the chromosome
ostream &operator<< (ostream &out, chromosome &chrom)
{
	out.setf(ios::scientific);
	out.precision(30);
	if( parameter::report_string )
		for( int i=0; i< chrom.Length; i++)
		{
			out << chrom.genes[i] << endl;
		}
	if( parameter::report_fitness )out << "   f= " << chrom.Fitness << endl;

	return out;
}

// evaluate fitness function
void chromosome::evaluate() { Fitness = objfunc(*this); };
