#include <contracts>

int main() {
    auto check_all_positive = [](auto&&... args)
        pre((args >= 0 && ...))
    {};
    check_all_positive(1, 2, 3);
}
