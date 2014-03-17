// -*- c++ -*-
//
//  File:         utility.cpp
//
//  Description:  utility functions and procedures.
//
//  Author:       Fernando Lobo
//
//  Date:         June/1999
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "utility.hpp"
#include "random.hpp"
using namespace std;

extern randomG RANDOM;

// outputs an error msg and stops the program 
void error( char *msg )
{
	cout << msg << endl;
	exit(1);
}

// if 'condition' is false, print an error message and abort the program
void errorcheck( char *str, bool condition )
{
	if( !condition )
	{
		cout << "ERROR: " << str << endl;
		exit( 1 );
	}
}

double sqr( double x )
{
	return x*x;
}

int min( int x, int y )
{
	if (x<y) return x;
	else return y;
}

int max( int x, int y )
{
	if (x>y) return x;
	else return y;
}

// return log x base b
double log( double x, double b )
{
	return log(x)/log(b); 
}

// make a random n-permutation of the numbers 0,1,2,...,n-1
void makeshuffle( int *shufflearray, const int n )
{
	int i;

	// initialize
	for( i=0; i<n; i++ ) shufflearray[i] = i;
	// shuffle
	for( i=0; i<n-1; i++ )
	{
		int other = RANDOM.uniform( i, n-1 );
		// swap( shufflearray[other], shufflearray[i] );
		int temp = shufflearray[other];
		shufflearray[other] = shufflearray[i];
		shufflearray[i] = temp;
	}
}

int decode(int *schema,int S,int *index)
{
	int addr=0;
	for(int i=0;i<S;++i)
		addr+=schema[i]*index[i];
	return addr;
}

void encode(int addr, int *schema, int S, int *index)	//verified!!
{
	int remainder = 0;
	int code_num = 0;
	for (int i=S-1; i>=0; i--){
		remainder = addr % index[i];
		schema[i] = (addr-remainder)/index[i];
		addr = remainder;
	}
}

int allocate_addr(double *prob)		//verified!!
{
	double pin = RANDOM.uniform01();
	int index = 0;
	while (pin > prob[index]){
		pin -= prob[index];
		index++;
	}
	return index;
}


double codegen(code_type ct)
{
	return RANDOM.uniform(ct.min,ct.max);
}

void uniformity_scattering(double* prob, int prob_size, int* counts, int counts_size)
{
	int index=0;
	double norm_factor = 0;
	int *temp_counts = new int [counts_size];
	int *shuffle_array = new int [counts_size];
	for(int i=0; i<prob_size; i++){
		for(int j=0; j<(int)(prob[i]*counts_size); j++){
			counts[index] = i;
			index++;
		}
		prob[i] = (prob[i]*counts_size - (int)(prob[i]*counts_size)) / counts_size ;
		norm_factor += prob[i];
	}
	/////////////////////////////////////////////////////////////////
	for(int i=0; i<prob_size; i++){
		prob[i] = prob[i] / norm_factor;
	}
	for(int i=index; i<counts_size; i++){
		counts[i] = allocate_addr(prob);
	}
	assert(index <=  counts_size);
	makeshuffle(shuffle_array, counts_size);
	for(int i=0; i<counts_size; i++){
		temp_counts[i] = counts[shuffle_array[i]];
	}
	for(int i=0; i<counts_size; i++){
		 counts[i] = temp_counts[i];
	}
	delete [] temp_counts;
}

void SUS(double* prob, int prob_size, int* counts, int counts_size)
{
	int *temp_counts = new int [counts_size];
	int *shuffle_array = new int [counts_size];
	double x=(RANDOM.uniform01()/counts_size);
	double *cumulate_prob = new double [prob_size];
	cumulate_prob[0] = prob[0];
	for(int i=1; i<prob_size; i++){
		cumulate_prob[i] = cumulate_prob[i-1] + prob [i];
	}
	assert(cumulate_prob[prob_size-1]>0.9999 && cumulate_prob[prob_size-1]<1.0001);
	int index=0;
	for(int i=0; i<counts_size; i++){
		if(x <= cumulate_prob[index]){
			counts[i] = index;
			x += (1.00/counts_size);
		}
		else{
			index++;
			i--;
		}
	}
	assert(index  <=  prob_size-1);
	makeshuffle(shuffle_array, counts_size);
	for(int i=0; i<counts_size; i++){
		temp_counts[i] = counts[shuffle_array[i]];
	}
	for(int i=0; i<counts_size; i++){
		 counts[i] = temp_counts[i];
	}
	delete [] temp_counts;
}

