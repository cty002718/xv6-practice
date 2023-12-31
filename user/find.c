#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *dir, char *file) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    
    if ((fd = open(dir, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "find: not a directory: %s\n", dir);
        close(fd);
        return;
    }

    if (strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: path too long\n");
        return;
    }
    strcpy(buf, dir);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        if (st.type == T_DIR) {
            find(buf, file);
        } else if (strcmp(de.name, file) == 0) {
            printf("%s\n", buf);
        }
    }
    
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find directory file_name");
        exit(1);
    }

    find(argv[1], argv[2]);
    return 0;
}