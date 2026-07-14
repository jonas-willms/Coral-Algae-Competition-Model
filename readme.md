### General Information
This code is based on work origninally created as part of an exam in a class called "Ecological Modelling with C++" 
The exam was to:
1: Find an ecological question (get it approved)
2: Implement and comment your code
I did not change much about the readme besides removing some details and adding this lines.


### About this program 

Author: Jonas Willms

Research question:
How does herbivory effect the composition of coral reefs?


#### BACKGROUND INFORMATION
Understanding the dynamics of coral reefs is of utmost importance to preserving them.
Most reefs are dominated either by corals or large, fleshy macroalage.
More and more reefs are and have been observed to shift, from once coral dominated states towards states, dominated by macroalage.
Research shows a strong correlation between corals and the presence of herbivory - for example in the Caribbean, the die off of the sea urchins (a once dominant herbivore), lead to the phase shift in many reefs with low parrotfish (another group of dominant herbivores) populations.
Here we want to investigate this phase separation w.r.t. the presence of herbivores, keeping algae at bay.


### ABOUT THIS PROGRAM 

#### What does the program do?
This program simulates a simplified reef, consisting of 4 different constituents:

Background: Essentially empty/freshly grazed areas

Turf algae: Small and juvenile algae of low height

Macroalage: Large, fleshy algae that directly compete with corals

Corals: Corals


Herbivory is implemented as a constant grazing pressure, representing the area that can maximally be grazed per year.
Destruction events/catastrophes can be included as well, as destruction events (for example through hurricanes or bleaching events) might shift transition points.

The program assumes periodic boundaries, to reduce finite size effects
Corals and turf algae can recruit independently of the current reef.
This mimics the presence of a larger area where corals and turf algae can seed from.

The reef is simulated as a grid, where each cell represents a certain area of the reef that can be inhabited by possibly a few coral polyps or multiplet of algae.
The area of one cell represents (according to the chosen transition probabilities) (10cm)^2.

The dynamics include cell intern dynamics:
    Death of corals, recruitment of corals and turf algae, as well as growth from turf algae to macroalgae.
And external dynamics:
    Overgrowth from neighboring cells and protection of large coral colonies from overgrowth, as they are rarely overgrown in real world observations.

Additionally, both algae can be "eaten" due to the grazing pressure and everything can be destroyed by "catrastrophic events".

#### How to use the program?
After starting the program, a window opens where the different base functions of the simulation class can be explored.

One can initialize a reef of a size defined in the code.
 - The sizes can be varied inside the mainwindow and secondwindow setup.
 - The program should adjust well to that - I chose reasonable values for computation times.
 - As they are not necessarily of significant interest here, I chose to not make them accessible during runtime.
    
After initializing, one can vary the herbivory and destruction using the sliders on the bottom.
- he herbivory is in percent, representing the area that can maximally be grazed, i.e. how many grid cells will be visited per year and set back to the background if algae are present.
- The destruction is in percent, representing the probability that a grid cell, regardless of the current state, is setback to background during a catastrophe.
    
If the sliders are not adjusted, both are initialized with 0%, all possible slider values are somewhat valid (yes, more than 100% of the reef can be visited by fishes per year).
To evolve the reef, one can perform a single time step or 10 to speed things up.
    - The default time step is set in the init function of the simulation class and can be varied there - it is not necessarily intended - If I did not forget anything it should not cause problems.
Destruction events/catastrophes can be triggered using the accordingly labeled button.

Similarly to the time steps, the transition probabilities are defined in the init function.
They are not meant to be changed, that's why they are not accessible for the user.
They are essentially educated guesses.
When changing them, be careful with the units.


To actually compare different herbivory scenarios, press the "Analysis Mode" button.
Pick the parameters and hit "Run Simulations".
Then for a given interval of herbivory and a steps size, the specified number of runs is executed, starting from a random - equally distributed - state.
After the equilibration time, the average over the evaluation time and number of runs is calculated for the number of coral states and compared to the number of macroalgae cells.
results close to 1 mean that corals dominate, close to 0 means that algae coverage is relatively high.
Only relative macroalgae and coral cover are taken into account, one could try different metrics than the one used here.

