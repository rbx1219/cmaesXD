#ifndef _cache_hpp
#define _cache_hpp

#include <iostream>
#include "subset.hpp"
#include "mpm.hpp"
using namespace std;

class cache
{
	private:
		int maxSz;        // maximum size of the cache ( ell*(ell+1)/2)
		int sz;           // number of subsets stored in the cache
		int *E;           // a collection of empty cache entries
		int Esize;        // number of elements of E

	public:

		subset *mergedSet;   // an array of subsets
		int *subset1_id;     // mergedSet = subset1 UNION subset2 
		int *subset2_id;     // ids refer to MPM subsets

		cache();
		~cache();
		long size() { return sz; }
		void insert( int id1, int id2, subset &S, int popsize );
		void removeEntry( int id );
		void replace_X_by_Y( int x, int y );
		void compact();
		friend ostream &operator<< (ostream &out, cache &C);
};


#endif
