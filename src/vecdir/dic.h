#pragma once
#include "vec.h"
#include <vector>
#include <queue>
#include <utility>
using namespace std;

namespace Xero
{
	namespace MemNet
	{
		typedef Vec Key;
		typedef Frates Query;

		template <typename T>
		class Pair
		{
			public:
				Pair() {}
				Pair(Key& k, T& v) : key(k), data(v) {}
				Key key;
				T data;
		};
		template <typename T>
		class ResultTip : public pair<Pair<T>, double>
		{
			public:
				ResultTip() {}
				ResultTip(Pair<T>* v, double s) : pair<Pair<T>, double>(v,s) {}
				Pair<T>* val();
				double score();
		};

		template <typename T>
		class Dic
		{
				vector<Pair<T>> data;
			public:
				typedef priority_queue<ResultTip<T> >  Result;
				Result* query(Query* query);
				static double score(Frates* a, Frates* b);
				static double score(Frates* a, Vec* b);
				void insert(Key& key, T& data);
		};
	}
}
