#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <algorithm>
#include <vector>
#include <random>


/*
Here is a set of functions used that are not specific to the simulation object,
therefore they have their own class.
*/

/**
 * @brief Get a list of random index pairs.
 *
 * This function generates a list of n random index pairs for a given size.
 *
 * @param n The number of random index pairs to generate.
 * @param N_x The maximum value for the row index (exclusive).
 * @param N_y The maximum value for the column index (exclusive).
 * @param rng The random number generator engine (mt19937) to use for generating random numbers.
 * @return A vector of pairs representing the random index pairs.
 *
 * @note Significant slowdown for n approximately N_x*N_y
 */
std::vector<std::pair<int, int>> getRandomIndexSet(int n, int N_x, int N_y, std::mt19937& rng);

/**
 * @brief Perform depth-first search (DFS) algorithm on a grid with periodic boundaries.
 *
 * This function performs a depth-first search algorithm on a 2D grid with periodic boundaries.
 * It utilizes a list to remember visited cells, reducing calculation time compared to a naive approach.
 * It only considers the 4 neighborhood to be connected.
 * Inspired by: https://en.wikipedia.org/wiki/Depth-first_search.
 *
 * @param grid A pointer to the 2D grid represented by a vector of vectors of integers.
 * @param i The row index of the current cell.
 * @param j The column index of the current cell.
 * @param N_x The maximum value for the row index (exclusive).
 * @param N_y The maximum value for the column index (exclusive).
 * @param list A pointer to the list used for remembering visited cells, represented by a vector of pairs.
 * @return The number of cells visited during the depth-first search.
 *
 * @note The function implements periodic boundaries for the grid.
 */
int dfs_rec(std::vector<std::vector<int>>& grid, int i, int j, int N_x, int N_y, std::vector<std::pair<int,int>>& list);

/**
 * @brief Count the number of coherent 1s in a grid and return a grid with the number of identical neighbors for each cell.
 *
 * This function takes a pointer to a grid represented by a vector of vectors of integers containing 0s and 1s.
 * It counts the number of coherent 1s and returns a grid where each cell with a 1 contains the number of neighbors.
 * The called sub routine only considers the 4 neighborhood to be connected
 *
 * @param grid A pointer to the 2D grid represented by a vector of vectors of integers.
 * @return A 2D vector representing the grid with the number of neighbors for each cell.
 *
 * @note The function uses depth-first search (DFS) with remembering to count the number of coherent 1s.
 */
std::vector<std::vector<int>> dfs_remembering_grid(std::vector<std::vector<int>>& grid);


/**
 * @brief Convert the given grid to a binarized version.
 *
 * This function takes a pointer to a 2D grid represented by a vector of vectors of integers and a specified value.
 * It creates a new grid where cells with the specified value are set to 1, and all other cells are set to 0.
 *
 * @param grid A pointer to the 2D grid represented by a vector of vectors of integers.
 * @param value The value to be considered for binarization.
 * @return A binarized version of the input grid, where cells matching the specified value are set to 1 and others to 0.
 *
 * @note I am aware that the name suggest that it returns a boolean array, which would be more consistent...
*/
std::vector<std::vector<int>> binarization(const std::vector<std::vector<int>>& grid, int value);

int test_dfs();
#endif // FUNCTIONS_H
