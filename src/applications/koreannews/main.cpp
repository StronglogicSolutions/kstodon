#include "koreannews.hpp"

/**
 * Tech News Main
 *
 * @param   [in]  {int}    argc
 * @param   [in]  {char**} argv
 * @returns [out] {int}
 */
int main(int argc, char** argv)
{
  kstodon::ExecuteConfig    config            = kstodon::ParseRuntimeArguments(argc, argv);
  kstodon::GenerateFunction gen_status_fn_ptr = &koreannews::GenerateStatus;
  kstodon::ReplyFunction    rep_status_fn_ptr = &koreannews::ReplyToStatus;

  koreannews::SetLanguage(config.language);

  kstodon::Bot bot{
    koreannews::NAME,
    gen_status_fn_ptr,
    rep_status_fn_ptr
  };

  kstodon::BotStats stats{};

  // for (const auto& private_conversation : bot.FindReplies()) // DMs
  // {
  //   stats.rx_msg++;
  //   (bot.ReplyToStatus(private_conversation.status)) ?  stats.tx_msg++ : stats.tx_err++;
  // }

  // for (const auto& comment : bot.FindComments())            // Comments
  // {
  //   stats.rx_msg++;
  //   (bot.ReplyToStatus(comment)) ? stats.tx_msg++ : stats.tx_err++;
  // }

  (bot.PostStatus(kstodon::Status{})) ?                                      // New Post
    stats.tx_msg++ : stats.tx_err++;

  kstodon::log(stats.to_string());

  return 0;
}
