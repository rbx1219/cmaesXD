#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "ecga.hpp"
#include "parameter.hpp"
#include "population.hpp"
#include "mpm.hpp"
#include "utility.hpp"
using namespace std;

extern randomG RANDOM;

//----------------------------------------------------------

void ecga::report( ofstream &outfile, population *pop, int gen )
{
	//
	// get information about the best individual in the population
	//
	chromosome bestChrom;
	bestChrom = (*pop)[ pop->best() ];

	//
	// print it
	//
	cout << "generation      : " << gen << endl
		<< "best fitness    : " << pop->minfit() << endl
		<< "avg fitness     : " << pop->avgfit() << endl;
	if( parameter::report_err )
	{
		cout << "Error value     : " << fabs( pop->minfit() - parameter::ans ) << endl;
	}
	cout << "best chromosome : " << endl << bestChrom << endl
		<< "------------------------------------------------------------"
		<< endl;

	outfile << "Generation " << gen << endl;
	outfile << *pop;
	outfile << "Splite rate: " << parameter::split_rate << endl;
	outfile << "Evaluated functions:" << parameter::eva_fun << endl;
}

bool ecga::done( population *pop, int gen )
{
	switch( parameter::stop_criteria )
	{
		case ALLELE_CONVERGENCE : return sc_allele_conv( *pop, gen );
		case MAX_GENERATIONS    : return sc_maxgen( *pop, gen );
		default: 
					  error("This should never happen.");
	}
}

//
// runs the ECGA and sends output information to the 'outfile'.
//
void ecga::run( ofstream &outfile, ofstream &outfile2 )
{
	int gen = 0;
	int bin_size=0;
	chromosome best_chromosome;
	best_chromosome.Fitness = HUGE_VAL;
	//
	// first population with random individuals
	//
	population *pop = new population( parameter::popsize );   
	pop->random();
	pop->evaluate();
	pop->statistics();  
	//report( outfile, pop, gen );
	if(best_chromosome.fitness() > (*pop).minfit())
	{
		best_chromosome.Fitness = pop->minfit();
		best_chromosome = (*pop)[pop->best()];
	}
	while( !done( pop, gen ) )      
	{ 
		gen++;
		//
		// apply selection
		//
		population *temp_pop = new population( pop->popsize() );
		pop->selection( temp_pop );
		//pop->truncational_selection( temp_pop );
		delete pop;     // delete unnecessary temporary
		pop = temp_pop;
		pop->FHH_binsetting();
		for(int i=0; i<(*pop).popsize(); i++)
		{
			for(int k=0; k<parameter::lchrom; k++){
				int code=(*pop)[i][k].code;
				assert((*pop).Ctable[k].T[code].max >= (*pop)[i][k].allele());
				assert((*pop).Ctable[k].T[code].min <= (*pop)[i][k].allele());
			}
		}
		bin_size += (pop->return_total_size()/parameter::lchrom);
		//
		// model the population with a greedy MPM search
		//
		mpm MPM( parameter::lchrom );
		MPM.model( pop, outfile );  
		//if( parameter::report_MPM ) cout << "MPM: " << MPM << endl;
		//
		// generate a new population using the MPM
		//
		temp_pop = new population( pop->popsize() ); 
		MPM.generate( pop, temp_pop );
		delete pop;
		pop = temp_pop;
		//
		// evaluate individuals and do the report
		//
		pop->evaluate();
		pop->statistics();
		if(best_chromosome.fitness() > (*pop).minfit())
		{
			best_chromosome.Fitness = pop->minfit();
			best_chromosome = (*pop)[pop->best()];
		}
		if( (gen%5==0) )
		{
			//pop->sort();
			//pop->lsearch();
			//pop->statistics();
		}
		//pop->statistics();
		//report( outfile, pop, gen );
		cout<<parameter::eva_fun<<endl;
		if(parameter::eva_fun>=30000)break;
	}

	if(parameter::eva_fun<100001)
	{
		pop->searchbest();
		pop->statistics();
		if(best_chromosome.fitness() > (*pop).minfit())
		{
		best_chromosome.Fitness = pop->minfit();
		best_chromosome = (*pop)[pop->best()];
		}
		report( outfile, pop, gen );
	}
	outfile2<<"avg bin: "<<bin_size/(gen+1)<<" fitness value: "<<best_chromosome.fitness()<<endl;
	//
	// cleanup
	//
	delete pop;
}
