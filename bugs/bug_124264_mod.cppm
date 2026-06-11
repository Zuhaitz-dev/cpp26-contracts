export module M;

export struct Box 
{
    int data[4] = {10, 20, 30, 40};

    // Inline member function with contract condition.
    constexpr int operator[](int index) const noexcept
        pre(index >= 0 && index < 4)
    {
    	return data[index];
    }
};
