// Hazal Kantar 202111036 - CENG 383 Project

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <sstream>
#include <queue>
#include <algorithm>

using namespace std;

struct Vertex {
    string name;
    int plateNumber;
};

class Graph {
public:
    Graph() : V(0), E(0), nameToIndex(), vertices(V), adj() {}

    Graph(int V) : V(V), E(0), nameToIndex(), vertices(V), adj() {
        for (int i = 0; i < V; i++) {
            vertices[i] = Vertex{"", 0};  // Default name and plate number
        }
    }

    Graph(ifstream &in);

    bool adjacent(const Vertex &x, const Vertex &y){
        int xIndex = nameToIndex[x.name];
        int yIndex = nameToIndex[y.name];
        return adj[xIndex].count(yIndex) > 0;
    }

    vector<Vertex> neighbors(const Vertex &x){
        int xIndex = nameToIndex[x.name];
        vector<Vertex> neighbors;
        for (auto [neighborIndex, weight] : adj[xIndex]) {
            neighbors.push_back(vertices[neighborIndex]);
        }
        return neighbors;
    }

    void add_vertex(const Vertex &x){
        int index = x.plateNumber;
        if (nameToIndex.count(x.name) == 0) {
            vertices.push_back(x);
            nameToIndex[x.name] = index++;
        }
        V++;
    }

    void remove_vertex(const Vertex &x){
        int xIndex = nameToIndex[x.name];
        if (xIndex != -1) {
            for (auto [neighborIndex, _] : adj[xIndex]) {
                remove_edge(vertices[xIndex], vertices[neighborIndex]);
            }
            adj.erase(xIndex);
            vertices.erase(vertices.begin() + xIndex);

            for (auto &[name, index] : nameToIndex) {
                if (index > xIndex) {
                    index--;
                }
            }
            nameToIndex.erase(x.name);
            V--;
        } else {
            cout << "Vertex doesn't exist!!" << endl;
        }
    }

    void add_edge(const Vertex &x, const Vertex &y, int weight){
        int xIndex = nameToIndex[x.name];
        int yIndex = nameToIndex[y.name];
        if (adj[xIndex].count(yIndex) == 0) {
            E++;
            adj[xIndex][yIndex] = weight;
        }
    }

    void remove_edge(const Vertex &x, const Vertex &y){
        int xIndex = nameToIndex[x.name];
        int yIndex = nameToIndex[y.name];
        if (adj[xIndex].count(yIndex) > 0) {
            E--;
            adj[xIndex].erase(yIndex);
        }
        else {
            cout << "Edge doesn't exist!!" << endl;
        }
    }

    string get_vertex_value(const Vertex &x){
        return x.name;
    }

    void set_vertex_value(Vertex &x, const string &v){
        x.name = v;
    }

    int get_V(){
        return V;
    }

    int get_E(){
        return E;
    }

    string toString(){
        stringstream ss;
        ss << "Graph with " << V << " vertices and " << E << " edges:" << endl;
        for (int i = 0; i < V; i++) {
            ss << "- " << vertices[i].name << " (" << vertices[i].plateNumber << "):";
            for (auto [neighborIndex, weight] : adj[i]) {
                ss << " " << vertices[neighborIndex].name << "(" << weight << ")";
            }
            ss << endl;
        }
        return ss.str();
    }

    int get_edge_value(const Vertex &x, const Vertex &y){
        int xIndex = nameToIndex[x.name];
        int yIndex = nameToIndex[y.name];
        if (adj[xIndex].count(yIndex) > 0) {
            return adj[xIndex][yIndex];
        }
        return -1; // Indicate no edge exists
    }

    void set_edge_value(const Vertex &x, const Vertex &y, int v){
        int xIndex = nameToIndex[x.name];
        int yIndex = nameToIndex[y.name];
        if (adj[xIndex].count(yIndex) > 0) {
            adj[xIndex][yIndex] = v;
        }
    }

