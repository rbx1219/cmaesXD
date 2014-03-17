#include"F26_8_peaks1.h"
#include"benchmark.h"

F26_8_peaks1::F26_8_peaks1(int dimension, double bias)
{
	double problem[8] = {1.2, 1.4, 1.6, 1.8, 1.7, 1.5, 1.3, 1.1};
	a= new double [8];
	for(int i=0; i<8; i++){
		a[i]=problem[i];
	}
	//m_dimension = dimension;
	//m_bias = bias;
	//m_func_name = FUNCTION_NAME1;
	
	// Note: dimension starts from 0
	//m_o = new double[m_dimension];
	//m_z = new double[m_dimension];

	// Load the shifted global optimum
	//loadRowVectorFromFile(DEFAULT_FILE_DATA1, m_dimension, m_o);
}


/*F01_shifted_sphere::F01_shifted_sphere(int dimension, double bias, string file_data)
{
	m_dimension = dimension;
        m_bias = bias;
        m_func_name = FUNCTION_NAME1;

	// Note: dimension starts from 0
	m_o = new double[m_dimension];
	m_z = new double[m_dimension];

	// Load the shifted global optimum
	loadRowVectorFromFile(file_data, m_dimension, m_o);
}*/

double F26_8_peaks1::f(double *x,int length)
{
	double f=0;
	double sum =0;
	for(int i=0; i<length; i++){
		f=0;
		for(int j=0; j<8; j++){
		f += (x[i]-j*10)*(x[i]-(j+1)*10)*interval_f(x[i], j*10, (j+1)*10)*a[j];
		}
		sum +=f;
	}

	return sum;

}
