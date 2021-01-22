#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "kstodon/kstodon.hpp"

class ConversationTracker {
public:
virtual ~ConversationTracker() {}
virtual std::vector<Conversation> FindReplies() = 0;
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

std::vector<Conversation> FindReplies() override {
  std::vector<uint64_t> status_ids = GetSavedStatusIDs(m_client.GetUsername());
  std::vector<Conversation> replies{};
  for (const auto& id : status_ids) {
    try {
      std::vector<Conversation> retrieved_statuses = m_client.FetchRepliesToStatus(id);
      replies.insert(
        replies.end(),
        std::make_move_iterator(retrieved_statuses.begin()),
        std::make_move_iterator(retrieved_statuses.end())
      );
    }
    catch (const request_error& e) {
      log(e.what());
    }
  }
  return replies;
}

private:
Client m_client;
};

} // namespace kstodon

#endif // __BOT_HPP__
