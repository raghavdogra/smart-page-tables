#ifndef __VPID_HPP
#define __VPID_HPP

#include <map>
extern int NUM_PAGES_PER_BLK;
extern int NUM_BLK_PER_MEM;
extern unsigned long PAGE_SIZE;
extern unsigned long BLK_SIZE;

extern unsigned long MEM_SIZE;

extern int NUM_PAGES_PER_MEM;
#define NUM_MEMS			2
extern int NUM_VPIDS;

int next_free_vpid(int line_num);
int insert_pid(int pid);


/* returns the vpid corresponding to a pid, -1 otherwise */
int get_vpid(int pid);

void remove_pid(int pid);

void display_vpid (void);

typedef struct {
	int pid;
	unsigned long vaddr;
	bool valid;
} PAGE_t;
extern PAGE_t * ram;

/*
typedef struct {
	PAGE_t page[NUM_PAGES_PER_BLK];
} BLOCK_t;

typedef struct {
	BLOCK_t block[NUM_BLK_PER_MEM];
} MEMORY_t;

extern MEMORY_t MEM[NUM_MEMS];
*/


extern std::map<int,int> vpid;

extern std::map<int, int> count_pages_per_proc;
void cuckoo(int pid, unsigned long vaddr, int new_vpid, int numpages, int debug_id);
#endif
