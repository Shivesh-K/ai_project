#include "bits/stdc++.h"
using namespace std;

int originalGraph[40][40], mst[40][40];
int adjMatrixForMST[40][40];
bool visited[40];
int parentMST[40];
int verticesMST[40];
unordered_map<string, int> mstMap;     // path, cost for that particular mst
unordered_map<string, int> closedList; // this is our closed List which we have kept to keep track of nodes that
                                       // have been extended , and can be used to prune away the paths.
// Each time we update the vertices , we update their parents too

class Node
{
    /*This class defines the State of the problem. It also defines how the Cities will
    be stored in the priority_queue*/
public:
    int citynum;
    string pathSoFar;
    int citiesNotVisited;
    char name;
    int heuristicCost;
    int actualCost;
    int totalCost;
    string state; // the state contains the sorted version of pathsofar + the last city visited
    vector<int> citiesLeft;

    bool operator<(Node other) const
    {
        return this->totalCost > other.totalCost;
    }

    Node() {}
    Node(
        int citynum,
        string pathSoFar,
        int citiesNotVisited,
        char name,
        int heuristicCost,
        int actualCost,
        int totalCost,
        string state,
        vector<int> citiesLeft) : citynum(citynum),
                                  pathSoFar(pathSoFar),
                                  citiesNotVisited(citiesNotVisited),
                                  name(name),
                                  heuristicCost(heuristicCost),
                                  actualCost(actualCost),
                                  totalCost(totalCost),
                                  state(state),
                                  citiesLeft(citiesLeft) {}

} City[50000];

int nodeTrack = 0;

priority_queue<Node> astar;

class TSP
{
    /* This Class defines the Basic functions for the TSP Problem, like taking the input 
    and calculating the distance matrix for the cities */
public:
    char node[3];
    int points[40][2], numCities;

    void inputData()
    {
        printf("Enter the number of cities: ");
        scanf("%d", &numCities);
        getc_unlocked(stdin);

        printf("Enter city names along with their coordinates:\n");
        for (int i = 0; i < numCities; ++i)
            scanf("%s %d %d", node, &points[i][0], &points[i][1]);
    }

    int calculateDistance(int x1, int y1, int x2, int y2)
    {
        return int(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)) + 0.5);
    }

    void createOriginalDistanceGraph()
    {
        for (int i = 0; i < numCities; i++)
        {
            originalGraph[i][i] = 0;
            adjMatrixForMST[i][i] = 0;
            for (int j = i + 1; j < numCities; j++)
            {
                originalGraph[i][j] = calculateDistance(points[i][0], points[i][1], points[j][0], points[j][1]);
                originalGraph[j][i] = originalGraph[i][j]; // this is a Symmetric TSP
                adjMatrixForMST[i][j] = adjMatrixForMST[j][i] = 0;
                mst[i][j] = mst[j][i] = INT_MAX;
            }
        }
    }
};

int distance(int city1, int city2)
{
    return originalGraph[city1][city2];
}

int generateMST(vector<int> vertices, vector<char> nodesLeft)
{
    /* This function returns the length of the minimumSpanningTree for the
    remaining unvisited cities */
    int size = vertices.size();

    if (size == 1)
        return 0;

    int pcity[40], pdist[40], minDistance = INT_MAX;
    vector<int>::iterator it1;
    vector<char>::iterator it2;
    int i = 0;
    string cities;
    sort(nodesLeft.begin(), nodesLeft.end());

    for (it1 = vertices.begin(), it2 = nodesLeft.begin();
         it1 != vertices.end(), it2 != nodesLeft.end();
         it1++, it2++)
    {
        pcity[i] = *it1;    // parent city
        pdist[i] = INT_MAX; // parent distance;
        cities += *it2;
        i++;
    }

    auto mit = mstMap.find(cities); // iterator for the hash map for the MST
                                    // so that we don't have to calculate the MST length again and again for each path.
    if (mit != mstMap.end())
        return mit->second;

    int newCity = pcity[size - 1]; // making the last city as the newCity for finding the MST
    int thisDistance;
    int length = 0, minIndex;

    for (int m = size - 1; m > 0; m--)
    {
        minDistance = INT_MAX;
        for (int j = 0; j < m; j++)
        {
            thisDistance = distance(pcity[j], newCity);

            if (thisDistance < pdist[j])
                pdist[j] = thisDistance;

            if (pdist[j] < minDistance)
                minDistance = pdist[j], minIndex = j;
        }
        newCity = pcity[minIndex];
        length += minDistance;
        pcity[minIndex] = pcity[m - 1];
        pdist[minIndex] = pdist[m - 1];
    }
    mstMap[cities] = length;
    return length;
}

