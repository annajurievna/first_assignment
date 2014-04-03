#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

using namespace std;

void ls_R (DIR *dir, char* path) {
    printf(" \n \n %s: \n", path);
    struct dirent *dent;
    while ( (dent = readdir(dir)) != NULL ) {
        if ( (strcmp(dent->d_name, ".") != 0 ) && (strcmp(dent->d_name, "..") != 0) )
            printf("   %s ", dent->d_name);
    }
    closedir(dir);
    struct stat sb;
    DIR *dir_copy = opendir(path);
    while ( (dent = readdir(dir_copy)) != NULL) {
        if ( (strcmp(dent->d_name, ".") == 0 ) || (strcmp(dent->d_name, "..") == 0) )
            continue;
        char new_path[512] = "";
        strcat(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, dent->d_name);

        struct stat sb;
        if ( stat(new_path, &sb) == -1 ) {
            printf("Stat read %s\n", dent->d_name);
            continue;
        }
        if (S_ISDIR( sb.st_mode) == 1) {
            DIR *new_dir = opendir(new_path);
            if ( new_dir == NULL ) {
                printf("Can't open %s\n", new_path);
                continue;
            }
            ls_R(new_dir, new_path);
        }
    }
    closedir(dir_copy);
}

int main ( int arvc, char* argv[]) {

    char cwd[512];
    if (getcwd(cwd, 512) == NULL) { // имя текущей директориитеперь записано в cwd
        printf("can't get current directory");
        exit(1);
    }
    else
        printf("current working directory is: %s\n", cwd);
    DIR *dir;
    dir = opendir(cwd);
    if ( dir == NULL ) {
        printf("Can't open current directory\n");
        exit(1);
    }
    struct dirent *dent;

    if ( arvc > 1 && argv[1][0] == '-' && argv[1][1] == 'l') {
        while ( (dent = readdir(dir)) != NULL ) {
            if ( (strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..") == 0) )
                continue;
            struct stat sb;
            if ( stat( dent->d_name, &sb) == -1) {
                printf("Can't read %s \n", dent->d_name);
                continue;
            }
            switch (sb.st_mode & S_IFMT) {
                case S_IFBLK:  printf("b"); break;
                case S_IFCHR:  printf("c"); break;
                case S_IFDIR:  printf("d"); break;
                case S_IFIFO:  printf("p"); break;
                case S_IFLNK:  printf("l"); break;
                case S_IFREG:  printf("-"); break;
                case S_IFSOCK: printf("s"); break;
                default:       printf("unknown?\n"); break;
            }
            if (sb.st_mode & S_IRUSR)
                printf("r");
            else
                printf("-");
            if (sb.st_mode & S_IWUSR)
                printf("w");
            else
                printf("-");
            if (sb.st_mode & S_IXUSR)
                printf("x");
            else
                printf("-");
            if (sb.st_mode & S_IRGRP)
                printf("r");
            else
                printf("-");
            if (sb.st_mode & S_IWGRP)
                printf("w");
            else
                printf("-");
            if (sb.st_mode & S_IXGRP)
                printf("x");
            else
                printf("-");
            if (sb.st_mode & S_IROTH)
                printf("r");
            else
                printf("-");
            if (sb.st_mode & S_IWOTH)
                printf("w");
            else
                printf("-");
            if (sb.st_mode & S_IXOTH)
                printf("x");
            else
                printf("- ");

            printf("%ld ", (long) sb.st_nlink);

            struct passwd *p;
            if ( (p = getpwuid(sb.st_uid)) == NULL )
                printf("Can't get user's name\n");
            else
                printf("%s ", p->pw_name);

            struct group *g;
            if ( (g = getgrgid(sb.st_gid)) == NULL)
                printf("can't get group's name\n");
            else
                printf( "%s ", g->gr_name);

            printf("%lld bytes ", (long long) sb.st_size);
            printf("%s", ctime(&sb.st_mtime));
            printf("%s\n", dent->d_name);
        }
    }
   else {
        if (arvc > 1 && argv[1][0] == '-' && argv[1][1] == 'R') {
            ls_R(dir, cwd);
        }
        else {
            while (( dent = readdir(dir) ) != NULL) {
                if ( (strcmp(dent->d_name, ".") != 0) && (strcmp(dent->d_name, "..") != 0) )
                    printf("%s   ", dent->d_name);
            }
        }

    }
    printf("\n");
    return 0;
}
