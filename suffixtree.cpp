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
		vector<Node*> mChildren;

		int mStartIndex;
		int mEndIndex;

		Node(int id, int index, int length, int stringDepth, Node *parent, Node *suffixLink = NULL)
		{
			mId = id;
			mIndex = index;
			mLength = length;
			mStringDepth = stringDepth;
			mParent = parent;
			mSuffixLink = suffixLink;

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

			vector<Node*>::iterator it = mChildren.begin();

			// determine lexicographic index
			for(it; it != mChildren.end(); ++it)
			{
				if((*sequence)[index] < (*sequence)[(*it)->mIndex])
					break;
			}

			mChildren.insert(it, n);
			
			return n;
		}



		Node* insertInternal(int id, int length, vector<Node*>::iterator it)
		{
			int index = (*it)->mIndex;
			int stringDepth = mStringDepth + length;

			Node *n = new Node(id, index, length, stringDepth, this);
			Node *child = *it;

			mChildren.insert(it, n);
			
			// determine iterator position of child to remove
			for(it = mChildren.begin(); it != mChildren.end(); ++it)
			{
				if((*it) == child)
				{
					break;
				}
			}

			mChildren.erase(it);

			child->mIndex += length;
			child->mLength -= length;
			child->mParent = n;

			n->mChildren.push_back(child);

			return n;
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

			// insert all of sequence's suffixes
			for(int index = 0; index < mSequence.length(); index++)
			{
				u = n->mParent;

				if(u->mSuffixLink != NULL)
				{
					if(u != mRoot)
					{
						v = u->mSuffixLink;
						n = findPath(v, index);
					}

					else
					{
						v = u->mSuffixLink;
						n = findPath(v, index);
					}
				}

				else
				{
					up = u->mParent;
					vp = up->mSuffixLink;

					if(up != mRoot)
					{
						v = nodeHops(vp, u->mIndex, u->mLength, 0);
						u->mSuffixLink = v;
						n = findPath(v, index);
					}

					else
					{
						v = nodeHops(vp, u->mIndex + 1, u->mLength - 1, 0);
						u->mSuffixLink = v;
						n = findPath(v, index);
					}
				}
			}
		}

		Node* findPath(Node* n, int index)
		{
			vector<Node*>::iterator it = n->mChildren.begin();

			// 1: iterate through children to find edge to traverse
			for(it; it != n->mChildren.end(); ++it)
			{
				if(mSequence[(*it)->mIndex] == mSequence[index + n->mStringDepth])
				{
					break;
				}

				if(mSequence[(*it)->mIndex] > mSequence[index + n->mStringDepth])
				{
					it = n->mChildren.end();
					break;
				}
			}

			// 2: edge found
			if(it != n->mChildren.end())
			{
				int i = 0;

				// 3: iterate through edge to find break point
				for(i; i < (*it)->mLength; i++)
				{
					if(mSequence[(*it)->mIndex + i] != mSequence[index + n->mStringDepth + i])
					{
						break;
					}
				}

				// 4: break point found
				if(i < (*it)->mLength)
				{
					// 5: insert a new internal node
					Node *internal = n->insertInternal(mNumInternals--, i, it);
					return internal->insertChild(&mSequence, mNumLeaves++, index);
				}

				// 4: break point not found (edge exhausted)
				else
				{
					// 5: continue traversal from child node via recursion
					return findPath(*it, index);
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
			vector<Node*>::iterator it = n->mChildren.begin();

			if(length == 0)
			{
				return mRoot;
			}

			// 1: iterate through children to find edge to traverse
			for(it; it != n->mChildren.end(); ++it)
			{
 				/* cout << mSequence[(*it)->mIndex] << " " << mSequence[index + i] << endl; */
				if(mSequence[(*it)->mIndex] == mSequence[index + i])
				{	
					// 2: recurse to next child node and return result

					if(i + (*it)->mLength == length)
					{
						return  *it;
					}

					else if(length - i < (*it)->mLength)
					{
						return n->insertInternal(mNumInternals--, length - i, it);
					}

					else
					{
						return nodeHops(*it, index, length, i + (*it)->mLength);
					}
				}
			}
		}

		void displayChildren(Node *n)
		{
			// iterate through children
			for(vector<Node*>::iterator it = n->mChildren.begin(); it != n->mChildren.end(); ++it)
			{
				cout << (*it)->mId << " ";
			}

			cout << endl;
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
			for(vector<Node*>::iterator it = n->mChildren.begin(); it != n->mChildren.end(); ++it)
			{
				dfsEnumerateHelper(*it, counter);
			}
		}

		void dfsPrint()
		{
			// begin depth first search from root
			dfsPrintHelper(mRoot);
		}

		void dfsPrintHelper(Node *n)
		{
			if(n->mChildren.size() < 1)
			{
				cout << n->mId << " ";

				for(int i = 0; i < n->mStringDepth; i++)
				{
					cout << mSequence[n->mId + i];
				}
				
				cout << endl;

				return;
			}

			for(vector<Node*>::iterator it = n->mChildren.begin(); it != n->mChildren.end(); ++it)
			{
				dfsPrintHelper(*it);
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
			if(n->mChildren.size() < 1)
			{
				int sequenceIndex = n->mId;
				sequenceIndex--;

				if(sequenceIndex < 0)
					sequenceIndex = mSequence.length() - 1;

				(*bwt)[(*index)++] = mSequence[sequenceIndex];
				cout << mSequence[sequenceIndex] << endl;
			}

			// depth first search through children
			for(vector<Node*>::iterator it = n->mChildren.begin(); it != n->mChildren.end(); ++it)
			{
				dfsBWTHelper(*it, bwt, index);
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
			if(n->mChildren.size() < 1)
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

			for(vector<Node*>::iterator it = n->mChildren.begin(); it != n->mChildren.end(); ++it)
			{
				dfsRepeatHelper(*it, lemrNode, lemrIndexes);
			}
		}

		void dfsLemrIndexesHelper(Node *n, vector<int> *lemrIndexes)
		{
			if(n->mChildren.size() < 1)
			{
				(*lemrIndexes).push_back(n->mId);
				return;
			}

			for(vector<Node*>::iterator it = n->mChildren.begin(); it != n->mChildren.end(); ++it)
			{
				dfsLemrIndexesHelper(*it, lemrIndexes);
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
			for(vector<Node*>::iterator it = n->mChildren.begin(); it != n->mChildren.end(); ++it)
			{
				dfsPrepareSTHelper(*it);
			}

			if(n->mStringDepth >= mX)
				{
					n->mStartIndex = n->mChildren.front()->mStartIndex;
					n->mEndIndex = n->mChildren.back()->mEndIndex;
				}
		}

		Node* findLoc(string *sRead)
		{
			Node* n = mRoot;
			int pRead = 0;
			Node* deepestNode = mRoot;

			while(pRead <= sRead->length() - 1)
			{
				vector<Node*>::iterator it = n->mChildren.begin();

				// 1: iterate through children to find edge to traverse
				for(it; it != n->mChildren.end(); ++it)
				{
					if(mSequence[(*it)->mIndex] == (*sRead)[pRead])
					{
						break;
					}

					if(mSequence[(*it)->mIndex] > (*sRead)[pRead])
					{
						it = n->mChildren.end();
						break;
					}
				}

				// 2: edge found
				if(it != n->mChildren.end())
				{
					int i = 0;

					// 3: iterate through edge to find break point
					for(i; i < (*it)->mLength; i++)
					{
						if(mSequence[(*it)->mIndex + i] != (*sRead)[pRead + i])
						{
							break;
						}
					}

					// 4: break point found
					if(i < (*it)->mLength && (i != (*it)->mLength - 1 && mSequence[(*it)->mLength - 1] != '$'))
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

						if((*it)->mStringDepth >= mX && deepestNode->mStringDepth < (*it)->mStringDepth)
						{
							deepestNode = (*it);
						}		

						n = (*it);
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