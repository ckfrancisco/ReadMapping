#include <iostream>
#include <string.h>
#include <vector>
#include "readmap.cpp"
using namespace std;

int main(int argc, char* argv[])
{
	string header;
	string sequence;

	ReadMap *readMap = new ReadMap();

	/* for(int i = 0; i < readMap->mSuffixTree->mNumLeaves; i++)
		cout << readMap->mSuffixTree->mList[i] << endl;
	cout << endl << endl; */

	readMap->mapReads(argv[2]);

	return 0;
}