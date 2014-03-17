#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <gsl/gsl_multimin.h>

#include "population.hpp"
#include "gene.hpp"
#include "chromosome.hpp"
#include "random.hpp"
#include "parameter.hpp"
#include "utility.hpp"
#include "elem.h"
#include "objfunc.hpp"
#include "codetable.h"
using namespace std;

extern randomG RANDOM;
int elem_comp(const void *a,const void *b);
// constructor
population::population( int popsize )
{ 
	PopSize = popsize;
	Chromosomes = new chromosome[ PopSize ];    // allocate memory
	MatingPool = new int[ PopSize ];
	Ctable = new code_table[ parameter::lchrom ];
};

// copy constructor
population::population( population &pop )
{
	PopSize = pop.PopSize;
	Stddev = pop.Stddev;
	Chromosomes = new chromosome[ PopSize ]; // allocate memory
	MatingPool = new int[ PopSize ];         // for pointer members
	Ctable = new code_table[ parameter::lchrom ];

	for( int i=0; i< PopSize; i++ )       // copy data of pointer members
	{
		Chromosomes[i] = pop.Chromosomes[i];
		MatingPool[i] = pop.MatingPool[i];
	}

	for( int i=0; i< parameter::lchrom; i++ )
		Ctable[i] = pop.Ctable[i];
}

// assignment operator
population & population::operator=( population &pop )
{
	if (this == &pop)
		return *this; 

	delete [] Chromosomes;              // deallocates the old population
	delete [] MatingPool;
	delete [] Ctable;

	PopSize = pop.PopSize;
	Stddev = pop.Stddev;
	Chromosomes = new chromosome[ PopSize ];    // make room for the new one
	MatingPool = new int[ PopSize ];
	Ctable = new code_table[ parameter::lchrom ];

	for( int i=0; i< PopSize; i++ )       // copy data of pointer members
	{
		Chromosomes[i] = pop.Chromosomes[i];
		MatingPool[i] = pop.MatingPool[i];
	}

	for( int i=0; i< parameter::lchrom; i++ )
		Ctable[i] = pop.Ctable[i];

	return *this;
}

// destructor
population::~population()
{ 
	delete [] Chromosomes; 
	delete [] MatingPool;
	delete [] Ctable;
};

// create a population with random individuals.
void population::random()
{
	for( int i=0; i< PopSize; i++ )
		Chromosomes[i].random();
}

void population::uniformly_random()
{
	double init=0;
	int* shuffle_array = new int [ PopSize]; 
	double* Array = new double [ PopSize];
	for( int k=0; k< parameter::lchrom; k++)
	{
		init = parameter::low_bound + (RANDOM.uniform01()*(parameter::up_bound-parameter::low_bound)/ PopSize);
		for(int i=0; i< PopSize; i++)
		{
			Array[i] = init;
			init += (parameter::up_bound-parameter::low_bound)/ PopSize;
		}
		makeshuffle(shuffle_array,  PopSize);
		for(int i=0; i< PopSize; i++)
		{
			Chromosomes[i][k] = Array[shuffle_array[i]];
		}
	}	
}

// index operator
chromosome & population::operator[](int index)
{
	assert( index>=0 && index<PopSize );
	return Chromosomes[index];
}

// print a population
ostream &operator<< (ostream &out, population &pop)
{
	if( parameter::report_pop )     // print the population
	{
		if( parameter::report_string ) out << setw(parameter::lchrom)<< "string";
		if( parameter::report_fitness ) out << setw(10) << "fit";
		out << endl;
		for( int i=0; i< pop.popsize(); i++ )
			out << pop[i] << endl;
	}

	out.setf(ios::scientific);
	out.precision(30);
	out << "Min fitness     : " << pop.minfit() << endl;
	out << "Avg fitness     : " << pop.avgfit() << endl;

	if( parameter::report_err )
	{
		out << "Error value     : " << fabs( pop.minfit() - parameter::ans ) << endl;
	}
	out << "Best chromosome : " << endl << pop[ pop.best() ] << endl;
	out << "Stddev : " << pop.Stddev << endl;

	//for(int i=0; i<parameter::Bin_num; i++){
	//	out << "Bin[" << i << "]" << pop.Ctable[0][i].max<<	"Sampled num" <<pop.Ctable[0][i].sampled_num<< "Expected value" << pop.Ctable[0][i].expectancy<<endl;
	//}
	out << "-------------------------------------------------------" << endl;

	return out;
}

