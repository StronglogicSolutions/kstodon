#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <vector>
#include <string>
#include <unordered_map>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "util.hpp"

class PostDataInterface {
public:
virtual ~PostDataInterface() {}
virtual std::string postdata() = 0;
};


struct File : public PostDataInterface {
std::string path;
std::string description;
std::string thumbnail;

std::string GetBytes() {
  return ReadFromFile(path);
}

File(std::string file_path) : path(file_path) {}

virtual ~File() override {}
/**
 * @brief
 *
 * TODO: Add thumbnail support
 *
 * @return std::string
 */
virtual std::string postdata() {
  std::string payload = GetBytes();
  return std::string{
    "file="         + payload + "&" +
    "description="  + description
  };
}

cpr::Multipart multiformdata() {
  std::string bytes = GetBytes();
  return cpr::Multipart{
    {"description", description},
    {"file", cpr::File{path}}
  };
}

};
inline std::string GetJSONStringValue(nlohmann::json data, std::string key) {
  if (!data.is_null() && data.contains(key) && !data[key].is_null())
    return data[key].get<std::string>();
  return "";
}

template<typename T>
inline T GetJSONValue(nlohmann::json data, std::string key) {
  if (!data.is_null() && data.contains(key) && !data[key].is_null())
    return data[key].get<T>();

  if (std::is_integral<T>::value)
    return static_cast<T>(0);
  if (std::is_floating_point<T>::value)
    return static_cast<T>(0);
  if constexpr (std::is_same_v<std::string, T>)
    return "";
  if constexpr (std::is_same_v<std::vector<std::string>, T>) {
    return std::vector<std::string>{};
  }

  throw std::invalid_argument{"Unsupported type"};
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
bool                      locked;
bool                      bot;
bool                      discoverable;
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

struct Mention {
std::string acct;
std::string id;
std::string url;
std::string username;

friend std::ostream &operator<<(std::ostream& o, const Mention& m) {
  o << "ID: " << m.id << "\n" << "Acct: " << m.acct << "\n" << "URL: " << m.url << "\nUsername:" << m.username << std::endl;

  return o;
}

};

inline Account ParseAccountFromJSON(nlohmann::json data) {
  Account account{};
  if (!data.is_null()) {
    account.id              = GetJSONStringValue(data, "id");
    account.username        = GetJSONStringValue(data, "username");
    account.acct            = GetJSONStringValue(data, "acct");
    account.display_name    = GetJSONStringValue(data, "display_name");
    account.locked          = GetJSONBoolValue  (data, "locked");
    account.bot             = GetJSONBoolValue  (data, "bot");
    account.discoverable    = GetJSONBoolValue  (data, "discoverable");
    account.group           = GetJSONBoolValue  (data, "group");
    account.created_at      = GetJSONStringValue(data, "created_at");
    account.note            = GetJSONStringValue(data, "note");
    account.url             = GetJSONStringValue(data, "url");
    account.avatar          = GetJSONStringValue(data, "avatar");
    account.header          = GetJSONStringValue(data, "header");
    account.locked          = GetJSONBoolValue  (data, "locked");
    account.last_status_at  = GetJSONStringValue(data, "last_status_at");
    account.followers_count = GetJSONValue<uint32_t>(data, "followers_count");
    account.following_count = GetJSONValue<uint32_t>(data, "following_count");
    account.statuses_count  = GetJSONValue<uint32_t>(data, "statuses_count");

    nlohmann::json fields = data["source"]["fields"];

    for (const auto& field : fields) {
      account.fields.emplace_back(AccountField{
        .name  = GetJSONStringValue(field, "name"),
        .value = GetJSONStringValue(field, "value")
      });
    }
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

friend std::ostream &operator<<(std::ostream& o, const Application& a) {
  o << "Name: " << a.name << "\n" << "URL: " << a.url << std::endl;

  return o;
}
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

friend std::ostream &operator<<(std::ostream& o, const Tag& t) {
  o << "Name: " << t.name << "\n" << "URL: " << t.url << std::endl;

  return o;
}
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

friend std::ostream &operator<<(std::ostream& o, const Media& m) {
o << "ID: " << m.id << "\nURL: " << m.url << "\n(TODO: Complete media ostream)" << std::endl;
return o;
}
};

struct Status : public PostDataInterface {
Status()
: sensitive(false) {}

Status(std::string text)
: sensitive(false), content(text) {}

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
uint32_t                 replies;
uint32_t                 reblogs;
uint32_t                 favourites;
std::string              content;
Account                  reblog;
Application              application;
Account                  account;
std::vector<Media>       media;
std::vector<Mention>     mentions;
std::vector<Tag>         tags;
std::vector<std::string> emojis;
Card                     card;
Poll                     poll;

friend std::ostream &operator<<(std::ostream& o, const Status& s) {
  o << "ID:"           << std::to_string(s.id) << "\n" <<
       "Created:"      << s.created_at << "\n" <<
       "To ID:"        << s.replying_to_id << "\n" <<
       "To acc:"       << s.replying_to_account << "\n" <<
       "Sensitive:"    << s.sensitive << "\n" <<
       "Spoiler:"      << s.spoiler << "\n" <<
       "Visibility:"   << s.visibility << "\n" <<
       "Language:"     << s.language << "\n" <<
       "URI:"          << s.uri << "\n" <<
       "URL:"          << s.url << "\n" <<
       "Replies:"      << s.replies << "\n" <<
       "Reblogs:"      << s.reblogs << "\n" <<
       "Favourites:"   << s.favourites << "\n" <<
       "Content:"      << s.content << "\n" <<
       "Reblog:"       << s.reblog << "\n" <<
       "APPLICATION\n" << s.application << "\n" <<
       "ACCOUNT\n"     << s.account << "\n";

       o << "MEDIA\n";
       for (const auto& media : s.media)
        o << media << std::endl;
      o << "MENTIONS\n";
       for (const auto& mention : s.mentions)
        o << mention << std::endl;
      o << "TAGS\n";
       for (const auto& tag : s.tags)
        o << tag << std::endl;
      o << "EMOJIS\n";
       for (const auto& emoji : s.emojis)
        o << emoji << std::endl;

      o << "Card:" << "TODO" << "\n" <<
           "Poll:" << "TODO" << "\n";
  return o;
}

virtual std::string postdata() override {
  std::string media_ids{};
  std::string delim{""};

  for (const auto& media_item : media) {
    media_ids += media_item.id;
    delim = ",";
  }

  return (replying_to_id.empty()) ?
  std::string{
    "status="       + content   + "&" +
    "media_ids[]="  + media_ids + "&" +
    "spoiler_text=" + spoiler   + "&" +
    "sensitive="    + std::to_string(sensitive)
  } :
  std::string{
    "status="       + content   + "&" +
    "media_ids[]="    + media_ids + "&" +
    "spoiler_text=" + spoiler   + "&" +
    "sensitive="    + std::to_string(sensitive) + "&" +
    "in_reply_to="  + replying_to_id
  };
}

virtual ~Status() override {}
};


inline std::vector<Tag> ParseTagsFromJSON(nlohmann::json data) {
  std::vector<Tag> tags_v{};

  if (!data.is_null()) {
    for (const auto& tag : data) {
      tags_v.emplace_back(Tag{
        .name = tag["name"],
        .url  = tag["url"]
      });
    }
  }

  return tags_v;
}

inline std::vector<Mention> ParseMentionsFromJSON(nlohmann::json data) {
  std::vector<Mention> mentions{};

  if (!data.is_null()) {
    for (const auto& mention : data) {
      mentions.emplace_back(Mention{
        .acct     = mention["acct"],
        .id       = mention["id"],
        .url      = mention["url"],
        .username = mention["username"]
      });
    }
  }

  return mentions;
}

inline Media ParseMediaFromJSON(nlohmann::json data) {
  Media media{};

  if (!data.is_null() && data.is_object()) {
    media.id                 = GetJSONStringValue(data, "id");
    media.type               = GetJSONStringValue(data, "type");
    media.url                = GetJSONStringValue(data, "url");
    media.preview_url        = GetJSONStringValue(data, "preview_url");
    media.remote_url         = GetJSONStringValue(data, "remote_url");
    media.preview_remote_url = GetJSONStringValue(data, "preview_remote_url");
    media.text_url           = GetJSONStringValue(data, "text_url");
    media.description        = GetJSONStringValue(data, "description");
    media.blurhash           = GetJSONStringValue(data, "blurhash");
    media.meta               = MediaMetadata{};

    if (data["meta"].contains("original")) {
      auto original = data["meta"]["original"];

      media.meta.original = MetaDetails{
        .width    = original["width"],
        .height   = original["height"],
        .size     = original["size"],
        .aspect   = original["aspect"]
      };
    }

    if (data["meta"].contains("small")) {
      auto small = data["meta"]["small"];

      media.meta.small = MetaDetails{
        .width    = small["width"],
        .height   = small["height"],
        .size     = small["size"],
        .aspect   = small["aspect"]
      };
    }
  }

  return media;
}

inline std::vector<Media> ParseMediaFromJSONArr(nlohmann::json data) {
  std::vector<Media> media_v{};

  if (!data.is_null() && data.is_array()) {
    for (const auto& item : data) {
      media_v.emplace_back(ParseMediaFromJSON(item));
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

  if (!data.is_null() && data.is_object()) {
    status.id                  = std::stoul(data["id"].get<std::string>());
    status.created_at          = GetJSONStringValue    (data, "created_at");
    status.replying_to_id      = GetJSONStringValue    (data, "in_reply_to");
    status.replying_to_account = GetJSONStringValue    (data, "in_reply_to_account_id");
    status.sensitive           = GetJSONBoolValue      (data, "sensitive");
    status.spoiler             = GetJSONStringValue    (data, "spoiler_text");
    status.visibility          = GetJSONStringValue    (data, "visibility");
    status.language            = GetJSONStringValue    (data, "language");
    status.uri                 = GetJSONStringValue    (data, "uri");
    status.url                 = GetJSONStringValue    (data, "url");
    status.replies             = GetJSONValue<uint32_t>(data, "replies_count");
    status.reblogs             = GetJSONValue<uint32_t>(data, "reblogs_count");
    status.favourites          = GetJSONValue<uint32_t>(data, "favourites_count");
    status.content             = GetJSONStringValue    (data, "content");
    status.reblog              = ParseAccountFromJSON  (data["reblog"]);;
    status.application.name    = GetJSONStringValue    (data["application"], "name");
    status.application.url     = GetJSONStringValue    (data["application"], "website");
    status.account             = ParseAccountFromJSON  (data["account"]);
    status.media               = ParseMediaFromJSONArr (data["media_attachments"]);
    status.mentions            = ParseMentionsFromJSON (data["mentions"]);
    status.tags                = ParseTagsFromJSON     (data["tags"]);
    status.emojis              = GetJSONValue<std::vector<std::string>>(data, "emojis");
  }

  return status;
}

inline std::vector<Status> JSONToStatuses(nlohmann::json data) {
  std::vector<Status> statuses{};

  for (const auto& status_data : data) {
    statuses.emplace_back(JSONToStatus(status_data));
  }

  return statuses;
}

#endif // __TYPES_HPP__
