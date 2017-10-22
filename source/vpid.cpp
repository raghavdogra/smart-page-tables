#include <algorithm>
#include <iostream>
#include <map>
#include <iterator>
#include "../include/vpid.hpp"
using namespace std;

map<int,int> vpid;
map<int, int> count_pages_per_proc;

static int rehash_count = 0;
static int next_vpid = -1;
/*returns the next free vpid. Can be changed in future*/

int next_free_vpid(int line_num) {
    next_vpid = (next_vpid + 1)% NUM_VPIDS;

    cout << line_num << ".testing vpid:key:" << next_vpid << "-value-" << vpid[next_vpid]<<endl;

    if (count_pages_per_proc.find(vpid[next_vpid]) == count_pages_per_proc.end()) {
        // not found in count pages totally new process
        cout << line_num << ".could not find proc in count_pages_per_proc " << endl;
        return next_vpid;
    }
    // found
    // count != 0 so keep looking
    for (auto it = vpid.begin() ; it != vpid.end(); it++) {
        if (count_pages_per_proc.find(it->second) == count_pages_per_proc.end()) {
            next_vpid = it->first;
            cout << line_num << ".found but count was not 0, looking for more vpids " << endl;
            return next_vpid;
        }
    }
    
    int old_num_vpids = NUM_VPIDS;
    for(int j = old_num_vpids; j < NUM_VPIDS; j++) {
        vpid.insert({j, -1});
    }
    cout << line_num << ".NEED FOR REHASH - rehash count "<< rehash_count << endl;
    rehash_count++;
    NUM_VPIDS = NUM_VPIDS * 2;
    NUM_BLK_PER_MEM = NUM_VPIDS;
    BLK_SIZE = (MEM_SIZE/NUM_MEMS)/NUM_BLK_PER_MEM;
    NUM_PAGES_PER_BLK = BLK_SIZE/PAGE_SIZE;
    PAGE_t *newram = new PAGE_t[2 * NUM_PAGES_PER_MEM];
    for(int i = 0; i < 2*NUM_PAGES_PER_MEM; i++) {
	newram[i].valid = ram[i].valid;
	newram[i].pid = ram[i].pid;
	newram[i].vaddr = ram[i].vaddr;
    }
    delete[] ram;
    ram = new PAGE_t[2 * NUM_PAGES_PER_MEM];
    for (int i = 0; i < NUM_PAGES_PER_MEM; i++) {
	ram[i].valid = false;
    }
    for(int i = 0; i < 2 * NUM_PAGES_PER_MEM; i++) {
	if(newram[i].valid) {
		cuckoo(newram[i].pid, newram[i].vaddr, get_vpid(newram[i].pid) , 1, i);	
	}
    }
    cout << "---------------------------------------" << endl;
    next_vpid = old_num_vpids;
    cout << "REHASh DONE" << endl;
    return next_vpid;

}

/* inserts the pid into vpid map, 
** returns the vpid assigned to the pid, -1 if pid already mapped to a vpid 
*/
/*
int insert_pid(int pid) {
    if(vpid.find(pid) != vpid.end()) {
	std::cout << "error: PID " << pid << "already mapped to a VPID!" <<std::endl;
	return -1;
    }
    int vpid_val = next_free_vpid();
    vpid.insert(pair <int,int> (pid,vpid_val));
    return vpid_val;
}
*/

/* returns the vpid corresponding to a pid, -1 otherwise */
int get_vpid(int pid) {
    for(auto itr = vpid.begin(); itr != vpid.end(); itr++) {
        if (itr->second == pid) return itr->first;
    }
    cout << "pid:" << pid << " Not found in vpid table" << endl;
    return -1;
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