Here, a regular catastrophe event with a given destruction probability can be added as well.
Whilst not accurately, this can represent something like an expected frequency of hurricanes.


#### What output is produced?

Produced is a snapshot of the reef structure after a number of time steps.
For the second window, a graph, showing the dominance of corals to macroalage for different herbivory scenarios, is produced.
If it is close to 1 corals dominate, close to 0, macroalage dominate.
Also, the final state of each complete evaluation is displayed.


#### How could the program be used?

To answer the Research question, one should average over long times and many runs - qualitatively it is answerable with this code.
This shows how the system tends to algae or coral dominated states, in dependence of the herbivory.

To actually gain new insight and be of higher interest, the herbivory dynamics should be more realisitc/complex and at connected to different fishing measures - as they can be imposed by policies.
At best, a reasonable herbivore population modeling, in dependence of the available algae and fishing could be modeled, to see how different fishing measures affect the reef composition.
Similarly, a more realistic type of destruction would be interesting - for example, only effecting corals (see bleaching).
Also, it would be very interesting to do a multiple parameter search, varying the catastrophe events and see how this affects transition thresholds for herbivory.
Whilst the parameters are educated guesses and seem somewhat reasonable, detailed values, taken from experimental data, should be used to improve the meaningfulness of results.
Last but not least, different types of corals could be simulated to archive higher realism.


#### Structure
The project is in multiple files.

simulation:
 - The simulation class is the heart of the project.
 - It implements a simulation object that describes the reef as a 2d grid on which different functions operate on to simulate the evolution of the reef.
 - It is mostly self-contained but needs additional functions implemented in "functions".
    
functions:
 - Here, additional functions (that are used by the simulation) that are not specific to the declaration and definitions in the simulation class are implemented.
 - For example, a deep first search algorithm on a 2d array is implemented in here.
 - The DFS works on any 2d array with 1s and 0s, no need to interpret anything as corals etc. -> therefore it is not in "simulation".
    
mainwindow:
 - Essentially used to show how the simulation object works.
    
secondwindow:
 - Used to do a little parameter search.
    
tests:
 - A test for the implemented DFS algorithm.
 - It just tries some predefined example cases and checks that they work.


#### process and usage of AI 

I pre wrote some basics version of a simulation algorithm in python to check that it produces somewhat reasonable output.
Then I did implement it in c++ as it's own class and "went with the flow" for adding more functionallites.
When starting I used some ChatGPT 3.5 to get back into handling all the data types (promts like "How do I create a 2d vector of integers") and to quickly ask questions about qt functinallityes.
I used it a bit more in the end to get a legend for the main window as I did not know how do propperly only create a legend but it still required more of my own work than I was hoping for.
I recicled some of the code of the lecture for plotting.
Most information was gained by lecture material, documentation or previouse knowledge from other programming laguages.

During later iterations of this AI was used in a similar manner, including ChatGPT 5 and Gemini.


#### Other
To get back from the second to the first window, just close it.
!!!But!!! If the simulations are still running in the second window, this doesn't work and the programm will continue in the background and eventually terminate.


#### References
Status and Trends of Caribbean Coral Reefs: 1970-2012 (Jeremy Jackson, Mary K. Donovan, Katie L. Cramer), 2014, https://www.researchgate.net/publication/264157662_Status_and_Trends_of_Caribbean_Coral_Reefs_1970-2012
Thresholds and the resilience of Caribbean coral reefs (Peter J. Mumby, Alan Hastings, Helen J. Edwards), 2007, https://www.nature.com/articles/nature06252
Coral reef community dynamics and disturbance: a simulation model (O. Langmead, C. Sheppard), 2004, https://www.sciencedirect.com/science/article/abs/pii/S0304380003004812

