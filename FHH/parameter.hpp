// -*- c++ -*-
//
//  File:         parameter.hpp
//
//  Description:  definition of parameters for the ECGA.
//
//  Author:       Fernando Lobo
//
//  Date:         June/1999
//

#ifndef _parameter_hpp
#define _parameter_hpp

class parameter    
{
 public:
  static long   seed;              // seed for random number generator
  static int    lchrom;            // chromosome length
  static int    popsize;           // population size
  static double pcross;
  static double split_rate;
  static double decrease_rate;
  static double max_model;
  static int    tournament_size;   // tournament size
  static int    Bin_num;
  static int    stop_criteria;     // stopping criteria for the algorithm
  static double stop_criteria_arg; // stop criteria argument (ex: maxgens= x)
  static bool   learn_MPM;         // on = learns linkage, off = compact GA

  static bool   report_err;
  static bool   report_pop;
  static bool   report_string;
  static bool   report_fitness;
  static bool   report_MPM;
  static double up_bound;
  static double low_bound;
  static double ans;
  static int eva_fun;
  static int num_fun;
};


//
// constant definitions for stopping criterias
//
const int ALLELE_CONVERGENCE          = 60;
const int MAX_GENERATIONS             = 61;


#endif
