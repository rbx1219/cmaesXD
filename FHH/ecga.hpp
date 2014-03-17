#ifndef _ecga_hpp
#define _ecga_hpp

#include <iostream>
#include <fstream>
#include "population.hpp"
using namespace std;

class ecga
{
	private:
		void report( ofstream &outfile, population *pop, int gen );
		bool done( population *pop, int gen );

	public:
		ecga() {}
		~ecga() {}
		void run( ofstream &outfile, ofstream &outfile2 );
};

#endif



