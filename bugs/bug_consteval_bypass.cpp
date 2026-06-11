#include <contracts>

consteval int compile_only_square(int x)
    pre(x > 0)
{
    return x * x;
}

int main() {
    constexpr int val = compile_only_square(-5);
}
