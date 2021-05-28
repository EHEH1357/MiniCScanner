// MiniCScanner.c : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Scanner.h"

FILE* sourceFile;						// miniC source program
FILE* outputFile;						// output file

extern char* tokenName[];

void icgError(int n);

int main(int argc, char *argv[])
{

	char fileName[30] = "", buffer[30] = "";
	struct tokenType token;
	int errorNo = 0;
	char *fileNamePtr = NULL, *context = NULL;
	char *a = "TokenName", *b = "TokenNumber", *c = "TokenValue";

	printf(" *** start of Mini C Compiler\n");
	if (argc != 2) {
		icgError(1);
		exit(1);
	}
	strncpy_s(fileName, sizeof(fileName), argv[1], sizeof(fileName)-1);
	printf("   * source file name: %30s\n", fileName);

	if (fopen_s(&sourceFile, fileName, "r") != 0) {
		icgError(2); 
		exit(1);
	}
	
	strncpy_s(buffer, sizeof(buffer), fileName, sizeof(fileName) - 1);
	fileNamePtr = strtok_s(buffer, ".", &context);
	strcat_s(buffer, sizeof(buffer), ".out");
	errorNo = fopen_s(&outputFile, buffer, "w");
	if (errorNo != 0) {

		fclose(sourceFile);

		icgError(2);
		exit(1);
	}

	// Test code for scanner
	printf(" === start of Scanner\n");
	printf("\nToken %10s | %3s | %12s\n\n", a, b, c);

	do {
		token = scanner(sourceFile);

		// write on console
		
		if (tconst <= token.number && token.number <= twhile) {
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
		}
		else if (tchar <= token.number && token.number <= tcase) {
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
		}
		else if (token.number == tident) {
			printf("Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.id);
		}
		else if (token.number == tnumber) {
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.num);
		}
		else if (token.number == tschar) {	
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.id);
		}
		else if (token.number == tsstring) {
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.id);
		}
		else if (token.number == tsreal_num) {
			printf("Token %10s ( %3d, %12.3lf )\n", tokenName[token.number], token.number, token.value.real);
		}
		else if (tnot <= token.number && token.number < teof) {	
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
		}
		else if (tlbrace <= token.number && token.number <= trbrace) {
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
		}
		else if (token.number == teof) {
			printf("Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
		}
		
		writeToken(token, outputFile);

	} while (token.number != teof);
	/*
	printf(" === start of Parser\n");
	root = parser();
	printTree(root, 0);
	printf(" === start of ICG\n");
	codeGen(root);
	*/
	printf(" *** end   of Mini C Compiler\n");

	fclose(sourceFile);
	fclose(outputFile);


}


void icgError(int n)
{
	printf("icg_error: %d\n", n);
	//3:printf("A Mini C Source file must be specified.!!!\n");
	//"error in DCL_SPEC"
	//"error in DCL_ITEM"
}

