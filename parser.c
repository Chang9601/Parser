#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// Parse an integer in decimal notation.
// e.g. 123, 0, -942
bool parseInteger(int *value, char **input)
{
	bool result = true;
	int sign = ((**input) == '-') ? -1 : 1;
	int integer = ((**input) != '-') ? ((**input) - '0') : 0;
	bool isDigit = integer >= 0 && integer <= 9;

	char *origin = *input;
	(*input)++;
	
	while (isdigit(**input)) {
		integer = 10 * integer + (**input - '0');
		(*input)++;
	}

	// Process a case where input = 0
	if (sign > 0 && integer == 0) {
		*value = 0;	
	} else if ((sign < 0 && (origin + 1) == *input) || !isDigit) {
		// Run into an non-digit character	
		result = false;
		*input = origin;
	} else {
		// Properly formatted integer literal
		*value = integer * sign;	
	}

	result = result && isDigit;
	
	return result;
}

// Parse a string.
// Strings are enclosed in double quotes.
// Strings do not contain a new line character.
// e.g. "513", "Turing", "Whoa!!!"
bool parseString(char **value, char **input)
{
	bool result = true;	
	bool isDoubleQuote = ((**input) == '\"') ? true : false;
	size_t len = 0;
	
	char *origin = *input;
	(*input)++;

	while ((**input) && (**input != '\n') && ((**input) != '\"')) {
		len++;	
		(*input)++;
	}	
	
	result = ((**input) == '\"') ? true : false;
	result = result && isDoubleQuote;

	if (result) {
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

// Parse a list.
// Lists are enclosed in square brackers ('[]')
// Lists contain other types of parsable data (integer, string, list)
// e.g. [], [1, 2], [1, "Wall", 3], [[], "Snow", [1, 2, [4, 5]]]
bool parseList(Node **head, char **input)
{
	*head = NULL;
	bool result = true;	
	// Result of recusive parsing
	bool subResult = true;
	bool isSquareBracket = (**input == '[') ? true : false;
	bool noSyntaxError = true;
	bool isPrevComma = true;	
	size_t numData = 0;
	size_t numCommas = 0;
			
	char *origin = *input;
	(*input)++;

	Data data;
	while ((**input) && (**input != ']')) {

		while (isspace(**input))
			(*input)++;
		
		if ((**input) == ',') {
			// If a previous value is ',', syntax error
			if (isPrevComma) {	
				noSyntaxError = false;
				break;
			}
			isPrevComm = true;
			numCommas++;
			(*input)++;
		}
		
		// If a data type is integer.
		if (isdigit(**input) || (**input == '-')) {
			data.type = DATA_INTEGER;
			parseInteger(&(data.integer), input);
			*head = _insertNode(*head, &data);
			numData++;
			isPrevComma = false;
			
		// If a data typee is string.
		} else if (**input == '\"') {
			data.type = DATA_STRING;
			parseSring(&(data.string), input);	
			*head = _insertNode(*head, &data);
			freeData(Data);
			numData++;
			isPrevComma = false;
		
		// If a data type is a list.
		} else if (**input == '[') {
			data.type = DATA_LIST;
			subResult = parseList(&(data.list), input);	
			if (subResult) {
				*head = _insertNode(*head, &data);
				numData++;
				isPrevComma = false;
				freeData(data);
			}
		}

		//(*input)++;
	}
}
