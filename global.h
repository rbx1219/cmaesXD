#ifndef GLOBAL_H
#define GLOBAL_H


int nfe = 0;
int funATT;
double best[25] = {
          -450.0 
        , -450.0 
	, -450.0 
	, -450.0
	, -310.0
	,  390.0
	, -180.0
	, -140
	, -330
	, -330
};

double domainupbound[25] = {
		 100
		,100
		,100
		,100
		,100
		,100
		,600
		,32
		,5
		,5
};

double domainlowbound[25] = {
    		-100
		,-100
		,-100
		,-100
		,-100
		,-100
		,0
		,-32
		,-5
		,-5
};


double solupbound[25] = {
		 100
		,100
		,100
		,100
		,100
		,100
		,600
		,32
		,5
		,5
};

double sollowbound[25] = {
    		-100
		,-100
		,-100
		,-100
		,-100
		,-100
		,-600
		,-32
		,-5
		,-5
};
#endif
