#ifndef __VPID_HPP
#define __VPID_HPP

#include <map>
#define NUM_PAGES_PER_BLK	512
#define NUM_BLK_PER_MEM		256
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

typedef struct {
	PAGE_t page[NUM_PAGES_PER_BLK];
} BLOCK_t;

typedef struct {
	BLOCK_t block[NUM_BLK_PER_MEM];
} MEMORY_t;

extern MEMORY_t MEM[NUM_MEMS];

extern std::map<int,int> vpid;

extern std::map<int, int> count_pages_per_proc;

#endif