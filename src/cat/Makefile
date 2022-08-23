CC = gcc
C_FLAGS = -Wall -Werror -Wextra -std=c11 -pedantic -g

s21_cat: s21_cat.c s21_cat.h
	$(CC) $(C_FLAGS) -o s21_cat s21_cat.c

test_cat:
	sh ./test_cat.sh

rebuild: clean s21_cat

cpplint:
	python3 ../../materials/linters/cpplint.py s21_cat.*

clean:
	rm -f s21_cat