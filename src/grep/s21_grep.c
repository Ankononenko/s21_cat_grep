// Copyright 2022 finchren
/*

You need to develop the grep utility:

Support of the following flags: -e, -i, -v, -c, -l, -n
Only pcre or regex libraries can be used for regular expressions
The source, header and make files must be placed in the src/grep/ directory
The resulting executable file must be placed in the directory src/grep/ and named s21_grep

*/

#include "s21_grep.h"

int main(int argc, char *argv[]) {
    Flags flags = {0};
    initialize_flags(&flags);
    Data data = {0};
    initialize_data(&data);
    if (check_start_conditions(argc, argv, &flags, &data)) {
        pass_flags_to_structure(&flags, &data);
        print_result(&flags, &data);
    } else {
        print_error_message(&data, "Flags were not valid");
    }
    return 0;
}

void print_result(Flags const* flags, Data* data) {
    const int number_of_files = data->number_of_files;
    // If file with patterns doesn't exist, even if "-s" error is thrown and grep doesn't run
    int should_run = TRUE;
    if (flags->f) {
        should_run = check_if_files_exist(flags, 0, data);
    }
    if (should_run) {
        // If -f then index_for_files++ else index_for_files = 0
        // So I don't open the file with patterns
        int index_for_files = 0;
        for (; index_for_files < number_of_files; ++index_for_files) {
            if (check_if_files_exist(flags, index_for_files, data)) {
                FILE *file = fopen(data->all_filenames_array[index_for_files], "r");
                // Check pattern with no flags, -e pattern and -f pattern
                while (parse_line(file, data)) {
                    int pattern_index = 0;
                    data->line_should_be_printed = FALSE;
                    while (pattern_index < data->pattern_index && !data->line_should_be_printed) {
                        check_pattern(flags, data, pattern_index);
                        ++pattern_index;
                    }
                    if (data->line_should_be_printed) {
                        print_line(flags, data, pattern_index, index_for_files);
                    }
                    check_if_last_newline(data);
                }
                print_result_no_line(flags, data, index_for_files);
                print_newline(flags, data, index_for_files);
                reset_num_values(data);
            } else {
                if (!flags->s) {
                    print_error_message(data, "File doesn't exist");
                }
            }
        }
    }
}

void print_result_no_line(Flags const* flags, Data* data, const int index_for_files) {
    if (flags->c || flags->l) {
        handle_h(flags, &data->filename_should_be_printed);
        if (!flags->h && !flags->l) {
            filenames_should_be_printed(flags, data);
            if (data->filename_should_be_printed) {
                print_filename(index_for_files, data, data->colon);
            }
        }
        if (flags->l && flags->c) {
            filenames_should_be_printed(flags, data);
            if (data->filename_should_be_printed) {
                if (!flags->h) {
                    print_filename(index_for_files, data, data->colon);
                }
            }
            printf("%d\n", data->pattern_found_in_the_file);
        }
        handle_c(flags, data);
        handle_l(flags, index_for_files, data);
    }
}

void print_line(Flags const* flags, Data* data, int pattern_index, int index_for_files) {
    if (!flags->c && !flags->l) {
        filenames_should_be_printed(flags, data);
        if (data->filename_should_be_printed) {
            print_filename(index_for_files, data, data->colon);
        }
        handle_n(flags, data->number_of_the_line);
        handle_o(flags, data, pattern_index/*, index_for_files*/);
        print_line_array(flags, data);
    }
}

void check_if_last_newline(Data* data) {
    data->is_last_newline = data->line_array[0] == '\n' ? TRUE : FALSE;
}

void print_newline(Flags const* flags, Data const* data, const int index_for_files) {
    int last_file_index = data->number_of_files - 1;
    int is_last_file = index_for_files == last_file_index ? TRUE : FALSE;
    if (!flags->c && !flags->l) {
        if (!data->is_last_newline && !is_last_file &&
        data->line_should_be_printed && !data->handle_o_printed_newline) {
            printf("%c", data->newline);
        }
    }
}

void check_pattern(Flags const* flags, Data* data, const int pattern_index) {
    if (flags->i) {
        handle_i(flags, data, pattern_index);
    }
    handle_e(data, pattern_index);
    if (flags->v) {
        handle_v(data);
    }
    if (data->line_should_be_printed) {
        ++data->number_of_matching_lines;
    }
}

