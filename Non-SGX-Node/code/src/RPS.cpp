#include <vector>
#include <random>
#include <sstream>
#include <exception>
#include <ctime>
#include <cstring>
#include <iostream>

#include "net/TCPListener.hpp"
#include "net/TCPConnection.hpp"
#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

#include "utils/log.hpp"
#include "utils/string.hpp"
#include "RPS.hpp"


using namespace std;

RPS::RPS(int rpsTcpPort, std::string bootstrapIP, std::string sgxId, int byz, std::string byzIP)
    : mListener(make_shared<TCPListener>(rpsTcpPort)),
    mRpsTcpPort(rpsTcpPort), mPushReqCnt(0), mPullReqCnt(0), mPingReqCnt(0), mData(), mBrahms(), mCyclonSize(10), mbyz(byz)
{

  std::vector<std::string> btstIP;
  splitString(btstIP, bootstrapIP, ',');

  std::vector<std::string> sgxIds;
  splitString(sgxIds, sgxId, ',');

  for(int i = 0; i < (int) btstIP.size(); i++){
    mData.GlobalAdd(btstIP[i]);
  }

  for(int i = 0; i < (int) sgxIds.size(); i++){
    mData.SGXAdd(sgxIds[i]);
  }

  mListenerThread = thread(&RPS::listeningThread, this);
  mSenderThread = thread(&RPS::sendingThread, this);

  stringstream ss;
  ss << "RPS initialized" << endl;
  LOG(ss.str().c_str());
}

void RPS::sendingThread()      // initiate RPS Request every RPS_SYNC_TIME
{
  stringstream ss;
  ss << "SendingThread initialized" << endl;
  LOG(ss.str().c_str());

  auto al1 = mBrahms.AL1();
  auto bl2 = mBrahms.BL2();
  auto gl3 = mBrahms.GL3();

  int tour = 0;

  while(true){
    ss << "SendingThread tour: " << tour << endl;
    LOG(ss.str().c_str());
    //Reset view
    mData.PullReset();
    mData.PushReset();
    mData.StreamReset();

    std::this_thread::sleep_for(std::chrono::milliseconds(10000)); //RPS period

    if(al1 != 0){
      for(int i = 0; i < al1; i++){
        RPS::pushRequest();

      }
    }
    if(bl2 != 0){
      for(int i = 0; i < bl2; i++){

        RPS::pullRequest();
      }
    }

    int samplersize = mBrahms.SamplerSize();

    for(int i = 0; i < samplersize; i++){
      RPS::samplerRequest(i);
    }

    mBrahms.MergeView(al1, bl2, gl3, &mData);


  tour++;
  /*
  for (unsigned long int i  = 0; i < mData.GlobalView().size(); i++)
      cout << mData.GlobalView()[i] << endl;
  */
  }
  
}

void RPS::listeningThread()
{
  stringstream ss;
  ss << "ListeningThread initialized" << endl;
  LOG(ss.str().c_str());
  while(true){
      //Verify if listening thread is closed
      if (mListener->IsClosed()) {
        break;
      }
      shared_ptr<TCPConnection> conn;
      try {
        conn = mListener->Accept();
        ReceiveRequest(conn);
      } catch (exception &e) {
        stringstream ss;
        ss << "Error accepting connection: " << e.what();
        LOG(ss.str().c_str());
        continue;
      }
  }
}

