//####################
//file: inputRNA.cpp
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#include "../include/inputRNA.h"


RNAprocessing::RNAprocessing(){
    // std::cout <<"no filename given"<<std::endl;
}


RNAprocessing::RNAprocessing(std::string filename){
    input_filename=filename;
}

int RNAprocessing::init(){
    int returnvalue=0;
    if (input_filename!=""){
        returnvalue+=loadfile();
        returnvalue+=process_sequence();
    }
    else {
        returnvalue+=create_random_sequence(99);
        returnvalue+=process_sequence();
    }

    return returnvalue;
}

int RNAprocessing::init(std::string inputfile){
    int returnvalue=0;
    input_filename=inputfile;
    returnvalue+=loadfile();
    returnvalue+=process_sequence();
    return returnvalue;
}

int RNAprocessing::init(int length){
    int returnvalue=0;
    returnvalue+=create_random_sequence(length*3);
    returnvalue+=process_sequence();
    return returnvalue;
}

std::map<std::string,double> RNAprocessing::gencode ={
    {"AAA",    0.67},  {"AAC",   1.56},  {"AAG",   1.51},
    {"AAT",   0.956},  {"ACA",   0.626},  {"ACC",   1.12},
    {"ACG",   0.156},  {"ACT",   1.74},  {"AGA",   1.72},
    {"AGC",   0.626},  {"AGG",   0.172},  {"AGT",   0.382},
    {"ATA",   0.313},  {"ATC",   1.3},  {"ATG",   0.782},
    {"ATT",   2.03},  {"CAA",   1.41},  {"CAC",   1.38},
    {"CAG",   0.156},  {"CAT",   0.843},  {"CCA",   1.56},
    {"CCC",   0.201},  {"CCG",   1.56},  {"CCT",   0.313},
    {"CGA",   0.27},  {"CGC",   0.27},  {"CGG",   0.156},
    {"CGT",   0.421},  {"CTA",   0.9},  {"CTC",   0.156},
    {"CTG",   0.9},  {"CTT",   0.0956},  {"GAA",   1.84},
    {"GAC",   2.53},  {"GAG",   0.313},  {"GAT",   1.55},
    {"GCA",   0.782},  {"GCC",   1.17},  {"GCG",   0.782},
    {"GCT",   1.82},  {"GGA",   0.469},  {"GGC",   2.7},
    {"GGG",   0.313},  {"GGT",   1.65},  {"GTA",   0.313},
    {"GTC",   1.24},  {"GTG",   0.287},  {"GTT",   1.93},
    {"TAC",   1.7},  {"TAT",   1.04},  {"TCA",   0.728},
    {"TCC",   1.44},  {"TCG",   0.156},  {"TCT",   2.24},
    {"TGC",   0.747},  {"TGG",   1.17},  {"TGT",   0.456},
    {"TTA",   1.09},  {"TTC",   1.46},  {"TTG",   1.92},
    {"TTT",   0.89},  {"TAA",   -1.},  {"TAG",   -1.},
    {"TGA",   -1.},
};

int RNAprocessing::loadfile(){
    std::ifstream infile;
    infile.open(input_filename.c_str());
    sequence="";
    std::string line;

    while(std::getline(infile, line))
    {
        sequence+=line;
    }

    infile.close();
    return 0;
}


int RNAprocessing::process_sequence(){

    auto n=sequence.length();
    rates.clear();
    std::locale loc; //to convert genetic code to UPPER case

    for (std::size_t i=0; i< n-2; i+=3){
        std::string codon;
        codon=std::toupper(sequence[i],loc);
        codon+=std::toupper(sequence[i+1],loc);
        codon+=std::toupper(sequence[i+2],loc);

        rates.push_back(gencode[codon]);
    }

    return 0;

}

int RNAprocessing::create_random_sequence(std::size_t length){

    std::uniform_int_distribution<int> dist(0,3);
    sequence="";

    for (std::size_t i =0; i < length; ++i){
        int number=dist(rng);
        if (number==0) sequence+="A";
        else if (number==1) sequence+="C";
        else if (number==2) sequence+="T";
        else if (number==3) sequence+="G";
        else{
            std::cout <<"error in random sequence creation"<<std::endl;
            exit(1);
        }
    }
    return 0;
}


int RNAprocessing::no_codons(){
    return int(rates.size());
}

const std::vector<double> RNAprocessing::return_hoppingrates(){
    return rates;
}
