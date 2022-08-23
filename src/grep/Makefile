CC = gcc
C_FLAGS = -Wall -Werror -Wextra -std=c11 -pedantic -g

s21_grep: s21_grep.c s21_grep.h
	$(CC) $(C_FLAGS) -o s21_grep s21_grep.c

test_grep:
	sh ./test_grep.sh

rebuild: clean s21_grep

cpplint:
	python3 ../../materials/linters/cpplint.py s21_grep.*

clean:
	rm -f 21_grep