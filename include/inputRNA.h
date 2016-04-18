//####################
//file: inputRNA.h
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#ifndef inputRNA
#define inputRNA

#include "misc.h"
#include <string> 
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <random>
#include <locale>


/******** inputRNA ************
 * Processes sequence data provided in file
 * "filename" or random sequence data 
 * to return elongation rate modifiers.
 * Sample usage: 
 *  RNAprocessing rna(filename); 
 *  rna.init(); 
 *  rna.return_hoppinrates(); 
 ******************************/
 class RNAprocessing{
    public:
        /** default constructor.
         * creates empty object ->
         * has to be initialized with init()
         */
	RNAprocessing();

        /**initializes RNAprocessing object
         * with filename of provided RNAdata 
         */
	RNAprocessing(std::string inputfile);

        /** returns the rate modifiers
         * that were generated from sequence data or a random sequence.
         * init() has to be called first!
         */
	const std::vector<double> return_hoppingrates();

        /** Initializes object and processes data.
         * Has to be called once to initialize rna object.
         */
        int init();

        /** Initializes object and processes data.
         * Has to be called once to initialize rna object.
         * tries to open "inputfile" and processes genome sequence in that file.
         */
        int init(std::string inputfile);

        /** Initializes object and processes data.
         * Has to be called once to initialize rna object.
         * Creates random sequence with given length. 
         * WARNING: not tested yet!
         */
        int init(int length);

        /** Returns the number of codons of sequence loaded into the object.
         * Each codon contains three letters (A,T,C,G)
         */
        int no_codons();

    private:
        /** Read sequence string from file.
         * Filename needs to be given on construction.
         */
	int loadfile();

        /** Convert sequence to hopping rates (elongation rates).
         * Uses either a saved random sequence or the sequence read with loadfile.
         */
	int process_sequence();

        /** Creates random sequence (string) of length.
         * Uses 4 letters at random -> no sanity checks!
         */
	int create_random_sequence(std::size_t length);

        /** Hopping rate per codons.
         * Container that stores hopping (elongation) rate 
         * modifiers of the RNA sequence that result from 
         * sequence processing.
         */
	std::vector<double> rates;

        /** sequence that is read from inputfile or generated randomly
         */
	std::string sequence;

        /** filename of the file containing the sequence data to read with loadfile()
         */
	std::string input_filename;

        /** contains conversion scheme from codon to hopping rate. From Gilchrist & Wagner (2006).
         */
	static std::map<std::string,double> gencode;

};
#endif
