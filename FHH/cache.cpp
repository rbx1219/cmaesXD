#include <iostream>
#include <cmath>
#include <cstdlib>

#include "cache.hpp"
#include "subset.hpp"
#include "parameter.hpp"
using namespace std;

int const EMPTY = -1;

//
// initializes a cache of subsets.
//
cache::cache()
{
	int ell = parameter::lchrom;
	maxSz = (ell * (ell-1)) / 2;
	mergedSet = new subset [ maxSz ];
	subset1_id = new int [ maxSz ];
	subset2_id = new int [ maxSz ];
	sz = 0;
	E = new int [ 2*ell ];
	Esize = 0;
}

//
// destructor
//
cache::~cache()
{
	delete [] mergedSet;
	delete [] subset1_id;
	delete [] subset2_id;
	delete [] E;
}

//
// store a subset and its complexity in the cache
//
void cache::insert( int id1, int id2, subset &S, int popsize )
{
//	assert( S.numCounts() <= popsize );
	assert( sz>=0 && sz<maxSz );
	int p;  // the position where S will be inserted
	if( Esize == 0 )  // there are no empty entries in the cache
		p = sz;
	else // pick an empty slot.
	{
		p = E[ Esize-1 ];
		Esize--;
	}
	//
	// now insert S
	//
	mergedSet[p] = S;
	subset1_id[p] = id1;
	subset2_id[p] = id2;
	sz++;
}

//
// remove all cache entries that have value 'id' in one of its IDs
// I don't actually remove the entries. Just marked them EMPTY;
//
void cache::removeEntry( int id )
{
	subset emptySet;
	for( int i=0; i< maxSz; i++ )
		if( subset1_id[i] == id || subset2_id[i] == id )
		{
			mergedSet[i] = emptySet;
			subset1_id[i]= EMPTY;
			subset2_id[i]= EMPTY;
			assert( Esize < 2*parameter::lchrom );
			E[ Esize ] = i;
			Esize++;
			sz--;
		}
}

//
// replace all subset_ids that have 'x' by 'y'.
//
void cache::replace_X_by_Y( int x, int y )
{
	for( int i=0; i< maxSz; i++ )
	{
		if( subset1_id[i] == x ) subset1_id[i] = y;
		if( subset2_id[i] == x ) subset2_id[i] = y;
	}
}

//
// compact the cache so that it doesn't have holes.
//
void cache::compact()
{
	int newMaxSz = maxSz - Esize;
	int p = maxSz-1;
	for( int i=0; i< Esize; i++ )
	{
		int e = E[i];
		if( e >= newMaxSz )
			// do nothing
			;
		else 
		{
			//
			// find a non-empty slot
			//
			while( subset1_id[p] == EMPTY )
				p--;
			//
			// make element 'e' of the cache become element 'p'
			//
			mergedSet[e] = mergedSet[p];
			subset1_id[e] = subset1_id[p];
			subset2_id[e] = subset2_id[p];
			p--;
		}
	}
	Esize = 0;
	maxSz= newMaxSz;
}

//
// print the cache
//
ostream &operator<< (ostream &out, cache &C)
{
	out << "sz: " << C.sz << "  maxSz: " << C.maxSz << endl;
	out << "# Empty slots: " << C.Esize << endl;
	out << "Empty slots: ";
	for( int i=0; i< C.Esize; i++ )
		out << C.E[i] << " ";
	out << endl;
	for( int i=0; i< C.maxSz; i++ ) 
		out << "(" << C.subset1_id[i] << ")"
			<< "(" << C.subset2_id[i] << ")"
			<< " " <<C.mergedSet[i] << endl;
	return out;
}

