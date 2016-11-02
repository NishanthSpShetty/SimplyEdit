OUT := editor
CC := gcc
OBJ_DIR := ./obj
SRC_DIR := ./src
OBJS := editor.o main.o


all: $(OBJS) obj
	gcc -o $(OUT) $(OBJ_DIR)/*.o
obj:
	test -d $(OBJ_DIR) || mkdir  $(OBJ_DIR)
main.o: lexer.o $(SRC_DIR)/main.c $(SRC_DIR)/editor.h
	gcc -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o $(OBJ_DIR)/lexer.o -ll

editor.o: $(SRC_DIR)/editor.h $(SRC_DIR)/editor.c 
	gcc -c $(SRC_DIR)/editor.c -o $(OBJ_DIR)/editor.o

lexer.o: $(SRC_DIR)/CSyntax.l $(SRC_DIR)/CSyntax.h
	flex -o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/CSyntax.l 
	gcc -c $(SRC_DIR)/lex.yy.c -o $(OBJ_DIR)/lexer.o 

clean:
	rm $(OBJ_DIR)/*.o
