// graph.hpp
#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstring>
#include <cmath>
#include "rapidxml_library/rapidxml.hpp"
#include "rapidxml_library/rapidxml_print.hpp"
#include "rapidxml_library/rapidxml_utils.hpp"
#include <fstream>
#include <cfloat>
#include <limits>

using namespace std;
using namespace rapidxml;

struct Coordinates {
	double latitud, longitud;
	Coordinates() : latitud(0), longitud(0) {};
	Coordinates(double _lat = 0, double _long = 0) : latitud(_lat), longitud(_long) {};

	bool operator ==(const Coordinates &new_coord) {
		return ((this->latitud == new_coord.latitud) && (this->longitud == new_coord.longitud));
	}
	
	inline friend ostream& operator <<(ostream& out, Coordinates& _coord) {
		out << "lat = " << _coord.latitud << " , long = " << _coord.longitud << endl;
		return out; 
	}
};

struct Node {
	unsigned int id = 0;
    string mapID = 0;
    Coordinates coord;
    double weight = 0;

	Node(unsigned int _id, string _mapID, Coordinates _coord, double _weight = 0) : 
		id(_id), mapID(_mapID), coord(_coord), weight(_weight) {};
};

class Graph {
    private:
        unsigned int size_of_graph = 0;
        vector<vector<double>> graph;
        vector<Node> node_list = {};
		vector<Node> street_list = {};
        unordered_map<string, unsigned int> id2val;
        xml_document<> doc;

    public:
        Graph() = delete;
        
        Graph(char* _doc) {
            // XML Parser
			file<> xmlFile(_doc);
			doc.parse<0>(xmlFile.data());
			getNodes();
			streetNodes();
            graph.resize(size_of_graph, std::vector<double>(size_of_graph));
            for (unsigned int i = 0; i < size_of_graph; ++i) {
                for (unsigned int j = 0; j < size_of_graph; ++j) {
                    if (i == j)
                        graph[i][j] = 0;
                    else
                        graph[i][j] = DBL_MAX;
                }
            }
            getEdges();
        }

        // Add all nodes to the graph
		void getNodes() {
			node_list.clear();
			for (xml_node<> *node = doc.first_node()->first_node(); node; node = node->next_sibling()) {
				if(strcmp(node->name(), "node") == 0) {
					auto point = Coordinates(getLatitud(node), getLongitud(node));
					node_list.push_back(Node(size_of_graph, node->first_attribute()->value(), point));
                    size_of_graph++;
				}
			}
		}

		void streetNodes() {
			street_list.clear();
			street_list = node_list;
			vector<string> temp;
			for (xml_node<> *node = doc.first_node()->first_node(); node; node = node->next_sibling()) {
				temp.clear();
				if (strcmp(node->name(), "way") == 0) {
					for(xml_node<> *nd = node->first_node("nd"); nd; nd = nd->next_sibling()) {
						temp.push_back(nd->first_attribute()->value());
					}
					for(xml_node<> *d = node->first_node("tag"); d; d = d->next_sibling()) {
						if(strcmp(d->first_attribute()->value(),"building") == 0) {
							for (int i = 0; i < temp.size(); ++i) {
								for (int j = 0; j < street_list.size(); ++j) {
									if (street_list[j].mapID == temp[i]) {
										street_list.erase(street_list.begin() + j);
										size_of_graph--;
									}
								}
							}
							temp.clear();
						}
					}
					for(xml_node<> *d = node->first_node("tag"); d; d = d->next_sibling()) {
						if(strcmp(d->first_attribute()->value(),"amenity") == 0) {
							for (int i = 0; i < temp.size(); ++i) {
								for (int j = 0; j < street_list.size(); ++j) {
									if (street_list[j].mapID == temp[i]) {
										street_list.erase(street_list.begin() + j);
										size_of_graph--;
									}
								}
							}
							temp.clear();
						}
					}
					for(xml_node<> *d = node->first_node("tag"); d; d = d->next_sibling()) {
						if(strcmp(d->first_attribute()->value(),"footway") == 0) {
							for (int i = 0; i < temp.size(); ++i) {
								for (int j = 0; j < street_list.size(); ++j) {
									if (street_list[j].mapID == temp[i]) {
										street_list.erase(street_list.begin() + j);
										size_of_graph--;
									}
								}
							}
							temp.clear();
						}
					}
				}
			}
			// Make hash table
			for (int i = 0; i < street_list.size(); ++i) {
				street_list[i].id = i;
				id2val[street_list[i].mapID] = i + 1;
			}
		}

