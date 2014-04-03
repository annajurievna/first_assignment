#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
using namespace std;

void cp_recurcive (DIR *dir, char* path_from, char *path_to) {
    struct dirent *dent;
    while ( (dent = readdir(dir)) != NULL ) {
        if ( (strcmp(dent->d_name, ".") == 0 ) || (strcmp(dent->d_name, "..") == 0) )
            continue;

        char new_path_from[512] = "";
        strcat(new_path_from, path_from);
        strcat(new_path_from, "/");
        strcat(new_path_from, dent->d_name);
        char new_path_to[512] = "";
        strcat(new_path_to, path_to);
        strcat(new_path_to, "/");
        strcat(new_path_to, dent->d_name);

        struct stat sb;
        if ( stat(new_path_from, &sb) == -1 ) {
            printf("Stat read %s\n", dent->d_name);
            continue;
        }
        if (S_ISDIR( sb.st_mode) == 1) {
            DIR *new_dir = opendir(new_path_from);
            if ( new_dir == NULL ) {
                printf("Can't open %s\n", new_path_from);
                continue;
            }
            int make = mkdir(new_path_to, S_IRWXU|S_IRGRP|S_IXGRP);
            if (make == - 1) {
                printf("\n Can't make a directory %s\n", new_path_to);
                continue;
            }
            printf("\nMaking dir %s\n", new_path_to);
            cp_recurcive(new_dir, new_path_from, new_path_to);
        }
        else {
            printf("\nCoping %s to %s\n", new_path_from, new_path_to);
            int file_r = open(new_path_from, O_RDONLY);
            if (file_r == -1) {
                printf("\n Can't open %s\n", new_path_from);
                continue;
            }
            struct stat sb;
            if ( stat(new_path_from, &sb) == -1 ) {
                printf("stat problem\n");
                continue;
            }
            int file_w = open(new_path_to, O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE);
            if (file_w == -1) {
                printf("\n Can't open and create %s\n", new_path_to);
                continue;
            }
            int size_of_file = (long)sb.st_size;
            int size_buf = 256;
            void *buffer;
            buffer = malloc(sizeof(char) * size_buf);
            int count = 1;
            int size_rd, size_wr;
            while ( size_of_file > count * size_buf ) {
            	size_rd = read(file_r, buffer, size_buf);
            	size_wr = write(file_w, buffer, size_buf);
            	if ( size_rd != size_wr) 
            		printf("Problem with coping\n");
            	free(buffer);
            	buffer = malloc(sizeof(char) * size_buf);
            	++ count;
            }
            free(buffer);
            if ( size_of_file > (count - 1) * size_buf ) {
            	buffer = malloc(sizeof(char) * size_buf);
            	int size_left = size_of_file - (count - 1) * size_buf;
            	size_rd = read(file_r, buffer, size_left);
            	size_wr = write(file_w, buffer, size_left);
            	if ( size_rd != size_wr) 
            		printf("Problem with coping\n");
            	free(buffer);
            }
            close(file_r);
        }
    }
}

int main( int argc, char* argv[] ) {

    if (argc < 3) {
        printf("Not enough arguments. \n");
        exit(1);
    }
    if ( strcmp(argv[1], argv[2]) == 0 ) {
        printf("Incorrect input!");
        exit(1);
    }
    char *pointer = strrchr(argv[1], '/');
    char new_dir[512] = "";
    strcat(new_dir, argv[2]);
    strcat(new_dir, pointer);
    int make = mkdir(new_dir, S_IRWXU|S_IRGRP|S_IXGRP);
    if (make == - 1) {
        printf("\n Can't make a directory %s\n", new_dir);
        exit(1);
    }
    DIR *dir = opendir(argv[1]);
    if ( dir == NULL ) {
        printf("Can't open %s\n", argv[1]);
        exit(1);
    }
    cp_recurcive(dir, argv[1], new_dir);
    return 0;
}
