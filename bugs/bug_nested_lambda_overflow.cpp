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
