#include "core/sentinel_scanner.h"
#include <utility>

SentinelScanner::SentinelScanner(std::string sentinel) 
    : sentinel_(std::move(sentinel)) {}

SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {
    std::string text = pending_;
    text += chunk;

std::size_t pos = text.find(sentinel_);
    if (pos != std::string::npos) {
        pending_.clear();
        return { text.substr(0, pos), true }; 
    }

 std::size_t hold = sentinel_.size() - 1;

    // nothing is safe yet, check
if (text.size() <= hold) {
        pending_ = text;
        return { "", false };
    }

    
    std::size_t safe_len = text.size() - hold; 
    
  
pending_ = text.substr(safe_len); 

    return { text.substr(0, safe_len), false };
}

SentinelScanner::Out SentinelScanner::flush() {
    SentinelScanner::Out out{ std::move(pending_), false };
    pending_.clear();
    return out;
}
