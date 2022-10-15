#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

static Node *_insertNode(Node *head, Data *data);
static void _deallocList(Node *head);

// Parse an integer in decimal notation.
// true: "123", "0", "-942", "-5230absf"
// false: "-", "ADVA542", "-gbcb42"
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

	// If an input equals 0.
	if (sign > 0 && integer == 0) {
		*value = 0;	
	// If an input contains non-digit characters.
	} else if ((sign < 0 && (origin + 1) == *input) || !isDigit) {
		result = false;
		*input = origin;
	// If an input contains integer literal.
	} else {
		*value = integer * sign;	
	}

	result = result && isDigit;
	
	return result;
}

// Parse a string.
// A string must be enclosed in double quotes. ("")
// A string does not contain a new line character. ('\n')
// The function does not parse escape codes.
// true: "\"513\"", "\"Turing\"", "\"Whoa!!\"", "\"This\\is\""
// false: "\"Saa", "\"My name is\n\"" "No left double quote\""
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
// A list must be enclosed in square brackers ('[]')
// A list contain 0 or more parsable data. (integer, string, list)
// true: "[]", "[1, 2]", "[1, \"Wall\", 3]", [[], "\"Snow\"", [1, 2, [4, 5]]]"
// false: "412[]", "[3, 5, ,,]", "[", "]]" , "]"
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
	size_t numComma = 0;
			
	(*input)++;

	// Use a Data type to store a value.
	Data data;
	while (isSquareBracket && (**input) && (**input != ']') && (**input != '\0')) {

		while (isspace(**input))
			(*input)++;
		
		// If both a current value and a previous value are ',', syntax error
		if ((**input) == ',') {
			if (isPrevComma) {	
				noSyntaxError = false;
				break;
			}
			isPrevComma = true;
			numComma++;
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
			parseString(&(data.string), input);	
			*head = _insertNode(*head, &data);
			freeData(data);
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
	}

	result = (**input == ']') ? true : false;
	if (result)
		(*input)++;
	result = result && isSquareBracket && subResult && noSyntaxError;
	// If the number of data is less than 2, the number of comma must be 0.
	// Otherwise, it must be one less than the number of data.
	if (numData == 0 || numData == 1) {
		result = result && (numComma == 0);
	} else {
		result = result && ((numData - 1) == numComma);
	}

	if (!result)
		_deallocList(*head);

	return result;
}

// Parse data based on type. (integer, string, list)
bool parseData(Data *data, char **input)
{
	bool result;

	// Skip whitespaces.
	while(isspace(**input))
		(*input)++;
	
	// Integer
	if (isdigit(**input) || (**input) == '-') {
		data -> type = DATA_INTEGER;
		result = parseInteger(&(data -> integer), input);
	// String
	} else if (**input == '\"') {
		data -> type = DATA_STRING;
		result = parseString(&(data -> string), input);
	// List
	} else if (**input == '[') {
		data -> type = DATA_LIST;
		result = parseList(&(data -> list), input);
	} else {
		result = false;
	}

	return result;
}

// Print parsed data.
void printData(Data data)
{
	if (data.type == DATA_INTEGER) {
		printf("%d", data.integer);
	} else if (data.type == DATA_STRING) {
		printf("\"%s\"", data.string);
	} else {
		printf("[");
		for(Node *curr = data.list; curr != NULL; curr = curr -> next) {
			printData(*(curr -> data));
			if(curr -> next)
				printf(", ");
		}
		printf("]");
	}
}

// Deallocate the memory of the data if its type is either a string or a list.
void freeData(Data data)
{
	if (data.type == DATA_STRING) {
		free(data.string);
	} else {
		while (data.list) {
			Node *next = data.list -> next;
			if(data.list -> data -> type == DATA_STRING)
				free(data.list -> data -> string);
			// Recursively deallocate memory of lists.
			if(data.list -> data -> type == DATA_LIST)
				freeData(*(data.list -> data));
			// Deallocate Data field first.
			free(data.list -> data);
			// Deallocate the list.
			free(data.list);
			data.list = next;
		}
	}
}

// Helper function for deallocating a incorrectly formatted list.
static void _deallocList(Node *head)
{
	while (head) {
		Node *next = head -> next;
		if (head -> data -> type == DATA_STRING)
			free(head -> data -> string);
		if (head -> data -> type == DATA_LIST)
			_deallocList(head -> data -> list);
		free(head -> data);
		free(head);
		head = next;
	}
}

// Helper function for allocating a node.
static Node *_insertNode(Node *head, Data *data)
{
	Node *node = calloc(1, sizeof(*node));
	node -> data = calloc(1, sizeof(*(node -> data)));
	node -> data -> type = data -> type;
	node -> next = NULL;	

	// If the type of data is integer.	
	if (data -> type == DATA_INTEGER) {
		node -> data -> integer = data -> integer;
	// If the type of data is string.
	} else if (data -> type == DATA_STRING) {
		node -> data -> string = calloc(1, strlen(data -> string) + 1);
		strncpy(node -> data -> string, data -> string, strlen(data -> string));
		node -> data -> string[strlen(data -> string)] = '\0';
	// If the type of data is list.
	} else {
		// Initialize a list.
		node -> data -> list = NULL;
		for(Node *curr = data -> list; curr != NULL; curr = curr -> next)
			node -> data -> list = _insertNode(node -> data -> list, curr -> data);	
	}	

	// If it is an empty list.
	if (head == NULL) {
		head = node;
	} else {
		Node *curr = head;
		while(curr -> next != NULL)
			curr = curr -> next;
		curr -> next = node;
	}

	return head;
}
