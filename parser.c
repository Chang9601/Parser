#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

bool parseNumber(int *value, char **input)
{
	bool result = true;
	int sign = ((**input) == '-') ? -1 : 1;
	int num = ((**input) != '-') ? ((**input) - '0') : 0;
	bool isDigit = num >= 0 && num <= 9;

	char *origin = *input;
	(*input)++;
	
	while(isdigit(**input)) {
		num = 10 * num + (**input - '0');
		(*input)++;
	}

	// Handle 0
	if(sign > 0 && num == 0) {
		*value = 0;	
	} else if((sign < 0 && (origin + 1) == *input) || !isDigit) {
		// Run into an non-digit character	
		result = false;
		*input = origin;
	} else {
		// Properly formatted integer literal
		*value = num * sign;	
	}

	result = result && isDigit;
	
	return result;
}

