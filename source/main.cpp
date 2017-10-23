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

int NUM_PAGES_PER_BLK;
int NUM_BLK_PER_MEM;
unsigned long PAGE_SIZE;
unsigned long BLK_SIZE;
unsigned long MEM_SIZE;
int NUM_PAGES_PER_MEM;
int NUM_VPIDS;
PAGE_t *ram;
static long long perfCount = 0;
static long long allocCount = 0;
static long long cont = 0;

void computePerf();

/* 
In case we change the hashing scheme, this should return H(tableid, pid) + vaddr
*/
int hash(int tableid, int pid, unsigned long vaddr, int blk_num) {
	return blk_num;
}


void delete_one_page( int pid, unsigned long vaddr, int blk_num, int try_num, int max_tries, int debug_id ) {
	if (try_num == max_tries) {
		/* Just printing for now, need to retry with next page */
		cout << debug_id << ".could not find allocation for vaddr:" << vaddr << endl;
		return;
	}

	/* Check if this pid and the same vaddr is already present in tbl1 or tbl2 */
	for( int i = 0; i < NUM_MEMS; i++ ) {
		if ( ram[ (i * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + (vaddr % BLK_SIZE) / PAGE_SIZE].pid == pid
			 && ram[ (i * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + (vaddr % BLK_SIZE) / PAGE_SIZE].valid == true
			 && ram[ (i * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + (vaddr % BLK_SIZE) / PAGE_SIZE].vaddr == vaddr ) {

			ram[ (i * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + (vaddr % BLK_SIZE) / PAGE_SIZE].valid = false;
			cout << debug_id << ".removal successful" << endl;
			return;
		}
	}
	
	delete_one_page( pid, vaddr, (blk_num + 1) % NUM_BLK_PER_MEM, try_num + 1, max_tries, debug_id );
	
	return;
}


void delete_pages(int pid, int key, int numpages, unsigned long vaddr, int debug_id) {
	for(int i = 0; i < numpages; i++)
		delete_one_page(pid, vaddr + (PAGE_SIZE * i), key, 0, NUM_VPIDS, debug_id);
	return;
}

/* 
pid, vaddr of page to be placed in phys mem, the block it is to be placed in, the first 500 MB
or the second 500 MB , the count of dislocations so far, total number of tries that if exceeded
implies we are caught in a circle
*/
int place( int pid, unsigned long vaddr, int blk_num, int tbl_num, int try_num, int max_tries, int debug_id, unsigned long offset ) {
	if (try_num == max_tries) {
		/* Just printing for now, need to retry with next page */
		cout << debug_id << ".max tries exceeded for vaddr:" << vaddr << endl;
		return 1;
	}

	/* Check if this pid and the same vaddr is already present in tbl1 or tbl2 */
	for( int i = 0; i < NUM_MEMS; i++ ) {
		if ( ram[ (i * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].pid == pid
			 && ram[ (i * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].valid == true
			 && ram[ (i * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].vaddr == vaddr ) {
			cout << debug_id << ".already here" << endl;
			return 0;
		}
	}

	if ( ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].valid == true ) {
		
		int repl_pid = ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].pid;
		unsigned long repl_vaddr = ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].vaddr;
		ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].pid = pid;
		ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].vaddr = vaddr;
		cout << debug_id << ".pid:" << pid << ",vaddr:" << vaddr << " replacing pid:" << repl_pid << ",vaddr:" << repl_vaddr << endl;

		if (tbl_num != (NUM_MEMS - 1))
			return place( repl_pid, repl_vaddr, blk_num, (tbl_num + 1) % NUM_MEMS, try_num + 1, max_tries, debug_id, offset);
		else
			return place( repl_pid, repl_vaddr, (blk_num + 1) % NUM_BLK_PER_MEM, (tbl_num + 1) % NUM_MEMS, try_num + 1, max_tries, debug_id, offset);
	}
	
		/* empty page, so just place it here */
	ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].valid = true;
	ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].pid = pid;
	ram[ (tbl_num * NUM_PAGES_PER_MEM) + (blk_num * NUM_PAGES_PER_BLK) + offset].vaddr = vaddr;
	cout << debug_id << ".allocated pid:" << pid << ",vaddr:" << vaddr << " in tbl:" << tbl_num << ",blk:" << blk_num << endl;
	
	return 0;
}

void computePerf() {
	for(int i = 0; i < 2 *NUM_PAGES_PER_MEM - 1; i++ ) {
		
		if(!ram[i].valid) {
			//cout << "are we ever here 2" << endl;
			cont = 0;
		}
		else if(ram[i+1].vaddr == (ram[i].vaddr + 4096) && ram[i+1].pid == ram[i].pid) {
			cont++;
			//cout << "Are we ever here 1" << endl;
			if(cont == 512) {
				//cout << "Are we ever here" << endl;
				perfCount++;
				cont = 0;
			}
		}
		else {
			//cout << "are we ever here 2" << endl;
			cont = 0;
		}
	}
}

void cuckoo(int pid, unsigned long vaddr, int new_vpid, int numpages, int debug_id) {
	int ret = 0;
	unsigned long offset = (vaddr % BLK_SIZE) / PAGE_SIZE;

	for(int i = 0; i < numpages; i++) {

		ret = place(pid, vaddr + (PAGE_SIZE * i), new_vpid, 0, 0, NUM_VPIDS * 2, debug_id, offset);

		if (ret == 1) {

			int count = 0;
			while ((ret != 1) || (count < (NUM_PAGES_PER_BLK - 1))) {
				count++;
				ret = place(pid, vaddr + (PAGE_SIZE * i), new_vpid, 0, 0, NUM_VPIDS * 2, debug_id, (offset + 1) % NUM_PAGES_PER_BLK);
			}

		}
	}
	return;
}


