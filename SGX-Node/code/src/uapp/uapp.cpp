#include "uapp/uapp.hpp"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

/* OCall functions */
extern "C" void ocall_print_string(const char* str)
{
  /* Proxy/Bridge will check the length and null-terminate 
    * the input string to prevent buffer overflow. 
    */
  printf("%s", str);
}
extern "C" void* ocall_malloc(size_t size)
{
  // this pointer is allocated in untrusted zone
  return malloc(size);
}
extern "C" void ocall_free(void* ptr)
{
  // this pointer is de-allocated in untrusted zone
  free(ptr);
}
