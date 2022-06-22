#ifndef RPS_HPP_
#define RPS_HPP_

#include <Define.hpp>

#define RPS_LISTENING_PORT 10337


#include <chrono>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <iostream>

#include "net/TCPListener.hpp"
#include "net/TCPConnection.hpp"
#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

using namespace std;

class RPS {

public:
  RPS(int rpsTcpPort, std::string bootstrapIP, std::string sgxId, int byz, std::string byzIP, int sgx, int CyclonSize, std::string controlIp);

  void sendingThread();
  void listeningThread();

  void pullRequest();
  void pushRequest();
  void ReceiveRequest(shared_ptr<TCPConnection> conn);
  void samplerRequest(int i);
  void samplerReply();
  void byz_attack();
  void sendView();
  
private:



  std::shared_ptr<TCPListener> mListener;
  std::thread mListenerThread, mQueryListenerThread, mSenderThread;
  unsigned int mRpsTcpPort;
  unsigned int mPushReqCnt, mPullReqCnt, mPingReqCnt;
  int msgx;
  Data mData;
  Brahms mBrahms;
  int mCyclonSize;
  int mbyz;
  
  std::string mControl;
};


#endif //BRAHMS_HPP_
