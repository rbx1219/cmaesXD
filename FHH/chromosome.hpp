#ifndef _chromosome_hpp
#define _chromosome_hpp

#include <iostream>
#include "gene.hpp"
#include "parameter.hpp"
using namespace std;

class chromosome
{
	private:
		int    Length;
	public:
		double Fitness;
		gene   *genes;
		chromosome( int ell = parameter::lchrom );
		chromosome(const chromosome &chrom);
		~chromosome();
		double fitness() { return Fitness; }
		void set_fitness( double fit ) { Fitness = fit; }
		void evaluate();
		void random();
		int  length() { return Length; }
		void copyGenes( chromosome &C, int *locus, int n );
		chromosome & operator=(const chromosome &chrom);
		gene & operator[](int loci) { return genes[loci]; }
		friend ostream &operator<< (ostream &out, chromosome &chrom);
};

#endif
