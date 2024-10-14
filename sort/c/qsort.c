#include <stdio.h>
#include <stdlib.h>


void swap(int *a, int *b)
{
	int temp = *a; *a = *b; *b = temp;
}


int partition(int *array, int low, int high)
{
	int pivot = array[low];
	int i = low;
	int j = high;

	printf("Partition\n");
	while (i < j)
	{
		while(array[i] <= pivot && i < high){
			printf("smaller than pivot.. \n");
			i++;
		}
		while(array[j] >= pivot && j > low){
			printf("greater than pivot.. \n");
			j--;
		}

		if (i < j)
		{
			printf("inner swap..\n");
			swap(&array[i],&array[j]);
		}
	}
 	printf("outer swap..\n");
	swap(&array[low],&array[j]);
	return j;

}

void quick_sort(int *array, int low, int high)
{
	if(low < high)
	{
		printf("\nQuick sort func\n");
		int partition_index = partition(array, low, high);
		quick_sort(array, low, partition_index-1);
		quick_sort(array, partition_index+1, high);
	}
}


int main()
{
	while(1)
	{
		printf("Quick sort ...\n");
		printf("Enter the number of elements in array: ");
		int elements;
		scanf("%d",&elements);
		int * array = (int *) malloc(sizeof(int) * elements);
		printf("\nEnter %d elements: \n",elements);
		for (int i = 0; i < elements; i++)
		{
			scanf("%d, ",&array[i]);
		}
		printf("\n");
		for (int k = 0; k < elements; k++)
		{
			printf("%d  ",array[k]);
		}

		quick_sort(array, 0, elements-1);
		printf("\n\nSorted elements: ");
	
		for (int j = 0; j < elements; j++)
		{
			printf("%d ",array[j]);
		}
		printf("\n\n");
		free(array);
	}
}
		
	