int calculateHeuristic(vector<int> vertices, vector<char> nodesLeft, int currentCityForExpansion)
{
    /* This function calculates the Heuristic value for the remaining path from the current city 
        to the remaining unvisited cities to the source city */
    int size = vertices.size();

    if (size == 0)
        return distance(currentCityForExpansion, 0);
    else if (size == 1)
        return distance(currentCityForExpansion, vertices[0]) + distance(vertices[0], 0);

    int pcity[40], pdist[40], minDistance = INT_MAX;
    string cities;
    int i = 0;

    for (int v : vertices)
    {
        pcity[i] = v;       // parent city
        pdist[i] = INT_MAX; // parent distance;
        i++;
    }

    int mst = generateMST(vertices, nodesLeft),
        nearestUnvisitedCityDistance = INT_MAX,
        nearestToSource = INT_MAX,
        thisDistance1,
        thisDistance2;

    for (i = 0; i < size; i++)
    {
        thisDistance1 = distance(pcity[i], currentCityForExpansion); // this is the distancefrom the unvisited city to the currentCityForExpansion
        thisDistance2 = distance(pcity[i], 0);                       // this is the distance from the source

        if (thisDistance1 < nearestUnvisitedCityDistance)
            nearestUnvisitedCityDistance = thisDistance1;

        if (thisDistance2 < nearestToSource)
            nearestToSource = thisDistance2;
    }

    return mst + nearestToSource + nearestUnvisitedCityDistance;
}

int optimumCost = INT_MAX;

Node createNode(int citynum, string pathSoFar, int citiesNotVisited, char name, int heuristicCost, int aCost, int totalCost, string state, vector<int> citiesLeft)
{
    /* This function Creates a new City to be explored */
    Node node(citynum, pathSoFar, citiesNotVisited, name, heuristicCost, aCost, totalCost, state, citiesLeft);
    return node;
}

int numExpanded = 1, totalNumofNodes = 1;

string startSearch()
{
    /* This is the main A* Search Function where the nodes are generated and put on the Priority Queue */
    Node current;
    int hn, tCost, aCost;
    char name;
    vector<int> vertices;
    vector<int> nextUnvisitedCities;
    vector<char> namesOfCitiesYettoVisit, namesOfCitiesYettoVisit2;
    string pathSoFar, state, npathSoFar, nstate;
    string res;

    while (!astar.empty() && astar.top().totalCost < optimumCost)
    {
        current = astar.top();
        astar.pop();

        pathSoFar = current.pathSoFar;
        state = current.state;
        vertices = current.citiesLeft;

        if (current.citiesNotVisited == 0)
        {
            int tcost = current.actualCost + distance(current.citynum, 0);

            if (tcost < optimumCost)
                optimumCost = tcost, res = current.pathSoFar;

            continue;
        }

        if (closedList.find(state) != closedList.end() && closedList[state] < current.totalCost)
            continue;

        if (closedList.find(state) != closedList.end() && closedList[state] > current.totalCost)
            closedList[state] = current.totalCost;
        else if (closedList.find(state) == closedList.end())
            closedList[state] = current.totalCost;

        numExpanded++;
        for (int next : vertices)
        {
            nextUnvisitedCities.clear();
            namesOfCitiesYettoVisit.clear();

            for (int v : vertices)
                if (v != next)
                    nextUnvisitedCities.push_back(v);

            for (int v : vertices)
                if (v != next)
                    namesOfCitiesYettoVisit.push_back(v > 25 ? 'a' + v - 26 : 'A' + v);

            hn = calculateHeuristic(nextUnvisitedCities, namesOfCitiesYettoVisit, next);

            aCost = current.actualCost + distance(current.citynum, next);
            tCost = hn + aCost;

            name = (next > 25 ? 'a' + next - 26 : 'A' + next);
            npathSoFar = pathSoFar + name;

            string temps = npathSoFar;
            sort(temps.begin(), temps.end());
            nstate = temps + name;

            totalNumofNodes++;
            astar.push(createNode(next, npathSoFar, current.citiesNotVisited - 1, name, hn, aCost, tCost, nstate, nextUnvisitedCities));
        }
    }
    return res;
}

int main()
{
    TSP tsp;
    tsp.inputData();
    tsp.createOriginalDistanceGraph(); // Till now we have the orginal Distance Graph
    int numCities = tsp.numCities;

    vector<int> v1;
    vector<char> v2;

    for (int i = 1; i < numCities; i++)
    {
        v1.push_back(i);
        v2.push_back(i > 25 ? 'a' + i - 26 : 'A' + i);
    }

    int initialHeuristic = calculateHeuristic(v1, v2, 0);

    City[nodeTrack++] = createNode(0, "A", numCities - 1, 'A', initialHeuristic, 0, initialHeuristic, "AA", v1);

    astar.push(City[0]);
    string res = startSearch();
    generateMST(v1, v2);

    cout << "\nThe optimumCost is " << optimumCost << " with path " << res + 'A';
    cout << "\nThe number of nodes expanded are " << numExpanded << " and the total nodes are " << totalNumofNodes;

    return 0;
}
