// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "feeserver.hh"

//#include <dlfcn.h>
#include <iostream>
#include<fstream>
#include "ce_interface.h"

using namespace std;


FeeServer::FeeServer(string feeservername, string libce)
    : DimCommand((feeservername+"_Command").c_str(),"C") ,
      name(feeservername),
      ackdata((cmdack_t*)malloc(12 + 1)), acksize(12 + 1),
      ack((feeservername+"_Acknowledge").c_str(), "C", ackdata, acksize)
{   
    
    // reset memory
    ackdata->id = 0;
    ackdata->errorCode = 0;
    ackdata->flags = 0;
    ackdata->checksum = 0;
    ackdata->payload[0] = '\0';
    
    
    // open pipe for communication with worker thread
    if (pipe(worker_pipe) != 0) {
        cerr << "Unable to open pipe for communication with worker thread"
             << endl;
        exit(-1);
        
    }
    
    // create worker thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&worker_thread_handle, &attr, 
                   FeeServer::worker_thread_wrapper, (void*)this);
    
    
    
    // open control engine
    // ===================
    /*    
    const char* error;
    ce_dl_handle = dlopen (libce.c_str(), RTLD_NOW); //RTLD_LAZY);
    if (!ce_dl_handle) {
        cerr << dlerror() << endl;
        exit(1);
    }
    
    ce_initialize = (void (*)(const char*))
        dlsym(ce_dl_handle, "ce_initialize");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    
    ce_exec_loop = (void (*)())dlsym(ce_dl_handle, "ce_exec_loop");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    
    ce_issue = (int (*)(char*,char**,int*))dlsym(ce_dl_handle, "ce_issue");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    
    ce_cleanup = (void (*)())dlsym(ce_dl_handle, "ce_cleanup");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    
    */

    // initialize CE
    ce_initialize(name.c_str());
    
}
    
void FeeServer::commandHandler()
{
    cmd_size_t *cmd = (cmd_size_t *)new char[getSize()+sizeof(size_t)];
    
    cmd->size = getSize() - 12 /*header*/ ;
    //printf("getSize()=%i\n",cmd->size);
    memcpy((void*)&(cmd->cmd), getString(), getSize());
    
    write(worker_pipe[1], &cmd, sizeof(cmd_size_t*));
    
}

void* FeeServer::worker_thread_wrapper(void* arg)
{
    FeeServer *fs = (FeeServer *)arg;
    
    fs->worker_thread();
    return NULL;
}

void FeeServer::worker_thread()
{
    cmd_size_t* cmdsize;

    while(read(worker_pipe[0], &cmdsize, sizeof(cmd_size_t*)) > 0) {
        
        int retsize   = cmdsize->size;
        cmdack_t *cmd = &(cmdsize->cmd);
        char* retdata = 0;
        
        ce_issue(cmd->payload, &retdata, &retsize);
        
        
        if (retsize + 12 /*header*/ + 1 /* trailing 0 */ > (int)acksize) {
            acksize = retsize + 12 + 1;
            ackdata = (cmdack_t*)realloc(ackdata, acksize);
        }
        
        ackdata->id        = cmd->id;
        ackdata->errorCode = 0;
        ackdata->flags     = cmd->flags;
        ackdata->checksum  = 0;

        memcpy(ackdata->payload, retdata, retsize);
        ackdata->payload[retsize] = '\0';

        ack.updateService((void *)ackdata,retsize + 12 + 1);
        
        
        delete[] reinterpret_cast<char*>(cmdsize);
    }	    
    
}


void FeeServer::start()
{
    DimServer::start(name.c_str());
    
    ce_exec_loop();
    
}
    

