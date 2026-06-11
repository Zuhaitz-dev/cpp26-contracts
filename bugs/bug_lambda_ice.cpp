#include <contracts>

int main() {
    int limit = 10;
    auto dynamic_check = [&limit](int value) pre(value < limit) {};
    dynamic_check(5);
}
