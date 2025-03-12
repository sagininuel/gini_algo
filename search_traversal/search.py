graph = {
    "A":["B","C"],
    "B":["A","D","E"],
    "C":["A","F"],
    "D":["B","F"],
    "E":["B","F"],
    "F":["C","E","D"],
    }


class Solution():
    def __init__(self):
        pass
    def breadth_first_search(self, graph, start):
        visited = []
        queue = [start]
        visited.append(start)
        
        while queue:
            node = queue.pop(0)
            print(node,end="")
            for neighbor in graph[node]:
                if neighbor not in visited:
                    queue.append(neighbor)
                    visited.append(neighbor)

    def depth_first_search(self, graph, start, visited=None):
        if visited == None:
            visited = []

        visited.append(start)
        print(start, end="")

        for neighbor in graph[start]:
            if neighbor not in visited:
                self.depth_first_search(graph, neighbor, visited)
        

    def shortest_path_first(self, graph, start, end):
        visited = {start: None} # store node and parent
        queue = [start]

        while queue:
            node = queue.pop(0)
            if node == end:
                path = []
                
                while node is not None: 
                    path.append(node)
                    node = visited[node] 
                return path[::-1]
            
            for neighbor in graph[node]:
                if neighbor not in visited:
                    visited[neighbor] = node
                    queue.append(neighbor)
    

if __name__ == "__main__":
    s = Solution()
    print(graph)
    s.breadth_first_search(graph, "A")
    print()
    s.depth_first_search(graph, "B")
    print()
    while True:
        try:
            [start, end] = input("Enter any two arbitrary graph vertices :\t").split(" ")
            print(f"\nShortest First Path:\t{start} -> {end}: {s.shortest_path_first(graph, start.upper(), end.upper())}\n")
            #_input = input("Enter two vertices \t i.e. 'A' 'B'\n").split(" ")
            #print(_input)
        except Exception as e:
            print(f"Exception thrown: {e}\nTry again .. ")
    print(s.shortest_path_first(graph, "A", "F"))
