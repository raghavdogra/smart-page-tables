#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include "../include/vpid.hpp"

using namespace std;



void parseFile(char *fileName) {
	int rehash_count = 0;
	ifstream myfile (fileName);
	string line;
	if (myfile.is_open()) {
		while ( getline (myfile,line) ) {
			vector<string> tokens;
			istringstream iss(line);
			copy(istream_iterator<string>(iss),
     		istream_iterator<string>(),
     		back_inserter(tokens));
    
			int pid = stoi(tokens[0], nullptr, 10);
			int numpages = stoi(tokens[2], nullptr, 10);
			
			if (tokens[1].compare("D") == 0) {
				// deallocation
				if (count_pages_per_proc.find(pid) != count_pages_per_proc.end()) {
					// meaning it is there in the map as expected
					count_pages_per_proc[pid] -= numpages;
				}
			}
			
			else if (tokens[1].compare("A") == 0) {
				// case 1: it is already there in the system and its count is not 0
				if ((count_pages_per_proc.find(pid) != count_pages_per_proc.end()) && count_pages_per_proc[pid] != 0) {
					count_pages_per_proc[pid] += numpages;
					continue;
				}
				// case 2: new process
				int new_vpid = next_free_vpid();
				cout << " new vpid:"<<new_vpid<< endl;

				if (new_vpid == -1) rehash_count++;
				else {
					vpid.insert(std::map<int, int>::value_type(new_vpid, pid));
					cout << "testing vpid insertion:key:" << new_vpid<<"-value-"<<vpid[new_vpid]<<endl;
					count_pages_per_proc.insert({pid, numpages});
					cout << "testing count_pages_per_proc insertion:key:"<<pid << "-value-"<<count_pages_per_proc[pid]<<endl;
				}
			}
		

		}
		myfile.close();
		cout << "rehash count " << rehash_count << endl;
	}
	else cout << "Unable to open file"; 
}

int main(int argc, char* argv[]) {
	if(argc < 4) {
		cout<< "Usage: simulate logfilename RAM(GB) Pagesize(KB)" << endl;
		exit(0);
	}
	for (int i = 0;i < NUM_SLOTS; i++) {
		vpid.insert({i,-1});
	}
	char *fileName = argv[1];
	int ram = atoi(argv[2]);
	int pageSize = atoi(argv[3]);
	parseFile(fileName);

	return 0;
}