//
//  create a new population through selection.
//  the caller should allocate memory for 'newpop'
//
void population::selection( population *newpop )
{
	tselect_without_replacement( *this );               

	// copy individuals from the mating pool into 'newpop'
	for( int i=0; i< PopSize; i++ )
		(*newpop)[i] = Chromosomes[ MatingPool[i] ];
}

void population::truncational_selection( population *newpop )
{
	elem *buf=new elem[PopSize];
	for(int i=0;i<PopSize;++i)
		buf[i].num=i,buf[i].val=Chromosomes[i].Fitness;
	qsort(buf,PopSize,sizeof(elem),elem_comp);
	int j=0;
	for(int i=0; i<PopSize; i++){
		j= (i) % (int)(PopSize/parameter::tournament_size);
		(*newpop)[i] = Chromosomes[buf[j].num];
	}
}

// 
//  evaluate objective function values of population members
//
void population::evaluate()
{
	for( int i=0; i< PopSize; i++ )
		Chromosomes[i].evaluate();
}


// compute statistics
void population::statistics()
{
	double fit = Chromosomes[0].fitness();
	double sumfit = fit;
	MaxFit = MinFit = fit;
	Best = 0;
	Stddev = 0.0;

	for( int i=1; i< PopSize; i++ )
	{
		fit = Chromosomes[i].fitness();
		if( fit > MaxFit ) MaxFit = fit;
		if( fit < MinFit ) { MinFit = fit; Best = i; }
		sumfit += fit;
	}

	AvgFit = sumfit / PopSize;

	for(int i=0;i<PopSize;++i)Stddev+= (Chromosomes[i].fitness()-AvgFit)*(Chromosomes[i].fitness()-AvgFit);
	Stddev /= PopSize;
	Stddev = sqrt(Stddev);
}



//-------- selection schemes ----------------------------------------
// tournament selection without replacement

// set things up for tournament selection without replacement
void pre_tselect_without_replacement( int *array, int &pick, int popsize )
{
	pick = 0;
	makeshuffle( array, popsize );
}

// tournament selection without replacement
int population::tournament_winner( int *shuffle, int &pick, int tournsize )
{
	int winner = shuffle[pick];
	for( int i=pick+1; i< pick+tournsize; i++ )
		if( Chromosomes[ shuffle[i] ].fitness() < Chromosomes[ winner ].fitness() )
			winner = shuffle[i];
	pick += tournsize;
	return winner;
}

// tournament selection without replacement
// the individuals that survive reproduction are placed in the mating pool
void tselect_without_replacement( population &pop )
{
	int *shuffle = new int [pop.popsize()];
	int pick;
	int s = parameter::tournament_size;

	pre_tselect_without_replacement( shuffle, pick, pop.popsize() );//take pick to zero & shuffle the shuffle array (permutation)
	pop.MatingPool[0]=pop.best();
 	for( int i=1; i< pop.popsize(); i++ )
	{
		if( pick+s > pop.popsize() ){
			pre_tselect_without_replacement( shuffle, pick, pop.popsize() );
		}
		pop.MatingPool[i] = pop.tournament_winner( shuffle, pick, s );
	}
	delete [] shuffle;
}

//-------- stopping criterias ----------------------------------------

//
// true if all the individuals have the same alleles
//
bool population::converged()
{
	return (Stddev<0.1);
}

// true if maxgen = gen
bool sc_maxgen( population &pop, int gen )
{
	return gen == parameter::stop_criteria_arg;
}


