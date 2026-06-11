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
