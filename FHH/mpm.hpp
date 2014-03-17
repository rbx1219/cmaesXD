#ifndef _mpm_hpp
#define _mpm_hpp

#include <iostream>
#include <fstream>

#include "subset.hpp"
#include "population.hpp"
using namespace std;
class mpm
{
	private:
		int     maxSets;      // maximum number of sets in the cache
		int     n_subsets;    // number of subsets
		subset  *subsets;     // the subsets
		double  mc;   // model complexity
		double  cpc;  // compressed population complexity

	public:
		mpm( int n_sets );
		~mpm();
		int num_subsets() { return n_subsets; }
		subset & operator[](const int i) const { return subsets[i]; }
		void insert( subset &S );
		void remove( int i );
		void merge( int i, int j, subset &S );
		bool converged() { return mc==0; }
		friend ostream &operator<< (ostream &out, mpm &MPM);

		double MC() { return mc; }
		double CPC() { return cpc; }
		double CC() { return mc + cpc; }
		double modelComplexity();
		double compressedPopulationComplexity();
		void combinedComplexity();

		void model( population *pop, ofstream &outfile );
		void generate( population *oldpop, population *newpop );
};


#endif
