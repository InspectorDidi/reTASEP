//####################
//file: run_class.cpp
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#include "../include/run_class.h"


run_class::run_class(parameter_container inparameters){
    parameters=inparameters;
}


run_class::run_class(){
    //default constructor (unused)...do nothing
}

std::uniform_real_distribution<double> run_class::uniform01(0.,1.);

int run_class::start_runs(RNAprocessing rna,parameter_container inparameters){

    orf_length=rna.no_codons();
    parameters=inparameters;

    //set the jumping probabilities for the stochastic TASEP
    hopping_rate.clear();
    hopping_rate.push_back(0.);//initialized with one element, since we need one initiation codon.
    std::vector<double> rate_modifiers= rna.return_hoppingrates();
    double k_p=parameters.k;
    for (auto r : rate_modifiers){
        //hopping_prob has(should have) orf_length+1 in the end!
        hopping_rate.push_back(k_p*r);
    }

    //reset hopping rates for mode==8
    if (parameters.mode==8){
        for (unsigned int i=0 ; i <hopping_rate.size();++i){
            hopping_rate[i]=parameters.k;
        }
    }

    alpha=parameters.alpha;
    beta=parameters.beta;
    gamma=parameters.gamma;

    clear_results_variables();

    //title for output
    if (parameters.detailed_output==1|| parameters.detailed_output>=4) 
        std::cout <<"#results of single runs"<<std::endl;

    //start individual runs
    for (unsigned int run_nu=0;run_nu<parameters.no_runs;++run_nu){
        run();
    }//for all runs

    print_output();
    return_detailed_output();

    return 0;
}


int run_class::start_runs(std::vector<bool>::size_type in_length,parameter_container inparameters){

    parameters=inparameters;
    orf_length=in_length;

    //set the jumping probabilities for the stochastic TASEP
    hopping_rate.clear();
    hopping_rate.push_back(0.);//initialized with one element, since we need one initiation codon.

    double k_p=parameters.k;
    for (int l=0; l < orf_length;++l){
        hopping_rate.push_back(k_p);//hopping_prob has orf_length+1 in the end!
    }

    //If parameters.mode between 4 and 6, introduce one slow step into transcript
    if (parameters.mode%10==4) {
        hopping_rate[1]=parameters.lambda;
    }
    else if (parameters.mode%10==5) {
        hopping_rate[int(orf_length/2.)]=parameters.lambda;
    }
    else if (parameters.mode%10==6) {
        hopping_rate[hopping_rate.size()-2]=parameters.lambda;
    }

    //set simulation parameters
    alpha=parameters.alpha;
    beta=parameters.beta;
    gamma=parameters.gamma;

    clear_results_variables();

    //title for output
    if (parameters.detailed_output==1) 
        std::cout <<"# results of single runs: <density> <yield> (+<simulation time if variable>)"<<std::endl;

    //start all runs
    for (unsigned int run_nu=0;run_nu<parameters.no_runs;++run_nu){
        run();
    }

    print_output();
    return_detailed_output();

    return 0;
}


