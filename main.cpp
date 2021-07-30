#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include "graph.hpp"
#include "pathfinding.hpp"

int main() {
	using namespace std::this_thread;
    using namespace std::chrono_literals;
	using std::chrono::system_clock;

	// Initialize graph
	Pathfinding graph((char*)"medium_map.osm");

	// Print graph matrix
	//graph.printMatrix(graph.getGraph());
	
	// Save nodes to file
	graph.saveNodes("visualization/src/public/files/nodes.txt");
	std::cout << "Nodes computed" << std::endl;
	
	// Get starting timepoint 
    auto start = std::chrono::high_resolution_clock::now();

	// Execute Floyd-Warshall algorithm
	graph.floydWarshall();

	// Get ending timepoint 
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	cout << "Time taken by function: " << duration.count() << " microseconds" << endl;

	//graph.printDistances(); graph.printPredecesors();
	std::cout << "Floyd computed" << std::endl;
	while (true) {
		// Get start and finish nodes
		vector<string> sf_nodes;
		string line;
		fstream points("visualization/src/public/files/out.txt");
		if (points.is_open())
			getline(points,line);
		points.close();
		stringstream ss(line);
		string str;
		while (getline(ss, str, ','))
			sf_nodes.push_back(str);
		
		graph.getPath(sf_nodes[0],sf_nodes[1]);
		
		//graph.printPath();
		graph.savePath("visualization/src/public/files/path.txt");

		// Clear graph
		//graph.clear();

		// Print empty graph
		//cout << endl << "------------ GRAPH WAS CLEARED ------------" << endl;
		//graph.print();

		sleep_for(100ms);
	}
}