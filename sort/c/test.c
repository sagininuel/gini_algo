#include <stdio.h>
#include <stdlib.h>



void swap(int * a, int * b)
{
	int temp = *a; *a = *b; *b = temp;
}


int partition(int low_index, int high_index, int * array)
{
	int pivot = array[low_index];
	int track_big = low_index;
	int track_small = high_index;

	while(track_big < track_small)
	{
		while(array[track_small] <= pivot && track_small > low_index)
			track_small--;
		
		while(array[track_big] >= pivot && track_big < high_index)
			track_big++;

		if(track_big < track_small)
			swap(&array[track_big], &array[track_small]);
	}

	swap(&array[track_small], &array[low_index]);
	return track_small;
}


void sort(int low, int high, int * array)
{
	if(low < high)
	{
		int partition_index = partition(low, high, array);
		sort(partition_index + 1, high, array);
		sort(low, partition_index - 1, array);
	}
}

int main()
{
	int array[10] = {1,3,0,4,1,2,9,2,7,17};

	sort(0, 9, array);
	printf("Sorted: ");
	for(int i = 0; i < 10; ++i)
	{
		printf("%d%s ", array[i], i < 9 ? "," : "]\n");
	}
}

