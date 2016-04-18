//####################
//file: run_class.h
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#ifndef __RUN_CLASS__
#define __RUN_CLASS__

#include "inputRNA.h"
#include "misc.h"

#include <vector>
#include <deque>
#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <fstream>
#include <cstring>

/** Stores two variables needed for each ribosome:
 * 1. ribosome position. 
 * 2. wether or not this ribosome can move forward on RNA (saves computation time).
 */
struct ribosome_type{
    int pos=-1;
    bool movement_allowed=true;
};

/** Performs actual stochastic simulations. Prints output to stdout. Sample usage:
 * run_class run();
 * run.start_runs(rna_objetc,parameters);
 */
class run_class{
    public:
        ///default constructor. Does nothing
        run_class();
        ///constructs run_class object with parameters
        run_class(parameter_container parameters);

        /** Starts runs with length and elongation rates adjusted according to rna object.
         * Prints results to stdout according to parameters.detailed_output.
         */
        int start_runs(RNAprocessing rna,parameter_container parameters);

        /* Starts runs with uniform elongation rates and given length.
         * Prints results to stdout according to parameters.detailed_output.
         */
        int start_runs(std::vector<bool>::size_type length,parameter_container parameters);

    private:

        /** performs the actual simulation.
         * Uses parameters set in the constructor of the class.
         * Writes observables to output containers:
         * occupation_over_time
         * yield_over_time
         * total_yield_over_time
         * occupation_per_codon
         * average_yield
         * average_occupancy
         * average_no_proteins_at_end
         */
        int run();

        /** Checks for collision between position pos_left and pos_right.
         * Takes footprint size of ribosome into account.
         * Returns TRUE (1) on collision, else FALSE (0).
         */
        bool collision(int pos_left,int pos_right);

        /** Updates movement_allowed flag of ribosome rib_id.
         * Writes TRUE or FALSE to movement_allowd flag on the ribosome.
         * If rib_id is invalid FALSE is returned!
         */
        bool update_collision_flag(int rib_id);

        /** Clears result variables to start next simulation run.
         * needs to be called before new simulation with different 
         * parameters is run. Also resizes some vectors.
         */
        bool clear_results_variables();

        /** Prints simulation results to stdout.
         * Depends on parameters.detailed_output.
         * prints nothing if parameters.detailed_output < -1.
         * increasing verbosity if detailed_output increased.
         */
        void print_output();

        /** Return more detailed output.
         * print time evolution for density, yield and 
         * effective initiation according to 
         * parameters.detailed_output.
         */
        void return_detailed_output();

        /** save the simulation observables into variables.
         * uses the data from all ribosomes and fills the appropiate counters.
         */
        void save_measurements_over_time(double next_measurement_time,unsigned int & yieldcounter, double total_yield);

        double measurement_timestep; ///<time interval for measurements of system

        parameter_container parameters;///<Contains system parameters.

        /** Stores the length of the open reading frame (ORF). 
        *ORF length is equal to TASEP lattice size.
        */
        int orf_length;

        /** Additional simulation time based on transcript length.
        * Makes sure that transcript is empty in the end (initiation site decays first in eucaryotes).
        * Calculation of time averaged quantities becomes tricky.
        */
        double t_end;


        double t_max_sum;///< container for average simulation time (if mode==1, drawn from exponential)


        ///Generates random 'real' numbers in the intervall [0;1).
        static std::uniform_real_distribution<double> uniform01;

        ///Contains all ribosomes in the system. 
        std::deque<ribosome_type> ribosomes;

        ///Contains the elongation rates for each codon position.
        std::vector<double> hopping_rate;

        ///Actual used TASEP parameter alpha.
        double alpha;
        ///Actual used TASEP parameter beta.
        double beta;
        ///Actual used TASEP parameter gamma.
        double gamma;

        std::vector<int> occupation_over_time; ///<Contains the ribosome occupation on the RNA over time.
        std::vector<int> yield_over_time;///<Contains the protein yield/time (flux) over time.
        std::vector<int> total_yield_over_time; ///<Contains the total yield until certain time

        ///Ribosome occupation per codon
        std::vector<int> occupation_per_codon;

        ///Average protein yield that will be returned.
        double average_yield;
        ///Average ribosome occupancy on RNA that will be returned.
        double average_occupancy;
        ///Average number of proteins that are on the transcript after simulation stops
        double average_no_proteins_at_end;

};
#endif
