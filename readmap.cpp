#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include "suffixtree.cpp"
#include "local.cpp"
using namespace std;

typedef struct stats {
	string mHeader;
	int mNSequences;
	int mLength;
	int mAvgLength;
	int mNEmpty;
	int mLongest;
	int mShortest;
}Stats;

class ReadMap
{
	public:
		string mGenomeFile;
		string mReadsFile;
		string mAlphabetFile;

		SuffixTree *mSuffixTree;
		LocalAlignment *mLocalAlignment;

		Stats mGenomeStats;
		Stats mReadsStats;

		int mStart;
		int mEnd;
		int mPercentIdenity;
		int mLengthCoverage;

		ReadMap()
		{
			mSuffixTree = NULL;
			mLocalAlignment = NULL;

			mStart = 0;
			mEnd = 0;
			mPercentIdenity = 0;
			mLengthCoverage = 0;
		}

		~ReadMap()
		{

		}

		void determineArgs(int agrc, char* argv[])
		{
			mGenomeFile = argv[1];
			mReadsFile = argv[2];
			mAlphabetFile = argv[3];
		}

		void constructSuffixTree()
		{
			ifstream genomeStream;

			string header;
			string sequence;

			genomeStream.open(mGenomeFile);
			getline(genomeStream, header);
			header = header.substr(1);

			string tmp;
			while(!genomeStream.eof())
			{
				genomeStream >> tmp;
				sequence += tmp;
			}

			mSuffixTree = new SuffixTree(header, sequence, 0);
			mSuffixTree->construction();
		}

		void prepareSuffixTree()
		{
			mSuffixTree->dfsPrepareST();
		}

		void mapReads()
		{
			ifstream readsStream;
			readsStream.open(mReadsFile);

			string rHeader;
			string rSequence;

			Node* deepestNode;

			int start = 0;
			int end = 0;
			int percentIdenity = 0;
			int lengthCoverage = 0;

			while(!readsStream.eof())
			{
				getline(readsStream, rHeader);
				rHeader = rHeader.substr(1);
				getline(readsStream, rSequence);

				

				deepestNode = mSuffixTree->findLoc(&rSequence);

				/* cout << rHeader << endl;

				for(int i = deepestNode->mStartIndex; i <= deepestNode->mEndIndex; i++)
				{
					cout << mSuffixTree->mList[i] << endl;
				}

				cout << endl; */

				for(int i = deepestNode->mStartIndex; i <= deepestNode->mEndIndex; i++)
				{
					start = i - rSequence.length();
					end = i + rSequence.length();

					if(start < 0)
						start = 0;
					if(end > mSuffixTree->mSequence.length())
						end = mSuffixTree->mSequence.length();

					LocalAlignment *localAlignment = new LocalAlignment(1, 1, -2, -5, -1,
						mSuffixTree->mHeader, mSuffixTree->mSequence.substr(0, mSuffixTree->mSequence.length() - 1), mSuffixTree->mSequence.length(),
						rHeader, rSequence, rSequence.length());

					localAlignment->createTable();
					localAlignment->initialize();
					localAlignment->recurrence();
					localAlignment->retrace();
					// localAlignment->printScore();

					percentIdenity = localAlignment->mNMatches / localAlignment->mLen;
					lengthCoverage = localAlignment->mLen / rSequence.length();

					if(percentIdenity >= 0.9 && lengthCoverage >= 0.8)
					{

					}
				}
			}
		}
};