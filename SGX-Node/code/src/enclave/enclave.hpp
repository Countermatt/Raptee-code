#ifndef ENCLAVE_HPP_
#define ENCLAVE_HPP_

#include <vector>
#include <string>


#ifdef NATIVE
#ifndef UAPP

extern "C" {

int ecall_Pull_Request(char** req,
                   unsigned int* reqSize,
                   char** destIP,
                   unsigned int* destIPSize,
                   Data* data);

int ecall_Push_Request(char** req,
                   unsigned int* reqSize,
                   char** destIP,
                   unsigned int* destIPSize,
                   Data* data);

int ecall_Pull_Reply(char** ans,
                 unsigned int* ansSize,
                 Data* data);

int ecall_Pull_Receive_Reply(std::string  ans, Data* data, std::string NodeType, unsigned int CyclonSize);

int ecall_Push_Receive_Request(std::string ans, Data* data, std::string NodeType, unsigned int CyclonSize);

  // - Merge View
void ecall_MergeView(int aL1, int bL2, int gL3, Data* data);

int ecall_Ping(char** req,
           unsigned int* reqSize,
           char** destIP,
           unsigned int* destIPSize,
           Data* data);

int ecall_Pong(char** ans,
           unsigned int* ansSize,
           Data* data);
}
#endif
#endif


#endif