void print_error_message(Data const* data, char* error_message) {
    if (!data->error_message_should_be_printed) {
        fprintf(stderr, "%s\n", error_message);
    }
}

void handle_o(Flags const* flags, Data* data, int pattern_index) {
    if (flags->o && !flags->v && !flags->i) {
        for (int index = 0; index < data->pattern_found_in_the_line; ++index) {
            printf("%s\n", data->pattern_array[pattern_index - 1]);
            data->handle_o_printed_newline = TRUE;
        }
    }
    // Here I print the original capitalization that was in the line
    // I write the original mathches found in the handle_i function
    // and iterate over them to print them out here
    if (flags->o && !flags->v && flags->i) {
        for (int index = 0; index < data->pattern_found_in_the_line; ++index) {
            printf("%s\n", data->inverted_matched_parts_array[index]);
            data->handle_o_printed_newline = TRUE;
        }
    }
    if (flags->o && flags->v && !flags->i) {
        printf("%s", data->line_array);
    }
    if (flags->o && flags->v && flags->i) {
        printf("%s", data->line_array);
    }
}

void handle_h(Flags const* flags, int* filenames_should_be_printed) {
    if (flags->h) {
        *filenames_should_be_printed = !*filenames_should_be_printed;
    }
}

void reset_num_values(Data* data) {
    data->number_of_the_line = 0;
    data->number_of_matching_lines = 0;
    data->pattern_found_in_the_file = FALSE;
    data->is_last_newline = FALSE;
    data->handle_o_printed_newline = FALSE;
}

void reset_line_values(Data* data) {
    memset(data->line_array, 0, MAX_LENGHT_OF_LINE * sizeof(char));
    memset(data->line_array_copy, 0, MAX_LENGHT_OF_LINE * sizeof(char));
    memset(data->inverted_matched_parts_array, 0, sizeof(char));
}

void print_number_of_the_line(const int line_number) {
    printf("%d:", line_number);
}

void handle_n(Flags const* flags, const int line_number) {
    if (flags->n) {
        print_number_of_the_line(line_number);
    }
}

void handle_l(Flags const* flags, const int index_for_files, Data const* data) {
    if (flags->l) {
        if (data->pattern_found_in_the_file) {
            print_filename(index_for_files, data, data->newline);
        }
    }
}

void print_line_array(Flags const* flags, Data const* data) {
    if (!flags->o) {
        printf("%s", data->line_array);
    }
}

void filenames_should_be_printed(Flags const* flags, Data* data) {
    if (!flags->h) {
    data->filename_should_be_printed = data->number_of_files > 1 ? TRUE : FALSE;
    }
}

void print_filename(const int index_for_files, Data const* data, char custom_char) {
    printf("%s%c", data->all_filenames_array[index_for_files], custom_char);
}

void print_number_of_matching_lines(Data const* data) {
    printf("%d\n", data->number_of_matching_lines);
}

void handle_c(Flags const* flags, Data* data) {
    if (flags->c && !flags->l) {
        print_number_of_matching_lines(data);
    }
}

void handle_v(Data* data) {
    data->line_should_be_printed = !data->line_should_be_printed;
    if (!data->pattern_found_in_the_file && data->line_should_be_printed) {
        data->pattern_found_in_the_file = TRUE;
    }
}

void handle_i(Flags const* flags, Data* data, const int pattern_index) {
    int line_lenght = 0, pattern_lenght = 0;
    line_lenght = strlen(data->line_array);
    pattern_lenght = strlen(data->pattern_array[pattern_index]);
    // Erase the array before I use it again
    memset(data->line_array_copy, 0, MAX_LENGHT_OF_LINE * sizeof(char));
    for (int index = 0; index <= line_lenght; ++index) {
        data->line_array_copy[index] = tolower(data->line_array[index]);
    }
    for (int index = 0; index <= pattern_lenght; ++index) {
        data->pattern_array[pattern_index][index] = tolower(data->pattern_array[pattern_index][index]);
    }
    if (flags->i && flags->o) {
        get_inverted_matched_parts(data, pattern_index);
    }
}

