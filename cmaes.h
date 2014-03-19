#include "Eigen/Dense"
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/normal_distribution.hpp"    
#include "FHH/benchmark.h"
#include "FHH/test_func.h"
#include "myrand.h"
#include "global.h"
#include "node.h"

extern test_func * testFunc;


namespace Eigen {
    namespace internal {
	template<typename Scalar> 
	    struct scalar_normal_dist_op 
	    {
		static boost::mt19937 rng;    // The uniform pseudo-random algorithm
		mutable boost::normal_distribution<Scalar> norm;  // The gaussian combinator

		EIGEN_EMPTY_STRUCT_CTOR(scalar_normal_dist_op)

		    template<typename Index>
		    inline const Scalar operator() (Index, Index = 0) const { return norm(rng); }
	    };

	template<typename Scalar> boost::mt19937 scalar_normal_dist_op<Scalar>::rng;

	template<typename Scalar>
	    struct functor_traits<scalar_normal_dist_op<Scalar> >
	    { enum { Cost = 50 * NumTraits<Scalar>::MulCost, PacketAccess = false, IsRepeatable = false }; };
    } // end namespace internal
} // end namespace Eigen


class CMAES
{
    public:
	int mu;
	int lambda;
	double sigma;
	double Ccov;
	int dimension;
	double mu_w;
	Node * population;
	Node mean;
	Node bestNode;
	Eigen::MatrixXd covar;
	static Eigen::internal::scalar_normal_dist_op<double> a;

	CMAES(int parent , int child , int dim , Node *refPopulation)
	{
	    mu = parent;
	    lambda = child;
	    dimension = dim;
	    mu_w = mu;
	    population = refPopulation;
	    sigma = 1.0;
	    Ccov = (double) mu_w / mu / mu;
	    mean.length = dimension;
	    mean.allele.setZero();
	    covar.setIdentity(dimension , dimension);
	    mean.allele = calculateMean(population , mu);
	    bestNode = findingbest();
	    testFunc = testFunctionFactory(funATT,dimension);
	    initial();
	}

	void run()
	{
	    bool shouldTerminate = false;
	    int generation = 0;
	    Eigen::VectorXd *y = new Eigen::VectorXd[mu];
	    Node *offspring = new Node[lambda+mu];

	    while(!shouldTerminate)
	    {
		delete[] y;
		delete[] offspring;

		y = new Eigen::VectorXd[mu];
		offspring = new Node[lambda+mu];

		Sample(offspring);
		for(int i = 0 ; i < mu ; i++)
		    offspring[lambda + i ] = population[i];

		sort_offspring_update_sigma(offspring , y);

		cout << " sigma : " << sigma << endl;

		update_mean(offspring);	

		update_covar(y);

		generation++;

		for(int i = 0 ; i < mu ; i++)
		    population[i] = offspring[i];
		bestNode = population[0];
		/*for(int i = 0 ; i < mu ; i++)
		  cout << population[i] << endl <<endl << endl;
		  cout << "mean : "<< mean << endl << endl;*/

		if(abs(bestNode.fitness - best[funATT-1]) < 0.00001)
		{
		    cout << nfe << endl << bestNode.allele << endl << bestNode.fitness << endl ;
		    shouldTerminate = true;
		    exit(0);
		}
		if(nfe % 1000 == 0)
		{
		    cout << nfe << endl << bestNode.allele << endl << bestNode.fitness << endl ;
		    fgetc(stdin);
		    //shouldTerminate = true;
		}
	    }
	    delete[] y ;
	    delete[] offspring;
	}


	double evaluate(Node *candidate)
	{
	    if(candidate->isEvaluated)
		return candidate->fitness;
	    double tmp[dimension];
	    for(int i = 0 ; i < dimension ; i++)
		tmp[i] = candidate->allele(i);
	    double result = testFunc->f(tmp , dimension);
	    candidate->setFitness(result);
	    nfe++;
	    return result;
	}


