#include "file.h"
#include <cstring>

using namespace std;

namespace Xero {
	namespace MemNet {

#define wrt(x) write((char*)&x,sizeof(x))
#define wr2(a,b) write((char*)a,sizeof(b))
#define rd(x) read((char*)&x, (streamsize)sizeof(x))
#define rd2(x,b) read((char*)x, sizeof(b))
		void write_string(const string& str, fstream& f)
		{
			f.write(str.c_str(), sizeof(char[str.length()+1]));
		}
		string read_string(fstream& f)
		{
			string re; char c;
			while(1) { f.read(&c, sizeof(char)); if (!c) break; re.push_back(c); }
			assert(re.length() == strlen(re.c_str()));
			return re;
		}
		template<>
		void wrt_vec (vector<string>& vec, fstream& f)
		{
			auto tmpsz = vec.size();
			f.wrt(tmpsz);
			for (auto a : vec)
				write_string(a, f);
		}
		template<>
		vector<string> rd_vec(fstream &f)
		{
			typename vector<string>::size_type sz; f.rd(sz);
			vector<string> re; re.reserve(sz);
			char* buf = new char[1024];
			int bi = 0;
			for (typename vector<string>::size_type i=0;i<sz;++i)
			{
				bi = 0;
				do { f.read(buf+bi,sizeof(char)); }
				while (buf[bi++] != '\0' && bi != 1024);
				if (bi == 1024) throw runtime_error("The word is too long to read = =.");
				re.push_back(string(buf));
			}
			return re;
		}
	}
}