// assumes binary coding.
bool sc_allele_conv( population &pop, int dummy )
{
	return pop.converged();
}



/*void population::encode(ofstream &out)
{
	elem *buf=new elem[PopSize];
	for(int k=0;k<parameter::lchrom;++k)
	{
		for(int i=0;i<PopSize;++i)
			buf[i].num=i,buf[i].val=Chromosomes[i][k].Allele;
		qsort(buf,PopSize,sizeof(elem),elem_comp);
		Ctable[k].T.clear();
		rec_encode(Ctable[k], buf ,parameter::up_bound,parameter::low_bound , (int)( (double)PopSize * parameter::split_rate),k,0,PopSize-1);
		out << k << " codesize=" << Ctable[k].size() << endl;

		//		for(int i=0;i<Ctable[k].size();++i)
		//		{
		//			out << Ctable[k].T[i].max << " " << Ctable[k].T[i].min << endl;
		//		}
	}
	parameter::split_rate*=parameter::decrease_rate;
	delete [] buf;
}*/

int elemsearch(elem *ary,double val,int s,int e)
{
	int i;
	for(i=e;i>=s;--i)if(ary[i].val<=val)break;
	if(i<s)i=s;
	return i;
}

/*void population::rec_encode(code_table &CT,elem *ary,double max,double min,int split_num,int dim,int s,int e)
{
	int m;
	double mid=RANDOM.uniform(min,max);
//	double mid=(max+min)/2;

	if(e-s+1>=split_num&&max-mid>1e-5&&mid-min>1e-5)
	{
		m=elemsearch(ary,mid,s,e);
		rec_encode(CT,ary,max,mid,split_num,dim,m+1,e);
		rec_encode(CT,ary,mid,min,split_num,dim,s,m);
	}
	else
	{
		int code=CT.add(max,min);
		for(int k=s;k<=e;++k)
			Chromosomes[ary[k].num][dim].code=code;
	}
}*/
int chrom_comp(const void *a,const void *b)
{
	double tmp= ((chromosome *)a)->fitness() - ((chromosome *)b)->fitness();
	if(tmp==0.0)return 0;
	else if(tmp>0.0)return 1;
	else return -1;
}

void population::sort()
{
	qsort(Chromosomes,PopSize,sizeof(chromosome),chrom_comp);
}

/*void population::lsearch()
  {
  size_t i;
  int status;

  const gsl_multimin_fdfminimizer_type *T;
  gsl_multimin_fdfminimizer *s;

  gsl_vector *x;
  gsl_multimin_function_fdf my_func;

  my_func.f = &my_f;
  my_func.df = &my_df;
  my_func.fdf = &my_fdf;
  my_func.n = parameter::lchrom;
  my_func.params = NULL;
  x = gsl_vector_alloc(parameter::lchrom);

  T = gsl_multimin_fdfminimizer_vector_bfgs;
  s = gsl_multimin_fdfminimizer_alloc(T,parameter::lchrom);

  for(int j=0;j<5;++j)
  {
  chromosome org=Chromosomes[j];

  for(i=0;i<parameter::lchrom;++i)
  gsl_vector_set(x,i,org[i].Allele);

  gsl_multimin_fdfminimizer_set(s, &my_func, x, 0.01, 1e-5);

  i=0;
  do{
  status = gsl_multimin_fdfminimizer_iterate(s);
  ++i;

  if(s->f>1e+100&&s->f<-1e+100)
  break;

  if(s->f>org.fitness()&&(s->f - org.fitness())>(org.fitness() - parameter::ans) )
  break;

  if (status)
  break;

  status = gsl_multimin_test_gradient(s->gradient,1e-10);

  }while( status == GSL_CONTINUE && i<20 );

  if(s->f<org.fitness()&&s->f<1e+100&&s->f>-1e+100)
  {
  for(i=0;i<parameter::lchrom;++i)
  org[i].Allele=gsl_vector_get(s->x, i);
  org.Fitness=s->f;
  }

  Chromosomes[j]=org;
  }
  gsl_multimin_fdfminimizer_free(s);
  gsl_vector_free(x);
  }*/
