//####################
//file: misc.h
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#ifndef MISC
#define MISC

#include <random>
#include <iostream>

/** Contains all system parameters for the model. 
 */
struct parameter_container{
    double alpha;             //de-novo initiation rate
    double beta;                //removal rate
    double gamma;                 //recycling rate
    double k;                   //hopping rate
    int footprint=1;            //ribosome size on RNA
    double t_max;               //maximum simulation time (or time distribution parameter for mode >=10)
    double dt;                  //measurement timestep 
    unsigned int no_runs=50;    //number of runs
    int detailed_output=0;      //output mode, possible values: -1,0,1,2,3,4
    int mode=0;                 //simulation mode: if < 10 simulation stops immediately. mode%10==1: exponential lifetime distribution, mode%10==2: exponential distribution+no decay in beginning, mode==0: uniform distribution
    double lambda;              //parameter (for various uses, depending on "mode")
};

/** share one RNG engine across whole project.
 */
extern std::mt19937_64 rng;

/**Prints parameters to output.  
 * Adds a "#" in front of each line to mark a 'comment' output.
 */
void print_parameters(std::ostream &output,parameter_container parameters,int N_max, int N_step);

/**Prints parameters to output.  
 * Adds a "#" in front of each line to mark a 'comment' output.
 */
void print_parameters(std::ostream &output,parameter_container parameters, std::string filename);

/**Prints parameters to output.  
 * Adds a "#" in front of each line to mark a 'comment' output.
 */
void print_parameters(std::ostream &output,parameter_container parameters,unsigned int length);

#endif
