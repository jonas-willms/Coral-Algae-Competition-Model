#include "simulation.h"
#include <iostream>

using namespace std;

void Simulation::init(int size_x, int size_y){
    this->size_x = size_x;
    this->size_y = size_y;

    //seed with the time when initializing
    rng.seed(time(nullptr));


    // Initialize the uniform real distribution for numbers between 0.0 and 1.0
    rng_01 = uniform_real_distribution<double>(0.0, 1.0);

    // set the field size
    reefField.resize(size_x, std::vector<int>(size_y));
    reefField_next.resize(size_x, std::vector<int>(size_y));

    // set the values randomly
    set_random_field_values();



    /* #################### Here are all the variables one might want to change to fit data #################### */

    dt = 0.1; // time step
    // TODO: Dynamically integrate dx into transition probabilities in future iterations.
    dx = 0.1; // spatial discretisation - not used as I included it in the edjucated guesses for the transition probabilities


    // Base transition probabilities (per year)
    // Note: Values are empirical approximations derived from literature.
    P_BT = 0.80; // Background to Turf algae
    P_BC = 0.80; // Background to Coral
    P_TC = 0.01; // Turf algae to Coral
    P_TM = 0.33; // Turf algae to Macroalgae
    P_CB = 0.15; // Coral mortality (Coral to Background)

    // Growth rates
    G_B = 0.0;  // growth rate of B
    G_T = 1.0;  // growth rate of T
    G_M = 0.5;  // growth rate of M
    G_C = 0.01; // growth rate of C

    // Area protection threshold: number of connected corals fields that are too large to be overgrown
    C_protected = 9;

    // Scale probabilities and growth rates by the time step (dt)
    P_BT *= dt;
    P_BC *= dt;
    P_TC *= dt;
    P_TM *= dt;
    P_CB *= dt;

    G_B *= dt;
    G_T *= dt;
    G_M *= dt;
    G_C *= dt;

    // Initialize Herbivores
    int percentage_max_graze = 0;// Configurable via GUI
    double percentage = static_cast<double>(percentage_max_graze) / 100.0;
    n_herb = static_cast<int>(percentage * size_x * size_y * dt);
    /*
     I'm just making the assumption here that if the
     total algae coverage is low, the fish will leave or die
     therefore I will just assume that this value is somewhat indicative of the value of fields
     that the fish can visit
     It  represents the area that can be grazed per year ( can be > 100%, maxium value I found somewhere is +200%)
    */

    // Disturbance / Catastrophe parameters
    catastrophy_every = 0;         // Step frequency for catastrophic events (0 = disabled)
    destruction_probability = 0.0; // Base probability of cell destruction, configurable via GUI

    // set the step counter to 0
    step_number = 0;
}


void Simulation::set_random_field_values(){
    //create a random state of the field

    uniform_int_distribution<int> stateDist(0, 3);


    for (int i = 0; i < size_x; ++i) {
        for (int j = 0; j < size_y; ++j) {
            // Generate random numbers between 0 and 3
            reefField[i][j] = stateDist(rng);
            reefField_next[i][j] = reefField[i][j]; // Sync buffer
        }
    }
}
// Definition of the step function
void Simulation::step() {


    // Apply large-scale catastrophic events prior local dynamics
    if ( (catastrophy_every > 0) && (step_number % catastrophy_every == 0) ){
        catastrophe();
    }


    // Internal dynamics: Growth onto background and turf and spontaneous death
    internal_dynamics();
    // Herbivory: Grazing effects
    herbivory();
    // External dynamics: Spatial competition and neighbor-dependent overgrowth
    external_dynamics();

    step_number++;
}


void Simulation::herbivory() {
    // Select random cells for grazing based on the herbivore population
    vector<pair<int, int>> idxs = getRandomIndexSet(n_herb, size_x, size_y, rng);

    for (int k = 0; k < n_herb; k++) {
        int i = idxs[k].first;  // Row index
        int j = idxs[k].second; // Col index

        // Herbivores graze on anything that is not coral, reducing it to background
        if (reefField[i][j] != CORAL) {
            reefField[i][j] = BACKGROUND;
        }
    }
}

// Default catastrophe uses the globally defined probability
void Simulation::catastrophe() {
    catastrophe(destruction_probability);
}

// Overloaded catastrophe allows for custom probability events
void Simulation::catastrophe(float probability) {
    // Iterate over each cell in reefField
    for (int i = 0; i < size_x; ++i) {
        for (int j = 0; j < size_y; ++j) {
            // If the random number is less than the probability, cell is destroyed
            if (rng_01(rng) < probability) {
                reefField[i][j] = BACKGROUND;
            }
        }
    }
}


// Definition internal dynamics
void Simulation::internal_dynamics() {

    // iterate over all cells and execute internal dynamic of each cell
    for(int i = 0; i < size_x; i++){
        for(int j  = 0; j < size_y; j++){
            reefField[i][j] = internal_cell_dynamics(reefField[i][j]);
        }
    }
}

// returns the new value of the cell, only looking at the internal transition probabilities
int Simulation::internal_cell_dynamics(int cell) {

    // Order of execution matches priority ordering
    if (cell == 0) {
        if (rng_01(rng) < P_BT ) {
            cell = 1; // T can seed on Background
        }
        if (rng_01(rng) < P_BC ) {
            cell = 3; // Corals can seed on Background
        }
    } else if (cell == 1) {
        if (rng_01(rng) < P_TC ) {
            cell = 3; // Corals can seed on T
        } else if (rng_01(rng) < P_TM ) {
            cell = 2; // if no Corals grow, T can grow to Macroalgae
        }
    } else if (cell == 3) {
        if (rng_01(rng) < P_CB ) {
            cell = 0; // Corals can die randomly
        }
    }

    return cell;
}


