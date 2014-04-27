#include<stdio.h>
#include<string.h>


int main()
{
	FILE * fin;
    for(int i = 0 ; i < 25 ; i++)
    {
	int nfe[25];
	double error[25];
	char input[10];
	double mean = 0.0;
	sprintf(input , "%d.log" , i+1);
	fin = fopen(input , "r");
	
	for(int j = 0 ; j < 25; j++)
	{
	    fscanf(fin,"%d %lf",&nfe[j] , &error[j]);
	    mean += error[j];
	}
	mean /= 25;
	for(int j = 0 ; j < 25 ; j++)
	    for(int k = j+1 ; k < 25 ; k++)
		if(error[k] < error[j])
		{
			int tmp = nfe[j];
			nfe[j] = nfe[k];
			nfe[k] = tmp;

			double dtmp = error[j];
			error[j] = error[k];
			error[k] = dtmp;
		}
	printf("problem %d : \n" , i+1);
    	printf(" best   %e \n" , error[0]);
	printf(" 7th    %e \n" , error[6]);
	printf(" median %e \n" , error[12]);
	printf(" 19th   %e \n" , error[18]);
	printf(" worst  %e \n" , error[24]);
	printf(" mean   %e \n" , mean);
	printf("\n");
    }
    fclose(fin);
	return 0;
}
