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
#define PopSize 400
#define upperbound 100
#define lowerbound -100

const int num_groups = 20;
int fe = PopSize;


bool shouldTerminate(int n)
{
    return (nfe == 100000);	
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
    //printf("\n\ngroup[ 0].UCBVALUE = %lf\n" , min);
    for(i = 1; i < num_groups ; i++)
    {

	double tmp = groups[i].calculateUCB(fe);
	//	printf("group[%2d].UCBVALUE = %lf\n" , i , tmp);
	if(tmp <min)
	{
	    minX = i;
	    min = tmp;
	}
    }
    /*end of 1*/

    /*2. CMAES in best UCB group*/
    Node *temp_pop = new Node[ groups[minX].nodes.size() ];

    list< Node >::iterator iter = groups[minX].nodes.begin();
    for(i = 0 ; i < groups[minX].nodes.size() ; i++ , iter++)
	temp_pop[i] = *iter;

    CMAES es(groups[minX].nodes.size() , 1 , dimension , temp_pop);
    es.run();
    Node temp_node(dimension);
    temp_node=es.generate();
    //cout << "generating" << endl << temp_node << endl << endl;	 
    /*end of 2*/

    /*3. if stage == 0 replace the worst node with the new one*/
    if(stage == 0)
	for(iter = groups[minX].nodes.begin() ; iter != groups[minX].nodes.end() ; ++iter)
	    if( iter->fitness == groups[minX].getmax() )
	    {
		groups[minX].nodes.erase(iter++);
		break;
	    }
    /*    if(stage == 1)
	  {
	  printf("in updating now choosing %d : size %d \n" , minX , groups[minX].nodes.size());
	  }*/

    groups[minX].push(temp_node , fe++);
    /*end of 3*/	

    delete[] temp_pop;
}

void updateGroups(GROUP *groups)
{

    /*1. determine the to-be-deleted group*/
    MyRand rand;    
    int *uni = new int[num_groups];
    rand.uniformArray(uni , num_groups , 0 , num_groups-1);
    int curFE = fe;
    int candidate = num_groups;
    for(int i = 0 ; i < num_groups ; i++)
	if(curFE - groups[uni[i]].LastModifiedFE > groups[uni[i]].nodes.size())
	{
	    candidate = uni[i];
	    break;
	}
    delete[] uni;
    /*end of 1*/	

    if(candidate != num_groups)
    {
	/*2. generating new node according to mean vectors of  (num_groups - 1) groups */
	Node *temp_pop = new Node[ num_groups -1 ];
	int count = 0;
	for(int i = 0 ; i < num_groups  ; i++)
	{
	    if(i == candidate)
		continue;
	    temp_pop[count++] = groups[i].get_best_vector();
	    printf("%2d : size %d\n" , i , groups[i].nodes.size());
	}
	CMAES es(num_groups -1, 1 , dimension , temp_pop);
	es.run();
	Node temp_node = es.generate();
	/*end of 2*/

	/*3. clear the candidate group and fill up remain nodes*/
	int remain = groups[candidate].nodes.size() -1;
	groups[candidate].clear();
	groups[candidate].push(temp_node , fe++);
	printf("from %d remain %d curNFE = %d\n" , candidate , remain,nfe);
	for(int i = 0 ; i < remain ; i++)
	    pull(groups , 1); //stage 1 doesn't erase Nodes
	/*end of 3*/

	delete[] temp_pop;

    }
}


int main(int argc , char **argv)
{
    Node *central = new Node[num_groups];
    Node *population = new Node[PopSize];

    MyRand rand;

    GROUP groups[num_groups];

    bool SwapFlag = true;
    int clusteringCategory[PopSize] = {0};
    int generation = 0;

    fun_num = atoi(argv[1]);

    for(int i = 0 ; i < num_groups ; i++)
	groups[i].length = dimension;

    /*initial pop */
    for(int i = 0 ; i < PopSize ; i++)
    {
	population[i].length = dimension;
	population[i].allele.setZero(dimension);
	for(int j = 0 ; j < dimension ; j++)
	    population[i].allele(j) = rand.uniform(lowerbound , upperbound);
	population[i].setFitness(groups[0].evaluate( population[i] , dimension) );    	
    }
    /*K-MEANS below*/
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


    /*    while(!shouldTerminate(generation ++))
	  {
	  pull(groups , 0);
	  updateGroups(groups);

	  }*/

    CMAES::a.rng.seed(time(NULL));	
    while(!shouldTerminate(generation++))
    {
	CMAES es(PopSize , 10 , dimension , population );
	es.run();
	Node best = es.generate();
	cout << "best is " << best.allele << endl <<endl << best.fitness <<endl;
	for(int i = 0 ; i < PopSize ; i++)
	    cout << "here" << population[i].allele << endl << endl ;
	if(generation == 1)
	    exit(0);
    }
    return 0;
}

