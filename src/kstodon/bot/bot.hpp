#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "kstodon/kstodon.hpp"

class ConversationTracker {
public:
virtual ~ConversationTracker() {}
virtual void FindReplies() = 0;
};


namespace kstodon {
class Bot : public ConversationTracker {

public:
Bot()
: m_client(Client{}) {
  if (!m_client.HasAuth()) {
    throw std::invalid_argument{"Client was not able to authenticate"};
  }
}

virtual void FindReplies() override {
  Account account = m_client.GetAccount();
  std::vector<uint64_t> status_ids = GetSavedStatusIDs(account.username);
  // TODO: Fetch replies to each of these IDs
}

private:
Client m_client;
};

} // namespace kstodon

#endif // __BOT_HPP__
