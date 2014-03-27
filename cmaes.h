#include "Eigen/Dense"
#include "Eigen/Eigenvalues"
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/normal_distribution.hpp"    
#include "FHH/benchmark.h"
#include "FHH/test_func.h"
#include "myrand.h"
#include "global.h"
#include "node.h"

extern test_func * testFunc;

using namespace Eigen;
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
	int dimension;
	double mu_w;
	double cc , cs , c1 , cmu , ds;
	double *refsigma = &sigma;
	Node * population;
	Node mean;
	Node bestNode;
	Eigen::MatrixXd covar;
	Eigen::MatrixXd *refcovar = &covar;
	Eigen::VectorXd pc , ps , yw;

	static Eigen::internal::scalar_normal_dist_op<double> a;



	CMAES(int parent , int child , int dim , Node *refPopulation)
	{
	    mu = 0.5 * child;
	    lambda = child;
	    dimension = dim;
	    mu_w = 0.3 * lambda;
	    population = refPopulation;
	    cc = (double) 4.0 / dimension;
	    cs = (double) 4.0 / dimension;
	    c1 = (double) 2.0 / dimension / dimension;
	    cmu = double(mu_w) / dimension / dimension;
	    ds = 1 + sqrt(mu_w / dimension);	
	    mean.length = dimension;
	    mean.allele.setZero();
	    mean.allele = calculateMean(population , parent);
	    bestNode = findingbest();
	    testFunc = testFunctionFactory(funATT,dimension);
	    initial();
	    pc.setZero(dimension);
	    ps.setZero(dimension);
	    yw.setZero(dimension);
	    sigma = 1.0;
	    covar.setIdentity(dimension , dimension);
	}

	void run()
	{
	    bool shouldTerminate = false;
	    int generation = 0;
	    Node *offspring = new Node[lambda];
	    Eigen::VectorXd *y = new Eigen::VectorXd[mu];
	    while(!shouldTerminate)
	    {
		delete[] y;
		delete[] offspring;

		y = new Eigen::VectorXd[mu];
		offspring = new Node[lambda];

		Sample(offspring);

		sort_offspring(offspring , y);
		yw.setZero(dimension);
		for(int i = 0 ; i < mu ; i++)
		    yw = yw + y[i];
		yw = yw / mu;

		update_mean(offspring);

		update_pc();
		update_ps();
		update_covar(y);
		update_sigma();
		//cout << covar <<  endl << endl << "================ " << sigma << endl;
		//		printf("cc : %lf cs : %lf cmu : %lf c1 : %lf ds : %lf\n" , cc , cs, cmu , c1 , ds);
		bestNode = offspring[0];
		if(abs(offspring[0].fitness - best[funATT-1]) < 0.0000001)
		{
		    printf("%.9lf %d\n",offspring[0].fitness , nfe);
			shouldTerminate = true;
	 	 			
		}
	    }
	}
	/*void run()
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

	  sort_offspring(offspring , y);


	  update_mean(offspring);	

	  update_covar(y);

	  generation++;

	  for(int i = 0 ; i < mu ; i++)
	  population[i] = offspring[i];
	  bestNode = population[0];

	 *refsigma = sigma;
	 *refcovar = covar;

	 if(abs(bestNode.fitness - best[funATT-1]) < 0.00001)
	 {
	 cout << nfe << endl << bestNode.allele << endl << bestNode.fitness << endl ;
	 shouldTerminate = true;
	 exit(0);
	 }
	 int count = 0;
	 if(generation > count * 100)
	 {
	 cout << nfe << endl << bestNode.allele << endl << endl ;
	 printf("%.6lf\n" , bestNode.fitness);
	 if(nfe > 100000)
	 exit(0);
	 count ++;
	//	   shouldTerminate = true;
	}
	}
	delete[] y ;
	delete[] offspring;
	}
	*/

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
		//	Eigen::VectorXd tmp = mean.allele + sigma * sample;
		Eigen::VectorXd tmp = mean.allele + sigma * sample;
		if(isFeasible(tmp))
		{
		    offspring[count].length = dimension;
		    offspring[count].allele = tmp;
		    count++;
		}
		curcount++;
		if(curcount - count > 1000)
		{
		    exit(0);
		    for(int i = 0 ; i < lambda - count ; i++)
			offspring[i+count] = population[i];
		    break;

		}
	    }
	}	


	void sort_offspring(Node *offspring , Eigen::VectorXd *y)
	{
	    double *EvaluationResult = new double[lambda];
	    double meanEvalResult = evaluate(&mean);
	    double bestEvalResult = evaluate(&bestNode);	
	    float successfulCount = 0;
	    for(int i = 0 ; i < lambda ; i++)
	    {
		EvaluationResult[i] = evaluate(&offspring[i]);
		if(EvaluationResult[i] < meanEvalResult  )
		    successfulCount = successfulCount + 1;
	    }
	    for(int i = 0 ; i < mu ; i++)
	    {
		for(int j = i+1 ; j < lambda ; j++)	
		    if(offspring[i].fitness > offspring[j].fitness)
		    {
			Node tmp = offspring[i];
			offspring[i] = offspring[j];
			offspring[j] = tmp;
		    }
		y[i] = (offspring[i].allele-mean.allele) / sigma;
	    }
	    delete[] EvaluationResult;
	}

	void update_mean(Node *pop)
	{
	    mean.allele = calculateMean(pop , mu);
	    mean.setFitness(evaluate(&mean));	
	}

	void update_pc()
	{
	    int hsig = 0;
	    if(ps.norm() < 1.5*sqrt(dimension))
		hsig = 1;
	    pc = (1.0-cc)*pc + hsig * sqrt(cc*(2-cc)*mu_w) * yw;
	}

	void update_ps()
	{
	    Eigen::SelfAdjointEigenSolver<MatrixXd> es(covar);
	    Eigen::MatrixXd tmp = es.operatorInverseSqrt();
	    //cout << ps << endl << endl << tmp << endl << endl << yw << endl << endl;	
	    ps = (1.0-cs)*ps + sqrt(cs*(2-cs)*mu_w) * tmp * yw;

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
	    covar = (1-c1-cmu) * covar + c1*pc*pc.transpose()  + cmu * Cmu;
	}

	void update_sigma()
	{
	    double tmp = sqrt(dimension) * ( 1.0 - 1.0 / (4*dimension) + 1.0 / (21 * dimension * dimension) );
	    double val = exp ( cs / ds	* (ps.norm() / tmp - 1 ) );
	    sigma = sigma * val;
	}
	Node generate()
	{
	    if(isFeasible(bestNode.allele))
		return bestNode;
	    else
	    {
		cout << "not feasible "<<endl;
		exit(0);		
	    }
	}
	bool isFeasible(Eigen::VectorXd sample)
	{
	    for(int i = 0 ; i < dimension ; i++)
		if(sample(i) > solupbound[funATT-1] || sample(i) < sollowbound[funATT -1] || sample(i) != sample(i))
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
