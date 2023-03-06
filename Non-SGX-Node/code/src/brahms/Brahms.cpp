#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

#include <iostream>
#include <ostream>

Brahms::Brahms(){
  mAL1 = 3;
  mBL2 = 3;
  mGL3 = 0;
  mSamplerSize = 5;
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
    data->GlobalAdd(data->RandomElement(pushView));
  }

  for(int i = 0; i < bL2; i++){
    data->GlobalAdd(data->RandomElement(pullView));
  }

  for(int i = 0; i < gL3; i++){
    data->GlobalAdd(data->RandomElement(sampleView));
  }

}
