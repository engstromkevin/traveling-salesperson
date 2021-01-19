#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Kevin Engstrom
// Note: I use a weight of -1 to represent infinity

using namespace std;

// Using INT_MAX brings risk of overflow, so this is safer
static int BIG_INT = INT_MAX / 2;

int countLines(ifstream &in, vector<vector<int>> &mat, vector<pair<int, string>> &cities, int count = 0) {
	string lineText;
	int lineCount = count;

	if (getline(in, lineText, '\n')) {
		lineCount++;
		// Recursively find the number of cities (number of lines in the .txt)
		int maxLines = countLines(in, mat, cities, lineCount);
		
		// This code starts once the stack begins to unravel		
		// Now read in variables using a stringstream to avoid substr() shenanigans
		stringstream ss = stringstream(lineText);
		// First, read in vertex id
		int vindex;
		ss >> vindex;
		ss.ignore();
		// Next, read in city name and save id/name pair
		string name;
		getline(ss, name, ' ');
		cities.insert(cities.begin(), make_pair(vindex, name));

		vector<int> weights = vector<int>(maxLines, -1);

		// Will read in each of the vertex id/edge weight pairs
		for (int i = 1; i <= maxLines; i++) {
			int vid;
			ss >> vid;

			if (i != vid) {
				// edge i is missing
			}

			int weight;
			ss >> weight;
			
			if(i == vid) {
				// edge is present
			}
			else {
				// edge is missing
			}

			if ((vid == vindex) && (weight != 0)) {
				// there is a self-loop, but nothing special has to be done
			}
			
			weights.at(vid - 1) = weight;
		}

		// insert vector<int> at beginning of mat when done, because its being read backwards
		mat.insert(mat.begin(), weights);

		return max(maxLines, lineCount);
	}

	return lineCount;
}

/* Way too slow! */
/* Calculates the shortest path from vid to v1, passing through each vertex in vSet exactly once */
int shortestPathBAD(int vid, int vSet, vector<vector<int>> adjMat, vector<vector<pair<int, int>>> &paths) {
	// A is a subset of V - {v1, vi}
	// D[i][A] = minimum(W[i][j] + D[j][A - {vj}])
	// vj is an element of A
	
	// vSet takes the place of A here
	// vid takes the place of i here
	// adjMat takes the place of W here
	
	if (vSet == 0) {
		if (adjMat[vid][0] > 0) {
			paths[vid][vSet].first = adjMat[vid][0];
			return adjMat[vid][0];
		}
		
		return BIG_INT;
	}

	int currMin = BIG_INT;
	if (paths[vid][vSet].first != -1) {
		currMin = paths[vid][vSet].first;
	}
	
	for (size_t j = 1; (1 << j) <= vSet; j++) {
		if ((adjMat[vid][j] > 0) && (j != vid)) {
			//cout << "currMin = min( " << currMin;
			//cout << ", " << adjMat[vid][j] + shortestPath(j, vSet - (1 << j), adjMat, paths) << " )" << endl;
			int w = adjMat[vid][j];
			int subset = vSet - (1 << j);
			int d = shortestPathBAD(j, subset, adjMat, paths);

			if (subset != 0) {
				//cout << "subset = " << vSet << ", j = " << j << ", subsubset = " << subset << endl;
				//cout << "\tw = " << w << ", d = " << d << endl;
			}
			
			int n = w + d;

			if (n < currMin) {
				cout << "subset = " << vSet << ", j = " << j << ", subsubset = " << subset << endl;
				cout << "\tw = " << w << ", d = " << d << endl;
				cout << "replace currMin, vid = " << vid << ", vSet = " << vSet << ", newMin = " << n << endl;
				currMin = n;
				paths[vid][vSet] = make_pair(n, j);
			}
			else if (currMin != BIG_INT) {
				paths[vid][vSet].first = currMin;
			}
		}
	}

	return currMin;
}

