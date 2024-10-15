graph = {"0":["3"], "3":["5","4"], "5":["7"], "7":["8"], "8":["4"], "4":[]}



def bfs(visited, queue, starting_node):
    visited.append(starting_node)
    queue.append(starting_node)

    while len(queue) > 0:
        explore_node = queue.pop() #ensures breadthwise exploration processing nodes in order of addititon to the queue
        print(f"Exploring node: {explore_node}")
        for neighbor in graph[explore_node]:
            if neighbor not in visited:
                visited.append(neighbor)
                queue.append(neighbor)

        



if __name__ == "__main__":
    print("Breadth First Search")
    starting_node = "0"
    visited  = []
    queue = []
    bfs(visited, queue, starting_node)
