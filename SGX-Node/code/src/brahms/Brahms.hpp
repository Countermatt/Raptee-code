#ifndef BRAHMS_HPP_
#define BRAHMS_HPP_

#include <string>
#include <vector>

#include "utils/string.hpp"
#include "data/Data.hpp"

class Brahms {

  public:

    Brahms();

    int AL1() { return mAL1; }
    int BL2() { return mBL2; }
    int GL3() { return mGL3; }
    int SamplerSize() { return mSamplerSize; }

    int Pull_Request(char** req,
                     unsigned int* reqSize,
                     char** destIP,
                     unsigned int* destIPSize,
                     Data* data);

    int Push_Request(char** req,
                     unsigned int* reqSize,
                     char** destIP,
                     unsigned int* destIPSize,
                     Data* data);

    int Pull_Reply(char** ans,
                   unsigned int* ansSize,
                   Data* data);

    int Pull_Receive_Reply(std::string  ans, Data* data);

    int Push_Receive_Request(std::string ans, Data* data);

    // - Merge View
    void MergeView(int aL1, int bL2, int gL3, Data* data);

    int Ping(char** req,
             unsigned int* reqSize,
             char** destIP,
             unsigned int* destIPSize,
             Data* data);

    int Pong(char** ans,
             unsigned int* ansSize,
             Data* data);

  private:
    int mAL1, mBL2, mGL3;
    int mSamplerSize;
};

#endif // BRAHMS_HPP_
