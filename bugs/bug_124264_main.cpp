import M;
#include <print>
#include <contracts>
#include <cstdlib>

void handle_contract_violation(const std::contracts::contract_violation& violation)
{
    std::println(stderr, "Contract broken: {}", violation.comment());
    std::_Exit(EXIT_FAILURE);
}

int main()
{
    Box b;

    /* Invoking the inline function triggers the compiler to 
     * inject a reference to an internal-linkage __tu_has_violation 
     * symbol inside this TU, causing a link error.
     */
    std::println("Value: {}", b[0]);
}
