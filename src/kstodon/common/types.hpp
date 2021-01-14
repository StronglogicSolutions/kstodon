#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <vector>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "util.hpp"

inline std::string GetJSONStringValue(nlohmann::json data, std::string key) {
  if (!data.is_null() && data.contains(key) && !data[key].is_null())
    return data[key].get<std::string>();
  return "";
}

inline bool GetJSONBoolValue(nlohmann::json data, std::string key) {
  if (!data.is_null() && data.contains(key) && !data[key].is_null())
    return data[key].get<bool>();
  return "";
}

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░ STRUCTS ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

struct AccountField {
std::string name;
std::string value;
};

struct Account {
std::string               id;
std::string               username;
std::string               acct;
std::string               display_name;
std::string               locked;
std::string               bot;
std::string               discoverable;
std::string               group;
std::string               created_at;
std::string               note;
std::string               url;
std::string               avatar;
std::string               header;
std::string               header_static;
uint32_t                  followers_count;
uint32_t                  following_count;
uint32_t                  statuses_count;
std::string               last_status_at;
std::vector<AccountField> fields;

friend std::ostream &operator<<(std::ostream& o, const Account& a) {
  std::string fields{};
  for (const auto& field : a.fields) fields += "\nName: " + field.name + "\nValue: " + field.value;

  o << "ID:           " << a.id              << "\n" <<
       "Username:     " << a.username        << "\n" <<
       "Account:      " << a.acct            << "\n" <<
       "Display Name: " << a.display_name    << "\n" <<
       "Locked:       " << a.locked          << "\n" <<
       "Bot:          " << a.bot             << "\n" <<
       "Discoverable: " << a.discoverable    << "\n" <<
       "Group:        " << a.group           << "\n" <<
       "Created:      " << a.created_at      << "\n" <<
       "Note:         " << a.note            << "\n" <<
       "URL:          " << a.url             << "\n" <<
       "Avatar:       " << a.avatar          << "\n" <<
       "Header:       " << a.header          << "\n" <<
       "Followers:    " << a.followers_count << "\n" <<
       "Following:    " << a.following_count << "\n" <<
       "Statuses:     " << a.statuses_count  << "\n" <<
       "Last Status:  " << a.last_status_at  << "\n" <<

       "FIELDS\n"       << fields;

  return o;
}
};

inline Account ParseAccountFromJSON(nlohmann::json data) {
  Account account{};
  account.id              = SanitizeJSON(data["id"].dump());
  account.username        = SanitizeJSON(data["username"].dump());
  account.acct            = SanitizeJSON(data["acct"].dump());
  account.display_name    = SanitizeJSON(data["display_name"].dump());
  account.locked          = SanitizeJSON(data["locked"].dump());
  account.bot             = SanitizeJSON(data["bot"].dump());
  account.discoverable    = SanitizeJSON(data["discoverable"].dump());
  account.group           = SanitizeJSON(data["group"].dump());
  account.created_at      = SanitizeJSON(data["created_at"].dump());
  account.note            = SanitizeJSON(data["note"].dump());
  account.url             = SanitizeJSON(data["url"].dump());
  account.avatar          = SanitizeJSON(data["avatar"].dump());
  account.header          = SanitizeJSON(data["header"].dump());
  account.followers_count = std::stoi(SanitizeJSON(data["followers_count"].dump()));
  account.following_count = std::stoi(SanitizeJSON(data["following_count"].dump()));
  account.statuses_count  = std::stoi(SanitizeJSON(data["statuses_count"].dump()));
  account.last_status_at = SanitizeJSON(data["last_status_at"].dump());

  nlohmann::json fields = data["source"]["fields"];

  for (const auto& field : fields) {
    account.fields.emplace_back(AccountField{
      .name  = SanitizeJSON(field["name"].dump()),
      .value = SanitizeJSON(field["value"].dump())
    });
  }

  return account;
}

struct Credentials {
std::string id;
std::string name;
std::string website;
std::string redirect_uri;
std::string scope;
std::string client_id;
std::string client_secret;
std::string vapid_key;
std::string code;

bool is_valid() {
  return
    !id.empty() &&
    !name.empty() &&
    !website.empty() &&
    !redirect_uri.empty() &&
    !scope.empty() &&
    !client_id.empty() &&
    !client_secret.empty() &&
    !vapid_key.empty() &&
    !code.empty();
}
};

struct Auth {
std::string access_token;
std::string token_type;
std::string scope;
std::string created_at;

bool is_valid() {
  return (
    !access_token.empty() &&
    !token_type.empty()   &&
    !scope.empty()        &&
    !created_at.empty()
  );
}
};

struct Application {
std::string name;
std::string url;
};