void population::lsearch()
{
	size_t i;
	int status;
	double size;

	const gsl_multimin_fminimizer_type *T;
	gsl_multimin_fminimizer *s;

	gsl_vector *x,*ss;
	gsl_multimin_function my_func;

	my_func.f = &my_f;
	my_func.n = parameter::lchrom;
	my_func.params = NULL;
	x = gsl_vector_alloc(parameter::lchrom);
	ss = gsl_vector_alloc(parameter::lchrom);

	T = gsl_multimin_fminimizer_nmsimplex;
	s = gsl_multimin_fminimizer_alloc(T,parameter::lchrom);

	for(int j=0;j<PopSize/10;++j)
	{
		chromosome org=Chromosomes[j];

		for(i=0;i<parameter::lchrom;++i)
			gsl_vector_set(x,i,org[i].Allele);

		gsl_vector_set_all (ss,(parameter::up_bound-parameter::low_bound)/10.0);
		gsl_multimin_fminimizer_set(s, &my_func, x, ss);

		i=0;
		do{
			status = gsl_multimin_fminimizer_iterate(s);
			++i;

			if(s->fval>1e+100&&s->fval<-1e+100)
				break;

			if(s->fval>org.fitness()&&(s->fval - org.fitness())>(org.fitness() - parameter::ans) )
				break;

			if (status)
				break;

//			status = gsl_multimin_test_size (size, 1e-100);

		}while( /*status == GSL_CONTINUE &&*/ i<40 );

		if(s->fval<org.fitness()&&s->fval<1e+100&&s->fval>-1e+100)
		{
			for(i=0;i<parameter::lchrom;++i)
				org[i].Allele=gsl_vector_get(s->x, i);
			org.Fitness=s->fval;
		}

		Chromosomes[j]=org;
	}
	gsl_multimin_fminimizer_free(s);
	gsl_vector_free(ss);
	gsl_vector_free(x);
}

/*void population::searchbest()
{
	chromosome org;
	org=Chromosomes[Best];

	size_t i;
	int status;

	const gsl_multimin_fdfminimizer_type *T;
	gsl_multimin_fdfminimizer *s;

	gsl_vector *x;
	gsl_multimin_function_fdf my_func;

	my_func.f = &my_f;
	my_func.df = &my_df;
	my_func.fdf = &my_fdf;
	my_func.n = parameter::lchrom;
	my_func.params = NULL;

	x = gsl_vector_alloc(parameter::lchrom);
	for(i=0;i<parameter::lchrom;++i)
		gsl_vector_set(x,i,org[i].Allele);

	T = gsl_multimin_fdfminimizer_vector_bfgs;
	s = gsl_multimin_fdfminimizer_alloc(T,parameter::lchrom);

	gsl_multimin_fdfminimizer_set(s, &my_func, x, 1e-5, 1e-20);

	do{
		status = gsl_multimin_fdfminimizer_iterate(s);

		if(s->f>1e+100&&s->f<-1e+100)
			break;

		if(s->f>org.fitness()&&(s->f - org.fitness())>(org.fitness() - parameter::ans) )
			break;

		if (status)
			break;

		status = gsl_multimin_test_gradient(s->gradient,1e-20);

	}while( status == GSL_CONTINUE && parameter::eva_fun<100000 );

	if(s->f<org.fitness()&&s->f<1e+100&&s->f>-1e+100)
	{
		for(i=0;i<parameter::lchrom;++i)
			org[i].Allele=gsl_vector_get(s->x, i);
		org.Fitness=s->f;
	}
	Chromosomes[Best]=org;

	gsl_multimin_fdfminimizer_free(s);
	gsl_vector_free(x);
}*/

