#include<cassert>
#include<iostream>
#include"gene.hpp"
#include"random.hpp"      // random number generator
#include"parameter.hpp"
using namespace std;

extern randomG RANDOM;

void gene::random()
{
	Allele=RANDOM.uniform(parameter::low_bound,parameter::up_bound);
}

// print the gene
ostream &operator<< (ostream &out, const gene &g)
{
	out << g.Allele; 
	return out;
}

// assignment operator: gene = gene
gene & gene::operator=(const gene &g)
{
	if (this== &g) return *this;
	Allele = g.Allele;
	return *this;
}

// assignment operator: gene = int
gene & gene::operator=(const double v)
{
	Allele = v;
	return *this;
}

