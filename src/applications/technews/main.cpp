#include "technews.hpp"

/**
 * Tech News Main
 *
 * @param   [in]  {int}    argc
 * @param   [in]  {char**} argv
 * @returns [out] {int}
 */
int main(int argc, char** argv)
{
  kstodon::GenerateFunction gen_status_fn_ptr = &technews::GenerateStatus;
  kstodon::ReplyFunction    rep_status_fn_ptr = &technews::ReplyToStatus;

  kstodon::Bot bot{
    technews::NAME,
    gen_status_fn_ptr,
    rep_status_fn_ptr
  };

  kstodon::BotStats stats{};

  for (const auto& private_conversation : bot.FindReplies()) // DMs
  {
    stats.rx_msg++;
    (bot.ReplyToStatus(private_conversation.status)) ?  stats.tx_msg++ : stats.tx_err++;
  }

  for (const auto& comment : bot.FindComments())            // Comments
  {
    stats.rx_msg++;
    (bot.ReplyToStatus(comment)) ? stats.tx_msg++ : stats.tx_err++;
  }

  (bot.PostStatus()) ?                                      // New Post
    stats.tx_msg++ : stats.tx_err++;

  kstodon::log(stats.to_string());

  return 0;
}
