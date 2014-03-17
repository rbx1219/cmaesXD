#ifndef _subset_hpp
#define _subset_hpp

#include <iostream>
#include <vector>
#include "population.hpp"
using namespace std;

class subset
{
	private:
		int num_counts;
		double mc;   // model complexity
		double cpc;  // compressed population complexity
	public:
		vector<int> L;         // a subset
		subset();
		~subset();
		subset & operator=(subset &S);
		int size() { return L.size(); }
		int numCounts() { return num_counts; }
		int *counts;
		void space_for_counts( population &pop );
		void insert( int elem ) { L.push_back( elem ); }
		void merge( subset &S1, subset &S2 );
		friend ostream &operator<< (ostream &out, subset &S);


		double MC() { return mc; }
		double CPC() { return cpc; }
		double CC() { return mc + cpc; }
		double modelComplexity( int popsize );
		double compressedPopulationComplexity( population &pop );
		void combinedComplexity( population &pop );

		void compute_freq_counts( population &pop );

};

#endif
