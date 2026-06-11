# Tracked Compiler Bugs

This directory logs active compiler bugs encountered during the exploration of experimental C++26 features.

## Bug 124264: C++20 Modules + C++26 Contracts Linker Failure

- **Tracker Link:** [GCC Bugzilla #124264](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=124264)
- **Status:** Open / Active (As of GCC 16.1 Experimental)
- **Discovered By:** Denys Kachalenko (February 2026)

### Description

When an `inline` function or a class member function containing a `pre()` or `post()` contract is defined inside a C++20 Module Interface, the compiler generates a local `COMDAT` section for that function in the consumer translation unit.

The compiler-generated contract wrapper `__tu_has_violation` is incorrectly marked with **internal linkage** inside the module's compiled object file (`.o`). When the consumer tries to resolve the inline function call, the linker fails because it cannot access the private internal symbol from across the file boundary.

### How to Reproduce

1. Compile the module interface:
   ```bash
   g++ -std=c++26 -fmodules -fcontracts -fcontract-evaluation-semantic=enforce -c bug_124264_mod.cppm -o bug_124264_mod.o
   ```

2. Compile the consumer main application:
   ```bash
   g++ -std=c++26 -fmodules -fcontracts -fcontract-evaluation-semantic=enforce -c bug_124264_main.cpp -o bug_124264_main.o
   ```

3. Attempt to link the objects together:
   ```bash
   g++ bug_124264_mod.o bug_124264_main.o -o bug_test
   ```

### Expected Linker Failure

```
/usr/bin/ld: bug_124264_main.o: in function `Box::operator[](int) const':
bug_124264_main.cpp:(.text...): undefined reference to `__tu_has_violation(...)'
collect2: error: ld returned 1 exit status
```

### Workarounds

- **Remove inline:** Forcing the function to be a strong symbol instead of an inline duplicate forces the code evaluation to happen inside the module's translation unit space, completely avoiding cross-file linkage conflicts.

- **Use Templates:** Template functions are instantiated directly inside the consumer's translation unit, forcing the local emission of a fresh `__tu_has_violation` helper that links cleanly.

## Known Runtime Issue: `std::terminate()` and `std::abort()` cause Segfaults

- **Tracked Under:** GCC Contracts Implementation Master Tracker (Internal Runtime/ABI)
- **Observed In:** GCC 16.1 Experimental

### Description

Inside the `handle_contract_violation` callback, invoking standard process termination utilities like `std::terminate()` or `std::abort()` triggers a Segmentation Fault (`SIGSEGV`) instead of a clean exit. This happens because the experimental compiler-generated contract wrapper `__tu_has_violation` does not cleanly align the stack frame or generate the necessary ABI metadata. When the standard library tries to inspect the stack during a termination sequence, it encounters an invalid state.

### Workaround

Use `std::_Exit(EXIT_FAILURE);`. Unlike `std::terminate` or `std::exit`, `std::_Exit` immediately relinquishes control back to the operating system without performing stack cleanup, calling destructors, or flushing standard streams, bypassing the corrupted stack metadata entirely.

## Bug 2: Internal Compiler Error (ICE) via Lambda Contract Captures

- **Status:** Brand New / Unreported (Encountered in GCC 16.1 Experimental, June 2026)
- **Crash Point:** `gimplify.cc:3426` in `gimplify_var_or_parm_decl`

### Description

Placing a C++26 `pre()` or `post()` contract on a C++11 lambda expression that captures variables from the surrounding local scope by reference (or value) triggers an Internal Compiler Error (ICE). The GCC middle-end fails to properly resolve captured variables within the compiler-synthesized contract validation block during the Gimplification phase.

### Code to Reproduce

```cpp
#include <contracts>

int main() {
    int limit = 10;
    auto dynamic_check = [&limit](int value) pre(value < limit) {};
    dynamic_check(5);
}
```

### Compiler Output

```
bug_prone_lambdas.cpp: In lambda function:
bug_prone_lambdas.cpp:6:19: internal compiler error: in gimplify_var_or_parm_decl, at gimplify.cc:3426
    6 |          pre(value < limit)
      |               ~~~~~~^~~~~~~
Please submit a full bug report, with preprocessed source.
```

## Bug 3: Compile-Time Contract Enforcement Defect

- **Status:** Language Specification Non-Compliance (Observed in GCC 16.1 Experimental, June 2026)

### Description

Contracts evaluated in constant expressions are downgraded to a warning instead of producing a hard compile error. According to the C++26 specification (P2900), a contract predicate that evaluates to `false` during a constant evaluation context must cause compilation to fail. Currently GCC 16.1 only issues a `warning: contract predicate is false in constant expression` and proceeds to emit a binary.

### 3.1 constexpr Bypass

A `constexpr` function with a violating contract compiles successfully if the function body does not contain an independent compile-time error.

```cpp
#include <print>
#include <contracts>

constexpr int fake_divisor(int a, int b)
    pre(b != 0)
{
    return a;
}

int main() {
    constexpr int compile_test = fake_divisor(10, 0);
    std::println("Value: {}", compile_test);
}
```

```
$ g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe bug_constexpr_compliance.cpp
bug_constexpr_compliance.cpp:5:5: warning: contract predicate is false in constant expression

$ ./a.out
Value: 10
```

### 3.2 consteval Bypass

An immediate function (`consteval`) with a violating contract also compiles, subverting the core guarantee that `consteval` produces a hard compile error on invalid inputs.

```cpp
#include <contracts>

consteval int compile_only_square(int x)
    pre(x > 0)
{
    return x * x;
}

int main() {
    constexpr int val = compile_only_square(-5);
}
```

```
$ g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe bug_consteval_bypass.cpp
bug_consteval_bypass.cpp:4:5: warning: contract predicate is false in constant expression

$ ./a.out
Value: 25
```

## Bug 4: Parser Failure on Variadic Fold Expressions in Generic Lambdas

- **Status:** Incomplete Template/Contract Synthesis (Observed in GCC 16.1 Experimental, June 2026)
- **Type:** Compiler Front-End / Parsing Defect

### Description

When a C++26 contract contains a C++17 unary or binary fold expression (such as `(args >= 0 && ...)`) operating on a modern C++20 generic lambda forwarding reference pack (`auto&&...`), the compiler fails to parse the expression.

The contract engine incorrectly attempts to apply a top-level `const` qualifier to the deduced reference types, violating core C++ type rules and completely corrupting the AST parse tree for the fold expansion.

### Code to Reproduce

```cpp
#include <contracts>

int main() {
    auto check_all_positive = [](auto&&... args)
        pre((args >= 0 && ...))
    {};
    check_all_positive(1, 2, 3);
}
```

### Compiler Output

```
bug_probe_variadic.cpp: In lambda function:
bug_probe_variadic.cpp:5:14: error: 'const' qualifiers cannot be applied to 'auto:11&&'
    5 |         pre((args >= 0 && ...))
      |              ^~~~
bug_probe_variadic.cpp:5:19: error: binary expression in operand of fold-expression
```

## Bug 5: Eager Template Evaluation and Concept/SFINAE Bypass in Contracts

- **Status:** Incomplete Template Integration (Observed in GCC 16.1 Experimental, June 2026)
- **Type:** Language Specification Non-Compliance / Substitution Failure

### Description

When a contract predicate contains a C++20 `requires` expression or ternary compile-time condition meant to guard type safety during template instantiation, the contract engine fails to honor the short-circuiting rules. It eagerly evaluates branches that should be discarded by substitution safety rules, causing invalid code paths to trigger hard compilation errors.

### Code to Reproduce

```cpp
#include <contracts>

template<typename T>
void process_data(T obj)
    pre(requires { obj.nested_value; } ? obj.nested_value > 0 : true)
{}

struct EmptyStruct {};

int main() {
    EmptyStruct e;
    process_data(e); // Should compile cleanly by evaluating the ternary path to 'true'
}
```

### Compiler Output

```
bug_probe_concepts.cpp: In instantiation of 'void process_data(T) [with T = EmptyStruct]':
bug_probe_concepts.cpp:22:46: error: 'const struct EmptyStruct' has no member named 'nested_value'
   22 |     pre(requires { obj.nested_value; } ? obj.nested_value > 0 : true)
      |
```

## Bug 6: Source-Location Line Table Underflow / Lambda Capture Amnesia

- **Status:** Serious Frontend/Diagnostic Corruption (Observed in GCC 16.1 Experimental, June 2026)
- **Type:** Internal Metadata Corruption & Scope-Resolution Failure

### Description

When a `contract_assert` evaluates variables captured across multiple nested generic lambda boundaries--specifically involving structured bindings unpacked in an outer scope--the contract scope-resolution mechanics fail to resolve implicit lambda reference tracking.

Crucially, the lookup failure completely corrupts the compiler's internal diagnostic line-number table mapping, forcing the compiler to output garbage negative line numbers (`-18826752`) in its error diagnostics.

### Code to Reproduce

```cpp
#include <contracts>
#include <tuple>

int main() {
    auto data = std::make_tuple(10, -20);
    auto [x, y] = data;

    auto outer_lambda = [&](auto factor) {
        auto inner_lambda = [&](auto offset) {
            contract_assert((x * factor) + y + offset > 0);
        };
        inner_lambda(5);
    };
    outer_lambda(2);
}
```


## Bug 7: Spaceship Operator (`<=>`) Auto Return Type Deduction Loop

- **Status:** Type System Pipeline Failure (Observed in GCC 16.1 Experimental, June 2026)
- **Type:** Core Language Feature Interference / Premature Type Evaluation

### Description

When a contract precondition (`pre`) is placed on a three-way comparison operator (`operator<=>`) that utilizes automated return type deduction (`auto`), and the contract predicate invokes a dependent comparison operator (like `<`), the compiler fails to lazily deduce the return type.

It attempts to resolve the dependent comparison using the spaceship operator before it has evaluated the function body's return statements, resulting in a premature utilization error of an undeduced placeholder.

### Code to Reproduce

```cpp
#include <contracts>
#include <compare>

struct CustomNumber {
    int val;

    auto operator<=>(const CustomNumber& other) const
        pre((*this) < other || other.val > 0)
    {
        return val <=> other.val;
    }
};

int main() {
    CustomNumber a{-5}, b{-10};
    auto res = (a <=> b);
}
```
