
#include <bits/stdc++.h>
using namespace std;

int V = 0;

// function to calc the node with minimum value which is not included in MST
int calc_min_key(vector<int>& key, vector<bool>& Included_vertices)
{
	
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (Included_vertices[v] == false && key[v] < min)
			min = key[v], min_index = v;

	return min_index;
}

// Function to print the resultant MST
void Print_MST(vector<int>& parent, vector<vector<int>>& graph)
{
	cout<<"Edge \tWeight\n";
	for (int i = 1; i < V; i++)
		cout<<parent[i]<<" - "<<i<<" \t"<<graph[i][parent[i]]<<" \n";
}

// Function to generate the MST from graph represented using adjancey list
void MST_using_prims(vector<vector<int>>& graph)
{
	vector<int> parent(V), key(V, INT_MAX);
	
  vector<bool>Included_vertices(V,false);

	key[0] = 0;
	parent[0] = -1; 
	for (int i= 0;i<V - 1;i++)
	{
		int u = calc_min_key(key, Included_vertices);

		Included_vertices[u] = true;

		for (int v = 0; v < V; v++)
			if (graph[u][v] && Included_vertices[v] == false && graph[u][v] < key[v])
				parent[v] = u, key[v] = graph[u][v];
	}

	Print_MST(parent, graph);
}

// Driver code
int main()
{
    cout<<"Enter the no. of nodes: ";
    cin>>V;
    
    vector<vector<int>>graph(V,vector<int>(V,0));
    cout<<"\nEnter the edge weights(V*V matrix) :";
    
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++)
            cin>>graph[i][j];
    }
    
	MST_using_prims(graph);

	return 0;
}
