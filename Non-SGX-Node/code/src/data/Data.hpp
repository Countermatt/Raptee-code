#ifndef DATA_HPP_
#define DATA_HPP_

#include <vector>
#include <string>
#include <functional>
#include <iostream>

class Data {

  public:

    Data()
          : mGlobalView(),
           mSamplerView(),
           mPushView(),
           mPullView(),
           mMyIP("127.0.0.1"),
           mIdentifiant("nonsgx"),
           mHasher(){}
           

    // - Getters
    std::vector<std::string> GlobalView() { return mGlobalView; }
    std::vector<std::string> SamplerView() { return mSamplerView; }
    std::vector<std::string> PushView() { return mPushView; }
    std::vector<std::string> PullView() { return mPullView; }
    std::string MyIP() { return mMyIP; }
    std::vector<std::string> StreamView() { return mStreamView; }
    std::string Identifiant() { return mIdentifiant; }

    std::string HashIdentifiant();
    std::hash<std::string> Hasher() { return mHasher; }
    // Random Element choice
    std::string RandomElement(std::vector<std::string> View);

    // - Add
    void PullAdd(std::string element);
    void PushAdd(std::string element);
    void GlobalAdd(std::string element);
    void SamplerAdd(std::string element, int samplersize);
    void StreamAdd(std::string element);

    void SamplerRemove(int i);
    // - Reset
    void PullReset();
    void PushReset();
    void GlobalReset();
    void StreamReset();

    void SamplerResize();

    void DisplayGlobal();
  private:

    std::vector<std::string> mGlobalView;
    std::vector<std::string> mSamplerView;
    std::vector<std::string> mPushView;
    std::vector<std::string> mPullView;
    std::vector<std::string> mStreamView;
    std::string mMyIP;
    std::string mIdentifiant;
    std::hash<std::string> mHasher;


};

#endif // DATA_HPP_
