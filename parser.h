#ifndef __PARSER_H__
#define __PARSER_H__

typedef struct _Node {
	struct _Data *data;
	struct _Node *next;
} Node;

typedef struct _Data {
	union {
		int integer;
		char *string;
		Node *list;
	};
	enum { DATA_INTEGER, DATA_STRING, DATA_LIST } type;
} Data;

bool parseNumber(int *value, char **input);
bool parseString(char **value, char **input);
bool parseList(Node *head, char **input);
void freeData(Data data);

#endif
