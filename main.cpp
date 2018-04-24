#include <iostream>
#include <string.h>
#include "help.cpp"
#include "suffixtree.cpp"
#include <vector>
using namespace std;

int main(int argc, char* argv[])
{
	string header;
	string sequence;
	string alphabet;

	determineArgs(argv[1], argv[2], &header, &sequence, &alphabet);
	/* cout << header << endl;
	cout << sequence << endl;
	cout << alphabet << endl << endl; */

	SuffixTree *suffixTree = new SuffixTree(sequence);
	suffixTree->construction();
	/* cout << endl << endl; */

	/* suffixTree->dfsPrint();
	cout << endl; */

	/* suffixTree->dfsEnumerate();
	cout << endl << endl; */

	suffixTree->dfsBWT();
	/* cout << endl << endl; */

	/* suffixTree->dfsRepeat();
	cout << endl; */

	/* cout << sequence.length() << endl;
	cout << suffixTree->mNumLeaves - 1 << endl; */

	return 0;
}