void get_inverted_matched_parts(Data* data, const int pattern_index) {
    int line_lenght = 0, pattern_lenght = 0, temp_array_index = 0, inverted_index = 0, matches = 0;
    char temp_array[MAX_LENGHT_OF_LINE];
    memset(temp_array, 0, MAX_LENGHT_OF_LINE * sizeof(char));
    line_lenght = strlen(data->line_array);
    pattern_lenght = strlen(data->pattern_array[pattern_index]);
    for (int l_index = 0; l_index < line_lenght; ++l_index) {
        for (int p_index = 0; p_index < pattern_lenght; ++p_index) {
            if (data->pattern_array[pattern_index][p_index] == data->line_array_copy[l_index]) {
                temp_array[temp_array_index] = data->line_array[l_index];
                ++temp_array_index;
                ++matches;
                if (p_index != pattern_lenght - 1) {
                    ++l_index;
                }
                if (p_index == pattern_lenght - 1 && matches >= pattern_lenght) {
                    strcpy(data->inverted_matched_parts_array[inverted_index], temp_array);
                    ++inverted_index;
                }
            } else {
                matches = 0;
            }
        }
        memset(temp_array, 0, MAX_LENGHT_OF_LINE * sizeof(char));
        temp_array_index = 0;
    }
}

void handle_e(Data* data, const int pattern_index) {
    data->line_should_be_printed = compare_strings(data, pattern_index);
}

int compare_strings(Data* data, const int pattern_index) {
    int are_equal = FALSE;
    regex_t reegex;
    regcomp(&reegex, data->pattern_array[pattern_index], 0);
    int compare_result = 0;
    compare_result = regexec(&reegex, data->line_array_copy, 0, NULL, 0);
    if (!compare_result) {
        are_equal = TRUE;
        if (!data->pattern_found_in_the_file) {
            data->pattern_found_in_the_file = TRUE;
        }
    }
    regfree(&reegex);
    find_how_many_times_pattern_is_in_the_line(data, pattern_index);
    return are_equal;
}

void find_how_many_times_pattern_is_in_the_line(Data* data, int pattern_index) {
    int number_of_occurrences = 0, i_two = 0;
    int line_lenght = (int)strlen(data->line_array_copy);
    int pattern_lenght = (int)strlen(data->pattern_array[pattern_index]);
    for (int i_one = 0; i_one <= line_lenght; ++i_one) {
        if (data->line_array_copy[i_one] == data->pattern_array[pattern_index][i_two]) {
            while (data->line_array_copy[i_one] == data->pattern_array[pattern_index][i_two] &&
            i_two < pattern_lenght) {
                ++i_one;
                if (!(i_two == (line_lenght - 1))) {
                    ++i_two;
                }
                if (data->line_array_copy[i_one] == data->pattern_array[pattern_index][i_two] &&
                (i_two == pattern_lenght - 1)) {
                    ++number_of_occurrences;
                }
            }
            i_two = 0;
        }
    }
    data->pattern_found_in_the_line = number_of_occurrences;
}

int parse_line(FILE *file, Data* data) {
    // Erase the line before I write the array again
    reset_line_values(data);
    ++data->number_of_the_line;
    int can_be_parsed = TRUE, going_to_be_printed = FALSE, index = 0, current_character = 0;
    while (can_be_parsed && current_character != EOF) {
        current_character = fgetc(file);
        if (current_character == EOF || current_character == data->newline) {
            can_be_parsed = FALSE;
            if (current_character == data->newline) {
                data->line_array[index] = current_character;
                ++index;
            }
        } else {
            data->line_array[index] = current_character;
            ++index;
        }
    }
    if (index) {
        going_to_be_printed = TRUE;
    }
    // Copy regular line to copy. Copy is going to be compared as it changes when flags are applied
    // Regular array gets printed as it is the condition
    memcpy(data->line_array_copy, data->line_array, index);
    return going_to_be_printed;
}

int check_start_conditions(const int argc, char *argv[], Flags* flags, Data* data) {
    int conditions = FALSE;
    // Condition to check if there are flags and text-files
    // Parse flags() should return TRUE or FALSE if the flags are valid or not
    // And the files exist
    if (argc >= 4 && parse_flags_patterns_filenames(argv, flags, data)) {
        conditions = TRUE;
    }
    return conditions;
}

