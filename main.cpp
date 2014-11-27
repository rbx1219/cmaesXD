#include <stdio.h>
#include "myrand.h"
#include <math.h>
#include "cmaes.h"
#include "Eigen/Dense"
#include "group.h"
#include "node.h"
#include <utility>
#include "global.h"

#define dimension 10
#define PopSize 450

double upperbound ;
double lowerbound ;

const int num_groups = 15 ;
int fe = PopSize;
int lambda;
int NewGroupCount = 0;
double sigma[num_groups];
double minFitness = 999999;
int Criteria[num_groups];
int min_group = 999;

Eigen::MatrixXd cov[num_groups];
Eigen::VectorXd psigma[num_groups];
Eigen::VectorXd pcov[num_groups];

double BigSigma ;
Eigen::MatrixXd BigCov;


bool shouldTerminate(int n)
{
    return (nfe >= 100000);	
}

double square_of_distance(Node a , Node b)
{
    double result = 0.0;
    for(int i = 0 ; i < dimension ; i++)
	result += (a.allele(i)-b.allele(i)) * (a.allele(i)-b.allele(i));	
    return result;
}

double calculateDistance(Node a , Node b)
{
    return square_of_distance(a,b);
}

void pull(GROUP *groups , int stage )
{
    int minX ;
    double min ;

    /* 1. calculate UCB */
    minX = 0;
    min=groups[0].calculateUCB(fe);
    int i;
    for(i = 1; i < num_groups ; i++)
    {

	double tmp = groups[i].calculateUCB(fe);
	if(tmp <min)
	{
	    minX = i;
	    min = tmp;
	}
    }
    /*end of 1*/

    /*2. CMAES in best UCB group*/

    int UCBTimer = 30;
    Node *temp_pop = new Node[ groups[minX].nodes.size() ];

    list< Node >::iterator iter = groups[minX].nodes.begin();
    for(i = 0 ; i < groups[minX].nodes.size() ; i++ , iter++)
	temp_pop[i] = *iter;


    Node best = groups[minX].get_best_vector();	
    CMAES es(1 , lambda , dimension , &best , UCBTimer , sigma[minX] , cov[minX], psigma[minX] , pcov[minX]);	
    es.run();

    Node temp_node(dimension);
    temp_node=es.generate();

    //  recording work
    if(temp_node.fitness < minFitness)
    {
	minFitness = temp_node.fitness ;
	min_group = minX;
    }
    if(best.fitness == temp_node.fitness)
	Criteria[minX] ++;
    else 
	Criteria[minX] = 0;
    sigma[minX] = es.sigma;
    cov[minX] = es.covar;
    psigma[minX] = es.ps;
    pcov[minX] = es.pc;
    /*3. if stage == 0 replace the worst node with the new one*/
    if(stage == 0)
	groups[minX].replace_worst(temp_node , fe++);
    if(stage == 1)
	groups[minX].push(temp_node , fe);
    /*end of 3*/	
    delete[] temp_pop;
    if(es.isBest())
	return;
}

void updateGroups(GROUP *groups , CMAES * es)
{

    /*1. determine the to-be-deleted group*/
    MyRand rand;    
    int *uni = new int[num_groups];
    rand.uniformArray(uni , num_groups , 0 , num_groups-1);
    int curFE = fe;
    int candidate = num_groups;
    for(int i = 0 ; i < num_groups ; i++)
	if(Criteria[uni[i]] == 1)			
	{
	    candidate = uni[i];
	    Criteria[candidate] = 0;
	    break;
	}
    if(candidate == num_groups)
	for(int i = 0 ; i < num_groups ; i++)
	    if(curFE - groups[uni[i]].LastModifiedFE > groups[uni[i]].nodes.size() *2 ) 
	    {
		candidate = uni[i];
		break;
	    }
    if(candidate == num_groups)
	return;
    //	candidate = uni[uni[uni[0]]];
    delete[] uni;
    /*end of 1*/	
    Node *temp_pop = new Node[ num_groups  ];
    for(int i = 0 ; i < num_groups  ; i++)
	temp_pop[i] = groups[i].get_best_vector();

    for(int i = 0 ; i < num_groups ; i++)
	es->population[i] = temp_pop[i];
    es->update_mean(es->population);
    es->run();
    Node temp_node = es->generate();
    if(temp_node.fitness < minFitness)
    {
	minFitness = temp_node.fitness ;
	min_group = candidate;

	groups[candidate].gID = NewGroupCount + num_groups;	
	NewGroupCount++;
	int remain = groups[candidate].nodes.size() -1;
	sigma[candidate] = 1.0;
	cov[candidate].setIdentity(dimension , dimension);
	psigma[candidate].setZero(dimension);
	pcov[candidate].setZero(dimension);
	groups[candidate].clear();
	groups[candidate].push(temp_node , fe++);
	for(int i = 0 ; i < remain ; i++)
	    pull(groups , 1); //stage 1 doesn't erase Nodes
	/*end of 3*/
    }
    delete[] temp_pop;
}


