#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <stdlib.h>
#include <sys/types.h>
#define COLOR_RESET  "\x1b[0m"
#define COLOR_RED    "\x1b[31m"



int grep(char *path, char *str) { //ищет вхождения подстроки str в сожержимое файла с адресом path
    FILE *file_r;
    file_r = fopen(path, "r");
    if ( file_r == NULL ) {
        printf("\n Can't open %s in grep\n", path);
        return 0;
    }
    fseek(file_r, 0, SEEK_END); //размер файла в символах
    int size_buf = ftell(file_r); // -// - //
    fseek(file_r, 0, SEEK_SET); // - // - //
    char *buffer = (char*)malloc(sizeof(char) * size_buf);
    fread(buffer, sizeof(char), size_buf, file_r);
    int index = 0; // внешний индекс, проходится по z функции
    int size_str = 0; //длина текущей строки
    while ( index < size_buf ) {
        while ( buffer[index] != '\n' ) { //если это не конец строки, увеличиваю индекс и длину текущей строки
            ++index;
            ++size_str;
        }
        //получили полную текущую строку
        int n = size_str + strlen(str) + 1; //длина z-функции для текущей строки ( с учетом символа $)
        char *s = (char*)malloc(sizeof(char) * n); // строка = подстрока, которую мы ищем + '$' + строка, в которой мы ищем подстроку
        int *z = (int*)malloc(sizeof(int) * n);
        for (int k = 0; k < n; ++k) //обнуляем z-функцию
            z[k] = 0;
        z[0] = strlen(str); //для первого символа z-функция равна длине подстроки
        //собираю строку s
        int  j = 0;
        for (j = 0; j < strlen(str); ++j)
            s[j] = str[j];
        s[j] = '$';
        for (int k = 0; k < size_str; ++k)
            s[j + 1 +k] = buffer [index - size_str + k];
        //сам алгоритм поиска вхождений подстроки
        for (int i = 1, l = 0, r = 0; i < n; ++i) {
            int k = i - l;
            if ( i <= r ) {
                if ( z[k] < r - i + 1)
                    z[i] = z[k];
                else
                    z[i] = r - i + 1;
            }
            while ( i + z[i] < n && s[ z[i] ] == s[ i + z[i] ])
                ++z[i];
            if (i + z[i] - 1 > r) {
                l = i;
                r = i + z[i] - 1;
            }
        }
        int i = strlen(str) + 1;
        bool check_str = false;
        for ( int j = i; j < n; ++j) {
        	if (z[j] == strlen(str))
        		check_str = true;
        }
        if ( check_str ) {
        	while (i < n) { //печатаю текущую строку, выделяя красным цветом вхождения подстроки
            	if ( z[i] == strlen(str) ) { //значит с i-ого символа начинается вхождение подстроки
         	    	for (int j = i; j < i + strlen(str); ++j)
                	    printf(COLOR_RED "%c", s[j]);
               		i+=strlen(str) - 1;
            	}
            	else
                	printf(COLOR_RESET "%c", s[i]);
            	++i;
        	}
        printf("\n");
        }
        size_str = 0; //обнуляю размер текущей строки
        ++index;
        free(s);
        free(z);
    }
    free(buffer);
	return 1;
}

void recursive_grep( DIR *dir, char *path, char *str ) {
    printf ("\n Checking %s \n", path);

    struct dirent *dent;
    while ( (dent = readdir(dir)) != NULL ) { //поиск всех файлов  вданной директории
        if ( ( strcmp( dent->d_name, "." ) != 0 ) && ( strcmp( dent->d_name, ".." ) != 0) ) {
            struct stat sb;
            char new_path[256] = "";
            strcat(new_path, path);
            strcat(new_path, "/");
            strcat(new_path, dent->d_name);
            if ( lstat(new_path, &sb) == -1) {
                printf("\n Stat problem with %s\n", new_path);
                exit(1);
            }
            if ( S_ISREG( sb.st_mode) == 1 ) {
                printf("\nGrep check %s\n", new_path);
                grep(new_path, str);
            }
        }
    }

    struct dirent *dent2;
  while ( (dent = readdir(dir)) != NULL ) { //поиск поддиректорий
        if ( (strcmp(dent->d_name, ".") != 0) && (strcmp(dent->d_name, "..") != 0) ) {
            char new_path[256] = "";
            strcat(new_path, path);
            strcat(new_path, "/");
            strcat(new_path, dent->d_name);

            struct stat sb;
            if ( lstat(new_path, &sb) == -1 ) {
                printf("Stat read %s\n", dent->d_name);
                exit(1);
            }
            if (S_ISDIR( sb.st_mode) == 1) {
                DIR *new_dir = opendir(new_path);
                if ( new_dir == NULL ) {
                    printf("Can't open %s\n", new_path);
                    exit(1);
                }
                recursive_grep(new_dir, new_path, str);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if ( argv[1][0] == '-' && argv[1][1] == 'R') {
        DIR* dir = opendir( argv[3] );
        recursive_grep( dir, argv[3], argv[2] );
    }
    else
        grep(argv[2], argv[1]);
    return 0;
}
