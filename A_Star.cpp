#include "bits/stdc++.h"
#define vii vector<int>
#define vc vector<char>
using namespace std;

int originalGraph[40][40];
int mst[40][40];
int adjMatrixForMST[40][40];
bool visited[40];
int parentMST[40];
int verticesMST[40];
unordered_map<string, int> mstMap;     // to store cost of already calculated MST
unordered_map<string, int> closedList; // to track node that are expanded and can be pruned

class Vertex
{
public:

    int cityIndex;
    string currPath;
    int citiesToBeVisited;
    char cityName;
    int hc;
    int ac;
    int tc;
    string state; // the state contains the sorted version of pathsofar + the last city visited
   
    vii citiesLeft;

    bool operator<(Vertex other) const
    {
        return this->tc > other.tc;
    }

    Vertex() {}
    Vertex(
        int cityIndex,
        string currPath,
        int citiesToBeVisited,
        char cityName,
        int hc,
        int ac,
        int tc,
        string state,
        vii citiesLeft) : cityIndex(cityIndex),
                                  currPath(currPath),
                                  citiesToBeVisited(citiesToBeVisited),
                                  cityName(cityName),
                                  hc(hc),
                                  ac(ac),
                                  tc(tc),
                                  state(state),
                                  citiesLeft(citiesLeft) {}

} City[50000];

int nodeTrack = 0;

priority_queue<Vertex> pq;

    /* Class used to define basic function of TSP */
class TSP
{
public:
    int points[40][2], cityCount;

    void getInput()
    {
        cout << "Enter the number of cities: ";
        cin >> cityCount;

        cout << "Cities will be named alphabetically in order that they are entered.\n";
        cout << "Enter cities' coordinates (xi yi):\n";
        for (int i = 0; i < cityCount; ++i)
            cin >> points[i][0] >> points[i][1];
    }

    int calculateDistance(int x1, int y1, int x2, int y2)
    {
        return int(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)) + 0.5);
    }

    void generateGraph()
    {
        for (int i = 0; i < cityCount; i++)
        {
            originalGraph[i][i] = 0;
            adjMatrixForMST[i][i] = 0;
            for (int j = i + 1; j < cityCount; j++)
            {
                originalGraph[i][j] = calculateDistance(points[i][0], points[i][1], points[j][0], points[j][1]);
                originalGraph[j][i] = originalGraph[i][j];
                adjMatrixForMST[i][j] = adjMatrixForMST[j][i] = 0;
                mst[i][j] = mst[j][i] = INT_MAX;
            }
        }
    }
};

int distance(long int c1, long int c2)

{
    return originalGraph[c1][c2];
}
   
/*
    below function is used to find the length of the MST in the unvisited cities
*/
int generateMST(vector<int> vertices, vector<char> nodesLeft)
{
    int size = vertices.size();

    if (size == 1)
        return 0;

    int parentCity[40], parentDistance[40], minDistance = INT_MAX;
   
    vii ::iterator it1;
   
    vc ::iterator it2;
   
    int it = 0;
   
    string cities;
    sort(nodesLeft.begin(), nodesLeft.end());

    for (it1 = vertices.begin(), it2 = nodesLeft.begin();
         it1 != vertices.end(), it2 != nodesLeft.end();
         it1++, it2++)
    {
        parentCity[it] = *it1;
        parentDistance[it] = INT_MAX;
        cities += *it2;
        it++;
    }

    auto mit = mstMap.find(cities); // hash map iterator for minimum spanning tree like dynamic programming
   
    if (mit != mstMap.end())
        return mit->second;

    int newCity = parentCity[size - 1]; // assigning last city as new node in minimum spanning tree
   
    int newdis;
   
    int length = 0, minIndex;

    for (int m = size - 1; m > 0; m--)
    {
        minDistance = INT_MAX;
        for (int j = 0; j < m; j++)
        {
            newdis = distance(parentCity[j], newCity);

            if (newdis < parentDistance[j])
                parentDistance[j] = newdis;

            if (parentDistance[j] < minDistance)
                minDistance = parentDistance[j], minIndex = j;
        }
        newCity = parentCity[minIndex];
        length += minDistance;
        parentCity[minIndex] = parentCity[m - 1];
        parentDistance[minIndex] = parentDistance[m - 1];
    }
    mstMap[cities] = length;
    return length;
}

