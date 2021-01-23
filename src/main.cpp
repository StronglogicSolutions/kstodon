#include <iostream>
#include <stdio.h>

#include "config.hpp"

int main(int argc, char** argv)
{
  if (argc < 2)
    throw std::invalid_argument{"KStodon called without arguments"};

  kstodon::ExecuteConfig config {kstodon::ParseRuntimeArguments(argc, argv)};
  kstodon::Bot           bot    {config.username};
  kstodon::BotStats      stats  {};
  std::string            std_out{};

  if (config.execute_bot)                               // BOT MODE
  {
    std::vector<Conversation> replies = bot.FindReplies();

    for (const Conversation& reply : replies)
    {
      stats.rx_msg++;                                   // rx
      if (reply.status.is_valid())
      {
        (bot.ReplyToStatus(reply.status)) ?
          stats.tx_msg++ : stats.tx_err++;              // tx or err
      }
    }
  }
  else                                                  // NORMAL MODE
  {
    std::vector<File> files{};

    if (!config.file_paths.empty())
    {
      for (const auto& path : config.file_paths)
        files.emplace_back(File{path});
    }
      (bot.PostStatus(Status{config.message}, files)) ?
        stats.tx_msg++ : stats.tx_err++;                // tx or err
  }

  std_out += "Bot execution complete:\n" +
               stats.to_string();

  log(std_out);

  return 0;
}
