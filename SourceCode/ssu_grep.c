#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "ssu_function.h"
#include "ssu_shell.h"
#include "ssu_grep.h"


//print usage
void pr_grepUsage(void){
	printf("Usage: ssu_grep STRING FILE... [OPTION]\n");
	printf("   -d: delete file that has STRING\n");
	printf("   -e NAME: don't check file that includes NAME in file name\n");
	printf("   -i: ignore case distinctions\n");
	printf("   -n: print line number with output line\n");
	printf("   -r: check all file in directory\n");
}

//read file
void grep(struct grepSignal *gs, char* string, char* filename, char* pathname){
	char *path;
	char buf[BUFSIZE];
	char *line;
	char *iLine;
	char *iString;
	char c;
	FILE *file;
	int linenum = 0;
	int ok = 0;
	int i;
	int fd;

	path = (char *)malloc(strlen(filename) + strlen(pathname) + 2);
	strcpy(path, pathname);
	strcat(path, "/");
	strcat(path, filename);

	if((file = fopen(path, "r")) == NULL){
		printf("ssu_grep: %s: No such file or directory\n", filename);
		free(path);
		return;
	}

	if((fd = open(path, O_RDONLY)) > 0){
		while(read(fd, &c, 1) > 0){
			if(c =='\0'){
				printf("binary file %s matches\n", path);
				free(path);
				return;
			}
		}
		close(fd);
	}

	// i option
	if(gs->is_i == 1){
		iString = (char *)malloc(strlen(string)+2);
		for(i = 0; i < strlen(string); i++){
			iString[i] = (char)tolower(string[i]);		
		}
		iString[i] = '\0';
	}

	//buf = (char *)malloc(BUFSIZE);
	while(!feof(file)) {
		if((line = fgets(buf, BUFSIZE, file)) != NULL){
			linenum++;
			//i option
			if(gs->is_i == 1){
				iLine = (char *)malloc(strlen(line)+2);
				for(i=0; i<strlen(line); i++){
					iLine[i] = (char)tolower(*(line+i));
				}
				iLine[i] = '\0';
				if(strstr(iLine, iString) != NULL){
					if(gs->is_r == 1) printf("%s:", path);
					if(gs->is_n == 1) printf("%d:", linenum);
					printf("%s", line);
					ok++;
				}
				free(iLine);
			}
			//no i option
			else{
				if(strstr(line, string) != NULL){
					if(gs->is_r == 1) printf("%s:", path);
					if(gs->is_n == 1) printf("%d:", linenum);
					printf("%s", line);
					ok++;
				}
			}
		}
	}

	//memory free, option execute
	if(gs->is_i == 1)
		free(iString);
	
	if(gs->is_d == 1 && ok != 0 && gs->is_sym == 0)
		remove(path);

	if(gs->is_d == 1 && gs->is_sym == 1 && ok != 0)
		gs->is_symrm = 1;
	if(path != NULL)
		free(path);
	if(file != NULL)
		fclose(file);
}

