// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "feeserver.hh"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void print_err(){

    cerr << "FeeServer name not set" << endl
         << "set the environment variable FEE_SERVER_NAME (export FEE_SERVER_NAME=trd-fee_00_1_2)" << endl
         << "or" << endl
         << "do ./feeserver FEE_SERVER_NAME=trd-fee_00_1_2 " << endl;

}


int main(int argc, char **argv)
{

    string name ; 

    if(argc==1){
        if (!getenv("FEE_SERVER_NAME")) {
            print_err();
            exit(-1);
        }
        name = getenv("FEE_SERVER_NAME");
    }else if(argc==2){
        char tmpname[256];
        sscanf(argv[1],"FEE_SERVER_NAME=%s", tmpname);
        name = string(tmpname);        
        if(name==""){
            print_err();
            exit(-1);
        }
        setenv("FEE_SERVER_NAME",name.c_str(), 1);
    }else{
        print_err();
        exit(-1);
    }


    
    
    string libce;
    if (!getenv("FEE_SERVER_LIBCE")) {
        libce = "libControlEngine.so";
    } else {
        libce = getenv("FEE_SERVER_LIBCE");
    }



    FeeServer fs(name,libce);


    fs.start();

}

