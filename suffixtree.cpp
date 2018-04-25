#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

class Node
{
	public:
		int mId;

		int mIndex;
		int mLength;

		int mStringDepth;

		Node *mParent;
		Node *mSuffixLink;
		Node *mChild;
		Node *mSibling;

		int mStartIndex;
		int mEndIndex;

		Node(int id, int index, int length, int stringDepth, Node *parent, Node* suffixLink = NULL)
		{
			mId = id;
			mIndex = index;
			mLength = length;
			mStringDepth = stringDepth;
			mParent = parent;
			mSuffixLink = suffixLink;
			mChild = NULL;
			mSibling = NULL;

			mStartIndex = -1;
			mEndIndex = -1;
		}

		~Node()
		{

		}

		Node* insertChild(string *sequence, int id, int index)
		{
			index += mStringDepth;
			int length = sequence->length() - index;
			int stringDepth = mStringDepth + length;

			Node *n = new Node(id, index, length, stringDepth, this);

			if(mChild == NULL)
			{
				mChild = n;
			}

			else if((*sequence)[index] < (*sequence)[mChild->mIndex])
			{
				n->mSibling = mChild;
				mChild = n;
			}

			else
			{
				Node *prevChild = mChild;
				Node *currChild = mChild->mSibling;

				while(1)
				{
					if(!currChild || (*sequence)[index] < (*sequence)[currChild->mIndex])
					{
						n->mSibling = currChild;
						prevChild->mSibling = n;
						break;
					}

					prevChild = currChild;
					currChild = currChild->mSibling;
				}
			}
			
			return n;
		}

		Node* insertInternal(int id, int length, Node *swap)
		{
			int index = swap->mIndex;
			int stringDepth = mStringDepth + length;

			Node *n = new Node(swap->mId, swap->mIndex + length, swap->mLength - length, swap->mStringDepth, swap);
			n->mChild = swap->mChild;

			swap->mId = id;
			swap->mIndex = index;
			swap->mLength = length;
			swap->mStringDepth = stringDepth;
			swap->mChild = n;

			for(Node* child = n->mChild; child; child = child->mSibling)
			{
				child->mParent = n;
			}

			return swap;
		}
};

class SuffixTree
{
	public:
		string mHeader;
		string mSequence;
		Node *mRoot;
		int mNumInternals;
		int mNumLeaves;

		int *mList;
		int mNextIndex;
		int mX;

		SuffixTree(string header, string sequence, int x)
		{
			mHeader = header;
			mSequence = sequence + "$";

			mRoot = new Node(-1, 0, 0, 0, mRoot, mRoot);
			mRoot->mParent = mRoot;
			mRoot->mSuffixLink = mRoot;

			mNumInternals = -2;
			mNumLeaves = 0;

			mList = new int[mSequence.length()];

			for(int i = 0; i < mSequence.length(); i++)
				mList[i] = -1;

			mNextIndex = 0;
			mX = x;
		}

		~SuffixTree()
		{

		}

		void construction()
		{
			Node *n = mRoot;

			Node *u = NULL;
			Node *up = NULL;

			Node *v = NULL;
			Node *vp = NULL;

			std::clock_t time_start;

			// insert all of sequence's suffixes
			for(int index = 0; index < mSequence.length(); index++)
			{
				findPath(mRoot, index);

				if(index % 10000 == 0)
				{
					cout << index << "   " << (std::clock() - time_start) / (double) CLOCKS_PER_SEC << endl;
				}
			}
		}

		Node* findPath(Node* n, int index)
		{
			Node *child = n->mChild;

			// 1: iterate through children to find edge to traverse
			for(child; child; child = child->mSibling)
			{
				if(mSequence[child->mIndex] == mSequence[index + n->mStringDepth])
				{
					break;
				}

				if(mSequence[child->mIndex] > mSequence[index + n->mStringDepth])
				{
					child = NULL;
					break;
				}
			}

			// 2: edge found
			if(child)
			{
				int i = 0;

				// 3: iterate through edge to find break point
				for(i; i < child->mLength; i++)
				{
					if(mSequence[child->mIndex + i] != mSequence[index + n->mStringDepth + i])
					{
						break;
					}
				}

				// 4: break point found
				if(i < child->mLength)
				{
					// 5: insert a new internal node
					Node *internal = n->insertInternal(mNumInternals--, i, child);
					return internal->insertChild(&mSequence, mNumLeaves++, index);
				}

				// 4: break point not found (edge exhausted)
				else
				{
					// 5: continue traversal from child node via recursion
					return findPath(child, index);
				}
			}

			// 2: edge not found
			else
			{
				// 3: insert a new child node
				return n->insertChild(&mSequence, mNumLeaves++, index);
			}

		}

		Node* nodeHops(Node* n, int index, int length, int i)
		{
			Node *child = n->mChild;

			if(length == 0)
			{
				return mRoot;
			}

			// 1: iterate through children to find edge to traverse
			for(Node *child = n->mChild; child; child = child->mSibling)
			{
 				/* cout << mSequence[child->mIndex] << " " << mSequence[index + i] << endl; */
				if(mSequence[child->mIndex] == mSequence[index + i])
				{	
					// 2: recurse to next child node and return result

					if(i + child->mLength == length)
					{
						return  child;
					}

					else if(length - i < child->mLength)
					{
						return n->insertInternal(mNumInternals--, length - i, child);
					}

					else
					{
						return nodeHops(child, index, length, i + child->mLength);
					}
				}
			}
		}

		void dfsEnumerate()
		{
			int counter = 0;

			// begin depth first search from root
			dfsEnumerateHelper(mRoot, &counter);
		}

