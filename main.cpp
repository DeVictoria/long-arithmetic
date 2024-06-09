#include "LN.h"
#include "return_codes.h"

#include <fstream>
#include <iostream>
#include <stack>
#include <string>

struct Check_error
{
	bool error_parameter_invalid = false;
};

void get_message(int code)
{
	switch (code)
	{
	case 0:
		std::cerr << "The operation completed successfully";
		break;
	case 1:
		std::cerr << "File can't be opened";
		break;
	case 2:
		std::cerr << "Not enough memory, memory allocation failed";
		break;
	case 3:
		std::cerr << "The data is invalid";
		break;
	case 4:
		std::cerr << "The parameter or number of parameters (argv) is incorrect";
		break;
	case 5:
		std::cerr << "Unsupported functionality";
		break;
	case 6:
		std::cerr << "invalid number of command line arguments";
		break;
	default:
		std::cerr << "Unknown error";
	}
}

LN get_pop(std::stack< LN >* stack1, Check_error* Check);

int reverse_Polish_notation(std::stack< LN >* stack, std::ifstream* input, char* order);

int main(int argc, char* argv[])
{
	std::string direct = "direct";
	std::string inverse = "inverse";
	if (argc != 4 || (argv[3] != direct && argv[3] != inverse))
	{
		printf("The parameter or number of parameters (argv) is incorrect\nfirst argument: input file\nsecond "
			   "argument: output file\n");
		return ERROR_PARAMETER_INVALID;
	}
	std::ifstream input(argv[1]);
	if (!input.is_open())
	{
		get_message(ERROR_CANNOT_OPEN_FILE);
		return ERROR_CANNOT_OPEN_FILE;
	}

	std::stack< LN > stack;
	int error_code = reverse_Polish_notation(&stack, &input, argv[3]);
	if (error_code != SUCCESS)
	{
		input.close();
		while (!stack.empty())
			stack.pop();
		get_message(error_code);
		return error_code;
	}

	std::ofstream output(argv[2]);
	if (!output.is_open())
	{
		input.close();
		while (!stack.empty())
			stack.pop();
		get_message(ERROR_CANNOT_OPEN_FILE);
		return ERROR_CANNOT_OPEN_FILE;
	}
	while (!stack.empty())
	{
		output << stack.top().print() << '\n';
		stack.pop();
	}
	input.close();
	output.close();
	return SUCCESS;
}

LN get_pop(std::stack< LN >* stack1, Check_error* Check)
{
	if (stack1->empty())
	{
		stack1->emplace(1);
		Check->error_parameter_invalid = true;
	}
	LN ans = stack1->top();
	stack1->pop();
	return ans;
}

int reverse_Polish_notation(std::stack< LN >* stack, std::ifstream* input, char* order)
{
	Check_error Check;
	std::string direct = "direct";
	try
	{
		std::string line;
		while (std::getline(*input, line))
		{
			switch (line[0])
			{
			case '+':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->push(x + y) : stack->push(y + x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '-':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->push(x - y) : stack->push(y - x);
				}
				else
					stack->emplace(line);
				break;
			}
			case '*':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->push(x * y) : stack->push(y * x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '/':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->push(x / y) : stack->push(y / x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '%':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->push(x % y) : stack->push(y % x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '~':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					stack->push(~x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '_':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					stack->push(-x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '<':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->emplace(x < y) : stack->emplace(y < x);
				}
				else if ((line[1] == '=') & (line.size() == 2))
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->emplace(x <= y) : stack->emplace(y <= x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '>':
			{
				if (line.size() == 1)
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->emplace(x > y) : stack->emplace(y > x);
				}
				else if ((line[1] == '=') & (line.size() == 2))
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					order == direct ? stack->emplace(x >= y) : stack->emplace(y >= x);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '=':
			{
				if ((line[1] == '=') & (line.size() == 2))
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					stack->emplace(x == y);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			case '!':
			{
				if ((line[1] == '=') & (line.size() == 2))
				{
					LN x = get_pop(stack, &Check);
					LN y = get_pop(stack, &Check);
					stack->emplace(x != y);
				}
				else
				{
					return ERROR_UNSUPPORTED;
				}
				break;
			}
			default:
				stack->emplace(line);
				break;
			}
			if (Check.error_parameter_invalid)
			{
				return INVALID_NUMBER_ARGUMENTS;
			}
		}
	} catch (...)
	{
		return ERROR_OUT_OF_MEMORY;
	}
	return SUCCESS;
}
