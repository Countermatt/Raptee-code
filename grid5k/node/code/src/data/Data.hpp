#ifndef DATA_HPP_
#define DATA_HPP_

#include <vector>
#include <string>

class Data {

  public:

    Data()
          : mGlobalView(),
           mSamplerView(),
           mPushView(),
           mPullView(),
           mSGXView(),
           mByzView(),
           mMyIP("127.0.0.1"),
           mIdentifiant("nonsgx"),
           mTerm(0){}


    // - Getters
    std::vector<std::string> GlobalView() { return mGlobalView; }
    std::vector<std::string> SamplerView() { return mSamplerView; }
    std::vector<std::string> PushView() { return mPushView; }
    std::vector<std::string> PullView() { return mPullView; }
    std::vector<std::string> SGXView() { return mSGXView; }
    std::vector<std::string> ByzView() { return mByzView; }

    std::string MyIP() { return mMyIP; }
    std::vector<std::string> StreamView() { return mStreamView; }
    std::string Identifiant() { return mIdentifiant; }

    std::string HashIdentifiant();

    // Random Element choice
    std::string RandomElement(std::vector<std::string> View);

    // - Add
    void PullAdd(std::string element);
    void PushAdd(std::string element);
    void GlobalAdd(std::string element);
    void SGXAdd(std::string element);
    void SamplerAdd(std::string element);
    void StreamAdd(std::string element);
    void ByzAdd(std::string element);

    void SamplerRemove(int i);
    void GlobalRemove(int i);
    // - Reset
    void PullReset();
    void PushReset();
    void GlobalReset();
    void StreamReset();
    void SGXReset();

    void SamplerResize();

    //Search in SGXVIew
    int SGXfind(std::string element);
    int Globalfind(std::string element);


    int getTerm() { return mTerm; }
    void updateTerm();
  private:

    std::vector<std::string> mGlobalView;
    std::vector<std::string> mSamplerView;
    std::vector<std::string> mPushView;
    std::vector<std::string> mPullView;
    std::vector<std::string> mStreamView;
    std::vector<std::string> mSGXView;
    std::vector<std::string> mByzView;
    std::string mMyIP;
    std::string mIdentifiant;
    int mTerm;

};

#endif // DATA_HPP_
