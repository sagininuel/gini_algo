

def partition(begin, end, num_list):
    pivot = num_list[begin] #arbitrary pivot
    i = begin
    j = end

    while i < j:
        while num_list[i] <= pivot and i < end:
            i += 1

        while num_list[j] >= pivot and j > begin:
            j -= 1

        if i < j:
            num_list[i], num_list[j] = num_list[j], num_list[i]

    num_list[j], num_list[begin] = num_list[begin], num_list[j]
    print("partitioned at: " + str(j))
    return j



def q_sort(begin, end, num_list):
    print(num_list[begin:end + 1])
    if begin < end:
        partition_index = partition(begin, end, num_list)
        q_sort(begin, partition_index - 1, num_list)
        q_sort(partition_index + 1, end, num_list)



if __name__ == "__main__":

    num_list = [2,3,5,1,5,0,0,56,1,3,0]

    q_sort(0, len(num_list)-1, num_list)
    print(num_list)


