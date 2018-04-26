#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include "suffixtree.cpp"
#include "local.cpp"

typedef struct stats {
	string mHeader = "";
	int mNSequences = 0;
	int mLength = 0;
	double mAvgLength = 0;
	int mNEmpty = 0;
	string mLongestHeader = "";
	int mLongest = 0;
	string mShortestHeader = "";
	int mShortest = 10000;
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
			genomeStatsStream.open("data/results/GenomeStatistics.txt");

			genomeStatsStream << "Report for " << mGenomeStats.mHeader << ":" << endl;
			genomeStatsStream << "Number of sequences  = " << mGenomeStats.mNSequences << endl;
			genomeStatsStream << "Total length =  " << mGenomeStats.mLength << endl;
			genomeStatsStream << "Average length of sequences = " << (int)mGenomeStats.mAvgLength << endl;
			genomeStatsStream << "Number of empty sequences = " << mGenomeStats.mNEmpty << endl;
			genomeStatsStream << "Longest length of a sequence = " << mGenomeStats.mLongest << endl;
			genomeStatsStream << "	" << mGenomeStats.mLongestHeader << endl;
			genomeStatsStream << "Shortest length of a sequence = " << mGenomeStats.mShortest << endl;
			genomeStatsStream << "	" << mGenomeStats.mShortestHeader << endl;

			mSuffixTree = new SuffixTree(header, sequence, 0);
			mSuffixTree->construction();

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
			string tmp = "data/results/MappingResults_" + mReadsFile.substr(5, mReadsFile.length() - 11) + ".txt";
			readsMappingStream.open(tmp);

			string rHeader;
			string rSequence;

			Node* deepestNode;

			int start = 0;
			int end = 0;
			int percentIdenity = 0;
			int lengthCoverage = 0;
			int nAlignments = 0;

			mReadsStats.mHeader = mReadsFile;
			mReadsStats.mNEmpty = 0;

			while(!readsStream.eof())
			{
				double bestLengthCoverage = -1;
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

				cout << "Hit Enter";
				cin.ignore();
				cout << endl << endl; */
						
				mReadsStats.mNSequences++; 
				mReadsStats.mLength += rSequence.length();

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

				for(int i = deepestNode->mStartIndex; i <= deepestNode->mEndIndex; i++)
				{
					nAlignments++;

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
					/* localAlignment->printScore(); */

					double percentIdenity = localAlignment->mNMatches / (double)localAlignment->mLen;
					double lengthCoverage = localAlignment->mLen / (double)rSequence.length();

					delete localAlignment;

					if(percentIdenity >= 0.9 && lengthCoverage >= 0.8)
					{
						if(bestLengthCoverage < lengthCoverage)
						{
							bestLengthCoverage = lengthCoverage;
							bestStartIndex = startIndex;
							bestEndIndex = endIndex;
						}
					}
				}

				if(bestLengthCoverage > 0)
					readsMappingStream << rHeader << " " << bestStartIndex << " " << bestEndIndex << endl;
				else
				{
					readsMappingStream << rHeader << " No hit found" << endl;
					mReadsStats.mNEmpty++;
				}
			}

			ofstream readsStatsStream;
			readsStatsStream.open("data/results/ReadsStatistics.txt");

			mReadsStats.mAvgLength = mReadsStats.mLength / (double)mReadsStats.mNSequences;

			readsStatsStream << "Report for " << mReadsStats.mHeader << ":" << endl;
			readsStatsStream << "Number of sequences  = " << mReadsStats.mNSequences << endl;
			readsStatsStream << "Total length =  " << mReadsStats.mLength << endl;
			readsStatsStream << "Average length of sequences = " << mReadsStats.mAvgLength << endl;
			readsStatsStream << "Number of empty sequences = " << mReadsStats.mNEmpty << endl;
			readsStatsStream << "Percentage of hits = " << (mReadsStats.mNSequences - mReadsStats.mNEmpty) / (double)mReadsStats.mNSequences << endl;
			readsStatsStream << "Longest length of a sequence = " << mReadsStats.mLongest << endl;
			readsStatsStream << "	" << mReadsStats.mLongestHeader << endl;
			readsStatsStream << "Shortest length of a sequence = " << mReadsStats.mShortest << endl;
			readsStatsStream << "	" << mReadsStats.mShortestHeader << endl;
			readsStatsStream << "Average alignments per read = " <<  nAlignments / (double)mReadsStats.mNSequences << endl;

			readsStream.close();
			readsStatsStream.close();
		}
};