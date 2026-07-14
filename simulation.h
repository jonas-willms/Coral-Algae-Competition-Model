#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector> // to use vectors
#include <random>   // for std::mt19937
#include <ctime>    // for time()
#include <iostream>
#include <tuple>


#include "functions.h"



/**
 * @brief The CellType enum differentiates between Background, Turf, Macroalgae, and Coral
 */
enum CellType { BACKGROUND = 0, TURF = 1, MACROALGAE = 2, CORAL = 3 };


/**
 * @brief The Simulation class simulates a coral reef with coral - algae interaction and herbivory.
 *
 * This class models a reef as a discrete grid where each grid cell can only be inhabited by:
 * nothing (B), Turf algae (T), Macroalgae (M) and Corals (C)
 * It models recrution, vegetation, herbivory and can include disasters destroying parts of the reef.
*/
class Simulation {
public:


    /* ################ Public functions ################ */

    /**
     * @brief Initializes the simulation with the given size parameters and sets up the field.
     *
     * @param size_x The size of the field along the x-axis.
     * @param size_y The size of the field along the y-axis.
     *
     * Initialize the simulation with educated guesses for all parameters that are not changable.
     * The field is initialized in a random state with equall probabilities.
     */
    void init(int size_x, int size_y);

    /*
     * @bried Performs a time step.
     *
     * Evolves the system for a single dt, defined in the init function.
    */
    void step();

    /*
     * @brief Set the herbivory as possible grazable area per year.
     *
     * @param n The maximal possible area in percent that can be grazed per year.
     *
    */
    void set_nherb_percent(int n);


    /*
     * @brief Set the reef into a random state and reset the counter.
     *
     * Sets the reef into a random state with equal probabilities for all possible field values and reset the step counter.
    */
    void reset_with_new_config();


    /**
     * @brief Sets each field back to the 0 state with the given probability.
     *
     * A simple model of a catrashopic event setting each effected field (probability probability) back to 0.
     *
     * @param probability The probability that a field will be set to the background state (0).
     *
     */
    void catastrophe(float probability );


    /**
     * @brief Get the time step dt.
     * @return dt Timesetep of the simulation.
     */
    float get_dt();

    /**
     * @brief Get the number of each cell type in the current reef.
     * @return Occupancy vector<int> containing the number of cells of each type.
     */
    std::vector<int> get_occupancy();

    /**
     * @brief Change the parameters for catastrophes.
     * @param e After how many years a new catastophe should happen.
     * @param p Probability of destruction for each cell.
     *
     */
    void change_catastrophe_params(int every, int probability);

    /**
     * @brief Get the reefField.
     * @return Read-only reference to the current reef state.
     */
    const std::vector<std::vector<int>>& get_reefField() const;

    /**
     * @brief Get the reefField_next.
     * @return Read-only reference to the next reef state buffer.
     */
    const std::vector<std::vector<int>>& get_reefField_next() const;
    /* ################ Public variables ################ */
    // None



private:
    /* ############### Private functions ############### */

    /**
     * @brief Execute internal dynamics.
     *
     * For each cell, the internal dynamics of each grid points are executed.
     * Calls internal_cell_dynamics() on each cell.
     */
    void internal_dynamics();

    /**
     * @brief Executes herbivory.
     *
     * Grabs a random set of indexes.
     * From these, if there are no corals, the cell is set to 0.
     * Represents fish eating the macro or turf algae present in the cell.
     */
    void herbivory();

    /**
     * @brief Realizes the effect on cells from other cells.
     *
     * The effects of vegetative growth from Turf, Macroalae and Corals, onto each other or the Background, are modeled here.
     * If a coral is part of a large colonie, it is unlikely to be overgrown and will be regarded as protected.
     * The following rules apply:
     * T can overgrow B
     * M can overgrow B
     * C can overgrow B
     *
     * M can overgrow T
     * C can overgrow T
     *
     * M can overgrow C only if C is part of a small community
     *
     * The number of 4 neighbors influences the probability of the overgrowth by the following equation:
     * P(overgrowth of X onto a field) = (1 - (1 - G_X)^ n_X),
     * where n_X is the number of 4 neighbors  of type X and G_X is the overgrowth probability.
     * If M and C try to overgrow onto the same cell, the chances are 50/50 - for a lack of a better description.
     */
    void external_dynamics();

    /**
     * @brief Represents the internal cell dynamics by taking a cell type and returning a new cell type.
     *
     * Uses death and recruitment rates to determine if and how a cell changes.
     * The following rules apply:
     *
     * T can seed on B
     * Corals can seed on B (coral recruitment)
     * Corals can seed on T
     *
     * M can grow from T if no corals grow (assumption: Small M are part of T)
     * C can die randomly
     *
     *
     * @param cell Integer representing the cell type
     * @return cell Integer representing the new cell type
     */
    int internal_cell_dynamics(int cell);

    /**
     * @brief Execute a catastrophe with the probability defined in the class.
     * Sets each sell back to B with probability destruction_probability
     */
    void catastrophe();

    /**
     * @brief Determine the number of neighbors of each type for a cell at (i,j)
     * @param i Index
     * @param j Index
     * @return tuple of integers with the number of neighbors of each type
     */
    std::tuple<int, int, int, int> determine_number_of_neighbors(int i, int j);


    /*
     * @brief Set the reef into a random state.
     *
     * Sets the reef into a random state with equal probabilities for all possible field values.
    */
    void set_random_field_values();


    /* ############### Private variables ############### */

    std::vector<std::vector<int>> reefField; // like the most important thing - the structure representing the reef
    std::vector<std::vector<int>> reefField_next; // reef at next dt to avoid memory allocation at each dt
    int size_x;
    int size_y;
    float dt; //
    float dx; // I might not use this, as it is included in the transition probabilities that I hardcoded

    // Transition probabilities (scaled in time)
    float P_BT;
    float P_BC;
    float P_TC;
    float P_TM;
    float P_CB;

    // Growth rates (scaled in time)
    float G_B;
    float G_T;
    float G_M;
    float G_C;

    // probability of a grid cell to be destroid during a catastrophy
    float destruction_probability;

    // Other parameters
    int n_herb;
    int herbivory_every;
    int catastrophy_every;
    int C_protected;
    int step_number;

    // Random number stuff
    std::mt19937 rng; // Random number generator
    std::uniform_real_distribution<double> rng_01; // Distribution for numbers between 0.0 and 1.0

};
#endif // SIMULATION_H
