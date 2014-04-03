#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main( int argc, char *argv[] ) {
	int fd[2];
	int pipe_result = pipe(fd);
	if ( pipe_result == -1) {
		printf("Pipe error\n");
		exit(0);
	}
	int proc_id = fork();
	if (proc_id == -1) {
		printf("Fork error\n");
		exit(0);
	}
	if (proc_id == 0) {
		close(STDOUT_FILENO);
		close(fd[0]);
		int dup2_result = dup2(fd[1], STDOUT_FILENO);
		if (dup2_result == -1) {
			printf("dup2 error\n");
			exit(0);
		}
		close(fd[1]);
		if ( execvp(argv[1], argv + 1) ) {
			printf("Exec error\n");
			exit(0);
		}
	}
	else {
		close(fd[1]);
		FILE *pipe_data = fdopen(fd[0], "r");
		int count_lines = 0;
		int next_char = getc(pipe_data);
		while ( feof(pipe_data) != 1 ) {
			if ( next_char == '\n' )
				++count_lines;
			putchar(next_char);
			next_char = getc(pipe_data);
		}
		close(fd[0]);
		fclose(pipe_data);
		printf("Number of lines: %d", count_lines);
	}
	return 0;
}
