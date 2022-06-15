#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <csignal>
#include <cxxopts.hpp>

#ifdef UAPP
#include "uapp/uapp.hpp"
#endif

#include "brahms/Brahms.hpp"
#include "RPS.hpp"


#ifdef UAPP
int SGX_CDECL main(int argc, char *argv[]) {
#else
int main(int argc, char* argv[]) {
#endif

  cxxopts::Options options("Brahms", "Implementation of Brahms in C++");

  std::string bsIPs;
  int rpsPort = 10337;

  bsIPs.append("127.0.0.2,127.0.0.2,127.0.0.2,127.0.0.2,127.0.0.3,127.0.0.2");

  std::string sgxPrivkeyPath, tmpDir;
  std::string sgxId;

  sgxPrivkeyPath.append("src/enclave/enclave-key.pem");

  options.add_options()
      ("bs_ips", "Other nodes' IPs for bootstrap (comma separated)", cxxopts::value<std::string>(bsIPs))
      ("rps_port", "Port of RPS process", cxxopts::value<int>(rpsPort))
      ("sgx_privkey", "Path to SGX private key", cxxopts::value<std::string>(sgxPrivkeyPath))
      ("tmp_dir", "Directory where to put e.g. RSA private key", cxxopts::value<std::string>(tmpDir))
      ("sgx_id", "Other SGX nodes HashId (comma separated)", cxxopts::value<std::string>(sgxId))
    ;

  auto opts = options.parse(argc, argv);

  if (opts.count("help")) {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

  //Initialisation de la clé privée de l'enclave

  unsigned int sgxPrivkeySize = 0;
  char* sgxPrivkey = nullptr;
  if (!sgxPrivkeyPath.empty()) {
    std::ifstream sgxPrivkeyFile(sgxPrivkeyPath.c_str(), std::ios::binary | std::ios::ate);
    if (!sgxPrivkeyFile.is_open()) {
      std::cout << "Failed opening SGX private key file " << sgxPrivkeyPath << std::endl;
      return EXIT_FAILURE;
    }
    // Current position is EOF due to std::ios::ate flag
    sgxPrivkeySize = sgxPrivkeyFile.tellg();
    sgxPrivkeyFile.seekg(0, std::ios::beg);
    // Read the whole file content
    sgxPrivkey = new char[sgxPrivkeySize];
    sgxPrivkeyFile.read(sgxPrivkey, sgxPrivkeySize);
    sgxPrivkeyFile.close();
  }

//initialisation de l'enclave
  #ifdef UAPP
    std::cout << "Powering up SGX enclave ..." << std::endl;
    if (initialize_enclave() != 0) {
      std::cout << "Failed booting SGX enclave." << std::endl;
      return EXIT_FAILURE;
    }
    std::cout << "SGX enclave is ON!" << std::endl;
  #endif



//Lancement du RPS
  auto rps = std::make_shared<RPS>(rpsPort, bsIPs, sgxId);
  while (true)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return EXIT_SUCCESS;
}
