#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include "suffixtree.cpp"
#include "local.cpp"
using namespace std;

typedef struct stats {
	string mHeader = "";
	int mNSequences = 0;
	int mLength = 0;
	int mAvgLength = 0;
	int mNEmpty = 0;
	string mLongestHeader = "";
	int mLongest = 0;
	string mShortestHeader = "";
	int mShortest = 0;
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

			mGenomeStats.mHeader = mGenomeFile;
			mGenomeStats.mNSequences = 1; 
			mGenomeStats.mLength = sequence.length();
			mGenomeStats.mAvgLength = sequence.length();
			mGenomeStats.mNEmpty = 0;
			mGenomeStats.mLongestHeader = header;
			mGenomeStats.mLongest = sequence.length();
			mGenomeStats.mShortestHeader = header;
			mGenomeStats.mShortest = sequence.length();

			ofstream genomeStatsStream;
			genomeStatsStream.open("GenomeStatistics.txt");

			genomeStatsStream << "Report for " << mGenomeStats.mHeader << ":" << endl;
			genomeStatsStream << "Number of sequences  = " << mGenomeStats.mNSequences << endl;
			genomeStatsStream << "Total length =  " << mGenomeStats.mLength << endl;
			genomeStatsStream << "Average length of sequences = " << mGenomeStats.mAvgLength << endl;
			genomeStatsStream << "Number of empty sequences = " << mGenomeStats.mNEmpty << endl;
			genomeStatsStream << "Longest length of a sequence = " << mGenomeStats.mLongest << endl;
			genomeStatsStream << "	>" << mGenomeStats.mLongestHeader << endl;
			genomeStatsStream << "Shortest length of a sequence = " << mGenomeStats.mShortest << endl;
			genomeStatsStream << "	>" << mGenomeStats.mShortestHeader << endl;

			mSuffixTree = new SuffixTree(header, sequence, 0);
			mSuffixTree->construction();

			cout << "TREE CONSTRUCTED" << endl;

			genomeStream.close();
			genomeStatsStream.close();
		}

		void prepareSuffixTree()
		{
			mSuffixTree->dfsPrepareST();
		}

		void mapReads()
		{
			ifstream readsStream;
			readsStream.open(mReadsFile);

			ofstream readsMappingStream;
			readsMappingStream.open("MappingResults.txt");

			string rHeader;
			string rSequence;

			Node* deepestNode;

			int start = 0;
			int end = 0;
			int percentIdenity = 0;
			int lengthCoverage = 0;
			int nAlignments = 0;

			mReadsStats.mHeader = mReadsFile;

			while(!readsStream.eof())
			{
				int bestLengthCoverage = -1;
				int bestStartIndex = -1;
				int bestEndIndex = -1;

				getline(readsStream, rHeader);
				getline(readsStream, rSequence);

				deepestNode = mSuffixTree->findLoc(&rSequence);

				/* cout << rHeader << endl;

				for(int i = deepestNode->mStartIndex; i <= deepestNode->mEndIndex; i++)
				{
					cout << mSuffixTree->mList[i] << endl;
				}

				cout << endl; */
						
				mReadsStats.mNSequences++; 
				mReadsStats.mLength += rSequence.length();

				mReadsStats.mNEmpty = 0;

				if(mReadsStats.mLongest < rSequence.length())
				{
					mReadsStats.mLongestHeader = rHeader;
					mReadsStats.mLongest = rSequence.length();
				}

				if(mReadsStats.mShortest > rSequence.length())
				{
					mReadsStats.mShortestHeader = rHeader;
					mReadsStats.mShortest = rSequence.length();
				}

				cout << "SEQUENCE #" << mReadsStats.mNSequences << endl;

				for(int i = deepestNode->mStartIndex; i <= deepestNode->mEndIndex; i++)
				{
					nAlignments++;

					cout << "SEQUENCE #" << mReadsStats.mNSequences << " " << nAlignments << endl;

					int startIndex = mSuffixTree->mList[i] - rSequence.length();
					int endIndex = mSuffixTree->mList[i] + rSequence.length();

					if(startIndex < 0)
						startIndex = 0;
					if(endIndex > mSuffixTree->mSequence.length())
						endIndex = mSuffixTree->mSequence.length();

					LocalAlignment *localAlignment = new LocalAlignment(1, 1, -2, -5, -1,
						"Genome", mSuffixTree->mSequence.substr(startIndex, endIndex - startIndex), endIndex - startIndex,
						"Read", rSequence, rSequence.length());

					localAlignment->createTable();
					localAlignment->initialize();
					localAlignment->recurrence();
					localAlignment->retrace();

					int percentIdenity = localAlignment->mNMatches / localAlignment->mLen;
					int lengthCoverage = localAlignment->mLen / rSequence.length();

					if(percentIdenity >= 0.9 && lengthCoverage >= 0.8)
					{
						if(bestLengthCoverage < lengthCoverage)
						{
							bestLengthCoverage = lengthCoverage;
							bestStartIndex = startIndex;
							bestEndIndex = endIndex;
						}
					}

					else
					{
						mReadsStats.mNEmpty++;
					}

					free(localAlignment);
				}

				readsMappingStream << rHeader << " " << bestStartIndex << " " << bestEndIndex << endl;
			}

			ofstream readsStatsStream;
			readsStatsStream.open("ReadsStatistics.txt");

			readsStatsStream << "Report for " << mReadsStats.mHeader << ":" << endl;
			readsStatsStream << "Number of sequences  = " << mReadsStats.mNSequences << endl;
			readsStatsStream << "Total length =  " << mReadsStats.mLength << endl;
			readsStatsStream << "Average length of sequences = " << mReadsStats.mAvgLength << endl;
			readsStatsStream << "Number of empty sequences = " << mReadsStats.mNEmpty << endl;
			readsStatsStream << "Longest length of a sequence = " << mReadsStats.mLongest << endl;
			readsStatsStream << "	>" << mReadsStats.mLongestHeader << endl;
			readsStatsStream << "Shortest length of a sequence = " << mReadsStats.mShortest << endl;
			readsStatsStream << "	>" << mReadsStats.mShortestHeader << endl;

			readsStream.close();
			readsStatsStream.close();
		}
};