#include <vector>
#include <random>
#include <sstream>
#include <exception>
#include <ctime>
#include <cstring>
#include <iostream>
#include <chrono>

#include "net/TCPListener.hpp"
#include "net/TCPConnection.hpp"
#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

#include "utils/log.hpp"
#include "utils/string.hpp"
#include "RPS.hpp"


using namespace std;

RPS::RPS(int rpsTcpPort, std::string bootstrapIP, std::string sgxId, int byz, std::string byzIP, int sgx, int CyclonSize, std::string controlIp)
    : mListener(make_shared<TCPListener>(rpsTcpPort)),
    mRpsTcpPort(rpsTcpPort), mPushReqCnt(0), mPullReqCnt(0), mPingReqCnt(0), mData(), mBrahms(), mbyz(byz), msgx(sgx), mCyclonSize(0), mControl()
{

  std::vector<std::string> btstIP;
  splitString(btstIP, bootstrapIP, ',');

  std::vector<std::string> sgxIds;
  splitString(sgxIds, sgxId, ',');

  std::vector<std::string> byzIds;
  splitString(byzIds, byzIP, ',');

  if(msgx == 0)
    mCyclonSize = CyclonSize;

  for(int i = 0; i < (int) btstIP.size(); i++){
    mData.GlobalAdd(btstIP[i]);
  }

  for(int i = 0; i < (int) sgxIds.size(); i++){
    mData.SGXAdd(sgxIds[i]);
  }

  for(int i = 0; i < (int) byzIds.size(); i++){
    mData.SGXAdd(byzIds[i]);
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

  while(mData.getTerm() == 0){
    ss << mData.getTerm() << endl;
    ss << "SendingThread tour: " << tour << endl;
    LOG(ss.str().c_str());
    //Reset view
    mData.PullReset();
    mData.PushReset();
    mData.StreamReset();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //RPS period

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

    if(mbyz == 0){
      RPS::byz_attack();
    }

    int samplersize = mBrahms.SamplerSize();

    for(int i = 0; i < samplersize; i++){
      RPS::samplerRequest(i);
    }
    ss << "SendingThread Send View" << endl;
    RPS::sendView();
    mBrahms.MergeView(al1, bl2, gl3, &mData);


  tour++;
  /*
  for (unsigned long int i  = 0; i < mData.GlobalView().size(); i++)
      cout << mData.GlobalView()[i] << endl;
  */

  }
  
  ss << "SendingThread Stopped" << endl;
  std::terminate();
}

void RPS::listeningThread()
{
  stringstream ss;
  ss << "ListeningThread initialized" << endl;
  LOG(ss.str().c_str());
  while(mData.getTerm() == 0){
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
  ss << "ListeningThread Stopped" << endl;
  std::terminate();
}

void RPS::pushRequest()
{
  stringstream ss;

  auto cnt = mPushReqCnt;
  mPushReqCnt++;
  using clock = std::chrono::system_clock;
  using ms = std::chrono::duration<double, std::milli>;
  char* req = nullptr;
  unsigned int reqSize = 0;
  char* destIP = nullptr;
  unsigned int destIPSize = 0;
  int res = 0;

  //Mise en place de la push request
  if(msgx == 1)
    res = mBrahms.Push_Request(&req, &reqSize, &destIP, &destIPSize, &mData);
  else{
    auto before = clock::now();
    res = mBrahms.ecall_Push_Request(&req, &reqSize, &destIP, &destIPSize, &mData);
    ms duration = 0.22*(clock::now() - before);
    std::this_thread::sleep_for(duration);
  }
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
  using clock = std::chrono::system_clock;
  using ms = std::chrono::duration<double, std::milli>;
  auto cnt = mPullReqCnt;
  mPullReqCnt++;

  char* req = nullptr;
  unsigned int reqSize = 0;
  char* destIP = nullptr;
  unsigned int destIPSize = 0;
  int res = 0;

  //Mise en place de la push request
  if(msgx == 1)
    res = mBrahms.Pull_Request(&req, &reqSize, &destIP, &destIPSize, &mData);
  else{
    auto before = clock::now();
    res = mBrahms.ecall_Pull_Request(&req, &reqSize, &destIP, &destIPSize, &mData);
    ms duration = 0.19*(clock::now() - before);
    std::this_thread::sleep_for(duration);
  }
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

    if(msgx == 1)
      res = mBrahms.Pull_Receive_Reply(requestip, &mData);
    else{
      auto before = clock::now();
      res = mBrahms.ecall_Pull_Receive_Reply(requestip, &mData, nodeId, mCyclonSize);
      ms duration = 0.19*(clock::now() - before);
      std::this_thread::sleep_for(duration);
    }
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
  using clock = std::chrono::system_clock;
  using ms = std::chrono::duration<double, std::milli>;
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
  if(request[1].compare("Terminate") == 0){
    ss << " Test" << endl;
    mData.updateTerm();
    
  }
  //Pull Reply
  if(type == 1){
    if(msgx == 1)
      res = mBrahms.Pull_Reply(&ans, &ansSize, &mData);
    else{
      auto before = clock::now();
      res = mBrahms.ecall_Pull_Reply(&ans, &ansSize, &mData, request[0],mCyclonSize);
      ms duration = 0.19*(clock::now() - before);
      std::this_thread::sleep_for(duration);
    }
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
    if(msgx == 1)
      mBrahms.Push_Receive_Request(ip, &mData);
    else{
      auto before = clock::now();
      mBrahms.ecall_Push_Receive_Request(ip, &mData);
      ms duration = 0.22*(clock::now() - before);
      std::this_thread::sleep_for(duration);
    }
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



void RPS::byz_attack() //Function dedicated for byzantine attack
{

}


void RPS::sendView()
{
  stringstream ss;

  char* req = nullptr;
  unsigned int reqSize = 0;

  int res = 0;

  //Mise en place de la view push
  res = mBrahms.Send_View(&req, &reqSize, &mData);
  if (res != 0) {
    ss << "Error creating View_REQ" << ": " << res;
    LOG(ss.str().c_str());
    return;
  }

  auto qIP = string(mControl);

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
    ss << "Error sending View_REQ" << " to " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  ss << "Sent View_REQ" << " to " << qIP;
  ss.clear();
  ss.str("");

}