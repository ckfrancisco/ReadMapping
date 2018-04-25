#include <iostream>
#include <string.h>
#include <iomanip>
#include "alignment.cpp"

using namespace std;

class LocalAlignment: public Alignment
{
	public:

        LocalAlignment(int alignment, int match, int mismatch, int h, int g,
            string s1header, string s1, int m,
            string s2header, string s2, int n):
        Alignment(alignment, match, mismatch, h, g,
            s1header, s1, m,
            s2header, s2, n)
        {

        }

        ~LocalAlignment()
        {

        }

        int initialize()
        {
            mTable[0][0].ins = 0;
            mTable[0][0].sub = 0;
            mTable[0][0].del = 0;

            for(int i = 1; i < mM + 1; i++)
            {
                mTable[i][0].ins = 0;
                mTable[i][0].sub = 0;
                mTable[i][0].del = 0;
            }
            
            for(int j = 1; j < mN + 1; j++)
            {
                mTable[0][j].ins = 0;
                mTable[0][j].sub = 0;
                mTable[0][j].del = 0;
            }
        }

        void recurrence()
        {
            int scoremax = 0;

            for(int i = 1; i < mM + 1; i++)
            {
                for(int j = 1; j < mN + 1; j++)
                {
                    mTable[i][j].ins = maxIns(mTable[i][j - 1]);
                    if(mTable[i][j].ins < 0)
                        mTable[i][j].ins = 0;

                    if(scoremax < mTable[i][j].ins)
                    {
                        scoremax = mTable[i][j].ins;
                        mIMax = i;
                        mJMax = j;
                    }

                    mTable[i][j].sub = maxSub(mTable[i - 1][j - 1], mS1[i - 1], mS2[j - 1]);
                    if(mTable[i][j].sub < 0)
                        mTable[i][j].sub = 0;

                    if(scoremax < mTable[i][j].sub)
                    {
                        scoremax = mTable[i][j].sub;
                        mIMax = i;
                        mJMax = j;
                    }

                    mTable[i][j].del = maxDel(mTable[i - 1][j]);
                    if(mTable[i][j].del < 0)
                        mTable[i][j].del = 0;

                    if(scoremax < mTable[i][j].del)
                    {
                        scoremax = mTable[i][j].del;
                        mIMax = i;
                        mJMax = j;
                    }
                }
            }
        }

        void retrace()
        {
            int i = mIMax;
            int j = mJMax;
            int k = 0;

            int ins = 0;
            int sub = 0;
            int del = 0;

            int curr = 0;
            int prev = -1;

            mS1RevSeq.resize(mIMax + mJMax, '\0');
            mS2RevSeq.resize(mIMax + mJMax, '\0');

            mScore = mTable[mIMax][mJMax].ins;
            if(mScore < mTable[mIMax][mJMax].sub)
                mScore = mTable[mIMax][mJMax].sub;
            if(mScore < mTable[mIMax][mJMax].del)
                mScore = mTable[mIMax][mJMax].del;

            while(i > 0 || j > 0)
            {

                if(mTable[i][j].ins == 0 && mTable[i][j].sub == 0 && mTable[i][j].del == 0)
                    break;

                if(i == 0 && j > 0)
                {
                    mS1RevSeq[k] = '-'; mS2RevSeq[k] = mS2[j - 1];

                    if(prev != INS)
                        mNOpens++;
                    mNGaps++;

                    j--; k++;
                    prev = INS;
                }

                else if(i > 0 && j == 0)
                {
                    mS1RevSeq[k] = mS1[i - 1]; mS2RevSeq[k] = '-';

                    if(prev != DEL)
                        mNOpens++;
                    mNGaps++;

                    i--; k++;
                    prev = DEL;
                }

                else
                {
                    ins = mTable[i][j].ins;
                    sub = mTable[i][j].sub;
                    del = mTable[i][j].del;

                    if(prev == INS)
                    {
                        sub += mH;
                        del += mH;
                    }

                    if(prev == DEL)
                    {
                        ins += mH;
                        sub += mH;
                    }

                    if(ins >= sub && ins >= del)
                    {
                        mS1RevSeq[k] = '-'; mS2RevSeq[k] = mS2[j - 1];

                        if(prev != INS)
                            mNOpens++;
                        mNGaps++;

                        j--; k++;
                        prev = INS;
                    }
                    else if(sub >= ins && sub >= del)
                    {
                        mS1RevSeq[k] = mS1[i - 1]; mS2RevSeq[k] = mS2[j - 1];

                        if(mS1RevSeq[k] == mS2RevSeq[k])
                            mNMatches++;
                        else
                            mNMismatches++;

                        i--; j--; k++;
                        prev = SUB;
                    }
                    else
                    {
                        mS1RevSeq[k] = mS1[i - 1]; mS2RevSeq[k] = '-';

                        if(prev != DEL)
                            mNOpens++;
                        mNGaps++;

                        i--; k++;
                        prev = DEL;
                    }
                }
            }
            
            mS1RevSeq[k] = 0; mS2RevSeq[k] = 0;

            mLen = mNMatches + mNMismatches + mNGaps;
		    mPMatches = 100 * (double)mNMatches / mLen;
		    mPGaps = 100 * (double)mNGaps / mLen;
            
            return;
        }
};