		void dfsEnumerateHelper(Node *n, int *counter)
		{
			// print current node's string depth
			cout << n->mStringDepth << " ";
			(*counter)++;
			if(*counter % 10 == 0)
				cout << endl;

			// depth first search through children
			for(Node* child = n->mChild; child; child = child->mSibling)
			{
				dfsEnumerateHelper(child, counter);
			}
		}

		void dfsPrint()
		{
			// begin depth first search from root
			dfsPrintHelper(mRoot);
		}

		void dfsPrintHelper(Node *n)
		{
			if(n->mChild == NULL)
			{
				cout << n->mId << " ";

				for(int i = 0; i < n->mStringDepth; i++)
				{
					cout << mSequence[n->mId + i];
				}
				
				cout << endl;

				return;
			}

			for(Node* child = n->mChild; child; child = child->mSibling)
			{
				dfsPrintHelper(child);
			}
		}

		void dfsBWT()
		{
			string bwt(mSequence.length() + 1, ' ');
			int index = 0;

			// begin depth first search from root
			dfsBWTHelper(mRoot, &bwt, &index);
			// cout << bwt;
		}

		void dfsBWTHelper(Node *n, string *bwt, int *index)
		{
			if(n->mChild == NULL)
			{
				int sequenceIndex = n->mId;
				sequenceIndex--;

				if(sequenceIndex < 0)
					sequenceIndex = mSequence.length() - 1;

				(*bwt)[(*index)++] = mSequence[sequenceIndex];
				cout << mSequence[sequenceIndex] << endl;
			}

			// depth first search through children
			for(Node* child = n->mChild; child; child = child->mSibling)
			{
				dfsBWTHelper(child, bwt, index);
			}
		}

		void dfsRepeat()
		{
			Node *lemrNode = mRoot;
			vector<int> lemrIndexes;

			dfsRepeatHelper(mRoot, &lemrNode, &lemrIndexes);

			Node* n = lemrNode;

			while(n->mParent != mRoot)
			{
				n = n->mParent;
			}

			for(int i = 0; i < lemrNode->mStringDepth; i++)
			{
				cout << mSequence[n->mIndex + i];
			}

			cout << endl;

			vector<int>::iterator it = lemrIndexes.begin();

			// determine lexicographic index
			for(it; it != lemrIndexes.end(); ++it)
			{
				cout << *it << " ";
			}
		}

		void dfsRepeatHelper(Node *n, Node **lemrNode, vector<int> *lemrIndexes)
		{
			if(n->mChild == NULL)
			{
				return;
			}

			else
			{
				if(n->mStringDepth > (*lemrNode)->mStringDepth)
				{
					(*lemrNode) = n;
					(*lemrIndexes).clear();
					dfsLemrIndexesHelper((*lemrNode), lemrIndexes);
				}
			}

			for(Node* child = n->mChild; child; child = child->mSibling)
			{
				dfsRepeatHelper(child, lemrNode, lemrIndexes);
			}
		}

		void dfsLemrIndexesHelper(Node *n, vector<int> *lemrIndexes)
		{
			if(n->mChild == NULL)
			{
				(*lemrIndexes).push_back(n->mId);
				return;
			}

			for(Node* child = n->mChild; child; child = child->mSibling)
			{
				dfsLemrIndexesHelper(child, lemrIndexes);
			}
		}

		void dfsPrepareST()
		{
			// begin depth first search from root
			dfsPrepareSTHelper(mRoot);
		}

		void dfsPrepareSTHelper(Node *n)
		{
			if(n->mId >= 0)
			{
				mList[mNextIndex] = n->mId;

				if(n->mStringDepth >= mX)
				{
					n->mStartIndex = mNextIndex;
					n->mEndIndex = mNextIndex;
				}
				mNextIndex++;
				return;
			}

			// depth first search through children
			for(Node* child = n->mChild; child; child = child->mSibling)
			{
				dfsPrepareSTHelper(child);
			}

			if(n->mStringDepth >= mX)
				{
					n->mStartIndex = n->mChild->mStartIndex;

					Node* child = n->mChild;
					for(child; child->mSibling != NULL; child = child->mSibling);

					n->mEndIndex = child->mEndIndex;
				}
		}

		Node* findLoc(string *sRead)
		{
			Node* n = mRoot;
			int pRead = 0;
			Node* deepestNode = mRoot;

			while(sRead->length() != pRead)
			{
				Node* child = n->mChild;

				// 1: iterate through children to find edge to traverse
				for(child; child; child = child->mSibling)
				{
					if(mSequence[child->mIndex] == (*sRead)[pRead])
					{
						break;
					}

					if(mSequence[child->mIndex] > (*sRead)[pRead])
					{
						child = NULL;
						break;
					}
				}

				// 2: edge found
				if(child)
				{
					int i = 0;

					// 3: iterate through edge to find break point
					for(i; i < (child)->mLength; i++)
					{
						if(mSequence[(child)->mIndex + i] != (*sRead)[pRead + i])
						{
							break;
						}
					}

					// 4: break point found
					if(i < (child)->mLength)
					{
						if(n->mStringDepth >= mX && deepestNode->mStringDepth < n->mStringDepth)
						{
							deepestNode = n;
						}

						n = n->mSuffixLink;
					}

					// 4: break point not found (edge exhausted)
					else
					{
						// 5: continue traversal from child node via loop
						pRead = pRead + i;		
						n = (child);

						if(n->mStringDepth >= mX && deepestNode->mStringDepth < n->mStringDepth)
						{
							deepestNode = n;
						}

						continue;
					}
				}

				// 2: edge not found
				else
				{
					if(n->mStringDepth >= mX && deepestNode->mStringDepth < n->mStringDepth)
					{
						deepestNode = n;
					}

					n = n->mSuffixLink;
				}
			}

			return deepestNode;
		}
};