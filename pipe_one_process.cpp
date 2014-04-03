#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

extern __inline__ int64_t rdtsc()//Магия для вычисления времени
{
   int64_t x;
   __asm__ __volatile__ ("rdtsc\n\tshl $32, %%rdx\n\tor %%rdx, %%rax" : "=a" (x) : : "rdx");
   return x;
}

int main (int arvc, char* argv[]) {
    unsigned int length = strlen( argv[1] );
    int current_buf = 1; // размер куска, который передается через pipe
    int fd[2];
    int pipe_result = pipe(fd);
    if (pipe_result == -1) {
        printf("Pipe error\n");
        exit(0);
    }
    while ( current_buf <= length ) {
        int i = 0;
        int64_t clockb;
        int64_t clocke;
        clockb = rdtsc();

        while ( i < length ) {
            char *buffer = (char*)malloc(sizeof(char) * current_buf);
            int actual_size; //нужно, для того чтобы не считать лишние символы
            if ( length - i < current_buf )
                actual_size = length - i;
            else
                actual_size = current_buf;
            int size_written = write(fd[1], argv[1] + i, actual_size);
            if ( size_written == -1 ) {
                printf("Write error\n");
                exit(0);
            }
            int size_read = read(fd[0], buffer, actual_size);
            if ( size_read == -1 ) {
                printf("Read error\n");
                exit(0);
            }
            i += current_buf;
            free(buffer);
         }
        clocke = rdtsc();
        printf("Block size: %d  Time: %ld\n", current_buf, clocke - clockb);
        ++current_buf;
    }
    return 0;
}
