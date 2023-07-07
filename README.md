# coroutines
Tiny coroutine (cooperative threads) API written in C.

# note

This implementation does not support any compile-time space checks and provides unsafe API functions whose wrong execution position can result in UB.
It also doesn't provides modern coroutines aka generators and implements basic management of cooperative tasks based on Round-robin processing.

# todo

- [ ] error-prone API (int/result format);
- [ ] protected stack;
- [ ] extendable stack;
- [x] runtime space checks;
- [ ] compile-time space checks.
