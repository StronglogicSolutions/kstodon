#include <iostream>
#include <stdio.h>
#include "kstodon/kstodon.hpp"

struct ExecuteConfig {
std::string              message;
std::vector<std::string> file_paths;
std::string              description;
};

ExecuteConfig ParseRuntimeArguments(int argc, char** argv) {
  ExecuteConfig config{};

  for (int i = 1; i < argc; i++) {
    std::string argument = SanitizeInput(argv[i]);
    if (argument.find("--header") == 0){
      config.message = argument.substr(9);
      continue;
    }
    else
    if (argument.find("--description") == 0) {
      config.description = argument.substr(14);
      continue;
    }
    else
    if (argument.find("--filename") == 0) {
      config.file_paths.emplace_back(argument.substr(11));
      continue;
    }
  }

  return config;
}

int main(int argc, char** argv) {
  std::string       std_out{};
  kstodon::Client   kstodon_client{};
  std::vector<File> files{};

  if (argc < 2) {
    throw std::invalid_argument{"KStodon called without arguments"};
  }

  ExecuteConfig config = ParseRuntimeArguments(argc, argv);

  if (!config.file_paths.empty()) {
    for (const auto& path : config.file_paths)
      files.emplace_back(File{path});
  }

  std_out +=
    (kstodon_client.PostStatus(Status{config.message}, files)) ?
      "Post succeeded" :
      "Post failed";

  std::cout << std_out << std::endl;

  return 0;
}
