// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-


#include <pthread.h>
#include <dis.hxx>


#include <string>

class FeeServer: public DimCommand
{
public:
    FeeServer(std::string feeservername, std::string libce);

    void start();
    
protected:
    
    // use same structure for commands and ACK data
    struct cmdack_t {

        // command/ACK header
        unsigned int id;
        short errorCode;
        unsigned short flags;
        int checksum;

        // payload (any size possible)
        char payload[1];
    };

    struct cmd_size_t {
        size_t size;
        cmdack_t cmd;
    };

    // DIM command handler, gets called when commands arrive
    void commandHandler();

    // Wrapper for the actual method to run the control engine.  This
    // method has to be static for compatibility with pthreads. It
    // then calls the non-static method worker_thread()
    static void* worker_thread_wrapper(void* arg);

    // The method to create the worker thread. It reads commands from
    // a pipe and hands them to the control engine for execution.
    void worker_thread();
    
    int worker_pipe[2];

    pthread_t worker_thread_handle;


    //void *ce_dl_handle;
    //void (*ce_initialize) (const char* feeservername);
    //void (*ce_exec_loop)  ();
    //int  (*ce_issue)      (char* command, char** result, int* size);
    //void (*ce_cleanup)    ();


    std::string name;

    cmdack_t *ackdata;
    size_t acksize;
    DimService ack;

};


