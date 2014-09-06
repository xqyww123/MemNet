//#define CONVERT
#ifdef CONVERT
#include <cstdlib>
#include <cstdio>
#include <fstream>
using namespace std;
typedef long long LL;

int main()
{
	fstream f("freq_org", fstream::in);
	fstream fout("freq.dat", fstream::out);
	LL n = 450000000LL;
	//int i=0;
	//while (true)
	//{
	//	string str; LL a;
	//	f>>str>>a;
	//	if (f.eof()) break;
	//	if (a <= 10) continue;
	//	n += a;
	//}
	//f.clear();
	//f.seekg(0, fstream::beg);
	while (true)
	{
		string str; LL a;
		f>>str>>a;
		if (f.eof()) break;
		if (a <= 10) continue;
		fout << str << " "<< ((double)a/n) << endl;
	}
	fout.close();
	f.close();
	return 0;

}
#endif