int main(int argc , char **argv)
{
    //parameter declaration/*{{{*/
    Node *central = new Node[num_groups];
    Node *population = new Node[PopSize];
    double node1000, node10000;
    bool flag1000 = false, flag10000 = false;
    MyRand rand;

    GROUP groups[num_groups];

    bool SwapFlag = true;
    int clusteringCategory[PopSize] = {0};
    int generation = 0;

    funATT = atoi(argv[1]);
    lambda = atoi(argv[2]);

    upperbound = domainupbound[funATT-1] , lowerbound = domainlowbound[funATT-1];
    for(int i = 0 ; i < num_groups ; i++)
	groups[i].length = dimension;
    /*}}}*/
    /*initial pop *//*{{{*/
    for(int i = 0 ; i < PopSize ; i++)
    {
	population[i].length = dimension;
	population[i].allele.setZero(dimension);
	for(int j = 0 ; j < dimension ; j++)
	    population[i].allele(j) = rand.uniform(lowerbound , upperbound);
	population[i].setFitness(groups[0].evaluate( population[i] , dimension) );    	
    }/*}}}*/

    int *shuffle = new int[2 * PopSize];
    rand.uniformArray(shuffle, PopSize, 0, PopSize-1);
    rand.uniformArray(shuffle + PopSize, PopSize, 0, PopSize-1);
    Node *offspring = new Node[PopSize];
    for(int i = 0 ; i < PopSize; i++)
    {
    	if(population[2*i].fitness < population[2*i+1].fitness)
	    offspring[i] = population[2*i];
	else
	    offspring[i] = population[2*i + 1];
    }
	
    for(int i = 0 ; i < PopSize; i++)
	population[i] = offspring[i];
    delete[] offspring;
    delete[] shuffle;	   
    
    /*K-MEANS below*//*{{{*/
    int *uniarray = new int[PopSize];
    rand.uniformArray(uniarray , PopSize , 0 , PopSize-1);

    for(int i = 0 ; i < num_groups ; i++)
	central[i] = population[uniarray[i]];
    delete[] uniarray;
    while(SwapFlag)
    {
	SwapFlag = false;
	for(int i = 0 ; i < PopSize ; i++)
	{	
	    double minDistance = 99999999.999999;
	    int closet = clusteringCategory[i];
	    for(int j = 0 ; j < num_groups ; j++)
	    {
		if( calculateDistance(population[i] , central[j]) < minDistance)
		{
		    minDistance = calculateDistance(population[i] , central[j]);
		    closet = j;
		}
	    }
	    if(closet != clusteringCategory[i])
	    {
		SwapFlag = true;
		clusteringCategory[i] = closet;
	    }
	}		
	/*updating central*/
	for(int j = 0 ; j < num_groups ; j++)
	{
	    Node tmp(dimension);
	    int count = 0;
	    for(int i = 0 ; i < PopSize ; i++)
		if(clusteringCategory[i] == j)
		{
		    count++;
		    tmp.allele = tmp.allele + population[i].allele;
		}
	    central[j].allele = tmp.allele / count;
	}


    }//end of K means

    for(int i = 0 ; i < PopSize ; i++)
	groups[ clusteringCategory[i] ].push(population[i] , PopSize);

    /*}}}*/
    CMAES::a.rng.seed(time(NULL));	
/*
    //CMAES with UCB
    Node hill[num_groups];
           for(int i = 0 ; i < num_groups ; i++)
	     {
	     groups[i].gID = i;
	     sigma[i] = 1.0;
	     cov[i].setIdentity(dimension , dimension);
	     psigma[i].setZero(dimension);
	     pcov[i].setZero(dimension);
	     Criteria[i] = 0;
	     hill[i] = groups[i].get_best_vector();
	     }

	     CMAES outerES(num_groups , atoi(argv[2]) , dimension , hill , 1);
	     double error;
	     while(!shouldTerminate(generation ++))
	     {
	     pull(groups , 0);
	     updateGroups(groups , &outerES);
	     error = minFitness - best[funATT -1];
	     if(nfe >=1000 && !flag1000)
	     flag1000 = true, node1000 = error;
	     if(nfe >= 10000 && !flag10000)
	     flag10000 = true, node10000 = error;
	     }

		
	     error = error > 0 ? error : 1e-15;
//	     printf("%e %e %e\n", node1000,node10000,error)	;
*/
    //pure CMAES
    Node bestNode = population[0];	
    double error = 99999999;
    CMAES es(1 , atoi(argv[2]) , dimension , &bestNode , 1);
    while(!shouldTerminate(generation++))
    {
	es.run();
	bestNode = es.generate();

	if(error >bestNode.fitness - best[funATT-1])
	    error = bestNode.fitness - best[funATT-1];
	if(nfe >=1000 && !flag1000)
	    flag1000 = true, node1000 = error;
	if(nfe >= 10000 && !flag10000)
	    flag10000 = true, node10000 = error;
	error = error > 0 ? error : 1e-15;
    }

    //printf("%e %e %e\n",node1000,node10000,error);
    /*
	//2-layer
	for(int i = 0 ; i < num_groups ; i++)
	{
	Node *temp_pop = new Node[groups[i].nodes.size()];
	list< Node >::iterator iter = groups[i].nodes.begin();
	for(int j = 0 ; j < groups[i].nodes.size() ; j++ , iter++)
	temp_pop[j] = *iter;


	CMAES es(groups[i].nodes.size(),atoi(argv[2]) , dimension, temp_pop, 1000);
	es.run();
	hill[i] = es.generate();
	delete[] temp_pop;
	}
	double minXD = hill[0].fitness;
	for(int i = 1 ; i < num_groups ; i++)
	if(hill[i].fitness < minXD)
	minXD = hill[i].fitness;
	minXD = minXD - best[funATT-1];
	minXD = minXD > 0 ? minXD : 1e-15;
	printf("%e ",minXD)	;
	CMAES XD(num_groups,atoi(argv[2]) ,dimension , hill, -1);
	XD.run();
	cout << nfe << endl;
	double    error = XD.generate().fitness - best[funATT-1];
	error = error > 0 ? error : 1e-15;
	if(minXD > error)
	minXD = error;
	printf("%e \n",minXD);*/
	return 0;
}

