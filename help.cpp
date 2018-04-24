#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

void determineArgs(string sequenceFile, string alphabetFile, string *header, string *sequence, string *alphabet)
{
	ifstream sequenceStream;
	ifstream alphabetStream;

	sequenceStream.open(sequenceFile);
	alphabetStream.open(alphabetFile);

	getline(sequenceStream, *header);
	*header = header->substr(1);

	string tmp;
	while(!sequenceStream.eof())
	{
		sequenceStream >> tmp;
		*sequence += tmp;
	}

	while(!alphabetStream.eof())
	{
		alphabetStream >> tmp;
		*alphabet += tmp;
	}
}