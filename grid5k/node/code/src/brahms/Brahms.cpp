#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

#include <iostream>
#include <ostream>

Brahms::Brahms(){
  mAL1 = 50;
  mBL2 = 50;
  mGL3 = 0;
  mSamplerSize = 10;
}

//Push
int Brahms::Push_Request(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();



  //Choix du destinataire
  std::string dIP;
  dIP = data->RandomElement(View);

  //Copie de l'IP du destinataire dans la requête
  *destIPSize = dIP.size();
  *destIP = new char[*destIPSize];


  dIP.copy(*destIP, *destIPSize);

  //Construction de la requête
  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("PushRequest");
  reqIP.append(";");
  reqIP.append(myIP);


  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Push_Receive_Request(std::string ans,
                                 Data* data)
{
  data->PushAdd(ans);
  data->StreamAdd(ans);
  return 0;
}


//Pull
int Brahms::Pull_Request(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 Data* data)
{

  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  //Choix du destinataire
  std::string dIP;
  dIP = data->RandomElement(View);

  //Copie de l'IP du destinataire dans la requête
  *destIPSize = dIP.size();
  *destIP = new char[*destIPSize];
  dIP.copy(*destIP, *destIPSize);



  //Mise en place de la requête
  std::string reqIP;

  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("PullRequest");
  reqIP.append(";");
  reqIP.append("0.0.0.0");
  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Pull_Reply(char** ans,
               unsigned int* ansSize,
               Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("PullReply");
  reqIP.append(";");

  int viewsize = View.size();
  for(int i = 0; i < viewsize; i++){
      reqIP.append(View[i]);
      reqIP.append(",");

  }


  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;

}

int Brahms::Pull_Receive_Reply(std::string ans,
                               Data* data)
{
  std::vector<std::string> View;

  splitString(View, ans, ',');


  for(int i = 0; i < (int) View.size(); i++){
    data->PullAdd(View[i]);
    data->StreamAdd(View[i]);
  }

  return 0;
}


//Sample Ping Pong
int Brahms::Ping(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  //Choix du destinataire
  std::string dIP;
  dIP = data->RandomElement(View);

  //Copie de l'IP du destinataire dans la requête
  *destIPSize = dIP.size();
  *destIP = new char[*destIPSize];
  dIP.copy(*destIP, *destIPSize);

  //Construction de la requête
  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("Ping");
  reqIP.append(";");
  reqIP.append(myIP);

  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Pong(char** ans,
               unsigned int* ansSize,
               Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("Pong");
  reqIP.append(";");
  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;

}

//Merge View
void Brahms::MergeView(int aL1,
               int bL2,
               int gL3,
               Data* data)
{

  auto pullView = data->PullView();
  auto pushView = data->PushView();
  auto sampleView = data->SamplerView();

  data->GlobalReset();

  for(int i = 0; i < aL1; i++){
    auto e = data->RandomElement(pushView);
    if(data->Globalfind(e)){
      data->GlobalAdd(e);
    }
    
  }

  for(int i = 0; i < bL2; i++){
    //data->GlobalAdd(data->RandomElement(pullView));
    auto e = data->RandomElement(pullView);
    
    if(data->Globalfind(e)){
      data->GlobalAdd(e);
    }
  }

  for(int i = 0; i < gL3; i++){
    //data->GlobalAdd(data->RandomElement(sampleView));
    auto e = data->RandomElement(sampleView);
    if(data->Globalfind(e)){
      data->GlobalAdd(e);
    }
  }
  int tmp = 0;
  auto globalView = data->GlobalView();

  while ((int) data->GlobalView().size() < aL1+bL2+gL3){
     data->GlobalAdd(globalView[tmp]);
     tmp ++;
     if(tmp == (int) globalView.size()){
       tmp = 0;
     }
  }

}


int Brahms::ecall_Push_Request(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();



  //Choix du destinataire
  std::string dIP;
  dIP = data->RandomElement(View);

  //Copie de l'IP du destinataire dans la requête
  *destIPSize = dIP.size();
  *destIP = new char[*destIPSize];


  dIP.copy(*destIP, *destIPSize);

  //Construction de la requête
  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("PushRequest");
  reqIP.append(";");
  reqIP.append(myIP);


  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::ecall_Push_Receive_Request(std::string ans,
                                 Data* data)
{
  data->PushAdd(ans);
  data->StreamAdd(ans);
  return 0;
}


//Pull
int Brahms::ecall_Pull_Request(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 Data* data)
{

  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  //Choix du destinataire
  std::string dIP;
  dIP = data->RandomElement(View);

  //Copie de l'IP du destinataire dans la requête
  *destIPSize = dIP.size();
  *destIP = new char[*destIPSize];
  dIP.copy(*destIP, *destIPSize);



  //Mise en place de la requête
  std::string reqIP;

  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("PullRequest");
  reqIP.append(";");
  reqIP.append("0.0.0.0");
  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::ecall_Pull_Reply(char** ans,
               unsigned int* ansSize,
               Data* data, 
               std::string NodeId, 
               int CyclonSize)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();
  auto SGXList = data->SGXView();
  int viewsize = 0;
  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("PullReply");
  reqIP.append(";");

  if (data->SGXfind(NodeId)){
    viewsize = CyclonSize; 
  }
  else{
    viewsize = View.size();
  }
  for(int i = 0; i < viewsize; i++){
    reqIP.append(View[i]);
    reqIP.append(",");
  }


  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;

}

int Brahms::ecall_Pull_Receive_Reply(std::string ans,
                               Data* data, 
                               std::string NodeId,
                               int CyclonSize)
{
  std::vector<std::string> View;

  splitString(View, ans, ',');


  for(int i = 0; i < (int) View.size(); i++){
    data->PullAdd(View[i]);
    data->StreamAdd(View[i]);
  }
  if (data->SGXfind(NodeId)){ 
    for(int i = 0; i < (int) CyclonSize; i++){
      data->GlobalRemove(0);
    }
    for(int i = 0; i < (int) CyclonSize; i++){
      data->GlobalAdd(View[i]);
    }
  }


  return 0;
}


//Sample Ping Pong
extern "C" int ecall_Ping(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  //Choix du destinataire
  std::string dIP;
  dIP = data->RandomElement(View);

  //Copie de l'IP du destinataire dans la requête
  *destIPSize = dIP.size();
  *destIP = new char[*destIPSize];
  dIP.copy(*destIP, *destIPSize);

  //Construction de la requête
  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("Ping");
  reqIP.append(";");
  reqIP.append(myIP);

  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::ecall_Pong(char** ans,
               unsigned int* ansSize,
               Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("Pong");
  reqIP.append(";");
  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;

}

//Merge View
void Brahms::ecall_MergeView(int aL1,
               int bL2,
               int gL3,
               Data* data)
{

  auto pullView = data->PullView();
  auto pushView = data->PushView();
  auto sampleView = data->SamplerView();

  data->GlobalReset();

  for(int i = 0; i < aL1; i++){
    auto e = data->RandomElement(pushView);
    if(data->Globalfind(e)){
      data->GlobalAdd(e);
    }
    
  }

  for(int i = 0; i < bL2; i++){
    //data->GlobalAdd(data->RandomElement(pullView));
    auto e = data->RandomElement(pullView);
    
    if(data->Globalfind(e)){
      data->GlobalAdd(e);
    }
  }

  for(int i = 0; i < gL3; i++){
    //data->GlobalAdd(data->RandomElement(sampleView));
    auto e = data->RandomElement(sampleView);
    if(data->Globalfind(e)){
      data->GlobalAdd(e);
    }
  }
  int tmp = 0;
  auto globalView = data->GlobalView();

  while ((int) data->GlobalView().size() < aL1+bL2+gL3){
     data->GlobalAdd(globalView[tmp]);
     tmp ++;
     if(tmp == (int) globalView.size()){
       tmp = 0;
     }
  }
}

int Brahms::Send_View(char** ans,
               unsigned int* ansSize,
               Data* data)
{
  auto gView = data->GlobalView();
  auto myIP = data->MyIP();
  auto sView = data->SamplerView();

  std::string reqIP;
  reqIP.append(data->HashIdentifiant());
  reqIP.append(";");
  reqIP.append("SendView");
  reqIP.append(";");

  int gviewsize = gView.size();
  for(int i = 0; i < gviewsize; i++){
      reqIP.append(gView[i]);
      reqIP.append(",");

  }
  
  reqIP.append(";");
    int sviewsize = sView.size();
  for(int i = 0; i < sviewsize; i++){
      reqIP.append(sView[i]);
      reqIP.append(",");

  }



  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;

}