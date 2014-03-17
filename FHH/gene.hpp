#ifndef _gene_hpp
#define _gene_hpp

#include <iostream>
#include "random.hpp"
using namespace std;

class gene  
{
	public:
		double Allele;
		int code;
		gene() {}
		gene( const double v ) { Allele = v; }
		gene( const gene &g ) { Allele = g.Allele; }
		~gene() {}
		void random();
		double allele() { return Allele; }
		gene & operator=(const gene &g);
		gene & operator=(const double v );
		int operator==(const gene &g) { return Allele == g.Allele; }
		int operator==(const double v)   { return Allele == v; }
		int operator!=(const gene &g) { return Allele != g.Allele; }
		int operator!=(const double v)   { return Allele != v; }
		friend ostream &operator<< (ostream &out, const gene &g);
};
#endif
