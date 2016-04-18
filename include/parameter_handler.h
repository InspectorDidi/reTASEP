//####################
//file: parameter_handler.h
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#ifndef PARAMETERHANDLER_H
#define	PARAMETERHANDLER_H



#include <ctime>
#include <cmath>

#include <vector>

#include <string>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <map>


const char* const DEFAULT_PARAMETER_FILE = "parameters.inp";


/*********************************
 * Parameter handler class.
 * Reads all input from parameter files and/or
 * from command line arguments. 
 * Parameters are Key=Value pairs.
 * *******************************/
class parameter_handler {
public:
    parameter_handler();//unused

    /** reads command line input and file.
     * Fills the key-value map "parameters".
     */
    parameter_handler(int argc, char** argv);
    parameter_handler(std::istringstream& instream);



    /** reads from key-value map "parameters".
     * Sets the variable (second argument "*_parameter") with
     * the value of the key "parameter_name". 
     * If strict=true, error message is printed and program exits
     * if key not found.
     */
    bool SetValue(const char* parameter_name,bool &b_parameter,bool strict=false);
    bool SetValue(const char* parameter_name,int  &int_parameter,bool strict=false);
    bool SetValue(const char* parameter_name,unsigned int  &int_parameter,bool strict=false);
    bool SetValue(const char* parameter_name,double &double_parameter,bool strict=false);
    bool SetValue(const char* parameter_name,std::string &s_parameter,bool strict=false);

    /** wrapper function around SetValue().
     * returns value to given key.
     * No error handling!
     */
    std::string ReturnString(const char* parameter_name);
    int ReturnInt(const char* parameter_name);
    double ReturnDouble(const char* parameter_name);
    bool ReturnBool(const char* parameter_name);

    /** Print all processed parameters to stdout.
     */
    void PrintAll();

   
private:
    
    //container to store key-value pairs
    struct s_parameter
    {
       std::string name;
       std::string value;  //stored as string converted later
    };

    bool debug;

    /** stores all parameters that have been processed.
     */
    std::map<std::string,std::string> parameters;

    /** Processes all additional parameters given as key=value pairs in the command line.
     */
    void HandleInputArguments(int argc, char* argv[]);
    
    /** Reads and processes input strings.
     * This is the core function of this class.
     * Takes key=value pairs as string input and returns the key
     * and value in "new_parameter". 
     * Handles also comments, empty lines etc. in the string.
     */
    bool ParseString(std::string input,s_parameter &new_parameter);

};

#endif	/* PARAMETERHANDLER_H */

