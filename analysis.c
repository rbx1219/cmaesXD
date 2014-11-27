#include<stdio.h>
#include<string.h>
#include<math.h>

int main()
{
    FILE * fin;
    FILE *fout  = fopen("BestLog","w+");
    FILE *fout2 = fopen("MedianLog","w+");
    FILE *fout3 = fopen("WorstLog","w+");
    FILE *fout4 = fopen("MeanLog","w+");
    FILE *fout5 = fopen("VarianceLog","w+");
    for(int i = 0 ; i < 25 ; i++)
    {
	double error1000[25];
	double error[25];
	double error10000[25];
	char input[10];
	double mean = 0.0;
	double sum2 = 0.0;
	sprintf(input , "%d.log" , i+1);
	fin = fopen(input , "r");
	for(int j = 0 ; j < 25; j++)
	{
	    //fscanf(fin,"%lf %lf %lf", &error1000[j] , &error10000[j],&error[j]);
	    	    fscanf(fin,"%lf",&error[j]);
	    mean += error[j];
	    sum2 += error[j]*error[j];	
	}
	mean /= 25;
	sum2 = sum2 / 25 - mean * mean;
	sum2 = sqrt(sum2);
	for(int j = 0 ; j < 25 ; j++)
	    for(int k = j+1 ; k < 25 ; k++)
		if(error[k] < error[j])
		{
		    double dtmp = error[j];
		    error[j] = error[k];
		    error[k] = dtmp;
		}
	for(int j = 0 ; j < 25 ; j++)
	    for(int k = j+1 ; k < 25 ; k++)
		if(error1000[k] < error1000[j])
		{
		    double dtmp = error1000[j];
		    error1000[j] = error1000[k];
		    error1000[k] = dtmp;
		}
	for(int j = 0 ; j < 25 ; j++)
	    for(int k = j+1 ; k < 25 ; k++)
		if(error10000[k] < error10000[j])
		{
		    double dtmp = error10000[j];
		    error10000[j] = error10000[k];
		    error10000[k] = dtmp;
		}
	printf("problem %2d : " , i+1);
	//    	printf("%e %e %e\n" , error1000[0],error10000[0],error[0]);
	fprintf(fout,"%e\n",error[0]);
	//	printf(" 7th    %e \n" , error[6]);
	//	printf("%e %e %e\n" ,  error1000[12],error10000[12],error[12]);
	fprintf(fout2,"%e\n",error[12]);
	//	printf(" 19th   %e \n" , error[18]);
	printf(" worst  %e \n" , error[24]);
	fprintf(fout3,"%e\n",error[24]);
	printf(" mean   %e \n" , mean);
	fprintf(fout4,"%e \n",mean );
	fprintf(fout5,"%e\n",sum2);
	//	printf("\n");
    }
    fclose(fin);
    fclose(fout);
    fclose(fout2);
    fclose(fout3);
    fclose(fout4);
    fclose(fout5);
    return 0;
}
