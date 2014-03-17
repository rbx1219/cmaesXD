#ifndef _objfunc_hpp
#define _objfunc_hpp

#include"chromosome.hpp"
#include<gsl/gsl_vector.h>

double objfunc( chromosome &x );
double objective_func( double *chrom, int lchrom );
double my_f(const gsl_vector *v,void *params);
void my_df(const gsl_vector *v,void *params,gsl_vector *df);
void my_fdf(const gsl_vector *v,void *params,double *f,gsl_vector *df);

#endif