int parse_flags_patterns_filenames(char *argv[], Flags* flags, Data* data) {
    int is_valid_input = TRUE;
    // Counters to know when to stop in iterating over the arrays
    int counter_for_flags = 0;
    // Iterate over the argv to sort out flags, pattern and filenames
    int element_index = 1;
    parse_flags(argv, flags, data, &counter_for_flags, &element_index, &is_valid_input);
    // When first index of argv element != '-' read the first pattern (for -e -f)
    // and then the rest may be continuation of flags and the filenames
    // Write to the array of patters only if the pattern wasn't written previously
    if (!data->is_prefious_flag_f) {
        if (data->pattern_array[0][0] == 0) {
            parse_pattern(data, argv, &element_index);
        }
    }
    // Parse flags in case of -e pattern -c filename
    parse_flags(argv, flags, data, &counter_for_flags, &element_index, &is_valid_input);
    // Parse filenames to the array of filenames
    parse_filenames(/*flags, */data, argv, &element_index);
    // Parse the patterns from the filename here for -f flag
    // Flags->f value is set in the parse flags function prior to this call
    if (flags->f) {
        parse_patterns_handle_f(flags, data);
    }
    return is_valid_input;
}

void parse_patterns_handle_f(Flags const* flags, Data* data) {
    FILE *file = NULL;
    if (!(file = fopen(data->pattern_file, "r"))) {
        print_error_message(data, "Pattern file doesn't exist");
    } else {
        char temp_pattern_array[MAX_LENGHT_OF_PATTERN];
        memset(temp_pattern_array, 0, MAX_LENGHT_OF_PATTERN * sizeof(char));
        int char_index = 0, pattern_array_index = 0, current_character = 0;
        pattern_array_index = flags->e ? 1 : 0;
        while (current_character != EOF) {
            current_character = fgetc(file);
            if (current_character != data->newline) {
                temp_pattern_array[char_index] = current_character;
                ++char_index;
            }
            if (current_character == data->newline) {
                strcpy(data->pattern_array[pattern_array_index], temp_pattern_array);
                memset(temp_pattern_array, 0, MAX_LENGHT_OF_PATTERN * sizeof(char));
                char_index = 0;
                ++pattern_array_index;
                ++data->pattern_index;
            }
        }
    }
    data->is_prefious_flag_f = FALSE;
    fclose(file);
}

void pass_flags_to_structure(Flags* flags, Data const* data) {
    int index = 0;
    // Until it is the end of the two-dimensional array,
    // I iterate over it and pass the values to the Flags structure
    // strcmp returns 0 if the values are equal, so I use !(not) to invert the value of zero to true
    while (strcmp(data->all_flags_array[index], "\0")) {
        if (!strcmp(data->all_flags_array[index], "-e") ||
        !strcmp(data->all_flags_array[index], "e")) {
            flags->e = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-i") ||
        !strcmp(data->all_flags_array[index], "i")) {
            flags->i = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-v") ||
        !strcmp(data->all_flags_array[index], "v")) {
            flags->v = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-c") ||
        !strcmp(data->all_flags_array[index], "c")) {
            flags->c = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-l") ||
        !strcmp(data->all_flags_array[index], "l")) {
            flags->l = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-n") ||
        !strcmp(data->all_flags_array[index], "n")) {
            flags->n = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-h") ||
        !strcmp(data->all_flags_array[index], "h")) {
            flags->h = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-s") ||
        !strcmp(data->all_flags_array[index], "s")) {
            flags->s = TRUE;
        } else if (!strcmp(data->all_flags_array[index], "-o") ||
        !strcmp(data->all_flags_array[index], "o")) {
            flags->o = TRUE;
        }
        ++index;
    }
}

int check_if_files_exist(Flags const* flags, const int filename_index, Data const* data) {
    int files_exist = TRUE;
    FILE *file = NULL;
    if (flags->f) {
        if ((file = fopen(data->pattern_file, "r")) == NULL) {
            files_exist = FALSE;
        } else {
            fclose(file);
        }
    } else {
        if ((file = fopen(data->all_filenames_array[filename_index], "r")) == NULL) {
            files_exist = FALSE;
        } else {
            fclose(file);
        }
    }
    return files_exist;
}