int run_class::run(){

    //set t_max:
    //1. to given value
    //2. draw from exponential distribution
    //3. draw from exponential+guaranteed lifetime
    double t_max=parameters.t_max;
    if (parameters.mode%10 == 1){
        std::exponential_distribution<double> exp_dist(1./parameters.t_max);
        t_max=exp_dist(rng);
        t_max_sum+=t_max;
    }
    else if (parameters.mode%10 == 2){
        double guaranteed_lifetime=800;
        std::exponential_distribution<double> exp_dist(1./(parameters.t_max-guaranteed_lifetime));
        t_max=guaranteed_lifetime+exp_dist(rng);
        t_max_sum+=t_max;
    }

    //initialize run
    unsigned int total_yield=0;
    unsigned int yieldcounter=0;
    unsigned int total_occupation=0;
    ribosomes.clear();

    double t=0;
    measurement_timestep=parameters.dt;
    double next_measurement_time=0.;

    std::vector<double> prob_container;
    std::vector<unsigned int> movable_ribosomes;
    prob_container.reserve(orf_length+3);
    movable_ribosomes.reserve(orf_length);


    //main loop over time t (while new ribosomes are initiating)
    while (t < t_max){
        prob_container.clear();
        movable_ribosomes.clear();

        bool ribosome_at_end=false;
        bool recycling_possible=false;
        bool initiation_possible=false;

        //fill rate container with all possible reactions
        for (unsigned int rib_id=0; rib_id< ribosomes.size(); ++rib_id){
            /// go through all ribosomes and add to rates if movement is possible
            if (ribosomes[rib_id].movement_allowed){
                movable_ribosomes.push_back(rib_id);
                if (ribosomes[rib_id].pos<orf_length){
                    prob_container.push_back(hopping_rate[ribosomes[rib_id].pos]);
                }
                else if (ribosomes[rib_id].pos==orf_length) {
                    ribosome_at_end=true;
                    prob_container.push_back(beta);
                    if (!collision(0,ribosomes[0].pos)){
                        prob_container.push_back(gamma);
                        recycling_possible=true;
                    }
                }
                else {
                    std::cout <<"error in assigning probabilities: ribosome position '"<<ribosomes[rib_id].pos<<"' invalid"<<std::endl;
                    exit(1);
                }
            }
        }
        if (ribosomes.size()==0 || !collision(0,ribosomes[0].pos)){
            initiation_possible=true;
            prob_container.push_back(alpha);
        }

        //calculate sum of all rates
        double prob_sum=0.;
        for (auto prob: prob_container)
            prob_sum+=prob;

        //increase time by random waiting time
        double r1=uniform01(rng); //calc. waiting time until next reaction
        double tau=-log(r1)/prob_sum;
        t+=tau;



        //now choose one reaction according to r2
        double r2=uniform01(rng)*prob_sum; //calc. next reaction
        unsigned int chosen_element=0;
        double sum=0.;
        for (auto prob: prob_container){
            sum+=prob;
            if (r2<sum)
                break;
            ++chosen_element;
        }

        //data output/saving every parameters.dt timesteps (even more than once if tau is large)
        while (t>next_measurement_time && next_measurement_time <=parameters.t_max){
            total_occupation+=ribosomes.size();
            save_measurements_over_time(next_measurement_time,yieldcounter,total_yield);
            next_measurement_time+=measurement_timestep;
        }

        unsigned int probs_last_id=prob_container.size()-1;

        //******** TASEP update ************************
        if (initiation_possible && chosen_element==probs_last_id){
            //chosen reaction is initiation (this is the last element in probs_container, if initiation_possible)
            ribosome_type first_rib;
            first_rib.pos=1;
            ribosomes.push_front(first_rib);
            update_collision_flag(0);
        }
        else if (((recycling_possible&&initiation_possible)&&chosen_element==probs_last_id-1)||
                ((recycling_possible&&!initiation_possible)&&(chosen_element==probs_last_id))
                ){
            //chosen reaction is reinitiation
            ribosome_type first_rib;
            first_rib.pos=1;
            ribosomes.push_front(first_rib);
            ribosomes.pop_back();
            update_collision_flag(0);
            update_collision_flag(ribosomes.size()-1);

            ++total_yield;
            ++yieldcounter;

        }//end if (gamma)
        else if (
                ((ribosome_at_end&&!recycling_possible&&!initiation_possible)&& chosen_element==probs_last_id)||
                ((ribosome_at_end&&recycling_possible&&initiation_possible)&& chosen_element==probs_last_id-2)||
                ((ribosome_at_end&&!recycling_possible&&initiation_possible)&& chosen_element==probs_last_id-1)||
                ((ribosome_at_end&&recycling_possible&&!initiation_possible)&& chosen_element==probs_last_id-1) 
                ){//beta
            //chosen reaction is termination
            ribosomes.pop_back();
            ++total_yield;
            ++yieldcounter;
            update_collision_flag(ribosomes.size()-1);
        }//end if (beta)
        else {//chosen_element is inside the lattice! -> elongation
            unsigned int chosen_ribosome_id=movable_ribosomes[chosen_element];
            ribosomes[chosen_ribosome_id].pos++;
            update_collision_flag(chosen_ribosome_id);
            update_collision_flag(chosen_ribosome_id-1);
        }
        //******** end TASEP update **************************

    } //end while t < t_max
    
    //add to the measurement counters for averages
    average_yield+=total_yield/(1.*parameters.no_runs);
    average_occupancy+=1.*total_occupation/double(t_max/measurement_timestep);
    average_no_proteins_at_end+=ribosomes.size()/double(parameters.no_runs);

    //print single run output
    if (parameters.detailed_output==1){ 
        std::cout <<"  "<<total_occupation/double(t_max/measurement_timestep*orf_length)<<" "<<total_yield<<" ";
        if (parameters.mode%10==1||parameters.mode%10==2){
            std::cout <<t_max<<" ";
        }
        std::cout<<std::endl;
    }

    return 0;
}


bool run_class::collision(int pos_left,int pos_right){
    if (pos_left + parameters.footprint >= pos_right){
        return true;
    }
    else
        return false;
}


