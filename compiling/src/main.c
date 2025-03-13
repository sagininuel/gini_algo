#include <stdio.h>
#include "math.h"


static int current_location_counter;
static char * name = "Current location counter";


int main(){
	printf("Pointer to sum_t: %p\n", add);
	sum_t = add;
	int sum = sum_t(2,3);
	printf("Sum: %d\n", sum);
}

