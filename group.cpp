#include "group.h"
#include "global.h"
#include "FHH/benchmark.h"
#include "FHH/test_func.h"

test_func *testFunc; 

double ucbtuned1();

void GROUP::update_mean()
{
    Node tmp;
    tmp.setZero(length);
    for(list<OneNode>::iterator i = nodes.begin() ; i != nodes.end() ; ++i)
	tmp = tmp + (*i).first;
    //    cout << tmp << endl <<" / " << nodes.size() << endl;
    mean = tmp / nodes.size();
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
    if(tmp != tmp)
    {
	printf("%lf / %d is the mean and %lf \n" , sum , AllUsedNumber , tmp);
    	printf("%lf %lf %d %lf %lf \n" , getVariance() , getmin() , total , getmax() , v);
    }
    return tmp;
}

void GROUP::push(Node n , int curFE)
{
    OneNode tmp;
    double nFit = evaluate(n , length);
    LastModifiedFE = curFE;
    tmp.first = n;
    tmp.second = nFit;
    nodes.push_back(tmp);

    sum += nFit;
    if(sum != sum)
    {
	list<OneNode>::iterator iter = nodes.begin();
	while(iter!= nodes.end())
	    cout<< iter++->first << endl << endl;
	
	printf("what happend\n");
	printf("nFit = %lf \n" , nFit);
	exit(0);
    }
    sum2+= nFit * nFit;

    AllUsedNumber ++ ;
    update_mean();
    max = getmax();
    return ;
}


void GROUP::clear()
{
    list< pair<Node , double> >::iterator iter = nodes.begin();
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
    testFunc = testFunctionFactory( fun_num , len);
    double vec[len];
    for(int i = 0 ; i < len ; i++)
	vec[i] = n(i);
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
    list<OneNode>::iterator iter;
    for(iter = nodes.begin() ; iter != nodes.end() ; ++iter)
	if( (*iter).second > tmpMax)
	    tmpMax = (*iter).second;
    return tmpMax;
}

double GROUP::getmin()
{
    double tmpMin = 9999999;
    list<OneNode>::iterator iter;
    for(iter = nodes.begin() ; iter != nodes.end() ; ++iter)
	if( iter->second < tmpMin)
	    tmpMin = iter->second;
    return tmpMin;

}