    int getVertexIndex(const string& cityName) const {
        auto it = nameToIndex.find(cityName);
        if (it != nameToIndex.end()) {
            return it->second; 

        } 
        else {
            cout << "Vertex with name '" << cityName << "' not found in the graph." << endl;
            return -1; // Example of returning a default value
        }
    }

    const vector<Vertex>& getVertices() const {
        return vertices;
    }


private:
    int V;
    int E;
    unordered_map<string, int> nameToIndex;                 // Map city name to vertex index
    vector<Vertex> vertices;                                // Store city names and plate numbers
    unordered_map<int, unordered_map<int, int>> adj;        // Adjacency list using map for weighted edges
};



void readCityCodes(Graph &graph) {
    ifstream cityCodes("cityCodes.txt");
    string line;

    while (getline(cityCodes, line)) {
        string plateNumber = line.substr(0, 2);
        string cityName = line.substr(3);
        graph.add_vertex({cityName, stoi(plateNumber)});

    }
}

void readAdjacentCities(Graph &graph) {

    ifstream adjacentCities("adjacent_cities.txt");
    string line;

    while (getline(adjacentCities, line)) {
        
        stringstream ss(line);
        string cityName;
        vector<string> cityNames;
        while (getline(ss, cityName, ',')) {
            cityNames.push_back(cityName);
        }

        int vertexIndex = graph.getVertexIndex(cityNames[0]);


        // set edges for  neighbor cities
        for (int i = 1; i < cityNames.size(); i++) {
            int neighborIndex = graph.getVertexIndex(cityNames[i]);
            graph.add_edge(graph.getVertices()[vertexIndex], graph.getVertices()[neighborIndex], 0); // initially set weight= 0

        }
    }
}

void readCityDistances(Graph &graph) {

    ifstream cityDistances("CityDistances.txt");
    string line;
    getline(cityDistances, line); // Skip the first line

    while (getline(cityDistances, line)) {
        stringstream ss(line);
        string plateNumber, cityName, dist;
        vector<string> distanceStr;
        getline(ss, plateNumber, ';');
        getline(ss, cityName, ';');

        int vertexIndex = graph.getVertexIndex(cityName);

        while (getline(ss, dist, ';')) {
            distanceStr.push_back(dist);
        }

        // explore the neighbors of current city and update edge values:
        for(int i = 0; i<graph.getVertices().size(); i++){
            if(graph.adjacent(graph.getVertices()[vertexIndex], graph.getVertices()[i])){
                int neighborIndex = graph.getVertices()[i].plateNumber - 1;
                
                int distance = stoi(distanceStr[neighborIndex - 1]);
                graph.set_edge_value(graph.getVertices()[vertexIndex], graph.getVertices()[neighborIndex], distance);
            }
        }

    }
}



void findKClosestCities( Graph& graph, int currentCityIndex, int k) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    currentCityIndex++;

    // explore the neighbors of the current city
    for (int i = 0; i < graph.getVertices().size(); i++) {
        if (graph.adjacent(graph.getVertices()[currentCityIndex], graph.getVertices()[i])) {
            int neighborIndex = graph.getVertices()[i].plateNumber - 2;

            //cout << "Adjacent city with plate number: " << i << " is: " << graph.getVertices()[neighborIndex].name << ", index: " << neighborIndex << endl;

            int distance = graph.get_edge_value(graph.getVertices()[currentCityIndex], graph.getVertices()[i]);

            //cout<< "dist: "<<distance<<endl;

            pq.push({distance, neighborIndex});
        }
    }

    currentCityIndex--;
    
    cout << "K closest cities from " << graph.getVertices()[currentCityIndex].name << ":" << endl;
    int count = 0;
    while (!pq.empty() && count < k) {
        auto closest = pq.top(); pq.pop();
        cout << graph.getVertices()[closest.second].name << " (" << closest.first << ") km." << endl;
        count++;
    }
}

