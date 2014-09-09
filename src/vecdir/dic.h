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
		class ResultTip : public pair<double, Pair<T>*>
		{
			public:
				ResultTip() {}
				ResultTip(Pair<T>* v, double s) : pair<double, Pair<T>*>(s,v) {}
				Pair<T>* val();
				double score();
		};

		template <typename T>
		class Dic
		{
			public:
				vector<Pair<T>*> data;
				typedef priority_queue<ResultTip<T> >  Result;
				Result* query(Query* query);
				Result* query(WVec& query);
				static double score(Frates* a, Frates* b);
				static double score(Frates* a, Vec* b);
				void insert(Key& key, T& data);
				void insert(WVec& vec, T& data);
				void remove(Pair<T>* pair) { data.erase(find(data.begin(), data.end(), pair)); }

				typedef function<void(T&, fstream&)> DataSerialize;
				void write(fstream& f, DataSerialize ser);
				typedef function<T(fstream& f)> DataDeserialize;
				static Dic* read(fstream& f, DataDeserialize deser);
		};
	}
}

#include "dic.cpp"