// dir search
void dirGrep(struct grepSignal* gs, char* string, char*filename, char* pathname){
	DIR *dir_info;
	struct dirent *dir_entry;
	struct stat file_info;
	char *path;

	//exception : if filename ‘.’
	path = (char *)malloc(strlen(filename) + strlen(pathname) + 2);
	strcpy(path, pathname);
	if((filename[0] == '.') && (strlen(filename) == 1)){
	}
	else{
		strcat(path, "/");
		strcat(path, filename);
	}
	dir_info = opendir(path);

	if(dir_info != NULL){
		while(dir_entry = readdir(dir_info)){
			// '.', '..' don't read
			if(strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
				continue;
			checkFile(gs, string, dir_entry->d_name, path);
		}
	}

	closedir(dir_info);
	free(path);
}

//check file format
void checkFile(struct grepSignal* gs, char *string, char *filename, char *pathname){
	char *path;
	struct stat file_info;
	struct stat sym_info;

	path = (char *)malloc(strlen(filename) + strlen(pathname) + 2);
	strcpy(path, pathname);
	if((filename[0] == '.') && (strlen(filename) == 1)){
	}
	else{
		strcat(path, "/");
		strcat(path, filename);
	}

	//e option.
	if(gs->is_e == 1 && strstr(filename, gs->name))
		return;

	if(access(path, F_OK) == -1){
		printf("ssu_grep: %s: No such file or directory\n", filename);
		return;
	}

	if(lstat(path, &file_info) < 0){
		fprintf(stderr, "lstat err.\n");
		return;
	}

	if(S_ISREG(file_info.st_mode)){
		grep(gs, string, filename, pathname);
	}
	else if(S_ISDIR(file_info.st_mode)){
		// r option
		printf("%s is directory.\n", filename);
		if(gs->is_r == 1){
			dirGrep(gs, string, filename, pathname);
		}
	}
	else if(S_ISCHR(file_info.st_mode)){
		if(gs->is_r == 1) printf("%s/:", pathname);
		printf("%s is character special file.\n", filename);
	}
	else if(S_ISBLK(file_info.st_mode)){
		if(gs->is_r == 1) printf("%s/:", pathname);
		printf("%s is block special file.\n", filename);
	}
	else if(S_ISFIFO(file_info.st_mode)){
		if(gs->is_r == 1) printf("%s/:", pathname);
		printf("%s is FIFO.\n", filename);
	}
	else if(S_ISLNK(file_info.st_mode)){
		stat(path, &sym_info);
		gs->is_sym = 1;

		if(S_ISREG(sym_info.st_mode)){
			grep(gs, string, filename, pathname);
		}
		else if(S_ISDIR(sym_info.st_mode)){
			printf("%s is directory.\n", filename);
		}
		else if(S_ISCHR(sym_info.st_mode)){
			if(gs->is_r == 1) printf("%s/:", pathname);
			printf("%s is character special file.\n", filename);
		}
		else if(S_ISBLK(sym_info.st_mode)){
			if(gs->is_r == 1) printf("%s/:", pathname);
			printf("%s is block special file.\n", filename);
		}
		else if(S_ISFIFO(sym_info.st_mode)){
			if(gs->is_r == 1) printf("%s/:", pathname);
			printf("%s is FIFO.\n", filename);
		}
		else if(S_ISSOCK(sym_info.st_mode)){
			if(gs->is_r == 1) printf("%s/:", pathname);
			printf("%s is socket.\n", filename);
		}
		else{
		}

		if(gs->is_symrm == 1){
			unlink(path);
			gs->is_symrm = 0;
		}

		gs->is_sym = 0;
	}
	else if(S_ISSOCK(file_info.st_mode)){
		if(gs->is_r == 1) printf("%s/:", pathname);
		printf("%s is socket.\n", filename);
	}
	else{
	}
	if(path != NULL)
		free(path);
}

void ssu_grep(int argc, char *argv[]){
    struct grepSignal gs;
	gs.is_d = 0;
    gs.is_e = 0;
    gs.is_i = 0;
    gs.is_n = 0;
	gs.is_r = 0;
	gs.is_sym = 0;
	gs.is_symrm = 0;
	int fnum = 0;
	char *fname[8];
	char *path = ".";
	int i, j;
	struct stat file_info;

	if(argc < 3){
		pr_grepUsage();
		return;
	}

	for(i=2; i<argc; i++){
		if(*argv[i] == '-'){
			for(j=1; j < strlen(argv[i]); j++){
				if(*(argv[i]+j) == 'd'){
					gs.is_d = 1;
				}
				else if(*(argv[i]+j) == 'e'){
					if(strlen(argv[i]) == 2){
						if(i == (argc-1))
						{
							pr_grepUsage();
							return;
						}
						gs.is_e = 1;
						gs.name = argv[i+1];
						i++;
						break;
					}
					else{
						pr_grepUsage();
						return;
					}
				}
				else if(*(argv[i]+j) == 'i'){
					gs.is_i = 1;
				}
				else if(*(argv[i]+j) == 'n'){
					gs.is_n = 1;
				}
				else if(*(argv[i]+j) == 'r'){
					gs.is_r = 1;
				}
				else{
					printf("unknown option : %s\n", argv[i]);
					pr_grepUsage();
					return;
				}
			}
		}
		else{
			fname[fnum] = argv[i];
			fnum++;
		}
	}

	if(fnum == 0){
		pr_grepUsage();
		return;
	}

	for(i=0; i<fnum; i++){
		checkFile(&gs, argv[1], fname[i], path);	
	}
}
