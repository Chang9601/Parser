#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "parser.h"

static void _parser(char *input)
{
	Data data;

	while (parseData(&data, &input)) {
		printData(data);
		if (data.type == DATA_STRING || data.type == DATA_LIST)
			freeData(data);
		printf("\n");
	}
}

static void _test_parseInteger()
{
	// OK
	char *input1 = "123";
	_parser(input1);

	char *input2 = "0";
	_parser(input2);

	char *input3 = "-942";
	_parser(input3);

	// NO
	char *input4 = "-";
	_parser(input4);

	char *input5 = "King634";
	_parser(input5);

	char *input6 = "-Wow42";
	_parser(input6);
}

static void _test_parseString()
{
	// OK
	char *input1 = "\"123\"";
	_parser(input1);

	char *input2 = "\"Turing\"";
	_parser(input2);

	char *input3 = "\"Whoa\\!\"";
	_parser(input3);

	// NO
	char *input4 = "\"Wrong";
	_parser(input4);

	char *input5 = "\"Who is this\n\"";
	_parser(input5);

	char *input6 = "Again... Wrong!!!\"";
	_parser(input6);
}

static void _test_parseList()
{
	// OK
	char *input1 = "[]";
	_parser(input1);

	char *input2 = "[1, 2, 3 ,     4]";
	_parser(input2);

	char *input3 = " [[1,2,3], [[1]]]  \"abc\\z\" 123 \"SFASFXV\" 456 [1, \"\", \"ABC\", -34, \"XVS\",       20]\"sxvz\" []  [[35],     1,  [\"asf\", 1, 2],  \"ABSF\"]";
	_parser(input3);

	// NO
	char *input4 = "[,]";
	_parser(input4);
	
	char *input5 = "[,\"Syntax error\",]";
	_parser(input5);

	char *input6 = "[\"Another syntax error!!\"";
	_parser(input6);

	char *input7 = "[";
	_parser(input7);

	char *input8 = "]";
	_parser(input8);

	char *input9 = "[[1, 2, 3], , [1]]";
	_parser(input9);

	char *input10 = "[[\"HI!\", \"My\"] \"Nmas\"]";
	_parser(input10);
}

int main(int argc, char *argv[])
{
	_test_parseInteger();
	_test_parseString();
	_test_parseList();

	return EXIT_SUCCESS;
}
