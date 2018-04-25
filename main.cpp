#include <iostream>
#include <string.h>
#include <vector>
#include "readmap.cpp"
using namespace std;

int main(int argc, char* argv[])
{
	ReadMap *readMap = new ReadMap();
	readMap->determineArgs(argc, argv);
	readMap->constructSuffixTree();
	readMap->prepareSuffixTree();
	readMap->mapReads();

	// for(int i = 0; i < readMap->mSuffixTree->mNumLeaves; i++)
	// 	cout << readMap->mSuffixTree->mList[i] << endl;
	// cout << endl << endl;

	return 0;
}