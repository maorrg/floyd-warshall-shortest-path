// pathfinding.hpp
#pragma once
#include "graph.hpp"
#include <cfloat>
#include <limits>

class Pathfinding : public Graph {
    private:
        // Matrix containing the shortest distances between every pair of nodes
        vector<vector<double>> dist_matrix = {};
        // Matrix containing the intermediate vertices between every pair of nodes
        vector<vector<double>> predecesor = {};
        // Path vector
        vector<Node> nodes = {};
        vector<Node> path = {};
    
    public:
        Pathfinding(char* _doc) : Graph{_doc} {};
        
        void floydWarshall () {
            // Initialize the matrix
            dist_matrix = Graph::getGraph();
            predecesor.clear();
            predecesor.resize(Graph::size(), std::vector<double>(Graph::size()));
            std::cout << Graph::size() << std::endl;
            for (unsigned int i = 0; i < Graph::size(); ++i) {
                for (unsigned int j = 0; j < Graph::size(); ++j) {
                    if (dist_matrix[i][j] < DBL_MAX)
                        predecesor[i][j] = i;
                    else { predecesor[i][j] = -1; }
                    if (i == j)
                        predecesor[i][j] = -1;
                }
            }

            // Add nodes to the set of intermediate nodes
            for (unsigned int k = 0; k < Graph::size(); k++) {
                for (unsigned int i = 0; i < Graph::size(); i++) {
                    if (dist_matrix[i][k] < DBL_MAX) {
                        for (unsigned int j = 0; j < Graph::size(); j++) {
                            if (dist_matrix[k][j] < DBL_MAX) {
                                // If k is on the shortest path from i to j, then update
                                if (dist_matrix[i][k] + dist_matrix[k][j] < dist_matrix[i][j]) {
                                    dist_matrix[i][j] = dist_matrix[i][k] + dist_matrix[k][j];
                                    predecesor[i][j] = predecesor[k][j];
                                }
                            }
                        }
                    }
                }
            }
        }

        void printDistances() {
            // Print the shortest distance matrix
            std::cout << "Floyd-Warshall shortest distance: " << std::endl;
            Graph::printMatrix(dist_matrix);
        }

        void printPredecesors() {
            // Print the predecesor matrix
            std::cout << "Floyd-Warshall intermediate vertices: " << std::endl;
            Graph::printMatrix(predecesor);
        }

        void getPath(string src, string dst, bool print = 0) {
            std::unordered_map<string, unsigned int> id2val = Graph::getHash();
            nodes = Graph::getNodeList();
            path.clear();
            getPath(id2val[src] - 1, id2val[dst] - 1, print);
        }
        void getPath(unsigned int src, unsigned int dst, bool print = 0) {
            if (src != dst)
                getPath(src, predecesor[src][dst]);
            path.push_back(nodes[dst]);
        }

        void printPath() {
            for (auto it = nodes.begin(); it != nodes.end(); ++it)
                cout << it->coord;
        }

        void savePath(string file_name) {
			ofstream outfile (file_name);
			string output_str = "";
			
			for (int i = 0; i < path.size(); ++i) {
                output_str = output_str + path[i].mapID + "," 
					     + to_string(path[i].coord.latitud) + "," 
						 + to_string(path[i].coord.longitud)+ "\n";
                if (i != 0 && i != path.size() - 1) {
                    output_str = output_str + path[i].mapID + "," 
					     + to_string(path[i].coord.latitud) + "," 
						 + to_string(path[i].coord.longitud)+ "\n";
                }
			}
            if (output_str.size() > 0)
                output_str.pop_back();
            outfile << output_str; outfile.close();
		}          
};