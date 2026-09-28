// tests/p2/test_p2.cpp
//
// YOUR test suite goes here. At least 12 assert-based test cases — see
// spec §5 for the required categories and the sample test for the
// expected level of rigor.
//
// This file is a stub so the project builds out of the box; replace the
// body of main() with your own tests.

#include "core/conversation.h"
#include "core/message.h"
#include "core/sentinel_scanner.h"
#include "harness/harness.h"
#include "model/replay_client.h"
#include "model/scripted_client.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#define TEST(name) static void name()


TEST(DEEPCOPY) {
    Conversation a;
    a.append(Message(Role::User, "hello"));
    a.append(Message(Role::Assistant, "hi there"));

    Conversation b(a);
    assert(a.begin() != b.begin());
    assert(b.size() == 2);
    assert(b.at(0).content() == "hello");
    assert(b.at(1).content() == "hi there");
/////////////////////////////////////////////////////chek for if the copy is deep
    a.append(Message(Role::User, "more"));
    assert(a.size() == 3);
    assert(b.size() == 2);
    assert(b.at(1).content() == "hi there");
}
TEST(DEEPMOVE) { //autofilled from vs, provied eith my deepcopy function 
    Conversation a;
    a.append(Message(Role::User, "hello"));
    a.append(Message(Role::Assistant, "hi there"));
const Message* original_data = a.begin(); //buffer locale
    Conversation b(std::move(a));

    assert(b.begin() == original);
    assert(a.begin() == a.end()); //convo a is empty

    assert(a.begin() == nullptr);
    assert(a.size() == 0);
    assert(b.size() == 2);
    assert(b.at(0).content() == "hello");
    assert(b.at(1).content() == "hi there");
}


//copied from specs example loop, not my code
TEST(ScannerCatchesSentinelAtEveryBoundary) { //copied
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye." + sentinel;
    for (std::size_t split = 0; split <= text.size(); ++split) {
        SentinelScanner scanner(sentinel);
        auto out1 = scanner.feed(text.substr(0, split));
        auto out2 = scanner.feed(text.substr(split));
        assert((out1.sentinel_found || out2.sentinel_found) &&
               "sentinel must be caught regardless of split point");
        assert(out1.safe_text + out2.safe_text == "Goodbye.");
    }
}

TEST(CHECKBOUNDS) {
    Conversation conversation;
    assert(conversation.size() == 0);
    aasert(conversation.begin() == conversation.end());//empry convo check
    bool empty_access_threw = false;
    try {
        (void)conversation.at(0);
    } catch (const std::out_of_range&) {
        empty_access_threw = true;
    }
    assert(empty_access_threw);

    conversation.append(Message(Role::User, "first"));
    conversation.append(Message(Role::Assistant, "last"));
    assert(conversation.at(0).content() == "first");
    assert(conversation.at(1).content() == "last");

 bool size_access_threw = false;
    try {
        (void)conversation.at(conversation.size());
    } catch (const std::out_of_range&) {
        size_access_threw = true;
    }
    assert(size_access_threw);

bool past_end_access_threw = false;
    try {
        (void)conversation.at(conversation.size() + 1);
    } catch (const std::out_of_range&) {
        past_end_access_threw = true;
    }
    assert(past_end_access_threw);
}

int main() {
    DEEPCOPY(); 
    DEEPMOVE();
    ScannerCatchesSentinelAtEveryBoundary();
    CHECKBOUNDS();
    
    std::cout << "all tests passed\n"; //generic pass message, not my code 
    return 0;
}
