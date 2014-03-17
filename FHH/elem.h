#ifndef _elem_h
#define _elem_h
class elem{
public:
	double val;
	int num;

	friend int elem_comp(const void *a,const void *b)
	{
		double tmp= ((elem *)a)->val - ((elem *)b)->val;
		if(tmp==0.0)return 0;
		else if(tmp>0.0)return 1;
		else return -1;
	}
};
#endif
