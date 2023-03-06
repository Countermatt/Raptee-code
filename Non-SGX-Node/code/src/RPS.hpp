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
  RPS(int rpsTcpPort, std::string bootstrapIP);

  void sendingThread();
  void listeningThread();

  void pullRequest();
  void pushRequest();
  void ReceiveRequest(shared_ptr<TCPConnection> conn);
  void samplerRequest(int i);
  void samplerReply();

private:



  std::shared_ptr<TCPListener> mListener;
  std::thread mListenerThread, mQueryListenerThread, mSenderThread;
  unsigned int mRpsTcpPort;
  unsigned int mPushReqCnt, mPullReqCnt, mPingReqCnt;
  Data mData;
  Brahms mBrahms;
};


#endif //BRAHMS_HPP_
