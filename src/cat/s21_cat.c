// Copyright 2022 finchren

/*

You need to develop a cat utility:

- Support of all flags (including GNU versions):

    1	-b (GNU: --number-nonblank)	numbers only non-empty lines
    2	-e implies -v (GNU only: -E the same, but without implying -v) but also display end-of-line characters as $
    3	-n (GNU: --number)	number all output lines
    4	-s (GNU: --squeeze-blank)	squeeze multiple adjacent blank lines
    5	-t implies -v (GNU: -T the same, but without implying -v)	but also display tabs as ^I

- The source, header, and build files must be placed in the src/cat/ directory
- The resulting executable file must be placed in the directory src/cat/ and named s21_cat

*/

#include "s21_cat.h"

int main(int argc, char *argv[]) {
    // Initialize the value for the Valgrind to not to throw errors at me
    Flags flags = {0};
    initialize_flags(&flags);
    Data data = {0};
    initialize_data(&data);
    if (check_start_conditions(argc, argv, &data)) {
        pass_flags_to_structure(&flags, &data);
        print_result(&flags, &data);
    } else {
        // Error message, when entered flags are invalid
        fprintf(stderr, "Flags were not valid \n");
    }
    return 0;
}

void print_result(Flags const* flags, Data const* data) {
    for (int index_for_files = 0; index_for_files < data->number_of_files; ++index_for_files) {
        // is_previous_newline is set to TRUE to work around the begging of the file
        if (check_if_files_exist(index_for_files, data)) {
            int is_previous_newline = TRUE, ordinal = 1;
            int current_character = 0, next_character = 0;
            FILE *file = fopen(data->all_text_files_array[index_for_files], "r");
            next_character = fgetc(file);
            current_character = next_character;
            next_character = fgetc(file);
            while (current_character != EOF) {
                handle_flags(&current_character, &next_character,
                    &is_previous_newline, &ordinal, data, flags, file);
            }
            fclose(file);
        } else {
            // Error message when a file doesn't exist
            fprintf(stderr, "File doesn't exist");
        }
    }
}

void handle_flags(int* current_character, int* next_character,
    int* is_previous_newline, int* ordinal, Data const* data, Flags const* flags, FILE* file) {

    if (flags->b && *is_previous_newline) {
        handle_b(*current_character, *is_previous_newline, data, ordinal);
    }
    if (flags->s && is_newline(*current_character, data) && *is_previous_newline) {
        handle_s(*current_character, next_character, *is_previous_newline, data, file);
    }
    if (flags->n && !flags->b && *is_previous_newline) {
        handle_n(ordinal);
    }
    if ((flags->e && is_newline(*current_character, data)) ||
        (flags->e && is_unprintable(*current_character, data))) {
        if (flags->e && is_newline(*current_character, data)) {
            handle_e();
        }
        if (is_unprintable(*current_character, data)) {
            handle_v(current_character, next_character, file, data);
        }
    }
    if (flags->E && is_newline(*current_character, data)) {
        handle_e();
    }
    if (flags->T && is_tabulator(*current_character, data)) {
        handle_t(current_character, next_character, file, data);
    }
    if ((flags->t && is_tabulator(*current_character, data)) ||
        (flags->t && is_unprintable(*current_character, data))) {
        handle_t(current_character, next_character, file, data);
        if (is_unprintable(*current_character, data)) {
            handle_v(current_character, next_character, file, data);
        }
    }
    if (flags->v && is_unprintable(*current_character, data)) {
        handle_v(current_character, next_character, file, data);
    }
    // Follwing 2 if-else cases used to print out regular characters
    // when there are no T or t flags and the current char isn't \t
    // Otherwise the tabs get eaten.
    // Also these conditions are used to not print regular tabs when -t or -T flags are active
    if ((!flags->t || !flags->T) && !is_tabulator(*current_character, data)) {
        printf("%c", *current_character);
    }
    if ((!flags->t && !flags->T) && is_tabulator(*current_character, data)) {
        printf("%c", *current_character);
    }
    *is_previous_newline = *current_character == '\n' ? TRUE : FALSE;
    *current_character = *next_character;
    *next_character = fgetc(file);
}

int is_tabulator(const int current_character, Data const* data) {
    int is_tabulator = FALSE;
    if (current_character == data->tabulator) {
        is_tabulator = TRUE;
    }
    return is_tabulator;
}

int is_newline(const int current_character, Data const* data) {
    int is_newline = FALSE;
    if (current_character == data->newline) {
        is_newline = TRUE;
    }
    return is_newline;
}

int is_unprintable(const int current_character, Data const* data) {
    int is_unprintable = FALSE;
    if (((current_character >= 0 && current_character < 31) &&
        !is_newline(current_character, data) &&
        !is_tabulator(current_character, data)) || current_character == 127) {
        is_unprintable = TRUE;
    }
    return is_unprintable;
}

void handle_v(int* current_character, int* next_character, FILE *file, Data const* data) {
    while (is_unprintable(*current_character, data)) {
        if (*current_character == 127) {
            *current_character = '?';
        } else {
            *current_character += 64;
        }
        printf("^%c", *current_character);
        if (is_newline(*current_character, data) || *next_character == EOF) {
            return;
        }
        *current_character = *next_character;
        *next_character = fgetc(file);
    }
}

