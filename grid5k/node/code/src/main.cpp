#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <csignal>
#include <cxxopts.hpp>

#include "brahms/Brahms.hpp"
#include "RPS.hpp"

int main(int argc, char* argv[]) {


  cxxopts::Options options("Brahms", "Implementation of Brahms in C++");

  std::string bsIPs;
  int rpsPort = 10338;

  bsIPs.append("127.0.0.2,127.0.0.2,127.0.0.2,127.0.0.2,127.0.0.3,127.0.0.2");

  std::string tmpDir;
  std::string sgxId;
  std::string byzIP;
  int byz;
  int sgx;
  int cyclonSize;
  std::string controlIp;
  options.add_options()
      ("bs_ips", "Other nodes' IPs for bootstrap (comma separated)", cxxopts::value<std::string>(bsIPs))
      ("rps_port", "Port of RPS process", cxxopts::value<int>(rpsPort))
      ("is_byz", "is the node byzantine (0 yes, 1 no)", cxxopts::value<int>(byz))
      ("byz_ips", "Other byzantine nodes IPs", cxxopts::value<std::string>(byzIP))
      ("is_sgx", "is the node sgx (0 yes, 1 no)", cxxopts::value<int>(sgx))
      ("sgx_id", "Other sgx nodes id", cxxopts::value<std::string>(sgxId))
      ("cyclon_size", "size of cyclon", cxxopts::value<int>(cyclonSize))
      ("control_ip", "Other sgx nodes id", cxxopts::value<std::string>(controlIp))
    ;

  auto opts = options.parse(argc, argv);

  if (opts.count("help")) {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

//Lancement du RPS
  auto rps = std::make_shared<RPS>(rpsPort, bsIPs, sgxId, byz, byzIP, sgx, cyclonSize, controlIp);
  while (true){
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
    return EXIT_SUCCESS;
}