void population::searchbest()
{
	size_t i;
	int status;
	double size;

	chromosome org;
	org=Chromosomes[Best];
	const gsl_multimin_fminimizer_type *T;
	gsl_multimin_fminimizer *s;

	gsl_vector *x,*ss;
	gsl_multimin_function my_func;

	my_func.f = &my_f;
	my_func.n = parameter::lchrom;
	my_func.params = NULL;
	x = gsl_vector_alloc(parameter::lchrom);
	ss = gsl_vector_alloc(parameter::lchrom);

	T = gsl_multimin_fminimizer_nmsimplex;
	s = gsl_multimin_fminimizer_alloc(T,parameter::lchrom);

	for(i=0;i<parameter::lchrom;++i)
		gsl_vector_set(x,i,org[i].Allele);

	gsl_vector_set_all (ss,(parameter::up_bound-parameter::low_bound)/10.0);
	gsl_multimin_fminimizer_set(s, &my_func, x, ss);

	do{
		status = gsl_multimin_fminimizer_iterate(s);

		if(s->fval>1e+100&&s->fval<-1e+100)
			break;

		if(s->fval>org.fitness()&&(s->fval - org.fitness())>(org.fitness() - parameter::ans) )
			break;

		if (status)
			break;

//		status = gsl_multimin_test_size (size, 1e-100);

	}while( /*status == GSL_CONTINUE &&*/ parameter::eva_fun < 100000 );

	if(s->fval<org.fitness()&&s->fval<1e+100&&s->fval>-1e+100)
	{
		for(i=0;i<parameter::lchrom;++i)
			org[i].Allele=gsl_vector_get(s->x, i);
		org.Fitness=s->fval;
	}

	Chromosomes[Best]=org;

	gsl_multimin_fminimizer_free(s);
	gsl_vector_free(ss);
	gsl_vector_free(x);
}

void population::sampled_num_update()
{
	int code;
	int i=0;
	for(int k=0; k<parameter::lchrom; k++){
		for(int i=0; i<parameter::Bin_num; i++){
			Ctable[k][i].sampled_num *= parameter::decrease_rate;
			Ctable[k][i].current_sampled_num = 0;
		}
	}
	for(int i=0; i<PopSize; i++){
		for(int k=0; k<parameter::lchrom; k++){
			code = Chromosomes[i][k].code;
			Ctable[k][code].sampled_num ++;
			Ctable[k][code].current_sampled_num ++;
		}
	}
}
void population::pop_code_update()	//upload the chromosomes code value to fit the code_table range.
{
	elem *buf=new elem[PopSize];
	for(int k=0;k<parameter::lchrom;++k)
	{
		for(int i=0;i<PopSize;++i)
			buf[i].num=i,buf[i].val=Chromosomes[i][k].Allele;
		qsort(buf,PopSize,sizeof(elem),elem_comp);		//sort the population's k'th dimension value
		int j=0;
		for(int i=0; i<PopSize;i++){
			assert(buf[i].val <= Ctable[k][parameter::Bin_num-1].max);
			if(buf[i].val > Ctable[k][j].max){
				j++;
				i--;
			}
			else
				Chromosomes[buf[i].num][k].code = j;
		}
	}
	delete [] buf;
}
void population::pre_expectancy_update()  //need to count the sampled number of each bin first.
{
	assert(PopSize > 1);
	for(int k=0; k<parameter::lchrom; k++){
		for(int i=0; i<parameter::Bin_num; i++){
			Ctable[k][i].expectancy = 0;
			Ctable[k][i].max_expectancy = 0;
			Ctable[k][i].min_expectancy = HUGE_VAL;
		}
	}
	double worst_fitness_value = Chromosomes[0].Fitness;
	for(int i=1; i<PopSize; i++){
		if(worst_fitness_value < Chromosomes[i].Fitness)
			worst_fitness_value = Chromosomes[i].Fitness;
	}
	int code;
	double expectancy;
	int i=0;
	for(int i=0; i<PopSize; i++){
		expectancy = worst_fitness_value - Chromosomes[i].Fitness;
		for(int k=0; k<parameter::lchrom; k++){
			code = Chromosomes[i][k].code;
			Ctable[k][code].expectancy += expectancy;
			if(Ctable[k][code].max_expectancy < expectancy) Ctable[k][code].max_expectancy = expectancy;
			if(Ctable[k][code].min_expectancy > expectancy) Ctable[k][code].min_expectancy = expectancy;
		}
	}
	for(int k=0; k<parameter::lchrom; k++){
		for(int i=0; i<parameter::Bin_num; i++){
			if(Ctable[k][i].min_expectancy > Ctable[k][i].max_expectancy) {Ctable[k][i].min_expectancy = Ctable[k][i].max_expectancy;}
		}
	}
}

