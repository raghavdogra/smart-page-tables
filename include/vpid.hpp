#ifndef __VPID_HPP
#define __VPID_HPP
#include <map>
extern int next_vpid;
#define NUM_SLOTS       256

int next_free_vpid(void);
int insert_pid(int pid);


/* returns the vpid corresponding to a pid, -1 otherwise */
int get_vpid(int pid);

void remove_pid(int pid);

void display_vpid (void);


extern std::map<int,int> vpid;

extern std::map<int, int> count_pages_per_proc;
#endif