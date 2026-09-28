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

#include <utility> //no rules aganst this
#include <cassert>
#include <fstream> //when vs adds ofstream
#include <memory>// i dont know what this is but wsl yelled at me until i added it
#include <stdexcept>
#include <string>
#include <iostream>//i need this for my sentinel tests, i added manual scropts 

#define TEST(name) static void name()

TEST(GROWTHCHECK)
{
    Conversation c;

const std::size_t N = 45; //arbitrary 
    
 for (std::size_t i = 0; i < N; ++i) {
c.append(Message(Role::User, "msg" + std::to_string(i)));
assert(c.size() == i + 1);
     // the earlyur messages should not alter
    for (std::size_t j = 0; j <= i; ++j) {
     assert(c.at(j).content() == "msg" + std::to_string(j)); //check for random shit
        }
}
assert(c.end() - c.begin() == N); //chekc the edges 
}

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

    assert(b.begin() == original_data);
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
    assert(conversation.begin() == conversation.end());//empry convo check
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

TEST(CLEANSCANNER) {
 const std::string sentinel = "<|end_conversation|>";
const std::string text = "Piplup:D.";

    // one char at a time, then clean
    for (std::size_t chunk = 1; chunk <= text.size(); ++chunk) {
        SentinelScanner scanner(sentinel);
        std::string released;
 for (std::size_t i = 0; i < text.size(); i += chunk) {
            auto out = scanner.feed(text.substr(i, chunk));
            assert(!out.sentinel_found);
            released += out.safe_text;
        }
        auto tail = scanner.flush();  //clean
        assert(!tail.sentinel_found);
        released += tail.safe_text;
        assert(released == text);     // check clean
    }
}

TEST(SYSTEMROLECHECK) {
   Conversation c;
    c.append(Message(Role::System, "pinned"));       // the system message goes in FIRST
    for (std::size_t i = 0; i < 100; ++i) {
        c.append(Message(Role::User, "msg" + std::to_string(i)));
    }
 assert(c.size() == 101);
    assert(c.begin()->role() == Role::System);
    assert(c.at(0).role() == Role::System);
  assert(c.at(0).content() == "pinned");
    for (std::size_t j = 1; j < c.size(); ++j) {
assert(c.at(j).role() != Role::System); //role checl
    }
}

TEST(SCANNERFALSEALARM) {
    const std::string sentinel = "<|end_conversation|>";
    const std::string false_alarm[] = {
        "<|end_world|>",                      
        "<|end_conversation|",                     // missin final > ?
        "<|end_conversation|<|end_conversation|",  
        "<|<|end_<|end_conv",                      
    }; 
    for (const std::string& text : false_alarm) {
        for (std::size_t piece = 1; piece <= text.size(); ++piece) {
            SentinelScanner scanner(sentinel);
            std::string released;
            for (std::size_t i = 0; i < text.size(); i += piece) {
                auto out = scanner.feed(text.substr(i, piece));
                assert(!out.sentinel_found);   // no false alarm??
                released += out.safe_text;
            }
     auto tail = scanner.flush(); //clean
    assert(!tail.sentinel_found);
    released += tail.safe_text;
     assert(released == text);  //compare this 
        }
    }

}


TEST (SANNERBOUNDS) { //vs autofill scanner(sentinel), but the test passed
    const std::string sentinel = "<|end_conversation|>";
    const std::size_t bound = sentinel.size() - 1;


    {
        SentinelScanner scanner(sentinel); //vs autofil, it picked an arbitrary size, but the test passed
        const std::string unit = "<|end_"; 
        std::size_t fed = 0;
        std::size_t released = 0;
        for (int i = 0; i < 20000; ++i) { //arbitrary number of iterations, but the test passed
            auto out = scanner.feed(unit);
            assert(!out.sentinel_found);
            fed += unit.size();
            released += out.safe_text.size();
            assert(fed - released <= bound);
        }
        released += scanner.flush().safe_text.size();
        assert(fed == released);  //clean
    }

SentinelScanner scanner(sentinel);
        const std::string unit = "<|end_conversation|";  //> check from previosu
        std::size_t fed = 0;
        std::size_t released = 0;
        for (int i = 0; i < 1000; ++i) {
            for (char ch : unit) {
                auto out = scanner.feed(std::string(1, ch));
                assert(!out.sentinel_found);
            fed += 1;
                released += out.safe_text.size();
                assert(fed - released <= bound);
            }
        }
        released += scanner.flush().safe_text.size();
        assert(fed == released);
    }

