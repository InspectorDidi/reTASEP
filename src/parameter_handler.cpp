//####################
//file: parameter_handler.cpp
//Marvin Böttcher (boettcher@evolbio.mpg.de)
//2016
//####################

#include "../include/parameter_handler.h"


parameter_handler::parameter_handler()
{
//     debug=false;
}

parameter_handler::parameter_handler(std::istringstream& DataFile)
{
    debug=false;
    if (debug)
        std::cout<<"parsing parameter file and arguments"<<std::endl;

    std::string line_in_file;
    s_parameter new_parameter;
    while (std::getline(DataFile,line_in_file)){      //until end of file is reached
        if (ParseString(line_in_file, new_parameter)) {
            parameters[new_parameter.name]=new_parameter.value;
        }
    }
    
    if (debug)
        std::cout<<"#parsing finished"<<std::endl;
}


parameter_handler::parameter_handler(int argc, char** argv)
{
    debug=false;
    if (debug)
        std::cout<<"#parsing parameter file and arguments"<<std::endl;
    std::ifstream DataFile;
    if (argc>1){ // is there a second argument?
        DataFile.open(argv[1]);     //then try to open parameter file (first argument)
        //try to open default file on error. Otherwise
        if (DataFile.fail()) {
            DataFile.open(DEFAULT_PARAMETER_FILE);
            if (DataFile.fail()) {
                std::cout<<"#could not open parameter file '"<<argv[1]<<"' or '"<<DEFAULT_PARAMETER_FILE<<"'"<<std::endl;
                // exit(1);
            }
        }
    }
    else{
        DataFile.open(DEFAULT_PARAMETER_FILE);
        if (DataFile.fail()){
            std::cout<<"error: could not open default parameter file '"<<DEFAULT_PARAMETER_FILE<<"'"<<std::endl;
            exit(0);
        }
        else{
            std::cout<<"#Using default parameter file '"<<DEFAULT_PARAMETER_FILE<<"'"<<std::endl;
        }
    }
    
    if (debug){
        std::cout<<"#number of additional arguments="<<argc-2<<std::endl;
    }
    
    //Handle parameters in file first -> can be overwritten by command line arguments
    std::string line_in_file;
    s_parameter new_parameter;
    while (getline(DataFile,line_in_file)){      //until end of file is reached
        if (ParseString(line_in_file, new_parameter)) {
            parameters[new_parameter.name]=new_parameter.value;
        }
        
    }
    DataFile.close();

    HandleInputArguments(argc,argv);
    
    if (debug)
        std::cout<<"#parsing finished"<<std::endl;
}

bool parameter_handler::ParseString(std::string input, s_parameter &new_parameter)
{

    std::string end_token = "structure_info";
    //check if comment or empty
    if (input[0]=='#'  || input.empty() ||(input.find_first_not_of(" \t")==std::string::npos))
    {
        //nothing
        return false;
    }

    //truncating string after # for comments
    size_t found_comment=input.find("#");
    if (found_comment!=std::string::npos)
    {
        input=input.substr(0,found_comment);
    }
    
    size_t found_at=input.find_first_of(" \t"); //for follow up lines
    
    if  (found_at==0) {
        //follow up of line
        std::string buffer;
        buffer=input.substr(found_at+1,std::string::npos);
        
        size_t found_at_end;
        
        found_at=buffer.find_first_not_of(" \t");
        found_at_end=buffer.find_last_not_of(" \t");
        new_parameter.value+=" "+buffer.substr(found_at,found_at_end-found_at+1);
        return true;
        
        
    }
    
    if (new_parameter.name == end_token){ //after end_token stuctured data
        //follow up of line
        size_t found_first=input.find_first_not_of(" \t");
        size_t found_last=input.find_last_not_of(" \t");
        new_parameter.value+=" "+input.substr(found_first,found_last-found_first+1);
        
        return true;
    }

    //check if there is an equal sign
    if (input.find("=")==std::string::npos)
    {
        //there is no equal sign - old version: abort due to input error
        //std::cout<<"Ignoring the following line in parameter file"<<filename<<endl;
        //std::cout<<"'"<<input<<"'"<<endl;
        //return false;
    
        found_at=input.find_first_of(" \t");
        
        if  (found_at==0)
        {
            //follow up of line
            std::string buffer;
            buffer=input.substr(found_at+1,std::string::npos);
            
            size_t found_at_end;
            
            found_at=buffer.find_first_not_of(" \t");
            found_at_end=buffer.find_last_not_of(" \t");
            new_parameter.value+=" "+buffer.substr(found_at,found_at_end-found_at+1);
            
            return true;
            
            
        }
        else if  (found_at==std::string::npos) {
            new_parameter.name=input.substr(0,found_at);
            new_parameter.value=" ";
            if (debug) std::cout <<new_parameter.name<<" "<<new_parameter.value<<std::endl;
            return true;
        }
        
        new_parameter.name=input.substr(0,found_at);
        new_parameter.value=input.substr(found_at+1,std::string::npos);
        
        
        size_t found_at_end;
        found_at=new_parameter.name.find_first_not_of(" \t");
        found_at_end=new_parameter.name.find_last_not_of(" \t");
        new_parameter.name=new_parameter.name.substr(found_at,found_at_end-found_at+1);
        
        found_at=new_parameter.value.find_first_not_of(" \t");
        found_at_end=new_parameter.value.find_last_not_of(" \t");
        new_parameter.value=new_parameter.value.substr(found_at,found_at_end-found_at+1);
        
        if (new_parameter.value.empty()) new_parameter.value=" ";
        if (debug) std::cout <<new_parameter.name<<" "<<new_parameter.value<<std::endl;
        return true;
    }
    else //there is an equal sign
    {
        found_at=input.find("=");
        if (found_at==input.length()-1)
        {
            std::cout<<"parameter '"<<input<<"' has no value"<<std::endl;
            return false;
        }
        else
        {
            new_parameter.name=input.substr(0,found_at);
            new_parameter.value=input.substr(found_at+1,std::string::npos);



            //remove trailing and leading whitespaces or tabs from both strings
            size_t found_at_end;
            found_at=new_parameter.name.find_first_not_of(" \t");
            found_at_end=new_parameter.name.find_last_not_of(" \t");
            new_parameter.name=new_parameter.name.substr(found_at,found_at_end-found_at+1);

            found_at=new_parameter.value.find_first_not_of(" \t");
            found_at_end=new_parameter.value.find_last_not_of(" \t");
            new_parameter.value=new_parameter.value.substr(found_at,found_at_end-found_at+1);


            return true;
        }
    }
}


