//####################
//file: misc.cpp
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#include "../include/misc.h"

void print_parameters(std::ostream &output,parameter_container parameters,int N_max, int N_step){
    output <<"# N_max "<< N_max<<" | ";
    output <<"N_step "<< N_step<<" | ";
    output <<"alpha "<< parameters.alpha<<" | ";
    output <<"beta "<< parameters.beta<<" | ";
    output <<"gamma "<< parameters.gamma<<" | ";
    output <<"k "<< parameters.k<<" | ";
    output <<"T "<< parameters.t_max<<" | ";
    output <<"delta_t "<< parameters.dt<<" | ";
    output <<"lambda "<< parameters.lambda<<" | ";
    output <<"footprint_size "<< parameters.footprint<<" | ";
    output <<"mode "<< parameters.mode<<" | ";
    output <<"no_runs "<< parameters.no_runs<<" | ";
    output <<std::endl;
}

void print_parameters(std::ostream &output,parameter_container parameters, std::string filename){
    output <<"# rna_data "<< filename<<" | ";
    output <<"alpha "<< parameters.alpha<<" | ";
    output <<"beta "<< parameters.beta<<" | ";
    output <<"gamma "<< parameters.gamma<<" | ";
    output <<"k "<< parameters.k<<" | ";
    output <<"T "<< parameters.t_max<<" | ";
    output <<"delta_t "<< parameters.dt<<" | ";
    output <<"footprint_size "<< parameters.footprint<<" | ";
    output <<"no_runs "<< parameters.no_runs<<" | ";
    output <<std::endl;
}


void print_parameters(std::ostream &output,parameter_container parameters,unsigned int length){
    output <<"# CDSlength "<< length<<" | ";
    output <<"alpha "<< parameters.alpha<<" | ";
    output <<"beta "<< parameters.beta<<" | ";
    output <<"gamma "<< parameters.gamma<<" | ";
    output <<"k "<< parameters.k<<" | ";
    output <<"T "<< parameters.t_max<<" | ";
    output <<"delta_t "<< parameters.dt<<" | ";
    output <<"footprint_size "<< parameters.footprint<<" | ";
    output <<"no_runs "<< parameters.no_runs<<" | ";
    output <<std::endl;
}

