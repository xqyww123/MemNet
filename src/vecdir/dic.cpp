#include "dic.h"

using namespace std;

namespace Xero
{
	namespace MemNet
	{
		template<typename T>
		double Dic<T>::score(Frates* a, Frates* b)
		{
			double re = 0;
			if (a->size() > b->size())
			{Frates* c=a; a = b; b = c; }
			for (auto x : *a)
			{
				if (b->find(x.first) != b->end())
					re += x.second * (*b)[x.first]; 
			}
			return re;
		}
		template<typename T>
		double Dic<T>::score(Frates* a, Vec* b)
		{
			double re = 0;
			for (int i=0;i<b->spread.size();++i)
			{
				if (a->find(b->spread[i]) != a->end())
					re += b->freq[i] * (*a)[b->spread[i]]; 
			}
			return re;
		}

		template<typename T>
		typename Dic<T>::Result* Dic<T>::query(Query* query)
		{
			Result* heap = new Result();
			for (auto a : data)
			{
				heap.push(ResultTip<T>(&a, Dic<T>::score(query, &a.key)));
			}
			return heap;
		}

		template<typename T>
		Pair<T>* ResultTip<T>::val()
		{
			return this->first;
		}
		template<typename T>
		double ResultTip<T>::score() { return this->second; }

		template<typename T>
		void Dic<T>::insert(Key& key, T& v)
		{
			data.insert(Pair<T>(key, v));
		}
	}
}
