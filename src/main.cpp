//####################
//file: main.cpp
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################
#include "../include/run_class.h"
#include "../include/parameter_handler.h"

//define RNGengine (declared in include/misc.h)
std::mt19937_64 rng;

/** start of main with input handling and function calls
 * Takes input from input file and command line arguments
 * in form of Key=Value pairs. First argument is the 
 * parameter file. 
 * The parameter file has one Key=Value pair per line,
 * comments start with a '#'.
 * mandatory keys: alpha, beta, gamma, k, lambda, t_max, dt,
 * no_runs, mode, detailed_output. 
 *
 * run modes "mode=...":
 * standard run: 0; exp. dist. lifetime: 1; exp. dist.+fixed lifetime: 2;
 * single slow codon: 4-6; read RNA file, but av. elongation rate: 8.
 *
 * output modes "detailed_output=...":
 * 0 -> standard output: density, av. number of proteins
 * 1 -> detailed output: also print density and yield for each run
 * 2 -> time evolution density
 * 3 -> time evolution yield
 * 4 -> time evolution <density> <yield/time> <total yield until that time>
 * 5 -> print av. occupation per codon
 *
 * optional keys: length, RNA_filename.
 * example: ./simRNAtransl parameters.inp alpha=0.1 beta=0.5
 */
int main(int argc, char**argv){

    //generate real (system) random number and seed global RNGengine
    std::random_device sysrand;
    uint32_t random_seed=sysrand();
    rng.seed(random_seed); //seed RNGengine


    //simulation parameters
    parameter_container parameters; //all parameters are stored in parameter_container
    unsigned int length=400;
    std::string filename;//RNAdata filename container

    //set parameters
    parameter_handler params(argc,argv);
    params.SetValue("k",parameters.k,true);
    params.SetValue("alpha",parameters.alpha,true);
    params.SetValue("beta",parameters.beta,true);
    params.SetValue("gamma",parameters.gamma,true);
    params.SetValue("t_max",parameters.t_max,true);
    params.SetValue("dt",parameters.dt,true);
    params.SetValue("footprint",parameters.footprint,true);
    params.SetValue("lambda",parameters.lambda,true);
    params.SetValue("mode",parameters.mode,true);
    params.SetValue("no_runs",parameters.no_runs,true);
    params.SetValue("detailed_output",parameters.detailed_output,true);

    /* set run_mode according to input.
     *  1: single RNA run, 2: single normal run with given length
     */
    int run_mode=0;
    if (params.SetValue("length",length))
        run_mode=2;
    else if (params.SetValue("RNA_filename",filename))
        run_mode=1;
    else {
        std::cout <<"error in input: no filename or no length given"<<std::endl;
        exit(0);
    }

    //create simulation object
    run_class run;

    //choose simulation mode according to input
    if (run_mode==1){//RNA filename given, using actual codon rates
        RNAprocessing rna;//initialize RNA object
        rna.init(filename);//read RNA file and save in RNA object
        if (parameters.detailed_output>=0){//print some general output (with comment marker '#')
            std::cout <<"#RNAfilename is "<<filename<<" <<rng-seed used in this run is "<<random_seed<<">>"<<std::endl;
            print_parameters(std::cout,parameters,filename);
        }
        run.start_runs(rna,parameters);//start simulations (print output to stdout)
    }
    else if (run_mode==2){//single run with given parameters and length
        if (parameters.detailed_output>=0){//print some general output (with comment marker '#')
            std::cout <<"#<<rng-seed used in this run is "<<random_seed<<">>"<<std::endl;
            print_parameters(std::cout,parameters,length);
        }
        run.start_runs(length,parameters);//print some general output (with comment marker '#')
    }

    return 0;
}


