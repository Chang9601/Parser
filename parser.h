#ifndef __PARSER_H__
#define __PARSER_H__

typedef struct _Node {
	/* 포인터의 크기는 동일하기 때문에 전방 선언 가능 */
	struct _Data *data;
	struct _Node *next;
} Node;

typedef struct _Data {
	union {
		int integer;
		char *string;
		Node *list;
	};
	/* type은 자료형이 아니라 변수명 */
	enum { DATA_INTEGER, DATA_STRING, DATA_LIST } type;
} Data;

bool parseInteger(int *value, char **input);
bool parseString(char **value, char **input);
bool parseList(Node **head, char **input);
bool parseData(Data *data, char **input);
void printData(Data data);
void freeData(Data data);

#endif