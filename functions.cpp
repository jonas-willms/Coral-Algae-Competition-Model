//here helper functions for simulation.h may be moved to in the future
#include "functions.h"
#include <iostream>

using namespace std;

// get a list of n random index pairs
vector<pair<int, int>> getRandomIndexSet(int n, int N_x, int N_y, mt19937& rng){

    vector<pair<int, int>> indexes;

    // the function is not called realy often (1 per time step) we can probably redefine this without problems
    uniform_int_distribution<int> rowDist(0, N_x - 1);
    uniform_int_distribution<int> colDist(0, N_y - 1);

    //TODO: Proper error handling should be added eventually
    // Warn and return all grid indeces if more are requested
    if (n >= N_x * N_y) {
        cout << "Warning: Requested more unique indices than available. "
                     "Returning all grid cells.\n";

        std::vector<std::pair<int,int>> indexes;
        indexes.reserve(N_x * N_y);

        for (int i = 0; i < N_x; ++i)
            for (int j = 0; j < N_y; ++j)
                indexes.emplace_back(i, j);

        return indexes;
    }

    while (indexes.size() < static_cast<size_t>(n)) {

        int randomRow = rowDist(rng);
        int randomCol = colDist(rng);

        pair<int, int> newIndex{randomRow, randomCol};

        if (find(indexes.begin(), indexes.end(), newIndex) == indexes.end()) {
            indexes.push_back(newIndex);
        }
    }

    return indexes;
}



int dfs_rec(vector<vector<int>>& grid, int i, int j, int N_x, int N_y, vector<pair<int,int>>& list){

    // implement periodic boundaries
    if (i < 0){
        i = N_x - 1;
    } else if (i >= N_x ){
        i = 0;
    }
    if (j < 0){
        j = N_y - 1;
    }else if (j >= N_y){
        j = 0;
    }

    if (grid[i][j] == 0){
        return 0;
    }

    // modify cell
    grid[i][j] = 0;
    // push index into list
    list.emplace_back(i, j);

    // recursion
    return 1
        + dfs_rec( grid, i+1, j, N_x,N_y, list)
        + dfs_rec( grid, i-1, j, N_x,N_y, list)
        + dfs_rec( grid, i, j+1, N_x,N_y, list)
        + dfs_rec( grid, i, j-1, N_x,N_y, list);

}


vector<vector<int>> dfs_remembering_grid(vector<vector<int>>& grid)
{
    // Return an empty grid if the input is empty
    if (grid.empty()) {
        return {};
    }

    // Get the grid dimensions
    int N_x = grid.size();
    int N_y = grid[0].size();

    // Create a grid of the same size to store the cluster sizes
    vector<vector<int>> counter(N_x, vector<int>(N_y));

    int count = 0;
    vector<pair<int, int>> list; // Stores the coordinates of the current cluster

    for (int i = 0; i < N_x; i++) {
        for (int j = 0; j < N_y; j++) {

            // Skip cells that have already been visited
            if (grid[i][j] == 0) {
                continue;
            }

            list.clear();

            count = dfs_rec(grid, i, j, N_x, N_y, list);

            // Write the cluster size back to all cells in the cluster
            for (const auto& entry : list) {
                int row = entry.first;
                int col = entry.second;

                counter[row][col] = count;
            }
        }
    }

    return counter;
}

vector<vector<int>> binarization(const vector<vector<int>>& grid, int value)
{
    // Return an empty grid if the input is empty
    if (grid.empty()) {
        return {};
    }

    // Get the grid dimensions
    int N_x = grid.size();
    int N_y = grid[0].size();

    vector<vector<int>> bin(N_x, vector<int>(N_y));

    // Iterate through the grid and set the output to 0 or 1
    for (int i = 0; i < N_x; i++) {
        for (int j = 0; j < N_y; j++) {
            bin[i][j] = (grid[i][j] == value);
        }
    }

    return bin;
}
