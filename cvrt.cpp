#define CONVERT
#ifdef CONVERT
#include <cstdlib>
#include <cstdio>
#include <fstream>
using namespace std;
typedef long long LL;

int main()
{
	fstream f("freq_org", fstream::in);
	fstream fout("freq", fstream::out);
	LL n = 0;
	while (true)
	{
		string str; LL a;
		f>>str>>a;
		if (f.eof()) break;
		n += a;
	}
	f.seekg(0, fstream::beg);
	while (true)
	{
		string str; LL a;
		f>>str>>a;
		if (f.eof()) break;
		fout << str << " "<< ((double)a/n) << endl;
	}
	fout.close();
	f.close();
	return 0;

}
#endif
