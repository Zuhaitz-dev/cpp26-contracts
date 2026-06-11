#include <print>
#include <vector>
#include <algorithm>
#include <contracts>
#include <cstdlib>

double calculate_average(const std::vector<int>& numbers) 
{
    contract_assert(!numbers.empty());

    double sum = std::ranges::fold_left(numbers, 0.0, std::plus<>{});
    double avg = sum / numbers.size();

    contract_assert(avg >= 0.0);
    return avg;
}

void handle_contract_violation(const std::contracts::contract_violation& violation)
{
    std::println(stderr, "[ASSERTION FAILED] {}", violation.comment());
    std::_Exit(EXIT_FAILURE);
}

int main()
{
	std::vector<int> empty_dataset;
	double crash = calculate_average(empty_dataset);
}