        void connect(Node& src, Node& dst, bool isDirectional = 0) {
            // Euclidian distance
            double cost = sqrt(pow((src.coord.latitud - dst.coord.latitud),2) + pow((src.coord.longitud - dst.coord.longitud),2));
            // Make cost bigger
            cost *= 10000;
            graph[src.id][dst.id] = cost;
            if (!isDirectional)
				graph[dst.id][src.id] = cost;
        }

        // Add all edges to the graph
		void getEdges() {
			for (xml_node<> *node = doc.first_node()->first_node(); node; node = node->next_sibling()) {
				if (strcmp(node->name(), "way") == 0) {
					bool dir_val = false;
					for(xml_node<> *d = node->first_node("tag"); d; d = d->next_sibling()){
						if(strcmp(d->first_attribute()->value(),"oneway") == 0){
							if(strcmp(d->first_attribute()->next_attribute()->value(),"yes")==0)
								dir_val = true;
						}
					}
					for (xml_node<> *nd = node->first_node(); nd; nd = nd->next_sibling()) {
						if (strcmp(nd->name(), "nd") == 0) {
							auto src_id = nd->first_attribute()->value();
							if (src_id) {
								if (nd->next_sibling()) {
									xml_node<>* dst = nd->next_sibling();
									if (strcmp(dst->name(), "nd") == 0) {
										auto dst_id = dst->first_attribute()->value();
										if (id2val[src_id] != 0 && id2val[dst_id] != 0)
											connect(street_list[id2val[src_id] - 1], street_list[id2val[dst_id] - 1], dir_val);
									}
								}
							}
						}
					}
				}
			}
		}
        
        void clear() {
			graph.clear();
            node_list.clear();
			street_list.clear();
            size_of_graph = 0;
		}

        unsigned int size() { return size_of_graph; }

        vector<vector<double>> getGraph() { return graph; }

		unordered_map<string, unsigned int> getHash() { return id2val; }

        // Print distance matrix
        void printMatrix(vector<vector<double>>& _graph) {
            cout << "Graph matrix representation: " << endl;
            for (unsigned int i = 0; i < _graph.size(); ++i) {
                for (unsigned int j = 0; j < _graph.size(); ++j) {
                    if (_graph[i][j] == DBL_MAX)
                        cout << "INF" << "     ";
                    else
                        printf("%.1f     ", round(_graph[i][j]));
                }
                cout << endl;
            }
            cout << endl;
        }
        
        // Print graph values
		void printNodes() {
			cout<<"Graph vector representation: " << endl;
            for (auto it = street_list.begin(); it != street_list.end(); ++it) {
				cout << setprecision(11) << endl << "Node " << it->mapID << ": " << it->coord;
			}
		}

        // ------------ UTILITY FUNCTIONS ------------ //
		double getLatitud(xml_node<>* node){
			for(xml_attribute<>* atributte = node->first_attribute(); atributte; atributte = atributte->next_attribute()){
				if(strcmp(atributte->name(), "lat") == 0)
					return atof(atributte->value());
			}
			return 0;
		}

		double getLongitud(xml_node<>* node){
			for(xml_attribute<>* atributte = node->first_attribute(); atributte; atributte = atributte->next_attribute()){
				if(strcmp(atributte->name(), "lon") == 0)
					return atof(atributte->value());
			}
			return 0;
		}

		vector<Node> getNodeList() { return street_list; }

		void saveNodes(string file_name) {
			ofstream outfile (file_name);
			string output_str = "";
			
			for (auto it = street_list.begin(); it != street_list.end(); ++it) {
				output_str = output_str + it->mapID + "," 
					     + to_string(it->coord.latitud) + "," 
						 + to_string(it->coord.longitud)+ "\n";
			}
			if (output_str.size() > 0)
				output_str.pop_back();
			outfile << output_str; outfile.close();
		}
};