void RPS::pushRequest()
{
  stringstream ss;

  auto cnt = mPushReqCnt;
  mPushReqCnt++;

  char* req = nullptr;
  unsigned int reqSize = 0;
  char* destIP = nullptr;
  unsigned int destIPSize = 0;
  int res = 0;

  //Mise en place de la push request
  res = mBrahms.Push_Request(&req, &reqSize, &destIP, &destIPSize, &mData);

  if (res != 0) {
    ss << "Error creating Push_REQ #" << cnt << ": " << res;
    LOG(ss.str().c_str());
    return;
  }

  auto qIP = string(destIP, destIPSize);
  if (destIP != nullptr) delete[] destIP;

  shared_ptr<TCPConnection> conn;
  try {
    conn = TCPConnection::Dial(qIP, mRpsTcpPort);
  } catch (exception& e) {
    ss << "Error dialing " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  try {
    conn->Write(req, reqSize);
  } catch (exception& e) {
    ss << "Error sending Push_REQ #" << cnt << " to " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  ss << "Sent Push_REQ #" << cnt << " to " << qIP;
  ss.clear();
  ss.str("");

}

void RPS::pullRequest()
{
  stringstream ss;

  auto cnt = mPullReqCnt;
  mPullReqCnt++;

  char* req = nullptr;
  unsigned int reqSize = 0;
  char* destIP = nullptr;
  unsigned int destIPSize = 0;
  int res = 0;

  //Mise en place de la push request
  res = mBrahms.Pull_Request(&req, &reqSize, &destIP, &destIPSize, &mData);

  if (res != 0) {
    ss << "Error creating Pull_REQ #" << cnt << ": " << res;
    LOG(ss.str().c_str());
    return;
  }
  auto qIP = string(destIP, destIPSize);
  if (destIP != nullptr) delete[] destIP;

  shared_ptr<TCPConnection> conn;
  try {
    conn = TCPConnection::Dial(qIP, mRpsTcpPort);
  } catch (exception& e) {
    ss << "Error dialing " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  try {
    conn->Write(req, reqSize);
  } catch (exception& e) {
    ss << "Error sending Pull_REQ #" << cnt << " to " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  ss << "Sent Pull_REQ #" << cnt << " to " << qIP;
  ss.clear();
  ss.str("");

  //Receive RPS Answer
    char* ans = nullptr;
    unsigned int ansSize = 0;
    try {
      conn->Read(&ans, &ansSize);
    } catch (exception &e) {
      ss << " Error receiving Pull_ANS #" << cnt << " from " << qIP << ": " << e.what();
      LOG(ss.str().c_str());
      conn->Close();
      if (req != nullptr) delete[] req;
      return;
    }
    conn->Close();

    std::vector<std::string> request;
    splitString(request, ans, ansSize, ';');
    auto requestip = request[2];
    auto nodeId = request[0];

    res = mBrahms.Pull_Receive_Reply(requestip, &mData);
    if (res != 0) {
      ss << "Error handling Pull_ANS #" << cnt << ": " << res;
      LOG(ss.str().c_str());
      if (ans != nullptr) delete[] ans;
      if (req != nullptr) delete[] req;
      return;
    }
    if (ans != nullptr) delete[] ans;
    if (req != nullptr) delete[] req;


}


void RPS::samplerRequest(int i)
{
  mData.SamplerResize();
  auto streamview = mData.StreamView();
  int samplesize = mData.SamplerView().size();

  if( i >= samplesize){
    mData.SamplerAdd(mData.RandomElement(streamview));
  }
  else{
    stringstream ss;

    auto cnt = mPingReqCnt;
    mPingReqCnt++;

    char* req = nullptr;
    unsigned int reqSize = 0;
    char* destIP = nullptr;
    unsigned int destIPSize = 0;
    int res = 0;
    int test = 0;
    //Mise en place de la push request
    res = mBrahms.Ping(&req, &reqSize, &destIP, &destIPSize, &mData);
    if (res != 0) {
      ss << "Error creating Ping_REQ #" << cnt << ": " << res;
      LOG(ss.str().c_str());
      return;
    }

    auto qIP = string(destIP, destIPSize);
    if (destIP != nullptr) delete[] destIP;

    shared_ptr<TCPConnection> conn;
    try {
      conn = TCPConnection::Dial(qIP, mRpsTcpPort);
    } catch (exception& e) {
      ss << "Error dialing " << qIP << ": " << e.what();
      LOG(ss.str().c_str());
      if (req != nullptr) delete[] req;
      test = 1;
    }

    try {
      conn->Write(req, reqSize);
    } catch (exception& e) {
      ss << "Error sending Pull_REQ #" << cnt << " to " << qIP << ": " << e.what();
      LOG(ss.str().c_str());
      if (req != nullptr) delete[] req;
      return;
    }

    //Receive RPS Answer
      char* ans = nullptr;
      unsigned int ansSize = 0;
      try {
        conn->Read(&ans, &ansSize);
      } catch (exception &e) {
        ss << " Error receiving Pull_ANS #" << cnt << " from " << qIP << ": " << e.what();
        LOG(ss.str().c_str());
        conn->Close();
        if (req != nullptr){
          delete[] req;
          test = 1;
        }
      }
      conn->Close();

    if(test == 1){
      mData.SamplerRemove(i);
      mData.SamplerAdd(mData.RandomElement(mData.StreamView()));
    }
  }

}

void RPS::ReceiveRequest(shared_ptr<TCPConnection> conn)
{
  stringstream ss;

  char* req = nullptr;
  unsigned int reqSize = 0;
  char* ans = nullptr;
  unsigned int ansSize = 0;
  int res = 0;

  int type = 4;
  std::vector<std::string> request;


  //Read RPS Request
  try {
    conn->Read(&req, &reqSize);
  } catch (exception &e) {
    conn->Close();
    return;
  }
  //Determine if it's a Push Request, a Pull request, or a ping
  splitString(request, req, reqSize, ';');

  if(request[1].compare("PushRequest") == 0){
    type = 0;
  }
  if(request[1].compare("PullRequest") == 0){
    type = 1;
  }
  if(request[1].compare("Ping") == 0){
    type = 2;
  }
  //Pull Reply
  if(type == 1){

    res = mBrahms.Pull_Reply(&ans, &ansSize, &mData);
    if (req != nullptr) delete[] req;

    if (res != 0) {
      conn->Close();
      return;
    }

    //Answer to RPS Request
    try {
      // Send it the Message
      conn->Write(ans, ansSize);
    } catch (exception &e) {
      conn->Close();
      if (ans != nullptr) delete[] ans;
      return;
    }
    conn->Close();
    if (ans != nullptr) delete[] ans;

  }

  //Push Process
  else if(type == 0){
    auto ip = request[2];
    auto nodeId = request[0];
    mBrahms.Push_Receive_Request(ip, &mData);
  }

  //Ping Reply
  else if(type == 2){
    res = mBrahms.Pong(&ans, &ansSize, &mData);

    if (res != 0) {
      conn->Close();
      return;
    }

    //Answer to RPS Request
    try {
      // Send it the Message
      conn->Write(ans, ansSize);
    } catch (exception &e) {
      conn->Close();
      if (ans != nullptr) delete[] ans;
      return;
    }
    conn->Close();
    if (ans != nullptr) delete[] ans;
  }
}



void RPS::byz_attack(int byz, std::string byzIP) //Function dedicated for byzantine attack
{

}