void Simulation::external_dynamics() {
    // Precompute overgrowth probabilities to avoid pow() inside the loops.
    double prob_grow_T[5], prob_grow_M[5], prob_grow_C[5];
    for (int n = 0; n <= 4; ++n) {
        prob_grow_T[n] = 1.0 - std::pow(1.0 - G_T, n);
        prob_grow_M[n] = 1.0 - std::pow(1.0 - G_M, n);
        prob_grow_C[n] = 1.0 - std::pow(1.0 - G_C, n);
    }

    // Prepare the buffer.
    // Copy the current state to preserve cells that undergo no transitions.
    reefField_next = reefField;

    int n_B, n_T, n_M, n_C;

    // Generate coral clusters to evaluate size-based protection
    // TODO: Consider optimizing binarization and dfs to avoid re-allocating vectors every step
    vector<vector<int>> coral_grid = binarization(reefField, CORAL);
    vector<vector<int>> coral_dfs_grid = dfs_remembering_grid(coral_grid);

    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_y; j++) {

            // Large coral colonies are protected
            if (reefField[i][j] == CORAL && coral_dfs_grid[i][j] >= C_protected) {
                continue;
            }

            // Determine neighbor counts for spatial competition
            tie(n_B, n_T, n_M, n_C) = determine_number_of_neighbors(i, j);

            bool CM_competition = false;

            // Turf overgrows Background
            if (reefField[i][j] == BACKGROUND) {
                if (rng_01(rng) < prob_grow_T[n_T]) {
                    reefField_next[i][j] = TURF;
                }
            }

            // Macroalgae overgrows Background and Turf
            if (rng_01(rng) < prob_grow_M[n_M]) {
                CM_competition = true;
                reefField_next[i][j] = MACROALGAE;
            }

            // Coral overgrows everything but Macroalgae
            if (rng_01(rng) < prob_grow_C[n_C]) {
                if (!CM_competition) {
                    reefField_next[i][j] = CORAL;
                } else {
                    // Competition case: Both Macroalgae and Coral attempt to overgrow the same cell.
                    // Resolved stochastically (equal probability).
                    if (rng_01(rng) < 0.5) {
                        reefField_next[i][j] = MACROALGAE;
                    } else {
                        reefField_next[i][j] = CORAL;
                    }
                }
            }
        }
    }

    // Update the main grid by swapping pointers
    std::swap(reefField, reefField_next);
}

// Calculate the state distribution of the (4 adjacent cells)
// utilizing periodic boundary conditions.
tuple<int, int, int, int> Simulation::determine_number_of_neighbors(int i, int j) {

    // Initialize neighbor counts
    int n_B = 0, n_T = 0, n_M = 0, n_C = 0;

    // Define spatial offsets (North, South, East, West)
    const vector<pair<int, int>> offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    // Iterate over the 4 neighboring cells
    for (const auto& offset : offsets) {
        int k = i + offset.first;
        int l = j + offset.second;

        // Apply periodic boundary conditions (wrap-around)
        if (k < 0) {
            k = size_x - 1;
        } else if (k >= size_x) {
            k = 0;
        }

        if (l < 0) {
            l = size_y - 1;
        } else if (l >= size_y) {
            l = 0;
        }

        // Tally the state of the neighbor
        switch (reefField[k][l]) {
        case BACKGROUND:
            n_B++;
            break;
        case TURF:
            n_T++;
            break;
        case MACROALGAE:
            n_M++;
            break;
        case CORAL:
            n_C++;
            break;
        default:
            // TODO: Implement robust error logging for invalid cell states
            break;
        }
    }

    return make_tuple(n_B, n_T, n_M, n_C);
}

void Simulation::set_nherb_percent(int n) {
    double percentage = static_cast<double>(n) / 100.0;
    n_herb = static_cast<int>(percentage * size_x * size_y * dt);
}


vector<int> Simulation::get_occupancy() {
    std::vector<int> counts(4, 0); // Initialize counts for all 4 states to zero

    // Iterate through each cell and tally the states
    for (int i = 0; i < size_x; ++i) {
        for (int j = 0; j < size_y; ++j) {
            int cell_state = reefField[i][j];

            // Bounds check to ensure we only increment valid states
            if (cell_state >= BACKGROUND && cell_state <= CORAL) {
                counts[cell_state]++;
            } else {
                // TODO: Implement proper error handling/logging for invalid states
                cout << "Somehow something new has entered the reef. "
                     << "If you see this message (outside of the code) something went wrong. ";
            }
        }
    }

    return counts;
}

float Simulation::get_dt(){
        return this->dt;

}

void Simulation::reset_with_new_config(){
        set_random_field_values();
        step_number = 0; // reset step counter
}



void Simulation::change_catastrophe_params(int every, int probability){

        this->catastrophy_every = static_cast<int>(every / dt + 0.5); // + 0.5 for correct rounding
        this->destruction_probability = static_cast<float>(probability) / 100.0; // probability in % to float

}

const vector<vector<int>>& Simulation::get_reefField() const {
        return reefField;
}

const vector<vector<int>>& Simulation::get_reefField_next() const {
        return reefField_next;
}
