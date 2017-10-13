#include<iostream>
#include <stdlib.h>
using namespace std;



int main(int argc, char* argv[]) {
	if(argc < 4) {
		cout<< "Usage: simulate logfilename RAM(GB) Pagesize(KB)" << endl;
		exit(0);
	}
	char *fileName = argv[1];
	int ram = atoi(argv[2]);
	int pageSize = atoi(argv[3]);
	cout << fileName << ram << pageSize;
}
