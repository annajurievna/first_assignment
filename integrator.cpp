#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

double F(double x) {
	return x * x;
}

double integrate(double a, double b, double delta){
	double result = 0;
	double x = a;
	while (x < b) {
        result += delta*F(x);
        x += delta;
    }
	return result;
}

double integrator(double a, double b, double delta, int divide){
	int fd[2];
	int pipe_result = pipe(fd);
	if (pipe_result == -1) {
        printf("Pipe error\n");
        exit(0);
    }
	for(int i = 0; i < divide; ++i){
		int prid = fork();
		if( prid == -1 ){
				printf("Fork error\n");
				exit(0);
        }
		if( prid == 0 ){
			close(fd[0]);
			double beg_int = a + (b - a) / divide * i;
			double end_int = a + (b - a) / divide * (i + 1);
			double integrate_result = integrate(beg_int, end_int, delta);
			write(fd[1], &integrate_result, sizeof(integrate_result));
			close(fd[1]);
			exit(1);
		}
	}
	close(fd[1]);
	double current_result = 0;
	double total_result = 0;
	for(int i = 0; i < divide; ++i){
		if (read(fd[0], &current_result, sizeof(current_result)) != sizeof(current_result)) {
            printf("Read error\n");
            exit(0);
        }
        total_result += current_result;
	}
	close(fd[0]);
	return total_result;
}

int main (int argc, char *argv[]){
    if ( argc < 3 ) {
        printf("Not enough arguments\n");
        return 0;
    }
    double a, b;
    if ( argv[1][0] == '-' ) {
    	a = atoi(argv[1] + 1);
    	a -= 2 * a;
    }
    else
    	a = atoi(argv[1]);
    if ( argv[2][0] == '-' ) {
    	b = atoi(argv[2] + 1);
    	b -= 2 * b;
    }
    else
    	b = atoi(argv[2]);
    int divide = atoi(argv[3]);
    double delta = 0.00000001;
	double intgr = integrator(a, b, delta, divide);
	printf("%f\n", intgr);
	return 0;
}
