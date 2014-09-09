#pragma once
#include "dic.h"
#include <cmath>
#include <cstring>

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
					re += sqrt(x.second * (*b)[x.first]); 
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
					re +=sqrt(b->freq[i] * (*a)[b->spread[i]]); 
			}
			return re;
		}

		template<typename T>
		typename Dic<T>::Result* Dic<T>::query(Query* query)
		{
			Result* heap = new Result();
			for (auto a : data)
			{
				heap->push(ResultTip<T>(a, Dic<T>::score(query, &a->key)));
			}
			return heap;
		}

		template<typename T>
		Pair<T>* ResultTip<T>::val()
		{
			return this->second;
		}
		template<typename T>
		double ResultTip<T>::score() { return this->first; }

		template<typename T>
		void Dic<T>::insert(Key& key, T& v)
		{
			data.push_back(new Pair<T>(key, v));
		}
		template<typename T>
		void Dic<T>::insert(WVec& wv, T& data)
		{
			Vec* vec = Vec::cal_spread(&wv);
			insert(*vec, data);
			delete vec;
		}
		template<typename T>
		typename Dic<T>::Result* Dic<T>::query(WVec& query)
		{
			Frates* spr = Vec::cal_rate(&query);
			Result* re = this->query((Query*)spr);
			delete spr;
			return re;
		}


		/**** write region *****/
#define wrt(x) write((char*)&x,sizeof(x))
#define wr2(a,b) write((char*)a,sizeof(b))
#define rd(x) read((char*)&x, (streamsize)sizeof(x))
#define rd2(x,b) read((char*)x, sizeof(b))
		template<typename T>
		void wrt_pr(Pair<T>* pr, fstream& f, typename Dic<T>::DataSerialize ser)
		{
			pr->key.write(f);
			ser(pr->data, f);
		}

		template<typename T>
		void Dic<T>::write(fstream& f, typename Dic<T>::DataSerialize ser)
		{
			f.write("MemDic", sizeof(char[7]));
			auto tmpsz = data.size();
			f.wrt(tmpsz);
			for (auto a : data)
			{
				wrt_pr(a,f,ser);
			}
		}
		template<typename T>
		Pair<T>* rd_pr(fstream& f, typename Dic<T>::DataDeserialize deser)
		{
			auto* re = new Pair<T>();
			re->key = *Key::read(f);
			re->data = deser(f);
			return re;
		}
		template<typename T>
		Dic<T>* Dic<T>::read(fstream& f, typename Dic<T>::DataDeserialize deser)
		{
			char buf[10];
			f.rd2(buf, char[7]);
			if (strcmp(buf,"MemDic")) throw runtime_error("There is no MemDic in the file = =.");
			Dic* re = new Dic();
			vector<string>::size_type sz; f.rd(sz);
			re->data.reserve(sz);
			for (vector<string>::size_type i=0;i<sz;++i)
			{
				re->data.push_back(rd_pr<T>(f, (typename Dic<T>::DataDeserialize)deser));
			}
			return re;
		}
		/**** write region end */
	}
}
