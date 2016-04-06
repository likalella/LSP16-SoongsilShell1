#ifndef SSU_GREP_H
#define SSU_GREP_H

struct grepSignal{
	int is_d;
	int is_e;
	int is_i;
	int is_n;
	int is_r;
	int is_sym;
	int is_symrm;
	char *name;
};

void grep(struct grepSignal *, char* , char* , char*);
void dirGrep(struct grepSignal *, char* , char* , char*);
void checkFile(struct grepSignal *, char* , char* , char*);

#endif
