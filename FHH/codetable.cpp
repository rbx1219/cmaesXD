#include"codetable.h"
#include<cmath>
using namespace std;
code_table::code_table(){ T.clear(); }
code_table::~code_table(){ T.clear(); }
int code_table::add(double max,double min)
{
	code_type tmp;
	tmp.max=max,tmp.min=min;
	T.push_back(tmp);
	return T.size()-1;
}
code_table & code_table::operator=( code_table & CT)
{
	if(this == &CT)
		return *this;

	T.clear();
	for(int i=0;i<CT.T.size();++i)
		T.push_back(CT.T[i]);

	return *this;
}
