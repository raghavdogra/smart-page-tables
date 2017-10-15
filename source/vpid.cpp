#include <algorithm>
#include <iostream>
#include <map>
#include <iterator>
#include "../include/vpid.hpp"
using namespace std;

map<int,int> vpid;
map<int, int> count_pages_per_proc;


static int next_vpid = -1;
/*returns the next free vpid. Can be changed in future*/

int next_free_vpid(void) {
    cout << endl;
    next_vpid = (next_vpid + 1)% NUM_SLOTS;

    cout << "testing vpid:key:"<< next_vpid << "-value-" << vpid[next_vpid]<<endl;

    if (count_pages_per_proc.find(vpid[next_vpid]) == count_pages_per_proc.end()) {
        // not found in count pages totally new process
        cout << "could not find proc in count_pages_per_proc ";
        return next_vpid;
    }
    // found
    // count = 0
    if (count_pages_per_proc[vpid[next_vpid]] == 0) {
        cout << "found in count_pages_per_proc but count of pages is 0 ";
        return next_vpid;
    }
    // count != 0 so keep looking
    // TODO: check again
    for (auto it = vpid.begin() ; it != vpid.end(); it++) {
        if ((count_pages_per_proc.find(it->second) == count_pages_per_proc.end()) || (count_pages_per_proc[it->second] == 0)) {
            next_vpid = it->first;
            cout << "found but count was not 0, looking for more vpids ";
            return next_vpid;
        }
    }

    return -1; 

}
    


/* inserts the pid into vpid map, 
** returns the vpid assigned to the pid, -1 if pid already mapped to a vpid 
*/

int insert_pid(int pid) {
    if(vpid.find(pid) != vpid.end()) {
	std::cout << "error: PID " << pid << "already mapped to a VPID!" <<std::endl;
	return -1;
    }
    int vpid_val = next_free_vpid();
    vpid.insert(pair <int,int> (pid,vpid_val));
    return vpid_val;
}


/* returns the vpid corresponding to a pid, -1 otherwise */
int get_vpid(int pid) {
    auto itr = vpid.find(pid);
    if(itr != vpid.end()) {
	return itr->second;
    }
    else {
	cout << "error: PID not in vpid table";
	return -1;
    }
}

void remove_pid(int pid) {
    vpid.erase(pid);
    return;
}

void display_vpid (void) {
    if(vpid.empty())
	return;
    for(auto itr = vpid.begin();itr!=vpid.end();itr++) {
	cout << itr->first << " -> " << itr->second << std::endl;
    }
    return;
}

/*
int main() {
    int pids[] = {30000,21212,31231,12334,17657,1543,12,434,123,439,43,5236,88};    
    for(int i=0;i<13;i++) {
	insert_pid(pids[i]);
    }
    display_vpid();
    return 0;
}
*/
