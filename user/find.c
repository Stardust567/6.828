#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path) {
    char *p;
    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    return ++p;
}

void find(char* path, char* name) {
	int fd;
	struct stat st;
	struct dirent de;
	char buf[512];

	if ((fd = open(path, 0)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
    
	if (fstat(fd, &st) < 0) {
	    fprintf(2, "find: cannot stat %s\n", path);
	    close(fd);
	    return;
	}

    switch (st.type) {
	    case T_FILE:
	        if (strcmp(fmtname(path), name) == 0) printf("%s\n", path);
	        break;
	    case T_DIR:
	        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
	            fprintf(2, "find: path too long\n");
	            break;
	        }
	        strcpy(buf, path);
	        char* p = buf + strlen(buf);
	        *p++ = '/';
	        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
	            if (de.inum == 0) continue;
	            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
	            memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                find(buf, name);
	        }
	        break;
    }
	close(fd);
}

int main(int argc, char *argv[]) {
    if(argc <= 2) fprintf(2, "find: not enough params provided");
    find(argv[1], argv[2]);
    exit();
}