int findShortestPath(Graph &graph, int sourceIndex, int destinationIndex) {
    int numVertices = graph.get_V();
    vector<int> distances(numVertices, INT_MAX);
    vector<int> parents(numVertices, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<string> path;
    bool pathFound = false;

    sourceIndex++;

    destinationIndex++;

    if (sourceIndex < 0 || sourceIndex >= numVertices || destinationIndex < 0 || destinationIndex >= numVertices) {
        cout << "Invalid source or destination index." << endl;
    }

    distances[sourceIndex] = 0;
    pq.push({0, sourceIndex});

    while (!pq.empty()) {
        int currentDistance = pq.top().first;
        int currentVertex = pq.top().second;
        pq.pop();

        if (currentVertex == destinationIndex) {
            pathFound = true;
            break; 
        }


        int i=0;
        for (auto neighbor : graph.neighbors(graph.getVertices()[currentVertex])) {
            if (currentVertex == destinationIndex) {
                pathFound = true;
                break; 
            }
            int neighborIndex = graph.getVertexIndex(neighbor.name) - 2;

            int edgeWeight = graph.get_edge_value(graph.getVertices()[currentVertex], graph.getVertices()[neighborIndex]);

            int newDistance = currentDistance + edgeWeight;

            if (newDistance < distances[neighborIndex]) {
                distances[neighborIndex] = newDistance;
                parents[neighborIndex] = currentVertex;
                pq.push({newDistance, neighborIndex});

            }
        }

    }
    
    if(pathFound){

        sourceIndex--;
        int printIndex = destinationIndex-1;
        cout << "Shortest path from " << graph.getVertices()[sourceIndex].name << " to " << graph.getVertices()[printIndex].name;
        cout << " is : " << distances[destinationIndex] << endl;
        
        int dest = destinationIndex;
        int find = destinationIndex;

        while (find != -1) {
            string name = graph.getVertices()[find-1].name;

            path.push_back(name);
            find = parents[find];
        }

        reverse(path.begin(), path.end());

        cout << "Cities visited: ";
        for (const auto& visitedCity : path) {
            cout << visitedCity << " ";
        }

        int totalDist = distances[dest];

        cout << "\nTotal distance: " << totalDist << " km\n"; 

        return totalDist;
    }

    else{
        cout << "Path could not found!!" << endl;
        return -100;
    }
}

int main() {
    
    Graph graph; 

    readCityCodes(graph);
    readAdjacentCities(graph);
    readCityDistances(graph);

    int currentCityIndex = -1; 
    int plateNumber = -1;

    char choice;
    do {
        cout << "--------------------------" << endl;
        cout << "Menu:" << endl;
        cout << "a. Enter city" << endl;
        cout << "b. Print selected city" << endl;
        cout << "c. List k closest cities" << endl;
        cout << "d. Find shortest path to" << endl;
        cout << "x. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 'a': {
                string cityName;
                cout << "Enter city name: ";
                cin >> cityName;
                currentCityIndex = graph.getVertexIndex(cityName) - 1;
                plateNumber = currentCityIndex + 1;

                if (currentCityIndex != -1) {
                    cout << "City " << cityName << " selected." << endl;
                } else {
                    cout << "City not found." << endl;
                }

                break;
            }
            case 'b': {
                if (currentCityIndex != -1) {
                    cout << "Selected city: " << graph.getVertices()[currentCityIndex].name << " Plate number: " << plateNumber << endl;
                } else {
                    cout << "No city selected." << endl;
                }
                break;
            }
            case 'c': {
                if (currentCityIndex != -1) {
                    int k;
                    cout << "Enter the number of closest cities to find: ";
                    cin >> k;

                    findKClosestCities(graph, currentCityIndex, k);
                } else {
                    cout << "Please select a city first." << endl;
                }
                break;
            }
            case 'd': {
                string destinationCity;
                cout << "Enter destination city name or plate ID: ";
                cin >> destinationCity;
                int destinationIndex = graph.getVertexIndex(destinationCity) - 1;

                if (destinationIndex >= 0) {
                    int totalDist;
                    totalDist = findShortestPath(graph, currentCityIndex, destinationIndex);
                    //cout << "totalDist: " <<totalDist<<endl;
                } 
                else {
                    cout << "Destination city not found." << endl;
                }
                break;
            }
            case 'x':
                cout << "Exiting the program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 'x');


    return 0;
}