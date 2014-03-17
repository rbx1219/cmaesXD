#ifndef _codetable_h
#define _codetable_h
#include<vector>
using namespace std;
class code_type{
public:
	double max,min;
};
class code_table{
public:
	vector<code_type> T;
	code_table();
	~code_table();
	int add(double max,double min);
	int size(){ return T.size(); };
	code_table & operator=( code_table & CT);
};
#endif
