#ifndef GROUP_H
#define GROUP_H

#include <iostream>
#include <list>
#include <utility>
#include "Eigen/Dense"
#include "node.h"

using namespace std;



class GROUP
{
	private:
		Node mean;
		double sum;		
		double sum2;
		double min;
		double getMean();
		double getVariance();
	public:
		int gID;
		int length;
		list<Node> nodes;
		double max;
		int AllUsedNumber;
		int LastModifiedFE;
		double sigma;
		Eigen::MatrixXd covar;
		
		double evaluate(Node n , int len);		
		void clear();	
		void update_mean();
		double calculateUCB(int total);
		double getmax();
		double getmin();
		void replace_worst(Node a , int time);


		Node get_mean_vector()
		{
			update_mean();
			return mean;
		}
		Node get_best_vector();

		GROUP()
		{
			AllUsedNumber = 0 ;
			sum = 0.0;
			sum2 = 0.0;
			max = 0.0;
			LastModifiedFE = 0;
			sigma = 1;
		}
		GROUP(int dim)
		{
			length = dim;
			AllUsedNumber = 0;
			sum = 0.0;
			sum2 = 0.0;
			max = 0.0;
			sigma = 1;
			covar.setIdentity(length , length);
		}
		
		int getAllUsedNumber()
		{
			return AllUsedNumber;
		}
		void push(Node n , int curFE);
};


#endif
