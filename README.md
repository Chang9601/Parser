# 문자열 파서

## 소개
문자열에 저장된 데이터를 자료형 (정수, 문자열, 리스트)에 맞게 파싱한다.

## 목적
* 문자열 처리 능력을 향상시킨다.
* 구조체, 공용체, 열거형이 포함된 자료구조를 활용한다.
* 복잡한 동적 메모리 할당과 해제를 처리한다.
* 리스트를 다루는 재귀 알고리즘을 구현한다.

## 조건
* 정수의 경우 10진수로 표현된 정수만 올바른 정수이다. 
	* 올바른 형태: "123", "0", "-6342"
	* 올바르지 못한 형태: "-", "Num123", "-N52"
* 문자열의 경우 쌍따옴표 안에 포함되어 있으며 (단, 개행 문자('\n')는 제외) 이스케이프 문자는 파싱하지 않는다. 
	* 올바른 형태: "\"111\"", "\"Computer!!\"", "\"The\\cat\""
	* 올바르지 못한 형태: "\"Wrong!!", "\"Who is\n that?\"" "No left double quote\"")
* 리스트의 경우 꺾쇠괄호에 포함된 0개 이상의 파싱 가능한 자료형 (정수, 문자열, 리스트)을 포함하는 경우 파싱한다. 
	* 올바른 형태: "[]", "[1]", "[1, 3], "[2, \"WllS\", 8]", [[], "\"Edwardd\"", [6, 7, [1, 5]]]"
	* 올바르지 못한 형태: "32[]", "[3, 5, ,,]", "[", "]]" , "]", "]]", "ABC")
  
## 기능
조건을 만족하는 정수, 문자열, 리스트 파싱

## 개발 환경
* Linux (Ubuntu 20.04.3 LTS)
* C (gcc 9.3.0)
* gdb 9.2
* Valgrind 3.15.0

## 제작 기간
2022.10.05 - 2022.10.15
