#ifndef _F27_8_peaks2
#define _F27_8_peaks2
#include"test_func.h"
class F27_8_peaks2:public test_func{
	private:
		// Shifted global optimum
		//double *m_o;

		// In order to avoid excessive memory allocation,
		// a fixed memory buffer is allocated for each function object.
		//double *m_z;

		double *a;

	public:

		// Class variables
		//double *m_iSqrt;

		// Instance variables
		//double *m_biases;

		// Constructors
		F27_8_peaks2 (int dimension, double bias);
		//F01_shifted_sphere (int dimension, double bias, string file_data);

		// Function body
		double f(double *x,int length);
};

//const string FUNCTION_NAME1="Shifted Sphere Function";
//const string DEFAULT_FILE_DATA1="supportData/sphere_func_data.txt";
#endif