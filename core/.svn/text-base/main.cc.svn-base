// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "feeserver.hh"

#include <iostream>
#include <string>

using namespace std;




int main()
{

    if (!getenv("FEE_SERVER_NAME")) {
        cerr << "FeeServer name not set" << endl
             << endl
             << "set the environment variable FEE_SERVER_NAME to the" << endl
             << "name of the FeeServer (e.g. trd-fee_00_1_2)" << endl;
            
        
        exit(-1);
    }
    string name = getenv("FEE_SERVER_NAME");


    string libce;
    if (!getenv("FEE_SERVER_LIBCE")) {
        libce = "libControlEngine.so";
    } else {
        libce = getenv("FEE_SERVER_LIBCE");
    }



    FeeServer fs(name,libce);


    fs.start();

}

