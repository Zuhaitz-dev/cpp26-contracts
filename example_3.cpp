#include <print>
#include <contracts>
#include <cstdlib>

class Account 
{
 public:
    mutable int balance = 100;	// Or whatever else but this works.

    bool bad_predicate() const
    {
	balance -= 10;
	return balance >= 0;
    }

    void process_transaction()
    	pre(bad_predicate())
    {
    	std::println("Transaction processed. Inner balance: {}", balance);
    }
};

void handle_contract_violation(const std::contracts::contract_violation& violation)
{
    std::println(stderr, "Violated: {}", violation.comment());
    std::_Exit(EXIT_FAILURE);	
}

int main()
{
    Account acc;
    std::println("Starting balance: {}", acc.balance);

    acc.process_transaction();

    // If compiled with -fcontract-semantic=enforce (or =observe), balance becomes 90 because predicate runs.
    // With -fcontract-semantic=ignore, balance remains 100 because predicate is skipped.
    std::println("Ending balance: {}", acc.balance);
}
