#include "group.h"
#include "FHH/benchmark.h"
#include "FHH/test_func.h"

test_func *testFunc; 

double ucbtuned1();

void GROUP::update_mean()
{
    Node tmp(length);
    for(list<Node>::iterator i = nodes.begin() ; i != nodes.end() ; ++i)
	tmp.allele = tmp.allele + (*i).allele;
    //    cout << tmp << endl <<" / " << nodes.size() << endl;
    mean.allele = tmp.allele / nodes.size();
    mean.fitness = evaluate(mean , length);
    mean.isEvaluated = true;
}

double GROUP::calculateUCB(int total)
{
    //ucb_tuned1
    double v = getVariance() + sqrt(2*log(total) / AllUsedNumber);
    double V = (v>0.25) ? 0.25 : v;
    double max = getmax() , min = getmin();
    double newMean;
    if(max == min)
	newMean = 1.0;
    else
	newMean = (getMean() - min ) / (max - min);
    double tmp = newMean - sqrt(log(total) / AllUsedNumber *V);
    return tmp;
}

void GROUP::push(Node n , int curFE)
{
    Node tmp(length);
    LastModifiedFE = curFE;
    tmp.allele = n.allele;
    tmp.fitness = n.fitness;
    tmp.isEvaluated = true;
    nodes.push_back(tmp);

    sum += n.fitness;
    sum2+= n.fitness * n.fitness;

    AllUsedNumber ++ ;
    update_mean();
    max = getmax();
    return ;
}


void GROUP::clear()
{
    list<Node>::iterator iter = nodes.begin();
    while( iter!=nodes.end()  )
	nodes.erase(iter++);
    sum = 0.0;
    sum2 = 0.0;
    AllUsedNumber = 0;
    max = 0.0;
}
double GROUP::evaluate(Node n , int len)
{
    initial();
    extern int funATT;
    testFunc = testFunctionFactory( funATT , len);
    double vec[len];
    for(int i = 0 ; i < len ; i++)
	vec[i] = n.allele(i);
    return testFunc->f(vec , len);
}

double GROUP::getMean()
{
    return sum/AllUsedNumber;
}

double GROUP::getVariance()
{
    double meanValue = getMean();
    return sum2 / AllUsedNumber  - meanValue * meanValue;
}

double GROUP::getmax()
{
    double tmpMax = -999999;
    list<Node>::iterator iter;
    for(iter = nodes.begin() ; iter != nodes.end() ; ++iter)
	if( iter->fitness > tmpMax)
	    tmpMax = iter->fitness;
    return tmpMax;
}

double GROUP::getmin()
{
    double tmpMin = 9999999;
    list<Node>::iterator iter;
    for(iter = nodes.begin() ; iter != nodes.end() ; ++iter)
	if( iter->fitness < tmpMin)
	    tmpMin = iter->fitness;
    return tmpMin;

}

Node GROUP::get_best_vector()
{
	double max = nodes.begin()->fitness;
	list<Node>::iterator iter = nodes.begin();
	Node best(length) ;
	 best = *iter;
	for( ; iter != nodes.end() ; ++iter)
	{
		if(iter->fitness < max)
		{
			max = iter->fitness;
			best = *iter;
		}
	}
	best.fitness = max;
	best.isEvaluated = true;
	return best ;
}

void GROUP::replace_worst(Node candidate , int time)
{
	double max = getmax();
	list<Node>::iterator iter = nodes.begin();
	for( ; iter!= nodes.end() ; ++iter)
		if(iter->fitness = max)
		{

		    sum -= max;
		    sum2 -= max*max;
		    *iter = candidate;
		    double t = candidate.fitness;
		    sum += t;
		    sum2 += t*t;	
		    LastModifiedFE = time;	
		    break;	
		}

}
