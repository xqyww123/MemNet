#pragma once
#include <vec.h>
using namespace std;

namespace Xero
{
	namespace MemNet
	{
		typedef Vec Key;
		typedef Vec Query;

		template <typename T>
		class Pair<T>
		{
			public:
				Key key;
				T data;
		};

		template <typename T>
		class Dic<T>
		{

			public:
				Pair<T> query(Key key);
		};
	}
}
