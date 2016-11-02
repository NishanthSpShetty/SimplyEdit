#ifndef EDITOT_H
#define EDITOR_H

#define CHAR_PER_LINE 80
#define DEFAULT_FILE "newfile"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct line_t {
	char lines[CHAR_PER_LINE];
	int nchar;
};

//syntax colors
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//function declarations

int getch(); //-
char copy(); //-
char cut(); //-
void paste();//-
void deleteline(); //-
void cutline(); //-
void insert(); //-
int flushtofile();
void push(char); //-
void pop(); //-
void movefront(); //-
void moveback(); //-
void moveup(); //-
void movedown();//-
void replace();//-
void delete();//-
int cmdmode();//-
int prev();
void edit(); //-

#endif
