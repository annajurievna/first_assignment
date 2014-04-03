#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

const int size_of_file = 100;

extern __inline__ int64_t rdtsc()//Магия для вычисления времени
{
   int64_t x;
   __asm__ __volatile__ ("rdtsc\n\tshl $32, %%rdx\n\tor %%rdx, %%rax" : "=a" (x) : : "rdx");
   return x;
}

int main (int arvc, char *argv[]) {
    if (arvc < 2) {
        printf("Not enough arguments\n");
        return 0;
    }
    int size_of_block = atoi(argv[1]); //размер блока, который будет передаваться через pipe
    printf("welcome\n");
    int64_t clock_beg;
    int64_t clock_end;
    int fd[2];
    int pipe_result = pipe(fd);
    if (pipe_result == -1) {
        printf("Pipe error\n");
        exit(0);
    }
    int pid = fork();
    if (pid == -1) {
        printf("Fork error\n");
        exit(0);
    }
    if (pid == 0) {
        close(fd[0]); //закрыли на чтение
        int size_wr;
        int count = 1; //сколько раз отправляли блок размера size_sent;
        void *buffer;
        clock_beg = rdtsc();
        while ( size_of_block * count <= size_of_file ) {
            buffer = malloc(sizeof(char) * size_of_block);
            size_wr = write(fd[1], buffer, size_of_block);
            free(buffer);
            ++ count;
        }
        if ( size_of_file > (count - 1) * size_of_block ) {
            buffer = malloc(sizeof(char) * size_of_block);
            int size_left = size_of_file - (count - 1) * size_of_block;
            size_wr = write(fd[1], buffer, size_left);
            free(buffer);
        }
        clock_beg = rdtsc();
        printf("Block size: %d  Time: %ld\n", size_of_block, clock_end - clock_beg);
        close (fd[1]);
    }
    else {
        close (fd[1]);
        void *buffer;
        buffer = malloc(sizeof(char) * size_of_block);
        int size_rd;
        while ( (size_rd = read(fd[0], buffer, size_of_block)) != 0 ) ;
        close (fd[0]);
        free(buffer);
    }
    return 0;
}