void population::expectancy_update()  //need to count the sampled number of each bin first.
{
	assert(PopSize > 1);
	for(int k=0; k<parameter::lchrom; k++){
		for(int i=0; i<parameter::Bin_num; i++){
			Ctable[k][i].expectancy = 0;
			Ctable[k][i].max_expectancy = 0;
			Ctable[k][i].min_expectancy = HUGE_VAL;
		}
	}
	double worst_fitness_value = Chromosomes[0].Fitness;
	for(int i=1; i<PopSize; i++){
		if(worst_fitness_value < Chromosomes[i].Fitness)
			worst_fitness_value = Chromosomes[i].Fitness;
	}
	int code;
	double expectancy;
	int i=0;
	for(int i=0; i<PopSize; i++){
		expectancy = worst_fitness_value - Chromosomes[i].Fitness;
		for(int k=0; k<parameter::lchrom; k++){
			code = Chromosomes[i][k].code;
			Ctable[k][code].expectancy += expectancy;
			if(Ctable[k][code].max_expectancy < expectancy) Ctable[k][code].max_expectancy = expectancy;
			if(Ctable[k][code].min_expectancy > expectancy) Ctable[k][code].min_expectancy = expectancy;
		}
	}
	for(int k=0; k<parameter::lchrom; k++){
		for(int i=0; i<parameter::Bin_num; i++){
			if(Ctable[k][i].min_expectancy > Ctable[k][i].max_expectancy) {Ctable[k][i].min_expectancy = Ctable[k][i].max_expectancy;}
		}
	}
}

void population::Ctable_upload(code_table *newCT){
	delete [] Ctable;
	Ctable = new code_table [parameter::lchrom];
	for(int i=0;i<parameter::lchrom;i++){
		Ctable[i] = newCT[i];
	}
}

void population::Ctable_value_justify(code_table *newCT, int old_Bin){
	double* setting_base = new double [old_Bin];
	double* temp_memory = new double [old_Bin];
	for(int k=0; k<parameter::lchrom; k++){
		for(int i=0; i<old_Bin; i++){
			assert((Ctable[k][i].max_expectancy-Ctable[k][i].min_expectancy) >= 0);
			double avg_expectancy = (Ctable[k][i].expectancy+1e-12)/(Ctable[k][i].current_sampled_num+1e-6);
			temp_memory [i] = avg_expectancy*(Ctable[k][i].max-Ctable[k][i].min+1e-12);
			setting_base[i] = temp_memory[i]*parameter::decay+fabs(temp_memory[i]-Ctable[k][i].memory);
			Ctable[k][i].memory =temp_memory[i];
 		}
		for(int i=0; i<old_Bin; i++){
			//setting_base[i] += (parameter::decrease_rate*RANDOM.uniform(0.0, max_setting_base));
		}
		Bin_range_setting(Ctable[k], newCT[k], setting_base, old_Bin);	//verified
		Bin_value_transfer(Ctable[k], newCT[k], old_Bin);	//verified	
		parameter::decay *= parameter::decrease_rate;
	}
}




