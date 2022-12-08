#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

static Node *_insertNode(Node *head, Data *data);
static void _deallocList(Node *head);

// 정수를 10진수로 파싱
// 참: "123", "0", "-942", "-5230absf"
// 거짓: "-", "ADVA542", "-gbcb42"
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

	// 정수가 0일 경우
	if (sign > 0 && integer == 0) {
		*value = 0;	
	// 입력이 숫자가 아닌 문자를 포함하는 경우
	} else if ((sign < 0 && (origin + 1) == *input) || !isDigit) {
		result = false;
		*input = origin;
	// 입력이 정수 리터럴을 포함하는 경우
	} else {
		*value = integer * sign;	
	}

	result = result && isDigit;
	
	return result;
}

// 문자열 파싱
// 문자열은 반드시 큰 따옴표 안에 존재
// 문자열은 개행 문자('\n')를 포함하지 않음
// 함수는 이스케이프 코드를 파싱하지 않음
// 참: "\"513\"", "\"Turing\"", "\"Whoa!!\"", "\"This\\is\""
// 거짓: "\"Saa", "\"My name is\n\"" "No left double quote\""
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
		// 끝을 표시하는 널 문자 삽입
		*((*value) + len) = '\0';
		// 입력을 닫는 큰 따옴표 뒤의 문자로 설정
		(*input)++;
	} else {
		if(!isDoubleQuote)
			*input = origin;
	}

	return result;
}

// 리스트 파싱
// 리스트는 반드시 꺾쇠 괄호 안에 존재
// 리스트는 0개 이상의 파싱 가능한 데이터 (정수, 문자열, 리스트) 포함
// 참: "[]", "[1, 2]", "[1, \"Wall\", 3]", [[], "\"Snow\"", [1, 2, [4, 5]]]"
// 거짓: "412[]", "[3, 5, ,,]", "[", "]]" , "]"
bool parseList(Node **head, char **input)
{
	*head = NULL;
	bool result = true;	
	// 재귀 파싱의 결과
	bool subResult = true;
	bool isSquareBracket = (**input == '[') ? true : false;
	bool noSyntaxError = true;
	bool isPrevComma = true;	
	size_t numData = 0;
	size_t numComma = 0;
			
	(*input)++;

	// Data 구조체로 값 저장
	Data data;
	while (isSquareBracket && (**input) && (**input != ']') && (**input != '\0')) {

		while (isspace(**input))
			(*input)++;
		
		// 만약 현재 값과 이전 값이 둘 다 ','일 경우 구문 오류
		if ((**input) == ',') {
			if (isPrevComma) {	
				noSyntaxError = false;
				break;
			}
			isPrevComma = true;
			numComma++;
			(*input)++;
		}
		
		// 데이터 타입이 정수
		if (isdigit(**input) || (**input == '-')) {
			data.type = DATA_INTEGER;
			parseInteger(&(data.integer), input);
			*head = _insertNode(*head, &data);
			numData++;
			isPrevComma = false;
			
		// 데이터 타입이 문자열
		} else if (**input == '\"') {
			data.type = DATA_STRING;
			parseString(&(data.string), input);	
			*head = _insertNode(*head, &data);
			freeData(data);
			numData++;
			isPrevComma = false;
		
		// 데이터 타입이 문자열
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
	// 데이터의 개수가 두 개보다 적으면, 쉼표의 개수는 반드시 0
	// 그렇지 않을 경우 쉼표의 개수는 반드시 데이터의 개수보다 하나 적음
	if (numData == 0 || numData == 1) {
		result = result && (numComma == 0);
	} else {
		result = result && ((numData - 1) == numComma);
	}

	if (!result)
		_deallocList(*head);

	return result;
}

// 타입을 기준으로 데이터 파싱 (정수, 문자열, 리스트)
bool parseData(Data *data, char **input)
{
	bool result;

	// 여백 스킵
	while(isspace(**input))
		(*input)++;
	
	// 정수
	if (isdigit(**input) || (**input) == '-') {
		data -> type = DATA_INTEGER;
		result = parseInteger(&(data -> integer), input);
	// 문자열
	} else if (**input == '\"') {
		data -> type = DATA_STRING;
		result = parseString(&(data -> string), input);
	// 리스트
	} else if (**input == '[') {
		data -> type = DATA_LIST;
		result = parseList(&(data -> list), input);
	} else {
		result = false;
	}

	return result;
}

// 파싱된 데이터 출력
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

// 데이터의 타입이 문자열 혹은 리스트이면 데이터의 메모리를 해제
void freeData(Data data)
{
	if (data.type == DATA_STRING) {
		free(data.string);
	} else {
		while (data.list) {
			Node *next = data.list -> next;
			if(data.list -> data -> type == DATA_STRING)
				free(data.list -> data -> string);
			// 재귀적으로 리스트의 메모리를 해제
			if(data.list -> data -> type == DATA_LIST)
				freeData(*(data.list -> data));
			// Data 구조체의 필드를 먼저 해제
			free(data.list -> data);
			// 리스트 해제
			free(data.list);
			data.list = next;
		}
	}
}

// 잘못된 형식의 리스트를 해제하는 도움 함수
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

// 노드를 할당하는 도움 함수
static Node *_insertNode(Node *head, Data *data)
{
	Node *node = calloc(1, sizeof(*node));
	node -> data = calloc(1, sizeof(*(node -> data)));
	node -> data -> type = data -> type;
	node -> next = NULL;	

	// 데이터 타입이 정수	
	if (data -> type == DATA_INTEGER) {
		node -> data -> integer = data -> integer;
	// 데이터 타입이 문자열
	} else if (data -> type == DATA_STRING) {
		node -> data -> string = calloc(1, strlen(data -> string) + 1);
		strncpy(node -> data -> string, data -> string, strlen(data -> string));
		node -> data -> string[strlen(data -> string)] = '\0';
	// 데이터 타입이 리스트
	} else {
		// 리스트 초기화
		node -> data -> list = NULL;
		for(Node *curr = data -> list; curr != NULL; curr = curr -> next)
			node -> data -> list = _insertNode(node -> data -> list, curr -> data);	
	}	

	// 빈 리스트일 경우
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