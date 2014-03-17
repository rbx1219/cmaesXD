#include "utility.hpp"     // utility functions and procedures
#include "random.hpp"      // random number generator
#include "parameter.hpp"   // parameters for the ECGA
#include "ecga.hpp"        // ECGA code
#include "benchmark.h"
#include <cassert>
#include <iostream>
#include <cctype>
#include <cstdlib>
using namespace std;

long   parameter::seed;
int    parameter::lchrom;
int    parameter::popsize;
double parameter::pcross;
double parameter::split_rate;
double parameter::decrease_rate;
double parameter::max_model;
int    parameter::tournament_size;    
int    parameter::Bin_num;
int    parameter::stop_criteria;     
double parameter::stop_criteria_arg; 
bool   parameter::learn_MPM;   
double parameter::up_bound;
double parameter::low_bound;
double parameter::ans;
bool   parameter::report_err;
bool   parameter::report_pop;
bool   parameter::report_string;
bool   parameter::report_fitness;
bool   parameter::report_MPM;
int    parameter::eva_fun;
int    parameter::num_fun;

randomG RANDOM;
test_func *testfun;

// return the identifier of the stop criteria. return -1 if not found.
int find_stop_criteria( char *s )
{
	if( strcmp( s, "allele_convergence" ) == 0 ) return ALLELE_CONVERGENCE;
	if( strcmp( s, "max_generations" ) == 0 ) return MAX_GENERATIONS;
	return -1;
}


// 
// read parameters from input file.
// 
void read_parameters( ifstream &in )
{
	const int unset = -1;
	int op;
	char  s[100] = "";
	char  s2[100] = "";

	while( strcmp( s, "BEGIN" ) != 0 ) 
	{
		in.getline( s, 100 );
		cout << s << endl;
	}

	in >> s >> parameter::lchrom;
	cout << s << "   " << parameter::lchrom << endl; 
	errorcheck("Check lchrom.", parameter::lchrom > 0 );

	in >> s >> parameter::seed;
	cout << s << "   "  << parameter::seed << endl;
	//	errorcheck("seed must be in 0..1", parameter::seed > 0 && parameter::seed < 1 );

	in >> s >> parameter::popsize;
	cout << s << "   "  << parameter::popsize << endl;
	errorcheck("population size must be even.", parameter::popsize % 2 == 0 );
	errorcheck("population size must be positive.", parameter::popsize > 0 );

	in >> s >> parameter::pcross;
	cout << s << "   "  << parameter::pcross << endl;
	errorcheck("pcross must be in 0..1", parameter::pcross >= 0 && parameter::pcross <= 1 );

	in >> s >> parameter::split_rate;
	cout << s << "   "  << parameter::split_rate << endl;

	in >> s >> parameter::decrease_rate;
	cout << s << "   "  << parameter::decrease_rate << endl;

	in >> s >> parameter::max_model;
	cout << s << "   "  << parameter::max_model << endl;

	in >> s >> parameter::tournament_size;
	cout << s << "   "  << parameter::tournament_size << endl;

	in >> s >> parameter::Bin_num;
	cout << s << "   "  << parameter::Bin_num << endl;
	
	in >> s >> s2; parameter::learn_MPM        = (strcmp(s2,"on") == 0);
	cout << s << "   "  << s2 << endl;

	in >> s >> s2; op = find_stop_criteria( s2 );
	cout << s << "   "  << s2 << endl;
	parameter::stop_criteria = op;
	errorcheck("stop criteria not available.", op != -1 );

	in >> s >> parameter::stop_criteria_arg;
	cout << s << "   "  << parameter::stop_criteria_arg << endl;

	in.getline( s, 100 );

	in.getline( s, 100 );
	in.getline( s, 100 );
	in.getline( s, 100 );

	in >> s >> s2; parameter::report_err       = (strcmp(s2,"on") == 0); cout << s << "   "  << s2 << endl;
	in >> s >> s2; parameter::report_pop       = (strcmp(s2,"on") == 0); cout << s << "   "  << s2 << endl;
	in >> s >> s2; parameter::report_string    = (strcmp(s2,"on") == 0); cout << s << "   "  << s2 << endl;
	in >> s >> s2; parameter::report_fitness   = (strcmp(s2,"on") == 0); cout << s << "   "  << s2 << endl;
	in >> s >> s2; parameter::report_MPM       = (strcmp(s2,"on") == 0); cout << s << "   "  << s2 << endl;
	in >> s >> parameter::up_bound;
	in >> s >> parameter::low_bound;
	cout << parameter::up_bound << "," << parameter::low_bound << endl;
	in >> s >> parameter::ans;
	cout << s << parameter::ans << endl;
	in >> s >> parameter::num_fun;

	cout << "--------------------------------------------------------------------" << endl;
} // end of read_parameters


int main( int argc, char *argv[] )
{
	// process command line
	if( argc != 4 )   
	{
		cout << "Usage: " << argv[0] << " inputfile outputfile outputfile2" << endl;
		cout << "       Please read the README file." << endl;
		exit(1);
	}
	// read parameters from input file
	ifstream infile( argv[1] ); 
	read_parameters( infile );
	infile.close();
	// open output file
	ofstream outfile( argv[2] );  
	ofstream outfile2( argv[3] );
	// initilalize random number generator
	RANDOM.randomize( parameter::seed );
//	RANDOM.randomize();
	initial();		//upload the value of /supportData/fbias_data.txt into m_biases; sqrt(((double )i) + 1.0) into  m_iSqrt
	testfun=testFunctionFactory(parameter::num_fun,parameter::lchrom);
	// run the ECGA algorithm
	parameter::eva_fun=0;
	ecga ECGA;
	ECGA.run( outfile, outfile2 );
	cout << "ECGA done" << endl;
	cout << "Evaluated functions:" << parameter::eva_fun << endl;
	outfile.close();
	outfile2.close();
	return 0;
}
