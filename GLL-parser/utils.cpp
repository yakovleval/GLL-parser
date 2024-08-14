#include "utils.h"

bool is_terminal(char ch) {
	return isdigit(ch) || ispunct(ch) || islower(ch);
}