bool run_class::clear_results_variables(){
    average_occupancy=0.;
    average_yield=0.;
    average_no_proteins_at_end=0.;

    t_max_sum=0.;
    t_end=parameters.t_max;

    occupation_over_time.clear();
    occupation_over_time.resize(int(t_end/parameters.dt)+1,0);

    yield_over_time.clear();
    yield_over_time.resize(int((t_end)/parameters.dt)+1,0);

    total_yield_over_time.clear();
    total_yield_over_time.resize(int((t_end)/parameters.dt)+1,0);

    occupation_per_codon.clear();
    occupation_per_codon.resize(orf_length,0);
    return 0;
}

void run_class::save_measurements_over_time(double next_measurement_time,unsigned int & yieldcounter,double total_yield){

            occupation_over_time[int(next_measurement_time/measurement_timestep)]+=ribosomes.size();

            yield_over_time[int(next_measurement_time/measurement_timestep)]+=yieldcounter;
            total_yield_over_time[int(next_measurement_time/measurement_timestep)]+=total_yield;
            yieldcounter=0;

            for (auto rib: ribosomes){
                occupation_per_codon[rib.pos-1]++;
            }
}

void run_class::return_detailed_output(){ 
    if (parameters.detailed_output ==2){
        //print average density over time
        std::cout <<"# <time> <occupation density>"<<std::endl;
        double outtime=0.;
        for (auto x: occupation_over_time){
            std::cout <<"    "<<outtime<<" "<<x/double(orf_length)/double(parameters.no_runs)<<std::endl;
            outtime+=parameters.dt;
        }
    }
    else if (parameters.detailed_output ==3){
        //print average yield  over time
        std::cout <<"# <time> <yield>"<<std::endl;
        double outtime=0.;
        for (auto x: yield_over_time){
            std::cout <<"    "<<outtime<<" "<<x/double(parameters.no_runs)<<std::endl;
            outtime+=parameters.dt;
        }
    }
    else if (parameters.detailed_output ==4||parameters.detailed_output==-2){
        //print average density and yield over time
        if (parameters.detailed_output>=0) std::cout <<"# <time> <density> <yield/time> <prot. produced>"<<std::endl;
        double outtime=0.;
        int i=0;
        for (auto x: yield_over_time){
            std::cout <<"    "<<outtime<<" "<<occupation_over_time[i]/double(orf_length)/double(parameters.no_runs)
                <<" "<<x/double(parameters.no_runs)/parameters.dt
                <<" "<<total_yield_over_time[i]/double(parameters.no_runs)<<std::endl;
            outtime+=parameters.dt;
            ++i;
        }
    }
}

void run_class::print_output(){

    //if detailed_output==5, prints the average density per codon
    if ((parameters.detailed_output==5)||(parameters.detailed_output==-5)){
        if (parameters.detailed_output >0)
            std::cout <<"#occupation per codon: "<<std::endl;
        for (auto occ: occupation_per_codon){
            std::cout <<occ/double(1.*parameters.t_max/parameters.dt)/ parameters.no_runs<<" ";
        }
        std::cout<<std::endl;
    }

    //print title if detailed_output >=0
    if (parameters.detailed_output>=0){
        std::cout <<"#results <orf_length>   <density> <no_ribosomes> <yield> <eff_intitiation> "<<std::endl;
        std::cout <<orf_length<<"  ";
    }
    //print average density, yield and effective initiation
    if (parameters.detailed_output >=-1){
        //calculate av. density
        double av_occ_result=average_occupancy/ double(orf_length)/ parameters.no_runs;

        std::cout <<av_occ_result<<" "<<av_occ_result*double(orf_length)
            <<" "<<average_yield<<" "
            <<(average_yield+average_no_proteins_at_end)/double(parameters.t_max)<<"    ";

        //print simulation time for variable time runs
        if (parameters.mode%10==1 || parameters.mode%10==2)
            std::cout<<t_max_sum/double(parameters.no_runs)<<" ";
        std::cout <<std::endl;
    }
}

bool run_class::update_collision_flag(int ribosome_id){
    //returns FALSE if ribosome_id is invalid 
    if (ribosome_id<0 || static_cast<size_t>(ribosome_id) >= ribosomes.size())
        return false;

    if (ribosomes.size()==1 || static_cast<size_t>(ribosome_id) >= ribosomes.size()-1){
        ribosomes[ribosome_id].movement_allowed=true;
        return true;
    } 
    else{
        if (!collision(ribosomes[ribosome_id].pos,ribosomes[ribosome_id+1].pos)){
            ribosomes[ribosome_id].movement_allowed=true;
        }
        else 
            ribosomes[ribosome_id].movement_allowed=false;

        return true;
    }

    return false;
}

