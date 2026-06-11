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
