#include<iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace std;

void parseFile(char *fileName) {
	ifstream myfile (fileName);
	string line;
	if (myfile.is_open()) {
		while ( getline (myfile,line) ) {
			vector<string> tokens;
			istringstream iss(line);
			copy(istream_iterator<string>(iss),
     			istream_iterator<string>(),
     			back_inserter(tokens));
			for(int i = 0; i < tokens.size(); i++) {
				cout << tokens[i] << endl;
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file"; 
}

int main(int argc, char* argv[]) {
	if(argc < 4) {
		cout<< "Usage: simulate logfilename RAM(GB) Pagesize(KB)" << endl;
		exit(0);
	}
	char *fileName = argv[1];
	int ram = atoi(argv[2]);
	int pageSize = atoi(argv[3]);
	parseFile(fileName);
	return 0;
}
