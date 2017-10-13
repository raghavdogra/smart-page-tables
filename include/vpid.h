#define NUM_SLOTS       255

extern static int next_vpid;


int next_free_vpid(void);
int insert_pid(int pid);


/* returns the vpid corresponding to a pid, -1 otherwise */
int get_vpid(int pid);

void remove_pid(int pid);

void display_vpid (void);
