#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// Parse an integer in decimal notation.
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

	// Process a case where input = 0
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

// Parse a string
bool parseString(char **value, char **input)
{
	bool result = true;	
	bool isDoubleQuote = ((**input) == '\"') ? true : false;
	size_t len = 0;
	
	char *origin = *input;
	(*input)++;

	while((**input) && (**input != '\n') && ((**input) != '\"')) {
		len++;	
		(*input)++;
	}	
	
	result = ((**input) == '\"') ? true : false;
	result = result && isDoubleQuote;

	if(result) {
		*value = calloc(len + 1, sizeof(**value));
		strncpy(*value, origin + 1, len);
		// Insert a null terminator to mark the end.
		*((*value) + len) = '\0';
		// Set *input to the next character after the last double quotation mark.
		(*input)++;
	} else {
		if(!isDoubleQuote)
			*input = origin;
	}

	return result;
}