void population::Bin_range_setting(code_table &old_table, code_table &new_table, double *setting_base, int old_Bin)	//set new bin boundary according to the value of Array A
{
     double total = 0;
	 for (int i=0; i<old_Bin; i++)
	 {
		 total += setting_base[i];
	 }
	 double avg = total/parameter::Bin_num;		//value of setting base for each bin in new_table
	 double cumulative = 0;
	 double left_bound = old_table[0].min;
	 double right_bound = old_table[0].max;
	 int j=0;
	 new_table[0].min = parameter::low_bound; //same value all the time, never being changed
	 for(int i=0; i<parameter::Bin_num-1; i++)
	 {
		 while((avg*(i+1)) >= (cumulative+setting_base[j]))
		 {
			cumulative += setting_base[j];
			j++;
			left_bound = old_table[j].min;
			right_bound = old_table[j].max;
		 }
		 new_table[i].max =(((avg*(i+1))-cumulative)/setting_base[j])*(right_bound-left_bound)+left_bound;
		 new_table[i+1].min = new_table[i].max;
	 }
	 for(int i=j; i<old_Bin; i++){
		 cumulative += setting_base[i];
	 }
	 assert(fabs(cumulative-total) < 1e-5);
	 new_table[parameter::Bin_num-1].max = parameter::up_bound;  //same value all the time, never being changed
}

double non_zero(double x){
	if(x ==0)
		return 1e-10;
	else
		return x;
}

void population::Bin_value_transfer(code_table &old_table, code_table &new_table, int old_Bin){
	 for(int i=0; i<parameter::Bin_num; i++){
	    assert(new_table[i].expectancy==0 && new_table[i].sampled_num==0);  //make sure that the new code table has value 0 in sampled num and expectancy
	 }
	 double left_bound = parameter::low_bound;	//the index place we calculated the density value
	 int j=0;
	 for (int i=0; i<parameter::Bin_num; i++)  //calculate the new Transfered_value according to new/old bin boundary
	 {
		 while(new_table[i].max>old_table[j].max)
		 {
			 new_table[i].memory += (old_table[j].max-left_bound)*old_table[j].memory/non_zero(old_table[j].max-old_table[j].min);
			 left_bound = old_table[j].max;
			 j++;
		 }
		 new_table[i].memory += (new_table[i].max-left_bound)*old_table[j].memory/non_zero(old_table[j].max-old_table[j].min);
		 left_bound = new_table[i].max;
	 }
	 assert(j==old_Bin-1);
}



void population::population_check(){
	for(int k=0; k<parameter::lchrom; k++){
		for(int i=0; i<PopSize; i++){
			if(Chromosomes[i][k].Allele < parameter::low_bound || Chromosomes[i][k].Allele > parameter::up_bound){
				cout<<Chromosomes[i][k].Allele<<endl;
				system("Pause");
			}
		}
	}
}

void population::FHH_binsetting(){
	elem *buf=new elem[PopSize];
	int *series=new int[parameter::Bin_num];
	int *chrom_num=new int[parameter::Bin_num];
	int a=0;
	for(int k=0;k<parameter::lchrom;++k){
		makeshuffle(series,parameter::Bin_num);
		for(int i=0;i<parameter::Bin_num;i++)
			chrom_num[i] = (int)(PopSize/parameter::Bin_num);
		for(int i=0;i<(PopSize%parameter::Bin_num);i++)
			chrom_num[series[i]]++;
		for(int i=0;i<parameter::Bin_num;i++)
			a+=chrom_num[i];
		assert(a==PopSize);
		for(int i=0;i<PopSize;++i)
			buf[i].num=i,buf[i].val=Chromosomes[i][k].Allele;
		qsort(buf,PopSize,sizeof(elem),elem_comp);
		delete [] Ctable[k].T;
		int lindex=0;
		int rindex=0;
		double left_bound=parameter::low_bound;
		double right_bound=parameter::low_bound;
		double temp;
		for(int i=0;i<parameter::Bin_num;i++){
			lindex=rindex;
			rindex+=chrom_num[i];
			left_bound=right_bound;
			right_bound=0.5*(buf[rindex-1].val+buf[rindex].val);
			int code=Ctable[k].add(right_bound,left_bound);
			for(int i=lindex;i<rindex;i++){
				Chromosomes[i][k].code = code;
			}
		}
	}
}
