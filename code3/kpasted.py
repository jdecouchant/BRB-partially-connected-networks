import networkx as nx
from matplotlib import pyplot as plt

def generate_k_pasted(n,k):
    if n < 2*k:
        raise Exception('n has to be at least twice the value of k')

    free_node_label = 2*k

    Tree = nx.DiGraph()
    #Tree.add_nodes_from(range(0, n))
    
    Expanding_nodes = [0]
    Expanding_node = Expanding_nodes.pop()
    Future_Expanding_Nodes = []
    for i in range(1,k+1):
        Tree.add_edge(0,i)
        Tree.nodes[i]['type'] = 'shared'
    Tree.nodes[0]['type'] = 'root'
    root_expansion = False

    for iteration in range(2*k,n):
        free_node_label += 1
        if Tree.degree(Expanding_node) < 3*k -3:
            Tree.add_edge(Expanding_node,free_node_label)
            Tree.nodes[free_node_label]['type'] = 'shared'
        else:
            # G.neighbors in a DiGraph gives the out edges
            if not root_expansion:
                leaves = list(Tree.neighbors(Expanding_node))
                Future_Expanding_Nodes.extend(leaves[:k])
                Tree.remove_nodes_from(leaves[k:])
                root_expansion = True
            else:
                leaves = list(Tree.neighbors(Expanding_node))
                Future_Expanding_Nodes.extend(leaves[:k-1])
                Tree.remove_nodes_from(leaves[k-1:])

            if len(Expanding_nodes) == 0:
                Expanding_nodes = Future_Expanding_Nodes
                Future_Expanding_Nodes = []
            Expanding_node = Expanding_nodes.pop()
            Tree.nodes[Expanding_node]['type'] = 'internal'
            for i in range(k-1):
                Tree.add_edge(Expanding_node, free_node_label)
                Tree.nodes[free_node_label]['type'] = 'shared'
                free_node_label += 1


    node_label = free_node_label + 1
    G = nx.Graph()
    node_mapping = {}

    Nodes_To_Analize = []
    node = 0
    # types_inserted = set()
    while Tree.out_degree(node) > 0:
        # types_inserted.add(Tree.nodes[node]['type'])
        Nodes_To_Analize.append([node, Tree.nodes[node]['type']])
        node = next(Tree.neighbors(node))
    Nodes_To_Analize.append([node, Tree.nodes[node]['type']])    # last leaf


    for node in Tree.nodes():
        mapping = []
        #print(Tree.nodes[node])
        if Tree.nodes[node]['type'] != 'shared':
            mapping.append(node)
            G.add_node(node)
            G.nodes[node]['type'] = Tree.nodes[node]['type']
            for i in range(k-1):
                mapping.append(node_label)
                G.add_node(node_label)
                G.nodes[node_label]['type'] = Tree.nodes[node]['type']
                node_label += 1
            node_mapping[node] = mapping
        else:
            for i in range(k):
                mapping.append(node)
            node_mapping[node] = mapping
            G.add_node(node)
            G.nodes[node]['type'] = Tree.nodes[node]['type']

    for node in Tree.nodes():
        if Tree.nodes[node]['type'] != 'shared': # root or internal
            for edge in Tree.out_edges(node):
                for i in range(k):
                    G.add_edge(node_mapping[edge[0]][i], node_mapping[edge[1]][i])

    # return nx.convert_node_labels_to_integers(G)

    relabelling = {}
    counter = 0
    for node in G.nodes():
        relabelling[node] = counter
        counter += 1
    G = nx.relabel_nodes(G, relabelling)
    for i in range(len(Nodes_To_Analize)):
        Nodes_To_Analize[i][0] = relabelling[Nodes_To_Analize[i][0]]

    # print(Nodes_To_Analize)
    # print(list(G.nodes))

    return G, Nodes_To_Analize
