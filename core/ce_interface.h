#ifndef CE_INTERFACE_H
#define CE_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
  
  void  ce_initialize(const char* feeserver_name);
  void  ce_exec_loop();
  int   ce_issue(char* command, char** result, int* size);
  void  ce_cleanup();

#ifdef __cplusplus
}
#endif


#endif

