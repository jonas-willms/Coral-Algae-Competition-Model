#include "tests.h"
using namespace std;

int test_dsf() {
    // Create a sample grid with 0s and 1s
    vector<vector<int>> test_grid0 = {
        {1, 0, 0, 0, 1, 0, 1},
        {1, 1, 0, 1, 0, 1, 0},
        {0, 0, 1, 0, 1, 1, 1},
        {1, 1, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 1, 0, 1},
        {0, 1, 1, 0, 1, 0, 1},
        {0, 0, 1, 0, 1, 0, 0}
    };

    vector<vector<int>> solution_grid0 = {
        {4, 0, 0, 0, 4, 0, 4},
        {4, 4, 0, 1, 0, 5, 0},
        {0, 0, 1, 0, 5, 5, 5},
        {2, 2, 0, 0, 0, 5, 0},
        {0, 0, 4, 0, 4, 0, 2},
        {0, 4, 4, 0, 4, 0, 2},
        {0, 0, 4, 0, 4, 0, 0}
    };


    vector<vector<int>> test_grid1 = {
        {0, 0, 0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 1, 0}
    };

    vector<vector<int>> solution_grid1 = {
        {0, 0, 0, 0, 0, 13, 0},
        {0, 8, 8, 8, 0, 13, 0},
        {0, 8, 0, 8, 0, 13, 0},
        {0, 8, 8, 8, 0, 13, 0},
        {0, 0, 0, 0, 0, 13, 0},
        {13, 13, 13, 13, 13, 13, 13},
        {0, 0, 0, 0, 0, 13, 0}
    };

    vector<vector<int>> test_grid2 = {
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
    };

    vector<vector<int>> solution_grid2 = {
        {24, 24, 24, 24, 24, 24},
        {24, 24, 24, 24, 24, 24},
        {0, 0, 0, 0, 0, 0},
        {24, 24, 24, 24, 24, 24},
        {24, 24, 24, 24, 24, 24},
    };

    vector<vector<int>> test_grid3 ={
          {1, 1, 1},
          {1, 1, 1},
          {1, 1, 1},
    };

    vector<vector<int>> solution_grid3={
          {9, 9, 9},
          {9, 9, 9},
          {9, 9, 9},
          };

    // compute results for each test grid
    vector<vector<int>> result_grid0 = dfs_remembering_grid(test_grid0);
    vector<vector<int>> result_grid1 = dfs_remembering_grid(test_grid1);
    vector<vector<int>> result_grid2 = dfs_remembering_grid(test_grid2);
    vector<vector<int>> result_grid3 = dfs_remembering_grid(test_grid3);

    // compare
    bool test0 = (result_grid0 == solution_grid0);
    bool test1 = (result_grid1 == solution_grid1);
    bool test2 = (result_grid2 == solution_grid2);
    bool test3 = (result_grid3 == solution_grid3);


    cout << "Running test for implemented DFS...\n" << "1 for success, 0 for fail...\n";
    // print the results
    cout << "Comparison for test_grid0:" << (test0 ? "PASS" : "FAIL") << "\n";
    cout << "Comparison for test_grid1:" << (test1 ? "PASS" : "FAIL") << "\n";
    cout << "Comparison for test_grid2:" << (test2 ? "PASS" : "FAIL") << "\n";
    cout << "Comparison for test_grid3:" << (test3 ? "PASS" : "FAIL") << "\n";

    if (test0 && test1 && test2 && test3){
        cout << "Test successfull\n";
        return 1;
    }else{
        cout << "TEST FAILED!\n";
        return 0;
    }


}
