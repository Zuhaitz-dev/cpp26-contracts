#include <print>
#include <string_view>
#include <string>
#include <contracts>
#include <cstdlib>

int safe_divide(const int numerator, int denominator)
    pre(denominator != 0)
    post(res: numerator == 0 ? res == 0 : true)
{
    return numerator / denominator;
}

std::string trim_spaces(const std::string_view input)
    post(out: out.length() <= input.length())
{
    std::string result{input};
    result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
    result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);
    return result;
}

void handle_contract_violation(const std::contracts::contract_violation& violation)
{
	std::println(stderr, "[VIOLATION] Line: {} | Condition: {}",
			violation.location().line(), violation.comment());
	std::_Exit(EXIT_FAILURE);
}

int main()
{
	auto cleaned = trim_spaces("  Hello Contracts!  ");
	std::println("Result: '{}'", cleaned);

	int broken = safe_divide(10, 0);
}

