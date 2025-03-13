import inspect
import time
from enum import Enum
from functools import partial

class Prime:
    def __init__(self):
        pass

    @staticmethod
    def prime_engine(n:int) -> list:
        ret = []

        return ret

    @classmethod
    def roll(cls, instance):
        frame = inspect.currentframe()
        print(f"{frame.f_lineno}: Prime Numbers from 0 to N \t ['prev','int']")
        while True:
            try:
                user_data = input("N ->")
                if user_data.lower() == "prev":
                    break
                ret = cls.prime_engine(int(user_data))
                print(f"Prime numbers btwn 0 and {user_data}: {ret}")
                print()
            except Exception as e:
                print(f"[Prime]:{e.__traceback__.tb_lineno} Exception thrown: {e}")



class Search:
    graph ={
        "A":["B","C"],
        "B":["A","D","E"],
        "C":["A","F"],
        "D":["B","F"],
        "E":["B","F"],
        "F":["C","E","D"],
    }

    def __init__(self):
        pass
    
    class GraphTraversal:
        def __init__(self):
            pass
    
        @staticmethod
        def dfs(graph=None, root="A", visited=None):
            if visited == None:
                graph = Search.graph
                print(graph)
                try:
                    root = input("Choose a Node from the graph above\n->").upper()
                    print(f"Root: {root}")
                except Exception as e:
                    print(f"[dfs]:{e.__traceback__.tb_lineno} Exception thrown {e}")
    
                visited = []
                visited.append(root)
            
            print(root)
            for neighbor in graph[root]:
                print(f"Neighbor: {neighbor}")
                if neighbor not in visited:
                    visited.append(neighbor)
                    Search.GraphTraversal.dfs(graph, neighbor, visited)
            
            print(visited)

        @staticmethod
        def bfs(graph, root):
            pass

        @staticmethod
        def spf(graph, root, end):
            pass

    @staticmethod
    def search_options():
        class Options(Enum):
            DFS = partial(Search.GraphTraversal.dfs) #Depth First Search
            BFS = partial(Search.GraphTraversal.bfs) #Breadth First Search
            SPF = partial(Search.GraphTraversal.spf) #Shortest Path First
        
        return Options

    @classmethod 
    def roll(cls, instance):
        frame = inspect.currentframe().f_back # get current line of previous frame in stack frame
        options = {option.name:option.value for option in Search.search_options()}
        
        while True:
            try:
                print(f"{frame.f_lineno}: Search ['prev', 'dfs', 'bfs', 'SPF']")
                user_data = input("Search ->")
                if user_data.lower() == "prev":
                    break
                
                options[user_data.upper()]()
                print()

            except Exception as e:
                print(f"[Search]:{e.__traceback__.tb_lineno} Exception thrown: {e}")
    


class Select_Operation(Enum):
    PRIME = Prime()
    SEARCH = Search()


def user_data() -> str:
    print("Select Operation from the list:\t Enter 'exit' to close program")
    print("Operation List:\n1. Search\n2. Prime")
    data = input("Select ->")
    return data


if __name__ == "__main__":
    while True:
        try:
            ret = user_data()
            if ret.lower() == "exit":
                print("Bye ..")
                break
            operations = {operation.name:operation.value for operation in Select_Operation}
            operation_object = operations[ret.upper()]
            operation_object.__class__.roll(operation_object)
        
        except Exception as e:
            print(f"[main]:{e.__traceback__.tb_lineno} Exception thrown: {e}")
        
        time.sleep(0.5)
        print()