/*
    below function calculating the values of the heuristic function for unvisited path from current city to andthen get back to the starting node
*/
int calculateHeuristic(vector<int> vertices, vector<char> nodesLeft, int currentCityForExpansion)
{
    int size = vertices.size();

    if (size == 0)
        return distance(currentCityForExpansion, 0);
    else if (size == 1)
        return distance(currentCityForExpansion, vertices[0]) + distance(vertices[0], 0);

    int parentCtiy[40], parentDistance[40], minDistance = INT_MAX, i = 0;
    string cities;

    for (int v : vertices)
    {
        parentCtiy[i] = v;
        parentDistance[i] = INT_MAX;
        i++;
    }

    int mst = generateMST(vertices, nodesLeft),
        nearestUnvisitedCityDistance = INT_MAX,
        nearestToSource = INT_MAX,
        newdis1,
        newdis2;

    for (int i = 0; i < size; i++)
    {
        newdis1 = distance(parentCtiy[i], currentCityForExpansion);  // newdis1 containes the distance from unvisited node from the source
        newdis2 = distance(parentCtiy[i], 0);                  

        if (newdis1 < nearestUnvisitedCityDistance)
            nearestUnvisitedCityDistance = newdis1;

        if (newdis2 < nearestToSource)
            nearestToSource = newdis2;
    }

    return mst + nearestToSource + nearestUnvisitedCityDistance;
}

int optimumCost = INT_MAX, expandedNodes = 1, totalNumofNodes = 1;

/* This function Creates a new City to be explored */
Vertex createNode(int cityIndex, string currPath, int citiesToBeVisited, char cityName, int hc, int aCost, int tc, string state, vector<int> citiesLeft)
{
    Vertex node(cityIndex, currPath, citiesToBeVisited, cityName, hc, aCost, tc, state, citiesLeft);
    return node;
}

/* This is the main A* Search Function where the nodes are generated and put on the Priority Queue */
string aStarSearch()
{
    Vertex current;
    int hn, tCost, aCost;
    char cityName;
    vector<int> vertices;
    vector<int> nextUnvisitedCities;
    vector<char> namesOfCitiesYettoVisit;
    string currPath, state, npathSoFar, nstate, res;

    while (!pq.empty() && pq.top().tc < optimumCost)
    {
        current = pq.top();
        pq.pop();

        currPath = current.currPath;
        state = current.state;
        vertices = current.citiesLeft;

        if (current.citiesToBeVisited == 0)
        {
            int tcost = current.ac + distance(current.cityIndex, 0);

            if (tcost < optimumCost)
                optimumCost = tcost, res = current.currPath;

            continue;
        }

        if (closedList.find(state) != closedList.end() && closedList[state] < current.tc)
            continue;

        if (closedList.find(state) != closedList.end() && closedList[state] > current.tc)
            closedList[state] = current.tc;
        else if (closedList.find(state) == closedList.end())
            closedList[state] = current.tc;

        expandedNodes++;
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

            aCost = current.ac + distance(current.cityIndex, next);
            tCost = hn + aCost;

            cityName = (next > 25 ? 'a' + next - 26 : 'A' + next);
            npathSoFar = currPath + cityName;

            string temps = npathSoFar;
            sort(temps.begin(), temps.end());
            nstate = temps + cityName;

            totalNumofNodes++;
            pq.push(createNode(next, npathSoFar, current.citiesToBeVisited - 1, cityName, hn, aCost, tCost, nstate, nextUnvisitedCities));
        }
    }
    return res;
}

int main()
{
    TSP tsp;
    tsp.getInput();
    tsp.generateGraph();

    vector<int> cityIdx;
    vector<char> cityName;

    for (int i = 1; i < tsp.cityCount; i++)
    {
        cityIdx.push_back(i);
        cityName.push_back(i > 25 ? 'a' + i - 26 : 'A' + i);
    }

    int initialHeuristic = calculateHeuristic(cityIdx, cityName, 0);

    City[nodeTrack++] = createNode(0, "A", tsp.cityCount - 1, 'A', initialHeuristic, 0, initialHeuristic, "AA", cityIdx);

    pq.push(City[0]);
    string res = aStarSearch();
    generateMST(cityIdx, cityName);

    cout << "\nThe optimal cost is " << optimumCost << " with path ";
    for (char ch : res)
        cout << ch << " -> ";
    cout << "A";
    cout << "\nThe number of nodes that were expanded are " << expandedNodes << " and the total nodes are " << totalNumofNodes;

    return 0;
}
