# Repository Coding Standards & Architectural Rules
* **Context**: This repository is a high-performance game engine fork utilizing C++26. Strictly enforce the following standards and principles during your review:

## 1. Language Standard (C++26 Priority):
* **Rule**: Assume the codebase compiles with full C++26 support.
* **Rule**: Strongly encourage the use of modern C++23 and C++26 features where applicable (e.g., advanced constexpr, std::expected, monadic optional operations, views, and text formatting updates).
* **Rule**: If any older pattern can be modernized using C++23/26, explicitly point it out.

## 2. Toolchain & Hardening Environment (CRITICAL):
* **Rule**: The project is compiled and statically linked with a hardened libc++ using the "fast" hardening profile.
* **Rule**: All standard containers (like std::vector, std::string) and classes perform internal bounds checking, iterator validation, and basic nullptr checks by default, triggering a secure `__builtin_trap()` on violation.
* **Rule**: DO NOT flag or request manual defensive code checks (such as replacing `operator[]` with `.at()`, or adding explicit `if (ptr == nullptr)` validations before passing to std classes) unless it is vital for complex business logic. Assume the hardened standard library reliably guarantees boundaries and traps safely.

## 3. C++ Core Guidelines:
* **Rule**: Strictly enforce the C++ Core Guidelines (e.g., resource management, type safety).
* **Rule**: EXCEPTION: In the event of a conflict between older C++ Core Guidelines recommendations (like avoiding operator[]) and our modern C++23/C++26 hardened toolchain environment, always prefer the hardened C++23/C++26 approach.

## 4. Zero-Overhead Principle & Game Engine Performance:
* **Rule**: Every abstraction must follow the zero-overhead principle.
* **Rule**: Flag unnecessary heap allocations, cache-unfriendly data structures, and excessive dynamic dispatch (virtual calls) in performance-critical code paths.
* **Rule**: Watch out for redundant copies, lack of std::move where applicable, and pass-by-value of heavy objects.

## 5. Safety & Robustness:
* **Rule**: Look for core memory safety issues, undefined behavior, uninitialized variables, and race conditions that hardening cannot catch.
* **Rule**: Prioritize compile-time checks (static_assert, concepts) over runtime validation where possible.
