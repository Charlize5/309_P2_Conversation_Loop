# Design Log — Project 2

(500–800 words total. See spec §5 for what each section must cover.)
//A 500–800 word Markdown document defending your design. It must cover:
* Your growth factor choice and the proof of amortized $O(1)$ insertions.
* Evidence of how your code handles the Rule of Five safely.
* The mathematical argument proving your pending-buffer never exceeds the sentinel length.
* One thing you would design differently in hindsight.


## Growth factor and amortized cost
I used a growth facrot of 2. Its just a simple even number. My growth test resizes when the size flies past a power of 2. This allocates a new array with the exact same elements, and uses O(k) where k is num_elements. 

My test will fail if the factor relative to n is not reallocated sequentially along powers of 2: (i &(i -1)) ==0

## Rule of Five evidence

My test copy function proves that memory buffers were allocated when created an aribtarily large script. the b conversation was entirely unchanged from a, meaning that it was properly deep copied. the test passes whith the assertion verification that container b remains entirely unchanged and isolated,  a valid deep copy.

My deep move fuction chckss that moving a to b correctly moves the pointer with const message* original = a.begin. When a drops, its destructor runs without impacting b. 

## Sentinel scanner: bounded pending_ proof

To prove that the pending size is bounded, I feed chunks of strings into it. If text.find(sentinel_) != std::string::npos, the code calls pending_.clear(). The new buffer size drops to 0, which satisfies 0 ≤ hold.

if size > {hold}, the code creates an offset safe_len =size. It then removes the tail and compares the nginning and ending bounds. 

## What I would change differently
I think I went down a huge rabbit hole for the sentinel test questions. I had to add a lot of includes and helper classes for the manual scripts, and I had to rebuild the code multiple times just to get it to compile. I think I would avoid a complicated script structure loop, and probably just hardcode all of the requirements with things private to the function call. It may be longer code, but it will be much simpler to follow and check if its working properly. I needed to do a lot of researcg into cpp syntax libraries. 

Also - 80% of my time was spent attempting to connect, reconnect, or disconnect wsl, and git. 
I had a lot of frustrating errors, mainyl because I kept pasting my old repository in the https link for the git fetch. When I commited my changes, it would overwrite everything I did, and replace it with my old code from project 1.

