#include <iostream>
#include <stdio.h>

#include "config.hpp"

int main(int argc, char** argv) {
  if (argc < 2)
    throw std::invalid_argument{"KStodon called without arguments"};

  BotStats          stats{};
  std::string       std_out{};
  ExecuteConfig     config = ParseRuntimeArguments(argc, argv);

  if (config.execute_bot) {
    kstodon::Bot bot{config.username};

    std::vector<Conversation> replies = bot.FindReplies();

    for (const Conversation& reply : replies) {
      stats.rx_msg++;
      if (reply.status.is_valid()) {
        (bot.ReplyToStatus(reply.status)) ?
          stats.tx_msg++ : stats.tx_err++;
      }
    }
    std_out += "Bot execution complete:\n" +
               stats.to_string();
  }
  else {
    kstodon::Client   client{config.username};
    std::vector<File> files{};

    if (!config.file_paths.empty()) {
      for (const auto& path : config.file_paths)
        files.emplace_back(File{path});
    }

    std_out +=
      (client.PostStatus(Status{config.message}, files)) ?
        "Post succeeded" :
        "Post failed";
  }

  std::cout << std_out << std::endl;

  return 0;
}