void parseFile(char *fileName) {
	int rehash_count = 0;
	ifstream myfile (fileName);
	string line;
	int line_num = 0;
	int dealloc_bfr_alloc = 0;

	if (myfile.is_open()) {
		while ( getline (myfile,line) ) {
			line_num++;
			vector<string> tokens;
			istringstream iss(line);
			copy(istream_iterator<string>(iss),
     		istream_iterator<string>(),
     		back_inserter(tokens));
    		
			int pid = stoi(tokens[0], nullptr, 10);
			int numpages = stoi(tokens[2], nullptr, 10);
			unsigned long vaddr = stoul(tokens[3], nullptr, 16);

			cout<< endl;
			cout << line_num << ".New request arrived" << endl;
			cout << line_num << " " << tokens[0] << " " << tokens[1] << " " << tokens[2] << " " << tokens[3] << endl;
			// deallocation
			if (tokens[1].compare("D") == 0) {
				/*
				 *	if count_pages_per_proc map contains this process id and count of its pages is >= num
				 * 	of pages to dealloc then proceed else flag an error-why deallocate what u didnt allocate?
				 */
				if ((count_pages_per_proc.find(pid) != count_pages_per_proc.end()) && (count_pages_per_proc[pid] >= numpages)) {
					/*
					Reduce the count
					*/
					count_pages_per_proc[pid] -= numpages;

					cout << line_num << ".deallocated pages for pid:"<< pid << ", curr count of pages:" << count_pages_per_proc[pid] << endl;
					int key = get_vpid(pid);
					delete_pages(pid, key, numpages, vaddr, line_num);
					/*
					If count of pages for this process has been reduced to 0, delete it from map
					*/
					if (count_pages_per_proc[pid] == 0) {
						count_pages_per_proc.erase(pid);
						cout << line_num << ".erasing record for pid "<< pid<<endl;
						continue;
					}
				}
				else {
					/*
					Checking number of dealloc reqs that came bfr alloc reqs
					*/
					cout << line_num << ".dealloc req came before alloc" << endl;
					dealloc_bfr_alloc++;
				}
			}
			
			else if (tokens[1].compare("A") == 0) {
				/*
				If process is already in the system with non-zero page count, then proceed to increase its page count and continue
				*/
				if ((count_pages_per_proc.find(pid) != count_pages_per_proc.end()) && count_pages_per_proc[pid] != 0) {
					count_pages_per_proc[pid] += numpages;
					cout << line_num << ".process already there with non-0 count, pid:"<< pid << "-page cnt-" << count_pages_per_proc[pid] << endl;
					/* need to allocate pages here */
					int key = get_vpid(pid);
					cuckoo(pid, vaddr, key, numpages, line_num);
					allocCount++;
					computePerf();
					continue;
				}
				/*
				its a new process so fetch a new virtual pid - rethink this, a process would get a new pid every single time,
				hash function needs to always return the same value. new_vpid might change to H(pid)
				*/
				int new_vpid = next_free_vpid(line_num);
				cout << line_num << ".new vpid:" << new_vpid << endl;

				
				vpid[new_vpid] = pid;
				cout << line_num << ".testing vpid insertion:key:" << new_vpid << "-value-" << vpid[new_vpid] << endl;
				count_pages_per_proc.insert({pid, numpages});
				cout << line_num << ".testing count_pages_per_proc insertion:key:" << pid << "-value-" << count_pages_per_proc[pid] << endl;

				/* Starting with allocation */
				cuckoo(pid, vaddr, new_vpid, numpages, line_num);
				allocCount++;
				computePerf();
				
			}

			else {
				cout << line_num << ".ERROR!!!"<< endl;
			}

		}
		myfile.close();
		cout << endl << "rehash count " << rehash_count << endl;
		cout << "number of deallocations that came bfr allocations "<< dealloc_bfr_alloc << endl;
	}
	else cout << "Unable to open file"; 
}

int main(int argc, char* argv[]) {
	if(argc < 4) {
		cout<< "Usage: simulate logfilename RAM(GB) Pagesize(B)" << endl;
		exit(0);
	}

	NUM_VPIDS = 256;

	for (int i = 0;i < NUM_VPIDS; i++) {
		vpid.insert({i,-1});
	}

	char *fileName = argv[1];
	MEM_SIZE = atoi(argv[2]) * 1073741824;
	PAGE_SIZE = atoi(argv[3]);

	NUM_PAGES_PER_MEM = (MEM_SIZE/NUM_MEMS)/PAGE_SIZE; 
	// NUM_BLK_PER_MEM = (MEM_SIZE/NUM_MEMS)/NUM_VPIDS;

	NUM_BLK_PER_MEM = NUM_VPIDS;

	BLK_SIZE = (MEM_SIZE/NUM_MEMS)/NUM_BLK_PER_MEM;
	NUM_PAGES_PER_BLK = BLK_SIZE/PAGE_SIZE;
	
	ram = new PAGE_t[NUM_MEMS * NUM_PAGES_PER_MEM];
	// cout << "mem size:" << MEM_SIZE << ",page size:" << PAGE_SIZE << ",pages per mem:" << NUM_PAGES_PER_MEM << ",blks per mem:" << NUM_BLK_PER_MEM << ",blk size:" << BLK_SIZE << ",pages per blk:" << NUM_PAGES_PER_BLK << endl;
	for (int i = 0; i < NUM_PAGES_PER_MEM; i++) {
		ram[i].valid = false;
	}
	parseFile(fileName);
	delete [] ram;
	cout << endl << "perfCount: " << perfCount;
	double perf = (double) perfCount/ allocCount;
	cout << endl << "Perf: " << perf << endl;
	cout << endl << "cont:" << cont << endl;
	return 0;
}