struct MetaDetails {
uint32_t    width;
uint32_t    height;
std::string size;
float       aspect;
};
struct MediaMetadata {
MetaDetails original;
MetaDetails small;
};

struct Tag {
std::string name;
std::string url;
};

struct Card {};
struct Poll {};
struct Media {
std::string id;
std::string type;
std::string url;
std::string preview_url;
std::string remote_url;
std::string preview_remote_url;
std::string text_url;
MediaMetadata meta;
std::string description;
std::string blurhash;

bool has_media() { return !id.empty(); }
};

struct Status{
uint64_t                 id;
std::string              created_at;
std::string              replying_to_id;
std::string              replying_to_account;
bool                     sensitive;
std::string              spoiler;
std::string              visibility;
std::string              language;
std::string              uri;
std::string              url;
std::string              replies;
std::string              reblogs;
std::string              favourites;
std::string              content;
std::string              reblog;
Application              application;
Account                  account;
std::vector<Media>       media;
std::vector<std::string> mentions;
std::vector<Tag>         tags;
std::vector<std::string> emojis;
Card                     card;
Poll                     poll;
};


inline std::vector<Tag> ParseTagsFromJSON(nlohmann::json data) {
  std::vector<Tag> tags_v{};

  for (const auto& tag : data) {
    tags_v.emplace_back(Tag{
      .name = tag["name"],
      .url = tag["url"]
    });
  }

  return tags_v;
}

inline std::vector<Media> ParseMediaFromJSON(nlohmann::json data) {
  std::vector<Media> media_v{};

  if (!data.is_null(), data.is_array()) {
    for (const auto& item : data) {
      Media media{};

      media.id                 = GetJSONStringValue(item, "id");
      media.type               = GetJSONStringValue(item, "type");
      media.url                = GetJSONStringValue(item, "url");
      media.preview_url        = GetJSONStringValue(item, "preview_url");
      media.remote_url         = GetJSONStringValue(item, "remote_url");
      media.preview_remote_url = GetJSONStringValue(item, "preview_remote_url");
      media.text_url           = GetJSONStringValue(item, "text_url");
      media.description        = GetJSONStringValue(item, "description");
      media.blurhash           = GetJSONStringValue(item, "blurhash");
      media.meta               = MediaMetadata{};

      if (item["meta"].contains("original")) {
        auto original = item["meta"]["original"];

        media.meta.original = MetaDetails{
          .width    = original["width"],
          .height   = original["height"],
          .size     = original["size"],
          .aspect   = original["aspect"]
        };
      }

      if (item["meta"].contains("small")) {
        auto small = item["meta"]["small"];

        media.meta.small = MetaDetails{
          .width    = small["width"],
          .height   = small["height"],
          .size     = small["size"],
          .aspect   = small["aspect"]
        };
      }

      media_v.emplace_back(std::move(media));
    }

  }

  return media_v;
}


/**
 * @brief
 *
 * @param data
 * @return Status
 */
inline Status JSONToStatus(nlohmann::json data) {
  Status status{};

  auto is_object = data.is_object();
  auto is_array = data.is_array();
  auto is_null = data.is_null();

  if (!data.is_null() && data.is_object()) {
    status.id                  = std::stoul(data["id"].get<std::string>());
    status.created_at          = GetJSONStringValue  (data, "created_at");
    status.replying_to_id      = GetJSONStringValue  (data, "in_reply_to");
    status.replying_to_account = GetJSONStringValue  (data, "in_reply_to_account_id");
    status.sensitive           = GetJSONBoolValue    (data, "sensitive");
    status.spoiler             = GetJSONStringValue  (data, "spoiler");
    status.visibility          = GetJSONStringValue  (data, "visibility");
    status.language            = GetJSONStringValue  (data, "language");
    status.uri                 = GetJSONStringValue  (data, "uri");
    status.url                 = GetJSONStringValue  (data, "url");
    status.replies             = GetJSONStringValue  (data, "replies");
    status.reblogs             = GetJSONStringValue  (data, "reblogs");
    status.favourites          = GetJSONStringValue  (data, "favourites");
    status.content             = GetJSONStringValue  (data, "content");
    status.reblog              = GetJSONStringValue  (data, "reblog");
    status.application.name    = GetJSONStringValue  (data["application"], "name");
    status.application.url     = GetJSONStringValue  (data["application"], "website");
    status.account             = ParseAccountFromJSON(data["account"]);
    status.media               = ParseMediaFromJSON  (data["media_attachments"]);
    status.mentions            = data["mentions"].get<std::vector<std::string>>();
    status.tags                = ParseTagsFromJSON   (data["tags"]);
    status.emojis              = data["emojis"].get<std::vector<std::string>>();
  }

  return status;
}
#endif // __TYPES_HPP__
