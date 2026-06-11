#include <contracts>

template<typename T>
void process_data(T obj)
    pre(requires { obj.nested_value; } ? obj.nested_value > 0 : true)
{}

struct EmptyStruct {};

int main() {
    EmptyStruct e;
    process_data(e);
}
