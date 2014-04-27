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
#define PopSize 800

double upperbound ;
double lowerbound ;

const int num_groups = 20;
int fe = PopSize;
int lambda;
int NewGroupCount = 0;
double sigma[num_groups];
double minFitness = 999999;
int Criteria[num_groups];
int min_GID = 999;

Eigen::MatrixXd cov[num_groups];


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

    int UCBTimer = 25;
    Node *temp_pop = new Node[ groups[minX].nodes.size() ];

    list< Node >::iterator iter = groups[minX].nodes.begin();
    for(i = 0 ; i < groups[minX].nodes.size() ; i++ , iter++)
	temp_pop[i] = *iter;


    //    CMAES es(groups[minX].nodes.size() , lambda , dimension , temp_pop , UCBTimer , sigma[minX] , cov[minX]);
    Node best = groups[minX].get_best_vector();	
    CMAES es(1 , lambda , dimension , &best , UCBTimer , sigma[minX] , cov[minX]);	
    es.run();
    
    Node temp_node(dimension);
    temp_node=es.generate();
    
    //  recording work
    if(temp_node.fitness < minFitness)
	minFitness = temp_node.fitness , min_GID = groups[minX].gID;
    if(best.fitness == temp_node.fitness)
    	Criteria[minX] ++;
    else 
	Criteria[minX] = 0;
    sigma[minX] = es.sigma;
    cov[minX] = es.covar;
    //    cout << minX << " : " << setprecision(13) << temp_node.fitness << endl;
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
    if(stage == 0)	
	groups[minX].push(temp_node , fe++);
    if(stage == 1)
	groups[minX].push(temp_node , fe);
    /*end of 3*/	
    delete[] temp_pop;
    if(es.isBest())
	return;
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
	if(Criteria[uni[i]] == 2)			
	{
	    candidate = uni[i];
	    break;
	}
    if(candidate == num_groups)
    	for(int i = 0 ; i < num_groups ; i++)
	    if(curFE - groups[uni[i]].LastModifiedFE > groups[uni[i]].nodes.size() ) 
	    {
		candidate = uni[i];
		break;
	    }
    delete[] uni;
    /*end of 1*/	
    cout << "candidate : " << candidate  << endl;	
    if(candidate != num_groups)
    {
	NewGroupCount++;
	groups[candidate].gID = NewGroupCount + num_groups;	
	/*2. generating new node according to mean vectors of  (num_groups - 1) groups */
	Node *temp_pop = new Node[ num_groups -1 ];
	int count = 0;
	for(int i = 0 ; i < num_groups  ; i++)
	{
	    if(i == candidate)
		continue;
	    temp_pop[count++] = groups[i].get_best_vector();
	    //printf("%2d : size %d\n" , i , groups[i].nodes.size());
	}
	CMAES es(num_groups -1, lambda , dimension , temp_pop , 10);
	es.run();
	Node temp_node = es.generate();
	if(temp_node.fitness < minFitness)
	    minFitness = temp_node.fitness , min_GID = groups[candidate].gID;
	if(es.isBest())
	    printf("%d generates %.6lf\n",groups[candidate].gID , temp_node.fitness);
	/*end of 2*/

	/*3. clear the candidate group and fill up remain nodes*/
	int remain = groups[candidate].nodes.size() -1;
	sigma[candidate] = 1.0;
	cov[candidate].setIdentity(dimension , dimension);
	groups[candidate].clear();
	groups[candidate].push(temp_node , fe++);
	//printf("from %d remain %d curNFE = %d\n" , candidate , remain,nfe);
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

    funATT = atoi(argv[1]);
    lambda = atoi(argv[2]);

    upperbound = domainupbound[funATT-1] , lowerbound = domainlowbound[funATT-1];
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


    CMAES::a.rng.seed(time(NULL));	

    //CMAES with UCB
    for(int i = 0 ; i < num_groups ; i++)
    {
	groups[i].gID = i;
	sigma[i] = 1.0;
	cov[i].setIdentity(dimension , dimension);
    	Criteria[i] = 0;
    }
    while(!shouldTerminate(generation ++))
    {
	pull(groups , 0);
	updateGroups(groups);
	printf("current best is %lf in %d \n " , minFitness , min_GID);
	if(abs(minFitness - best[funATT -1 ]) < 1e-6)
	{
	    cout << "nfe : " << nfe << endl;
	    break;
	}
    }

    int sort[num_groups];	
    for(int i = 0 ; i < num_groups ; i++)
	sort[i] = i;
    for(int i = 0 ; i < num_groups; i++)
    {

	for(int j = i+1 ; j < num_groups ; j++)
	    if(groups[sort[j]].get_best_vector().fitness < groups[sort[i]].get_best_vector().fitness)
	    {
		int t = sort[i];
		sort[i] = sort[j];
		sort[j] = t;
	    }
//	printf("%d : %d %lf\n" , i, groups[sort[i]].gID , groups[sort[i]].get_best_vector().fitness);
    }

    printf("%d in %d generations\n" , NewGroupCount , generation);

    exit(0);	
    /*
    //pure CMAES
    
       while(!shouldTerminate(generation++))
       {
    //    CMAES es(PopSize , atoi(argv[2]) , dimension , population , -1);
    CMAES es(1 , atoi(argv[2]) , dimension , &population[0] , -1);
    es.run();
    Node best = es.generate();

    printf("%.15lf , %d\n" , best.fitness , nfe);
    
    }
     */ 

    /*	
    //  double CMAES
    Node *candidate_pop = new Node[num_groups];
    for(int i = 0 ; i < num_groups ; i++)
    {
    Node *curPop = new Node[groups[i].nodes.size()];
    int j = 0;
    list<Node>::iterator iter = groups[i].nodes.begin();
    while(iter != groups[i].nodes.end())
    {
    curPop[j] = *iter;
    j++ , iter++;
    }
    CMAES es(groups[i].nodes.size() , atoi(argv[2]) , dimension , curPop , 50 );
    es.run();	
    candidate_pop[ i ] = es.generate();
    groups[i].replace_worst(candidate_pop[i]);
    delete[] curPop;
    }

    CMAES outeres(num_groups , atoi(argv[2]) , dimension , candidate_pop , -1);
    while(!shouldTerminate(generation++))
    {

    outeres.run();
    cout << "outergenerates : " << setprecision(6) <<outeres.generate().fitness << endl << endl;
    if(outeres.isBest())
    cout << "haha";
    getchar();
    }
    delete[] candidate_pop;*/
    return 0;
}

