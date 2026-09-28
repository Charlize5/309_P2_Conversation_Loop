// include/core/message.h
// to match the provided files^^
#pragma once //included in the other headers(?)
#include <string>

enum class Role { System, User, Assistant };
//given for memory leaks
class Message {
public:
    // Default-constructs an empty System message with empty content.
    // Needed so Conversation can allocate raw array slots before
    // append() fills them in.
    Message() : role_(Role::System), content_("") {} //passed by value. no copies necessary

    Message(Role role, std::string content)
    : role_(role), content_(std::move(content)) {}
//defning meddage in header fiel
    Role               role()    const noexcept {return role_;}  // Who sent this message.
    const std::string& content() const noexcept {return content_;}  // The message text.
//editing this to make it a defition 
private:
//the roles shouldnt be called
    Role        role_;
    std::string content_;
}; 