void parameter_handler::HandleInputArguments(int argc, char* argv[])
{
    if (argc<=2)
    {
        //nothing to be done
        return;
    }
    else
    {
        for (int i=2;i<argc;i++)
        {
            s_parameter new_parameter;
            //string argument=argv[i];
            if (ParseString(argv[i], new_parameter))
            {

                std::map<std::string,std::string>::iterator it;

                if ((it= parameters.find(new_parameter.name))!=parameters.end()) {
                    if (debug) 
                    {
                        std::cout<<"override "<<it->first<<"=";//<<(*it).second<<endl;
                        std::cout<<"by       "<<new_parameter.name<<"="<<new_parameter.value<<std::endl;
                    }
                    (*it).second=new_parameter.value;
                }
                else
                {
                    parameters.insert(std::pair<std::string,std::string>(new_parameter.name,new_parameter.value));
                }


            }
        }
    }
}




void parameter_handler::PrintAll()
{
    for (std::map<std::string,std::string>::iterator it=parameters.begin();it != parameters.end() ;it++)
    {
        std::cout<<"name="<<std::setw(20)<<it->first<<", value="<<it->second<<std::endl;
    }

}





bool parameter_handler::SetValue(const char* name, bool& b_parameter,bool strict)
{
    std::map<std::string,std::string>::iterator it= parameters.find(name);
    if (it != parameters.end())
    {
        if (     (*it).second.find("true")!=std::string::npos
                 || atoi((*it).second.c_str())==1)
        {
            b_parameter=true;
            return true;
        }
        else if ((*it).second.find("false")!=std::string::npos
                 || atoi((*it).second.c_str())==0)
        {
            b_parameter=false;
            return true;
        }
        else
        {
            std::cout<<"Cannot set boolean parameter '"<<(*it).first<<"'"<<std::endl;
            std::cout<<"the value '"<<(*it).second<<"' is not boolean"<<std::endl;
            std::cout<<"'"<<(*it).first<<"' is set to false"<<std::endl;
            b_parameter=false;
            return false;
        }
    }
    if (debug || strict) std::cout <<"'"<<name<<"' missing in input"<<std::endl;
    if (strict) exit(1);
    return false;

}



bool parameter_handler::SetValue(const char* name, int& int_parameter,bool strict)
{

    std::map<std::string,std::string>::iterator it = parameters.find(name);
    if (it != parameters.end()) {
        int_parameter=atoi(it->second.c_str());
        return true;
    }
    
    if (debug || strict) std::cout <<"'"<<name<<"' missing in input"<<std::endl;
    if (strict) exit(1);
    return false;
}

bool parameter_handler::SetValue(const char* name,unsigned int& int_parameter,bool strict)
{

    std::map<std::string,std::string>::iterator it = parameters.find(name);
    if (it != parameters.end()) {
        int_parameter=atoi(it->second.c_str());
        return true;
    }
    
    if (debug || strict) std::cout <<"'"<<name<<"' missing in input"<<std::endl;
    if (strict) exit(1);
    return false;
}


bool parameter_handler::SetValue(const char* name, double& double_parameter,bool strict)
{
    std::map<std::string,std::string>::iterator it = parameters.find(name);
    if (it != parameters.end()) {
        double_parameter=atof(it->second.c_str());
        return true;
    }
    if (debug || strict) std::cout <<"'"<<name<<"' missing in input"<<std::endl;
    if (strict) exit(1);
    return false;
}



bool parameter_handler::SetValue(const char* name, std::string &dest_variable,bool strict)
{
    std::map<std::string,std::string>::iterator it= parameters.find(name);
    if (it != parameters.end()){
        size_t found_first=(*it).second.find_first_of("\"");
        size_t found_last=(*it).second.find_last_of("\"");
        if (found_first!=std::string::npos && found_last!=std::string::npos){
            dest_variable=(*it).second.substr(found_first+1,found_last-found_first-1);
        }
        else{
            dest_variable=(*it).second;
        }
        return true;
    }
    if (debug || strict) std::cout <<"'"<<name<<"' missing in input"<<std::endl;
    if (strict) exit(1);
    return false;
}



int parameter_handler::ReturnInt(const char* name)
{
    int to_be_returned=0;
    SetValue(name,to_be_returned);
    return to_be_returned;
}


double parameter_handler::ReturnDouble(const char* name)
{
    double to_be_returned=0.;
    SetValue(name,to_be_returned);
    return to_be_returned;
}




bool parameter_handler::ReturnBool(const char* name)
{
    bool to_be_returned=false;
    SetValue(name,to_be_returned);
    return to_be_returned;
}




std::string parameter_handler::ReturnString(const char* name)
{
    std::string to_be_returned;
    SetValue(name,to_be_returned);
    return to_be_returned;
}

