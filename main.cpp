#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <chrono>
#include "readmap.cpp"

using namespace std;

int main(int argc, char* argv[])
{
	ofstream timeStream;
	timeStream.open("data/results/TimeResults.txt");

	auto start = std::chrono::high_resolution_clock::now();
	auto check = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed;

	ReadMap *readMap = new ReadMap();
	readMap->determineArgs(argc, argv);



	check = std::chrono::high_resolution_clock::now();

	readMap->constructSuffixTree();

	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - check;

	timeStream << "Construction time = " << elapsed.count() << endl;



	check = std::chrono::high_resolution_clock::now();

	readMap->prepareSuffixTree();

	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - check;

	timeStream << "Preparation time = " << elapsed.count() << endl;



	check = std::chrono::high_resolution_clock::now();

	readMap->mapReads();

	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - check;

	timeStream << "Read Mapping time = " << elapsed.count() << endl;



	elapsed = finish - start;

	timeStream << "Total time = " << elapsed.count() << endl;

	/* for(int i = 0; i < readMap->mSuffixTree->mNumLeaves; i++)
	cout << readMap->mSuffixTree->mList[i] << endl;
	cout << endl << endl; */

	return 0;
}