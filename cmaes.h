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
	int terminate_generation;
	int dimension;
	double mu_w;
	double *weight;
	double cc , cs , c1 , cmu , ds;
	Node * population;
	Node mean;
	Node bestNode;
	Eigen::MatrixXd covar;
	Eigen::VectorXd pc , ps , yw;

	static Eigen::internal::scalar_normal_dist_op<double> a;



	CMAES(int parent , int child , int dim , Node *refPopulation , int terGen , double refsigma , Eigen::MatrixXd refcovar)
	{
	    sigma = refsigma;
	    covar = refcovar;
	    init(parent , child , dim , refPopulation , terGen);	
	}
	CMAES(int parent , int child , int dim , Node *refPopulation , int terGen)
	{
	    sigma = 1.0;
	    covar.setIdentity(dim , dim);
	    init(parent , child , dim , refPopulation , terGen);	
	}
	void init(int parent , int child , int dim , Node * refPopulation , int terGen)
	{
	    mu = 0.5 * child;
	    lambda = child;
	    dimension = dim;
	    mu_w = 0.0;		
	    weight = new double[mu];
	    double count = 0.0;
	    for(int i = 0 ; i < mu ; i++)
	    {
		weight[i] = log(mu+0.5) - log(i+1);
	    	count += weight[i];
	    }
	    for(int i = 0 ; i < mu ; i++)
	    {
		weight[i] /= count;
		mu_w +=  ( weight[i] * weight[i]) ;
	    }
	    mu_w = 1 / mu_w;
	    population = new Node[parent];
	    population = refPopulation;
	    cc = (double) 4.0 / dimension;
	    cs = (double) 4.0 / dimension;
	    c1 = (double) 2.0 / dimension / dimension;
	    cmu = double(mu_w) / dimension / dimension;
	    ds = 1 + sqrt(mu_w / dimension);	
	    mean.length = dimension;
	    mean.allele.setZero();
	    mean.allele = calculateMean(population , parent);
	    //bestNode = findingbest();
	    bestNode.length = dimension;
	    bestNode.allele.setZero(dimension);
	    bestNode= refPopulation[0];
	    testFunc = testFunctionFactory(funATT,dimension);
	    initial();
	    pc.setZero(dimension);
	    ps.setZero(dimension);
	    yw.setZero(dimension);
	    if(terGen == -1)
		terminate_generation = 100000 / lambda ;
	    else

		terminate_generation = terGen;	
	
	}
	~CMAES()
	{
		delete[] weight;
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
		    yw = yw + y[i] * weight[i];

		update_mean(offspring);

		update_pc();
		update_ps();
		update_covar(y);
		update_sigma();
		//cout << covar <<  endl << endl << "================ " << sigma << endl;
		//		printf("cc : %lf cs : %lf cmu : %lf c1 : %lf ds : %lf\n" , cc , cs, cmu , c1 , ds);
		bestNode = offspring[0];
		generation ++ ;
		if( generation >= terminate_generation)
		{
			//printf("%.9lf %d\n",offspring[0].fitness , nfe);
			//cout << bestNode.allele << endl << endl;
		    shouldTerminate = true;
		}
	    }
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
		    /*   for(int i = 0 ; i < lambda - count ; i++)
			 {
			 cout <<"graduate!!!" <<  population[i].allele<<endl;
			 offspring[i+count].length = population[i].length;
			 offspring[i+count].allele = population[i].allele;
			 }
			 break;*/
		    sigma /= 2;
		    curcount = count ;
			if(sigma != sigma)
			    sigma = 1.0;
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
		cout << bestNode.allele << endl;
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

	bool isBest()
	{
	    if(abs(bestNode.fitness - best[funATT-1]) < accuracy[funATT-1])
		return true;
	    return false;
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
	    for(int i = 1 ; i < mu ; i++)
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