void handle_t(int* current_character, int* next_character, FILE *file, Data const* data) {
    while (is_tabulator(*current_character, data) || is_tabulator(*next_character, data)) {
        printf("^I");
        // Used to work around the case when the current char is tab and next is newline
        // If there is no this condtition the newline will get printed
        // and shifted (other conditions wouldn't get checked)
        // For example -e flag
        if (!is_tabulator(*next_character, data)) {
            return;
        }
        *current_character = *next_character;
        *next_character = fgetc(file);
    }
}

void handle_e() {
    printf("$");
}

void handle_n(int* ordinal) {
    printf("%6d\t", *ordinal);
    ++*ordinal;
}

void handle_s(const int current_character, int* next_character,
    int is_previous_newline, Data const* data, FILE *file) {
    while (is_newline(current_character, data) && is_previous_newline && is_newline(*next_character, data)) {
        is_previous_newline = current_character == '\n' ? TRUE : FALSE;
        *next_character = fgetc(file);
    }
}

void handle_b(const int current_character, const int is_previous_newline, Data const* data, int* ordinal) {
    if (is_previous_newline && !is_newline(current_character, data)) {
        printf("%6d\t", *ordinal);
        ++*ordinal;
    }
}

int check_start_conditions(const int argc, char *argv[], Data* data) {
    int conditions = FALSE;
    // Condition to check if there are flags and text-files
    // Parse flags() should return TRUE or FALSE if the flags are valid or not
    // And the files exist
    if (argc >= 2 && parse_flags_and_text_files(argc, argv, data)) {
        conditions = TRUE;
    }
    return conditions;
}

void pass_flags_to_structure(Flags* flags, Data const* data) {
    int index = 0;
    // Until it is the end of the two-dimensional array,
    // I iterate over it and pass the values to the Flags structure
    // strcmp returns 0 if the values are equal, so I use !(not) to invert the value of zero to true
    while (strcmp(data->all_flags_array[index], "\0")) {
        if (!strcmp(data->all_flags_array[index], "-b") ||
            !strcmp(data->all_flags_array[index], "--number-nonblank")) {
            flags->b = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-e")) {
            flags->e = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-E")) {
            flags->E = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-n") ||
            !strcmp(data->all_flags_array[index], "--number")) {
            flags->n = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-s") ||
            !strcmp(data->all_flags_array[index], "--squeeze-blank")) {
            flags->s = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-t")) {
            flags->t = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-T")) {
            flags->T = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-v")) {
            flags->v = TRUE;
        }
        ++index;
    }
}

int parse_flags_and_text_files(const int argc, char *argv[], Data* data) {
    int is_valid_input = FALSE, letter_index = 0;
    // Counters to know when to stop in iterating over the arrays
    int counter_for_flags = 0;
    // Iterate over the argv to sort out flags from files
    for (int element_index = 1; element_index < argc; ++element_index) {
        if (argv[element_index][letter_index] == '-') {
            strcpy(data->all_flags_array[counter_for_flags], argv[element_index]);
            ++counter_for_flags;
        } else {
            strcpy(data->all_text_files_array[data->number_of_files], argv[element_index]);
            ++data->number_of_files;
        }
    }
    if (check_if_flags_are_valid(counter_for_flags, data)) {
            is_valid_input = TRUE;
    }
    return is_valid_input;
}

int check_if_files_exist(const int filename_index, Data const* data) {
    int files_exist = TRUE;
    FILE *file = NULL;
    if ((file = fopen(data->all_text_files_array[filename_index], "r")) == NULL) {
        files_exist = FALSE;
    } else {
        fclose(file);
    }
    return files_exist;
}

int check_if_flags_are_valid(const int counter_for_flags, Data const* data) {
    int flags_are_valid = FALSE, index_all_flags = 0, number_of_valid_flags = 0;
    while (index_all_flags != counter_for_flags) {
        for (int index_possible_flags = 0;
        index_possible_flags < TOTAL_NUMBER_OF_FLAGS; ++index_possible_flags) {
            if (!strcmp(data->all_flags_array[index_all_flags], possible_flags[index_possible_flags])) {
                ++number_of_valid_flags;
            }
        }
        ++index_all_flags;
    }
    if (counter_for_flags == number_of_valid_flags) {
        flags_are_valid = TRUE;
    }
    return flags_are_valid;
}

void initialize_flags(Flags* flags) {
    flags->b = FALSE;
    flags->s = FALSE;
    flags->n = FALSE;
    flags->e = FALSE;
    flags->E = FALSE;
    flags->T = FALSE;
    flags->t = FALSE;
    flags->v = FALSE;
}

void initialize_data(Data* data) {
    memset(data->all_flags_array, '\0', sizeof(char));
    memset(data->all_text_files_array, '\0', sizeof(char));
    data->number_of_files = 0;
    data->newline = '\n';
    data->tabulator = '\t';
}
