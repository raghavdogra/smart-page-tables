#include <algorithm>
#include <iostream>
#include <map>
#include <iterator>
#include "../include/vpid.hpp"
using namespace std;
#define DEBUG 1
map<int,int> vpid;
map<int, int> count_pages_per_proc;

static int rehash_count = 0;
static int next_vpid = -1;
/*returns the next free vpid. Can be changed in future*/

int next_free_vpid(int line_num) {
    next_vpid = (next_vpid + 1)% NUM_VPIDS;


    if (count_pages_per_proc.find(vpid[next_vpid]) == count_pages_per_proc.end()) {
        // not found in count pages totally new process
        return next_vpid;
    }
    // found
    // count != 0 so keep looking
    for (auto it = vpid.begin() ; it != vpid.end(); it++) {
        if (count_pages_per_proc.find(it->second) == count_pages_per_proc.end()) {
            next_vpid = it->first;
            return next_vpid;
        }
    }
    
    int old_num_vpids = NUM_VPIDS;
    NUM_VPIDS = NUM_VPIDS * 2;
    for(int j = old_num_vpids; j < NUM_VPIDS; j++) {
        vpid.insert({j, -1});
    }
    rehash_count++;
    NUM_BLK_PER_MEM = NUM_VPIDS;
    BLK_SIZE = (MEM_SIZE/NUM_MEMS)/NUM_BLK_PER_MEM;
    NUM_PAGES_PER_BLK = BLK_SIZE/PAGE_SIZE;
    
    PAGE_t *newram = new PAGE_t[2 * NUM_PAGES_PER_MEM];
    // make a deep copy of ram in newram
    for(int i = 0; i < 2*NUM_PAGES_PER_MEM; i++) {
#ifdef DEBUG
	if ((ram[i].valid == true) && (get_vpid(ram[i].pid) == -1)) {
		cout << "HOLE IN HISTORY DETECTED pid:"<< ram[i].pid << endl;
		// check count pages per proc, the entry should not exist
		if (count_pages_per_proc.find(ram[i].pid) == count_pages_per_proc.end()) {
			cout << "entry not there in count pages per proc so not setting valid correctly when proc exits"<< endl;
		}
		else {
			cout << "entry found in count pages per proc so changed vpid table wrong" << endl; 
		}
	}
#endif
	newram[i].valid = ram[i].valid;
	newram[i].pid = ram[i].pid;
	newram[i].vaddr = ram[i].vaddr;
    }
    delete[] ram;
    // now going to rehash every thing to ram again
    ram = new PAGE_t[2 * NUM_PAGES_PER_MEM];
    for (int i = 0; i < (2*NUM_PAGES_PER_MEM); i++) {
	ram[i].valid = false;
    }
    for(int i = 0; i < 2 * NUM_PAGES_PER_MEM; i++) {
	if(newram[i].valid) {
		/*
		somehow it is valid in newram but its entry does not exist in vpid table
		*/
		int temp = get_vpid(newram[i].pid);
#ifdef DEBUG
		if (temp == -1) cout << "problem is here " << newram[i].pid << endl;
#endif
		cuckoo(newram[i].pid, newram[i].vaddr, temp , 1, i);	
	}
    }
    next_vpid = old_num_vpids;
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
#ifdef DEBUG
    cout << "ALERT!!!"<<endl;
#endif
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