/* Calculates the shortest path from vid to v1, passing through each vertex in vSet exactly once */
int shortestPath(int vid, int vSet, vector<vector<int>> adjMat, vector<vector<pair<int, int>>>& paths) {
	// A is a subset of V - {v1, vi}
	// D[i][A] = minimum(W[i][j] + D[j][A - {vj}])
	// vj is an element of A

	// vSet takes the place of V here
	// subset takes the place of A here
	// adjMat takes the place of W here

	size_t n = adjMat.size();

	// Fill the initial values of D using W
	for (size_t i = 0; i < n; i++) {
		int edge = adjMat.at(i).at(0);
		// Check if edge exists
		if (edge < 0) {
			paths.at(i).at(0) = make_pair(BIG_INT, -1);
		}
		else {
			paths.at(i).at(0) = make_pair(edge, 0);
		}
	}

	// Go through each subset of vSet that doesn't contain v1
	for (size_t subset = 2; subset <= vSet; subset += 2) {
		for (size_t i = 1; i < n; i++) {
			// Make sure subset does not contain vertex with id = (i + 1)
			if ((subset & (1 << i)) == 0) {
				int currMin = BIG_INT;

				for (size_t j = 1; j < n; j++) {
					// Make sure i-j edge exists and is not just a vertex
					if ((adjMat.at(i).at(j) > 0) && (j != vid)) {
						// Make sure subset does contain vertex with id = (j + 1)
						if ((subset & (1 << j)) != 0) {
							int w = adjMat.at(i).at(j);
							int subsubset = subset - (1 << j);
							int d = paths.at(j).at(subsubset).first;

							if ((d > 0) && (d < BIG_INT)) {
								int newMin = w + d;

								if ((newMin > 0) && (newMin < currMin)) {
									paths.at(i).at(subset) = make_pair(newMin, j);
									currMin = newMin;
								}
								else if (currMin != BIG_INT) {
									paths.at(i).at(subset).first = currMin;
								}
							}
						}
					}
				}
			}
		}
	}

	// This gets the set of all vertices except for v1
	size_t fullset = (1 << adjMat.size()) - 2;
	int currMin = BIG_INT;

	for (size_t j = 1; j < n; j++) {
		// Make sure i-j edge exists and is not just a vertex
		if ((adjMat.at(0).at(j) > 0) && (j != vid)) {
			int w = adjMat.at(0).at(j);
			int subsubset = fullset - (1 << j);
			int d = paths.at(j).at(subsubset).first;

			if ((d > 0) && (d < BIG_INT)) {
				int newMin = w + d;

				if (newMin < currMin) {
					paths.at(0).at(fullset) = make_pair(newMin, j);
					currMin = newMin;
				}
				else if (currMin < newMin) {
					paths.at(0).at(fullset).first = currMin;
				}
			}
		}
	}
	
	return paths.at(vid).at(vSet).first;
}

string vertexSet(int n, int vSet) {
	string result = "";

	for (size_t i = 0; i < n; i++) {
		if ((vSet & (1 << i)) != 0) {
			int vertID = i + 1;
			result += " v";
			result += to_string(vertID);
			result += " ";
		}
	}

	return result;
}

string printPath(int startVertex, int set, int tourCost, vector<pair<int, string>> cities, vector<vector<pair<int, int>>> paths) {
	int numCities = cities.size();
	string output = "";
	int lengthOfPath = 0;

	if (tourCost < BIG_INT) {
		output += "Optimal Tour Cost = [" + to_string(tourCost) + ", ";
		int vertex = startVertex;
		output += "<" + cities[startVertex].second;
		while (set > 0) {
			int temp = paths[vertex][set].first;
			vertex = paths[vertex][set].second;

			// A path with a vertex of -1 is a clear sign this is wrong
			if (vertex < 0) {
				output = "No solution found";
				break;
			}

			output += "," + cities[vertex].second;
			lengthOfPath++;
			set -= (1 << vertex);
		}
		output += ">]";

		if ((startVertex == 0) && (lengthOfPath != (numCities - 1))) {
			output = "No solution found";
		}
	}
	else {
		output = "No solution found";
	}

	return output;
}

int main() {
	string filename;
	cout << "Enter Filename: ";
	getline(cin, filename);
	ifstream infile(filename);

	// Vector containing pairs of vertex id and city name for lookup later
	vector<pair<int, string>> cities = vector<pair<int, string>>();

	// Create and populate the initial adjacency matrix
	vector<vector<int>> adjMat = vector<vector<int>>();
	countLines(infile, adjMat, cities);
	int numCities = cities.size();

	infile.close();

	// Prints the values of the matrix for testing
	/*for (vector<int> weights : adjMat) {
		for (int w : weights) {
			cout << w << " ";
		}
		cout << endl;
	}
	cout << endl;*/
	
	// Combined distance matrix D and path matrix P into one vector of pairs<D[x][y], P[x][y]>
	vector<pair<int, int>> pathColumns = vector<pair<int, int>>((1 << numCities), make_pair(-1, -1));
	vector<vector<pair<int, int>>> paths = vector<vector<pair<int, int>>>(numCities, pathColumns);

	/*int v1 = (1 << 0);
	int v2 = (1 << 1);
	int v3 = (1 << 2);
	int v4 = (1 << 3);
	int v5 = (1 << 4);
	int v6 = (1 << 5);
	int v7 = (1 << 6);
	int v8 = (1 << 7);
	int v9 = (1 << 8);
	int v10 = (1 << 9);

	int j1 = v2 + v4 + v8 + v10 + v7;
	int vid = 5;
	int totalCost = shortestPath(vid, j1, adjMat, paths);
	cout << "\tD[v" << vid + 1 << "][" << vertexSet(numCities, j1) << "] = " << totalCost << endl;

	printPath(vid, j1, totalCost, cities, paths);*/
	
	int vStart = 0;
	int vSet = (1 << numCities) - 2;
	int tourCost = shortestPath(vStart, vSet, adjMat, paths);

	string output = printPath(vStart, vSet, tourCost, cities, paths);

	cout << output << endl;
	
	// Write the same output to an output file to avoid having to copy/paste manually
	string s = "output";
	s += filename.at(filename.length() - 5);
	ofstream outfile(s + ".txt");
	outfile << output << endl;

	outfile.close();
	return 0;
}