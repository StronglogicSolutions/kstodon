#include "config.hpp"

/**
 * KStodon Main
 *
 * KStodon Bot class will do one of the following:

 * 1. Post message to Mastodon
 * 2. Fetch replies to previous messages and send a new reply message
 *
 * @param   [in]  {int}    argc
 * @param   [in]  {char**} argv
 * @returns [out] {int}
 */
int main(int argc, char** argv)
{
  if (argc < 2)
    throw std::invalid_argument{"KStodon called without arguments"};

  kstodon::ExecuteConfig config {kstodon::ParseRuntimeArguments(argc, argv)};
  kstodon::Bot           bot    {config.username};
  kstodon::BotStats      stats  {};
  std::vector<kstodon::File>      files  {};
  std::string            std_out{};

  // if (config.execute_bot)                               // BOT MODE
  // {
  //   std::vector<kstodon::Conversation> replies = bot.FindReplies();

  //   for (const kstodon::Conversation& reply : replies)
  //   {
  //     if (reply.status.is_valid())
  //       (bot.ReplyToStatus(reply.status)) ?
  //         stats.tx_msg++ : stats.tx_err++;              // tx or err
  //     stats.rx_msg++;                                   // rx
  //   }
  // }
  // else                                                  // NORMAL MODE
  // {
  if (!config.file_paths.empty())
    for (const auto& path : config.file_paths)
      files.emplace_back(kstodon::File{path});

  (bot.PostStatus(kstodon::Status{config.message}, files)) ?
    stats.tx_msg++ : stats.tx_err++;                  // tx or err
  // }

  std_out += "Bot execution complete:\n" + stats.to_string();

  kstodon::log(std_out);

  return 0;
}
