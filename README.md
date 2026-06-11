# C++26 Contracts

Minimal examples exploring the C++26 contract programming feature.

## Examples

| File | Description |
|------|-------------|
| `example_0.cpp` | FixedCapacityBuffer with pre/post conditions |
| `example_1.cpp` | safe_divide and trim_spaces with contracts |
| `example_2.cpp` | Runtime contract_assert with vectors |
| `example_3.cpp` | Predicate side-effect demonstration |

## Requirements

- **GCC 16.1** (or later) with contracts support

## Compilation

All examples use the same flags:

```bash
g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe example_0.cpp
g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe example_1.cpp
g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe example_2.cpp
g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe example_3.cpp
```

## Tracked Bugs

See [bugs/](bugs/) for known compiler issues encountered while testing.

## Resources

- [C++ Contracts Reference](https://cppreference.com/cpp/language/contracts)