int check_if_flag_is_valid(char* flag_array) {
    int flags_are_valid = FALSE, index_possible_flags = 0;
    while (index_possible_flags < TOTAL_NUM_FLAGS && !flags_are_valid) {
        if (!strcmp(flag_array, possible_flags[index_possible_flags])) {
            flags_are_valid = TRUE;
        }
        ++index_possible_flags;
    }
    return flags_are_valid;
}

void parse_pattern(Data* data, char *argv[], int* element_index) {
    strcpy(data->pattern_array[data->pattern_index], argv[*element_index]);
    ++data->pattern_index;
    ++*element_index;
}

void parse_flags(char *argv[], Flags* flags, Data* data,
    int* counter_for_flags, int* element_index, int* is_valid_input) {
    int letter_index = 0, is_previous_e_flag = FALSE, first_element = 0;
    // Create a temp array for single char flags to use strcpy()
    char single_char_flag[MAX_LENGHT_OF_FLAG];
    memset(single_char_flag, 0, MAX_LENGHT_OF_FLAG * sizeof(char));
    // Iterate over the elements which start with '-',
    // Check if they are valid flags and pass them to an array
    while (argv[*element_index][letter_index] == '-') {
        ++letter_index;
        // Here I iterate over letters of the flags when they are written in one word
        while (argv[*element_index][letter_index] && is_valid_input) {
            single_char_flag[first_element] = argv[*element_index][letter_index];
            *is_valid_input = check_if_flag_is_valid(single_char_flag);
            strcpy(data->all_flags_array[*counter_for_flags], single_char_flag);
            ++*counter_for_flags;
            ++letter_index;
            if (single_char_flag[first_element] == 'e') {
                is_previous_e_flag = TRUE;
                flags->e = TRUE;
            }
            if (single_char_flag[first_element] == 'f') {
                data->is_prefious_flag_f = TRUE;
                flags->f = TRUE;
            }
        }
        ++*element_index;
        // The pattern is being copied to the array of patterns in case the previous flag was -e
        if (is_previous_e_flag) {
            parse_pattern(data, argv, element_index);
            is_previous_e_flag = FALSE;
        }
        if (data->is_prefious_flag_f) {
            ++*element_index;
            strcpy(data->pattern_file, argv[*element_index]);
        }
        letter_index = 0;
    }
}

void parse_filenames(Data* data, char *argv[], int* element_index) {
    while (argv[*element_index]) {
        strcpy(data->all_filenames_array[data->number_of_files], argv[*element_index]);
        ++data->number_of_files;
        ++*element_index;
    }
}

void initialize_data(Data* data) {
    memset(data->all_flags_array, 0, sizeof data->all_flags_array);
    memset(data->all_filenames_array, 0, sizeof data->all_filenames_array);
    memset(data->all_filenames_array, 0, sizeof data->all_filenames_array);
    memset(data->pattern_file, 0, MAX_LENGHT_OF_FILENAME * sizeof(char));
    data->pattern_index = 0;
    memset(data->line_array, 0, MAX_LENGHT_OF_LINE * sizeof(char));
    memset(data->line_array_copy, 0, MAX_LENGHT_OF_LINE * sizeof(char));
    memset(data->inverted_matched_parts_array, 0, sizeof data->inverted_matched_parts_array);
    data->number_of_files = 0;
    data->number_of_the_line = 0;
    data->handle_o_printed_newline = FALSE;
    data->newline = '\n';
    data->colon = ':';
    data->line_should_be_printed = FALSE;
    data->filename_should_be_printed = FALSE;
    data->number_of_matching_lines = 0;
    data->pattern_found_in_the_file = FALSE;
    data->pattern_found_in_the_line = 0;
    data->is_last_newline = FALSE;
    data->is_prefious_flag_f = FALSE;
}

void initialize_flags(Flags* flags) {
    flags->e = FALSE;
    flags->i = FALSE;
    flags->v = FALSE;
    flags->c = FALSE;
    flags->l = FALSE;
    flags->n = FALSE;
    flags->h = FALSE;
    flags->s = FALSE;
    flags->f = FALSE;
    flags->o = FALSE;
}