TEST(TURNLIMIT){ 
    // **Harness (Turn Limit):** Confirm the provided loop stops with `TurnLimit` when your `Conversation` is used underneath it.
   // 5 lines of piplup
    class FakeInput : public InputSource { //new helper class to test
    public:
        std::string read_line() override {
            if (remaining_ <= 0) {
                eof_ = true;
                return "";
            }
            --remaining_;
            return "piplup";
        }
        bool is_eof() const override { return eof_; }

    private:
        int remaining_ = 5;
        bool eof_ = false;//no need to call the counter
    };
    class CaptureOutput : public OutputSink { //auofill???
    public:
        void write(std::string_view text) override { captured += text; }
        std::string captured;
    };

    const std::string path = "tmp_turnlimit.script"; //autofill?? 
    {// write a script with 4 turns of piplup
        std::ofstream f(path);
        f << "role: assistant\none\n---\n"
             "role: assistant\ntwo\n---\n"
             "role: assistant\nthree\n---\n"
             "role: assistant\nfour\n"; //??test passed when I implemented the vs script
}
 HarnessConfig cfg;
 cfg.max_turns = 3;
    Harness harness(std::make_unique<ScriptedModelClient>(path), cfg);
    std::remove(path.c_str());//already loaded this script

    FakeInput in;
    CaptureOutput out;
    StopReason reason = harness.run(in, out); //

    assert(reason.kind == StopReason::Kind::TurnLimit);
    assert(harness.conversation().size() == 6); 
}
TEST(SENTINELHALT) {
    class FakeInput : public InputSource {
    public:
        std::string read_line() override {
            if (remaining_ <= 0) {
                eof_ = true;
                return "";
            }
            --remaining_;
            return "piplup";
        }
        bool is_eof() const override { return eof_; }

    private:
        int remaining_ = 5;
        bool eof_ = false;
    };
    class CaptureOutput : public OutputSink {
    public:
        void write(std::string_view text) override { captured += text; }
        std::string captured;
    };

    const std::string sentinel = "<|end_conversation|>";
    const std::string path = "tmp_sentinel.script";
    {
      //chunck the sentinen; //script copied from vs, but the test passed
        std::ofstream f(path);
        f << "chunk: 3\nrole: assistant\nBye now." << sentinel << " ignored\n"
          << "---\n"
          << "role: assistant\nnever reached\n";
    }

    HarnessConfig cfg;
    cfg.max_turns = 10; //
    Harness harness(std::make_unique<ScriptedModelClient>(path), cfg);
    std::remove(path.c_str());

    FakeInput in; 
    CaptureOutput out;
    StopReason reason = harness.run(in, out); //lol autofull, but the test passed

    assert(reason.kind == StopReason::Kind::Sentinel);

    assert(out.captured.find(sentinel) == std::string::npos);
    assert(out.captured.find("Bye now.") != std::string::npos);
    assert(out.captured.find("ignored") == std::string::npos);
    assert(out.captured.find("never reached") == std::string::npos);


    const Conversation& conv = harness.conversation();
    assert(conv.size() == 2);
    assert(conv.at(1).content() == "Bye now." + sentinel);
}

TEST(ROUNDTRIP) { 
    const std::string sentinel = "<|end_conversation|>";

    Conversation original;
    original.append(Message(Role::System, "piplup."));
    original.append(Message(Role::User, "hi"));
    original.append(Message(Role::Assistant, "piplup."));
    original.append(Message(Role::User, "bye"));
    original.append(Message(Role::Assistant, "Goodbye." + sentinel));

    const std::string path = "tmp_roundtrip.txt"; //same as the other test function
    {
        std::ofstream file(path); //copy from other function scripts
    bool first = true;
        for (const Message* m = original.begin(); m != original.end(); ++m) {
            if (!first) file << "---\n";
            first = false;
            file << "role: "
        << (m->role() == Role::System ? "system"
                : m->role() == Role::User ? "user"
                                               : "assistant") << "\n";
         file << m->content() << "\n";
        }
    }

    ReplayModelClient replay(path);
    std::remove(path.c_str());

    assert(replay.system_message() == "piplup.");

    ///replay the conversation and check that the messages match
    Conversation ignored;
    Message r1 = replay.generate(ignored);
    Message r2 = replay.generate(ignored);
    assert(r1.role() == Role::Assistant);
    assert(r1.content() == "piplup.");
    assert(r2.role() == Role::Assistant);
    assert(r2.content() == "Goodbye." + sentinel);


    bool exhausted = false;
    try {
        (void)replay.generate(ignored);
    } catch (const std::runtime_error&) {
        exhausted = true;
    }
    assert(exhausted);
}


int main() { //run all tests, even the vs autofill ones, and print a generic pass message if all tests pass
    DEEPCOPY(); 
    DEEPMOVE();
    GROWTHCHECK();
    ScannerCatchesSentinelAtEveryBoundary();
    CHECKBOUNDS();
    SYSTEMROLECHECK();
    CLEANSCANNER();
    SCANNERFALSEALARM();
    SANNERBOUNDS();
    TURNLIMIT();
    SENTINELHALT();
    ROUNDTRIP();
std::cout << "all tests passed\n"; //generic pass message for the console 
    return 0;
}
