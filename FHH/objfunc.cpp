#include"objfunc.hpp"
#include"chromosome.hpp"
#include"parameter.hpp"
#include"benchmark.h"
#include<gsl/gsl_vector.h>

#include<cstdio>
#include<cmath>
using namespace std;
extern test_func *testfun;
double objfunc( chromosome &x )
{
	//
	// call objective function
	//    
	double *ary=new double[x.length()];
	for(int i=0;i<x.length();++i)
		ary[i]=x[i].allele();

	double v = objective_func( ary , x.length() );
	return v;
}


//
// Objective function.
// Change this function to try different problems
//
double objective_func( double *x, int lchrom )
{
	parameter::eva_fun++;
	return testfun->f(x,lchrom);
}

double my_f(const gsl_vector *v,void *params)
{
	double *x=new double[parameter::lchrom];
	double ans;
	size_t i;
	for(i=0;i<parameter::lchrom;++i)
	{
		x[i]=gsl_vector_get(v,i);
	}
	ans=objective_func(x,parameter::lchrom);
	delete []x;
	return ans;
}
/*
void my_df(const gsl_vector *v,void *params,gsl_vector *df)
{
	size_t n=parameter::lchrom,i;
	gsl_vector *tmp=gsl_vector_alloc(n);
	double delta=1e-10,dif;
	double ans=my_f(v,params);
	for(i=0;i<n;++i)
	{
		gsl_vector_memcpy(tmp,v);
		gsl_vector_set(tmp,i, gsl_vector_get(v,i)+delta );
		dif=my_f(tmp,params);
		gsl_vector_set(df,i,(dif-ans)/delta);
	}
	gsl_vector_free(tmp);
}

void my_fdf(const gsl_vector *v,void *params,double *f,gsl_vector *df)
{
	*f=my_f(v,params);
	my_df(v,params,df);
}
*/
