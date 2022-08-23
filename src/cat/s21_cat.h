#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <stdio.h>
#include <string.h>

#define TOTAL_NUMBER_OF_FLAGS 12
#define MAX_LENGTH_OF_FLAG 18
#define NMAX 30

enum true_of_false{
    FALSE,
    TRUE
};

static const char possible_flags[TOTAL_NUMBER_OF_FLAGS][MAX_LENGTH_OF_FLAG] = {
    "-b", "--number-nonblank", "-e", "-E",
    "-n", "--number", "-s", "--squeeze-blank", "-t", "-T", "NULL", "-v"
};

typedef struct Flags {
    int b;
    int s;
    int n;
    int e;
    int E;
    int T;
    int t;
    int v;
} Flags;

typedef struct Data {
    // Create an array for all the flags - valid and invalid to sort them out later
    char all_flags_array[NMAX][NMAX];
    // And an array for text-file names - valid and invalid to sort later
    char all_text_files_array[NMAX][NMAX];
    int number_of_files;
    char newline;
    char tabulator;
} Data;

int check_start_conditions(const int argc, char *argv[], Data* data);
void initialize_flags(Flags* flags);
void initialize_data(Data* data);
int parse_flags_and_text_files(const int argc, char *argv[], Data* data);
int check_if_flags_are_valid(const int counter_for_flags, Data const* data);
int check_if_files_exist(const int filename_index, Data const* data);
void pass_flags_to_structure(Flags* flags, Data const* data);
void print_result(Flags const* flags, Data const* data);
void handle_b(const int current_character, const int is_previous_newline, Data const* data, int* ordinal);
void handle_s(const int current_character, int* next_character,
    int is_previous_newline, Data const* data, FILE *file);
void handle_n(int* ordinal);
void handle_e();
void handle_t(int* current_character, int* next_character, FILE *file, Data const* data);
void handle_v(int* current_character, int* next_character, FILE *file, Data const* data);
int is_unprintable(const int current_character, Data const* data);
int is_newline(const int current_character, Data const* data);
int is_tabulator(const int current_character, Data const* data);
void handle_flags(int* current_character, int* next_character,
    int* is_previous_newline, int* ordinal, Data const* data, Flags const* flags, FILE* file);

#endif  // SRC_CAT_S21_CAT_H_
