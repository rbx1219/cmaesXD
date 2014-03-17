#include <iostream>
#include <cmath>
#include <cstdlib>

#include "subset.hpp"
#include "parameter.hpp"
#include "utility.hpp"
#include "population.hpp"
using namespace std;
//
// constructor
//
subset::subset()
{
	L.clear();
	counts = NULL;
	num_counts = 0;
	mc = 0;
	cpc = 0;
}

//
// destructor
//
subset::~subset()
{
	delete [] counts;  // free memory
}

//
// assignment operator
//
subset & subset::operator=(subset &S)
{
	if (this == &S)
		return *this;

	L.clear();
	for( int i=0; i< S.L.size(); ++i)
		insert(S.L[i]);

	num_counts = S.num_counts;
	mc = S.mc;
	cpc = S.cpc;

	delete [] counts;
	counts = new int[ num_counts ];
	for( int i=0; i< num_counts; i++ )
		counts[i] = S.counts[i];

	return *this;
}

void subset::merge( subset &S1, subset &S2 )
{
	for(int i=0;i<S1.L.size();++i)
		insert(S1.L[i]);
	for(int i=0;i<S2.L.size();++i)
		insert(S2.L[i]);
}

//
// allocates spaces for the frequency counts.
//
void subset::space_for_counts( population &pop )
{
	delete [] counts;  // free memory
	num_counts=1;
	for( int i=0; i< L.size();++i)
		num_counts*=pop.Ctable[L[i]].size();
	counts = new int[ num_counts ];  // allocate memory
	for( int i=0; i< num_counts; i++ ) 
		counts[i] = 0;
}

// print the subset
ostream &operator<< (ostream &out, subset &S)
{
	for(int i=0;i<S.L.size();++i)
		out << S.L[i] << "  ";
	return out;
}

//
// returns the number of bits needed to store the model
// for this subset.
// 
double subset::modelComplexity( int popsize )
{
	return log( 1 + popsize, 2 ) * (num_counts - 1);
}

//
// returns the number of bits needed to store this subset of bits
// across the whole population.
//
/*
double subset::compressedPopulationComplexity( population &pop )
{
	double entropy = 0;
	int S=size();
	double maxarea = pow(parameter::up_bound - parameter::low_bound,(double)S);
	int *schema=new int[S];
	for(int i=0;i<S;++i)schema[i]=0;
	int *index = new int [ S ];
	index[0]=1;
	for( int i=1; i<S ;++i )
		index[i]=index[i-1]*pop.Ctable[L[i-1]].size();

	for( int j=0; j< num_counts; j++ )
	{
		int p=decode(schema,S,index);
		double area=1.0;
		for(int i=0;i<S;++i)
		{
			assert(schema[i]<pop.Ctable[L[i]].size());
			area*=pop.Ctable[L[i]].T[schema[i]].max - pop.Ctable[L[i]].T[schema[i]].min;
		}
		area=(area / maxarea );
		double prob = ( (double(counts[p])) / (double(pop.popsize())) );
		if( prob != 0 )prob*=area;
		if(!(prob>=0.0 && area>=0.0))
		{
			cout << area << " " << maxarea << " " << prob << endl;
			exit(1);
		}
		if( prob != 0 )
			entropy += -prob * log(prob,2);

		int c;
		for(c=0;c<S;++c)
		{
			if( schema[c]<pop.Ctable[L[c]].size()-1 )
			{
				schema[c]++;
				break;
			}
		}
		for(--c;c>=0;--c)schema[c]=0;
	}
	//
	// entropy gives the avg. number of bits needed to store this subset of bits
	// per individual under the current model
	// 

	delete [] schema;
	delete [] index;
	return entropy * (double)pop.popsize() * maxarea;
}*/

double subset::compressedPopulationComplexity( population &pop )
{
	double entropy = 0;
	for( int j=0; j< num_counts; j++ )
	{
		double prob = double(counts[j]) / pop.popsize();
		if( prob != 0 )
			entropy += -prob * log(prob,2);
	}
	//
	// entropy gives the avg. number of bits needed to store this subset of bits
	// per individual under the current model
	//
	return entropy * pop.popsize();
}

//
// compute the combined complexity
//
void subset::combinedComplexity( population &pop )
{
	mc = modelComplexity( pop.popsize() );
	cpc = compressedPopulationComplexity( pop );
}

//
// compute the frequency counts for this subset of genes
//
void subset::compute_freq_counts( population &pop )
{
	int S = size();
	int twoToTheS = numCounts();
	int *index = new int [ S ];
	index[0]=1;
	for( int i=1; i<S ;++i )
		index[i]=index[i-1]*pop.Ctable[L[i-1]].size();
	int *schema = new int [ S ];
	long p;

	for( int i=0; i< pop.popsize(); i++ )
	{
		for( int j=0; j< S; j++ )
		{
			int locus = L[j];
			int allele = pop[i][locus].code;
			schema[j] = allele;
		}
		p = decode( schema, S ,index);	
		assert( p>=0 && p<twoToTheS );
		counts[p]++;
	}
	delete [] schema;
	delete [] index;
}
