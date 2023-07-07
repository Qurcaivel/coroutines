# coroutines
Tiny coroutine (cooperative threads) API written in C.

# note

This implementation does not support any compile-time space checks and provides unsafe API functions whose wrong execution position can result in UB.
It also doesn't provide modern coroutines aka generators and implements basic management of cooperative tasks based on Round-robin processing.

# todo

- [x] error-prone API (int/~~result~~ format);
- [ ] protected stack;
- [ ] extendable stack;
- [ ] compile-time space checks.