	void Sample(Node * offspring)
	{
	    Eigen::VectorXd zero;
	    zero.setZero(dimension);
	    int count = 0;
	    int curcount = 0;
	    while(count != lambda )
	    {
		Eigen::MatrixXd sample = getMVN(zero , covar);
		//		Eigen::VectorXd tmp = mean.allele + sigma * sample;
		Eigen::VectorXd tmp = bestNode.allele + sigma * sample;
		if(isFeasible(tmp))
		{
		    offspring[count].length = dimension;
		    offspring[count].allele = tmp;
		    count++;
		}
		curcount++;
		if(curcount - count > 1000)
		{
		    for(int i = 0 ; i < lambda - count ; i++)
			offspring[i+count] = population[i];
		    break;

		}
	    }
	}	


	void sort_offspring_update_sigma(Node *offspring , Eigen::VectorXd *y)
	{
	    double *EvaluationResult = new double[lambda+mu];
	    double meanEvalResult = evaluate(&mean);
	    double bestEvalResult = evaluate(&bestNode);	
	    float successfulCount = 0;
	    for(int i = 0 ; i < lambda + mu ; i++)
	    {
		EvaluationResult[i] = evaluate(&offspring[i]);
		if(EvaluationResult[i] < bestEvalResult  && i <lambda)
		    successfulCount = successfulCount + 1;
	    }
	    for(int i = 0 ; i < mu ; i++)
	    {
		for(int j = i+1 ; j < lambda+mu ; j++)	
		    if(offspring[i].fitness > offspring[j].fitness)
		    {
			Node tmp = offspring[i];
			offspring[i] = offspring[j];
			offspring[j] = tmp;
		    }
		y[i] = (offspring[i].allele-bestNode.allele) / sigma;
	    }
	    sigma = sigma*exp( (1.0/3) * (successfulCount/lambda-1.0/5) / 0.8 );
	    delete[] EvaluationResult;
	}

	void update_mean(Node *pop)
	{
	    mean.allele = calculateMean(pop , mu);
	    mean.setFitness(evaluate(&mean));	
	}


	void update_covar(Eigen::VectorXd * pop)
	{
	    Eigen::MatrixXd Cmu;
	    Cmu.setZero(dimension , dimension);
	    Eigen::VectorXd tmp;
	    for(int i = 0 ; i < mu ; i++)
	    {
		tmp = pop[i];
		Cmu += (tmp*tmp.transpose()) ;
	    }
	    Cmu /= mu;
	    covar = (1-Ccov) * covar + Ccov * Cmu;
	}
	Node generate()
	{
	    if(isFeasible(population[0].allele))
		return population[0];
	    else
	    {
		cout << "not feasible "<<endl;
		exit(0);		
	    }
	}
	bool isFeasible(Eigen::VectorXd sample)
	{
	    for(int i = 0 ; i < dimension ; i++)
		if(sample(i) > 100 || sample(i) < -100 || sample(i) != sample(i))
		    return false;
	    return true;
	}

	Eigen::VectorXd calculateMean(Node * pop , int size)
	{
	    Node tmp(dimension);
	    for(int i = 0 ; i < size; i++)
		tmp.allele = tmp.allele + pop[i].allele;
	    tmp.allele = tmp.allele / size;
	    return tmp.allele;
	}

	Node findingbest()
	{
	    Node tmp(dimension);
	    int minx = 0;
	    int min = evaluate(&population[0]);
	    for(int i = 0 ; i < mu ; i++)
		if(evaluate(&population[i]) < min)
		{
		    min = evaluate(&population[i]);
		    minx = i;
		}
	    tmp = population[minx];
	    return tmp;
	}

	Eigen::MatrixXd getMVN(Eigen::VectorXd mean , Eigen::MatrixXd covar) 
	{
	    int size = dimension; // Dimensionality (rows)
	    int nn=1;     // How many samples (columns) to draw
	    Eigen::internal::scalar_normal_dist_op<double> randN; // Gaussian functor
	    //Eigen::internal::scalar_normal_dist_op<double>::rng.seed(ranseed); // Seed the rng
	    Eigen::MatrixXd normTransform(size,size);

	    Eigen::LLT<Eigen::MatrixXd> cholSolver(covar);

	    if (cholSolver.info()==Eigen::Success) 
	    {
		normTransform = cholSolver.matrixL();
	    }
	    else
	    {
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
		normTransform = eigenSolver.eigenvectors() 
		    * eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
	    }

	    Eigen::MatrixXd samples = (normTransform 
		    * Eigen::MatrixXd::NullaryExpr(size,nn,randN)).colwise() 
		+ mean;

	    return samples;
	}
};
