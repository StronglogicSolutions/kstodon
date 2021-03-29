#pragma once

#include "kstodon/client/client.hpp"
#include <nlp.hpp>

namespace kstodon {

using GenerateFunction = Status(*)();
using ReplyFunction    = Status(*)(Status status);

inline std::vector<File> GetDefaultFilesArg()
{
  return std::vector<File>{};
}

class Bot : public ConversationTracker {

public:
Bot(const std::string& username = "",
    GenerateFunction gen_fn_ptr = nullptr,
    ReplyFunction    rep_fn_ptr = nullptr)
: m_gen_fn_ptr(gen_fn_ptr),
  m_rep_fn_ptr(rep_fn_ptr),
  m_client(Client{
    username
  })
{
  if (!m_client.HasAuth()) {
    throw std::invalid_argument{"Client was not able to authenticate"};
  }
}

std::vector<Conversation> FindReplies() override {
  std::vector<uint64_t> status_ids = GetSavedStatusIDs(m_client.GetUsername());
  std::vector<Conversation> conversations;
  std::vector<Conversation> replies{};

  try {
    conversations = m_client.FetchConversations();
  }
  catch (const request_error& e) {
    log(e.what());
  }

  return ParseRepliesFromConversations(conversations, status_ids);
}

std::vector<Status> FindComments() override {
  std::vector<Status> comments{};

  for (const auto& id : GetSavedStatusIDs(m_client.GetUsername())) {
    try {
      std::vector<Status> replies = m_client.FetchChildStatuses(id);
      comments.insert(
        comments.end(),
        std::make_move_iterator(replies.begin()),
        std::make_move_iterator(replies.end())
      );
    }
    catch (const std::exception& e)
    {
      const std::string error_message = e.what();
      log(error_message);
      m_last_error = error_message;
    }
  }

  return comments;
}

/**
 * @brief
 *
 * @param   [in]  {Status}            status
 * @param   [in]  {std::vector<File>} files
 * @returns [out] {bool}
 */
template <typename T = File>
bool PostStatus(Status status = Status{}, std::vector<T> files = GetDefaultFilesArg()) {
  try {
    if constexpr(
      std::is_same_v<T, File> || std::is_same_v<T, std::string>
    )
    return m_client.PostStatus(
      status.is_valid() ?
        status :
        m_gen_fn_ptr(),
      files
    );
  }
  catch (const std::exception& e)
  {
    const std::string error_message = e.what();
    log(error_message);
    m_last_error = error_message;
  }
  return false;
}

/**
 * @brief ReplyTostatus
 *
 * @param   [in]  {Status}      status
 * @param   [in]  {std::string} message
 * @param   [in]  {bool}        remove_id
 * @returns [out] {bool}
 */
bool ReplyToStatus(Status status, std::string message = "", bool remove_id = true)
{
  Status reply;

  if (message.empty())
    reply = m_rep_fn_ptr(status);
  else
  {
    reply = Status{};
    reply.content = message;
  }

  reply.replying_to_id = std::to_string(status.id);
  reply.content        = MakeMention(status) + reply.content;
  reply.visibility     = status.visibility;

  if (m_client.PostStatus(reply))
  {
    RemoveStatusID(m_client.GetUsername(), string_to_uint64(status.replying_to_id));
    return true;
  }

  return false;
}

const bool SetUser(const std::string& username)
{
  return m_client.SetUser(username);
}

const std::string GetLastError() const
{
  return m_last_error;
}

private:
Client           m_client;
GenerateFunction m_gen_fn_ptr;
ReplyFunction    m_rep_fn_ptr;
std::string      m_last_error;
};

} // namespace kstodon
