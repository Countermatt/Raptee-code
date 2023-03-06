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
  int rpsPort = 10337;

  bsIPs.append("127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1,127.0.0.1");


  options.add_options()
      ("bs_ips", "Other nodes' IPs for bootstrap (comma separated)",
        cxxopts::value<std::string>(bsIPs))
      ("rps_port", "Port of RPS process", cxxopts::value<int>(rpsPort))
    ;

  auto opts = options.parse(argc, argv);

  auto rps = std::make_shared<RPS>(rpsPort, bsIPs);

  while (true)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return EXIT_SUCCESS;
}
