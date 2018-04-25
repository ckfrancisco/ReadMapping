#include <iostream>
#include <string.h>

using namespace std;

enum {
    SUB,
    INS,
    DEL
};

typedef struct cell {
	int ins;
    int sub;
    int del;
}Cell;

class Alignment
{
	public:

		int mAlignment;

        int mMatch;
        int mMismatch;
        int mH;
        int mG;

        string mS1Header;
        string mS1;
        int mM;

        string mS2Header;
        string mS2;
        int mN;

        Cell **mTable;

        int mNMatches;
        int mNMismatches;
        int mNGaps;
        int mNOpens;
        int mPMatches;
        int mPGaps;

        string mS1RevSeq;
        string mS2RevSeq;

        int mScore;
        
        int mIMax;
        int mJMax;
		
		int mLen;

		Alignment(int alignment, int match, int mismatch, int h, int g,
			string s1header, string s1, int m,
			string s2header, string s2, int n)
		{
			mAlignment = alignment;

			mMatch = match;
            mMismatch = mismatch;
            mH = h;
            mG = g;

            mS1Header = s1header;
            mS1 = s1;
            mM = m;

            mS2Header = s2header;
            mS2 = s2;
            mN = n;

            mTable = NULL;

            mNMatches = 0;
            mNMismatches = 0;
            mNGaps = 0;
            mNOpens = 0;
            mPMatches = 0;
            mPGaps = 0;

            mScore = 0;

			mLen = 0;
		}

		~Alignment()
		{

		}

		int createTable()
		{
			mTable = (Cell**)malloc(sizeof(Cell*) * (mM + 1));

			for(int i = 0; i < mM + 1; i++)
				mTable[i] = (Cell*)malloc(sizeof(Cell) * (mN + 1));
		}

		virtual int initialize()
        {
			
        }

        virtual void recurrence()
        {
			
        }

        virtual void retrace()
        {
			
        }

		int detScore(char s1, char s2)
		{
			if(s1 == s2)
				return mMatch;
			return mMismatch;
		}

		int maxIns(Cell c)
		{
			int max = c.del + mG + mH;

			if(max < c.sub + mG + mH)
				max = c.sub + mG + mH;
			if(max < c.ins + mG)
				max = c.ins + mG;
			
			return max;
		}

		int maxSub(Cell c, char s1, char s2)
		{
			int max = c.del;

			if(max < c.sub)
				max = c.sub;
			if(max < c.ins)
				max = c.ins;
			
			return max + detScore(s1, s2);
		}

		int maxDel(Cell c)
		{
			int max = c.del + mG;

			if(max < c.sub + mG + mH)
				max = c.sub + mG + mH;
			if(max < c.ins + mG + mH)
				max = c.ins + mG + mH;
			
			return max;
		}

		int newMaxIns(Cell c)
		{
			if(c.ins + mG >= c.sub + mG + mH && c.ins + mG >= c.del + mG + mH )
				return c.ins;
			if(c.sub + mG + mH  >= c.ins + mG && c.sub + mG + mH  >= c.del + mG + mH )
				return c.sub;
			else
				return c.del;
		}

		void printScore()
		{
			int i = 1;
			int j = 1;
			int k = 1;
			int igap = 0;
			int kgap = 0;
			int count = -1;
			mLen++;
				
			int c = 0;

			cout << "Scores: matches = " << mMatch <<
			", mismatches = " << mMismatch << 
			", h = " << mH << 
			", g = " << mG << endl << endl;

			cout << "Sequence 1 = \"" << mS1Header << "\", length = " << mM << endl;
			cout << "Sequence 2 = \"" << mS2Header << "\", length = " << mN << endl << endl;

			while(i < mLen)
			{
				count = i;

				cout << "s1 " << setfill(' ') << setw(6) << i - igap << "   ";
				for(i; i - count < 60 && i < mLen; i++)
				{
					cout << mS1RevSeq[mLen - i - 1];

					if(mS1RevSeq[mLen - i - 1] == '-')
						igap++;
				}

				cout << "   " << i - igap - 1 << endl;

				cout << "            ";
				for(j; j - count < 60 && j < mLen; j++)
				{
					if(mS1RevSeq[mLen - j - 1] == mS2RevSeq[mLen - j - 1])
						cout << "|";
					else
						cout << " ";
				}
				cout << endl;

				cout << "s2 " << setfill(' ') << setw(6) << k - kgap << "   ";
				for(k; k - count < 60 && k < mLen; k++)
				{
					cout << mS2RevSeq[mLen - k - 1];

					if(mS2RevSeq[mLen - k - 1] == '-')
						kgap++;
				}

				cout << "   " << k - kgap - 1 << endl << endl;
			}  

			if(!mAlignment)
				cout << "Global optimal alignment: " << mScore << endl;
			else
				cout << "Local optimal alignment: " << mScore << endl;

			/* cout <<
				(mMatch * mNMatches) +
				(mMismatch * mNMismatches) +
				(mG * mNGaps) +
				(mH  * mNOpens) << endl; */

			mLen--;

			cout << "Number of: matches = " << mNMatches <<
			", mismatches = " << mNMismatches <<
			", gaps = " << mNGaps <<
			", opening gaps = " << mNOpens << endl;

			cout << "Identities = " << mNMatches <<"/" << mLen << "(%" << mPMatches << "), " <<
			"Gaps = " << mNGaps << "/" << mLen << "(%" << mPGaps << ")" << endl;
		}
};