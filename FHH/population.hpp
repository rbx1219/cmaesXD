#ifndef _population_hpp
#define _population_hpp

#include <cassert>
#include <iostream>
#include <fstream>

#include "chromosome.hpp"
#include "codetable.h"
#include "elem.h"
using namespace std;

class population
{
	private:
		int        PopSize;             // population size
		chromosome *Chromosomes;        // individuals
		int        *MatingPool;         // mating pool

		// information for statistical purposes
		int        Best;                // index of best individual
		double     MaxFit;              // maximum fitness
		double     MinFit;              // minimum fitness
		double     AvgFit;              // average fitness    
		double     Stddev;
		
		int        tournament_winner( int *shufflearray, int &pick, int s );

	public:
		code_table *Ctable;

		population( int popsize );
		population( population &pop );
		~population();
		void random();
		void selection( population *newpop );
		void truncational_selection( population *newpop );
		void evaluate();   
		void statistics();
		void encode(ofstream &out);
		int popsize(){ return PopSize; }
		int best() { return Best; }
		double maxfit() { return MaxFit; }
		double minfit() { return MinFit; }
		double avgfit() { return AvgFit; }

		chromosome & operator[]( int index );
		population & operator=( population &pop );
		friend ostream &operator<< ( ostream &out, population &pop );
		bool converged();

		//
		// selection schemes
		// 
		friend void tselect_without_replacement( population &pop );  

		//
		// stopping criterias
		//
		friend bool sc_maxgen( population &pop, int gen );
		friend bool sc_allele_conv( population &pop, int gen );

		friend int elemsearch(elem *ary,double val,int s,int e);
		void rec_encode(code_table &CT,elem *ary,double max,double min,int split_num,int dim,int s,int e);
		
		friend int chrom_comp(const void *a,const void *b);
		void sort();
		void lsearch();
		void searchbest();
		int return_total_size();
		void FHH_binsetting();
};

#endif
