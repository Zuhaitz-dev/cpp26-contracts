#include <print>
#include <array>
#include <contracts>
#include <cstdlib>
#include <stacktrace>
// #include <exception>

template <typename T, size_t Capacity>
class FixedCapacityBuffer
{
    static_assert(Capacity > 0, "Buffer capacity must be greater than zero.");
 private:
    std::array<T, Capacity> data{};
    size_t current_size = 0;

 public:
    FixedCapacityBuffer() = default;

    void push(int value)
        pre(!is_full())
    {
        data[current_size] = value;
        current_size++;
    }

    T pop()
        pre(!is_empty())
        post(r: current_size < Capacity)
    {
        current_size--;
        return data[current_size];
    }

    bool is_full() const
    {
        return current_size == Capacity;
    }

    bool is_empty() const 
    {
        return current_size == 0;
    }

    size_t size() const 
    {
        return current_size;
    }

    constexpr size_t max_capacity() const 
    {
        return Capacity;
    }
};

void handle_contract_violation(const std::contracts::contract_violation& violation)
{
    const char *kind_str = "Unknown";
    switch (violation.kind())
    {
        case std::contracts::assertion_kind::pre:       kind_str = "Precondition"; break;
        case std::contracts::assertion_kind::post:      kind_str = "Postcondition"; break;
        case std::contracts::assertion_kind::assert:    kind_str = "Assert"; break;
    }

    std::println(stderr, "\n[BUG DETECTED] -> Contract broken.");
    std::println(stderr, "Type:      {}", kind_str);
    std::println(stderr, "Location:  {}:{}", violation.location().file_name(), violation.location().line());
    std::println(stderr, "Condition: {}\n", violation.comment());
    std::println(stderr, "Backtrace:");
    std::println(stderr, "{}", std::stacktrace::current());

    // std::terminate() and std::abort() segfault in GCC 16.1 (see bugs/)
    std::_Exit(EXIT_FAILURE);
}

int main()
{
    FixedCapacityBuffer<int, 2> buffer;
    buffer.push(10);
    buffer.push(20);
    buffer.push(30);
}
