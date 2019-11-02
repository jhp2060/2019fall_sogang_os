#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>

int main(int argc, char **argv){
	int i, arg[5];
	for (i = 1; i <= 4; i++) arg[i] = atoi(argv[i]);
	printf("%d %d\n", 
			fibonacci(arg[1]), sum_of_four_int(arg[1], arg[2], arg[3], arg[4]));

	return EXIT_SUCCESS;
}
