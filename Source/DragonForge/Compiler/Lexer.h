#ifndef DRAGON__compiler__lexer
#define DRAGON__compiler__lexer

/* Include */
#include "CompilerSpecifications.h"

/* Lexer */
// lexing types
typedef BASIC__u8 COMPILER__lexling_type;
typedef BASIC__address COMPILER__lexling_address;
typedef COMPILER__lexling_address COMPILER__lexling_start;
typedef COMPILER__lexling_address COMPILER__lexling_end;
typedef BASIC__u64 COMPILER__lexling_index;
typedef COMPILER__lexling_index COMPILER__lexling_count;
typedef BASIC__u64 COMPILER__lexling_depth; // used for comments and strings

// lexling types
typedef enum COMPILER__lt {
    COMPILER__lt__invalid,
    COMPILER__lt__left_parenthesis,
    COMPILER__lt__right_parenthesis,
    COMPILER__lt__left_curly_bracket,
    COMPILER__lt__right_curly_bracket,
    COMPILER__lt__name,
    COMPILER__lt__colon,
    COMPILER__lt__comma,
    COMPILER__lt__at,
    COMPILER__lt__equals,
    COMPILER__lt__exclamation_point,
    COMPILER__lt__hashtag,
    COMPILER__lt__string_literal,
    COMPILER__lt__end_of_file,
    COMPILER__lt__end_of_files,
    COMPILER__lt__COUNT,
} COMPILER__lt;

// lexling type
typedef struct COMPILER__lexling {
    COMPILER__lexling_type type;
    BASIC__buffer value;
    COMPILER__character_location location;
} COMPILER__lexling;

// create custom lexling
COMPILER__lexling COMPILER__create__lexling(COMPILER__lexling_type type, BASIC__buffer value, COMPILER__character_location location) {
    COMPILER__lexling output;

    // setup output
    output.type = type;
    output.value = value;
    output.location = location;

    return output;
}

// create null lexling
COMPILER__lexling COMPILER__create_null__lexling() {
    // return empty
    return COMPILER__create__lexling(COMPILER__lt__invalid, BASIC__create_null__buffer(), COMPILER__create_null__character_location());
}

// open lexling from string
COMPILER__lexling COMPILER__open__lexling_from_string(const char* string, COMPILER__lexling_type type, COMPILER__character_location location) {
    COMPILER__lexling output = COMPILER__create_null__lexling();

    // setup value
    output.value = BASIC__open__buffer_from_string((u8*)string, BASIC__bt__false, BASIC__bt__false);

    // setup type
    output.type = type;

    // setup location
    output.location = location;

    return output;
}

// lexlings
typedef struct COMPILER__lexlings {
    BASIC__counted_list data;
} COMPILER__lexlings;

// create custom lexlings
COMPILER__lexlings COMPILER__create__lexlings(BASIC__counted_list list) {
    COMPILER__lexlings output;

    // setup output
    output.data = list;

    return output;
}

// create null lexlings
COMPILER__lexlings COMPILER__create_null__lexlings() {
    // return empty
    return COMPILER__create__lexlings(BASIC__create_null__counted_list());
}

// close lexlings
void COMPILER__close__lexlings(COMPILER__lexlings lexlings) {
    // close buffer
    BASIC__close__counted_list(lexlings.data);

    return;
}

// get lexling by index
COMPILER__lexling COMPILER__get__lexling_by_index(BASIC__counted_list lexlings, COMPILER__lexling_index index) {
    return ((COMPILER__lexling*)lexlings.list.buffer.start)[index];
}

// read a lexling from an address
COMPILER__lexling COMPILER__read__lexling_from_current(BASIC__current current) {
    // return struct
    return *(COMPILER__lexling*)current.start;
}

// check one character
BASIC__bt COMPILER__calculate__valid_character_range(BASIC__current current, BASIC__character start, BASIC__character end) {
    // perform calculation
    return (BASIC__bt)((*(BASIC__character*)(current.start) >= start) && (*(BASIC__character*)(current.start) <= end));
}

// check one character for a name character
BASIC__bt COMPILER__calculate__valid_name_character(BASIC__current current) {
    return (COMPILER__calculate__valid_character_range(current, 'a', 'z') || COMPILER__calculate__valid_character_range(current, 'A', 'Z') || COMPILER__calculate__valid_character_range(current, '0', '9') || COMPILER__calculate__valid_character_range(current, '_', '_') || COMPILER__calculate__valid_character_range(current, '.', '.'));
}

// calculate character index
BASIC__character_index COMPILER__calculate__character_index(BASIC__buffer main_buffer, BASIC__buffer current) {
    return (BASIC__character_index)(current.start - main_buffer.start);
}

void COMPILER__compile__lex_one_buffer(COMPILER__lexlings* lexlings, BASIC__buffer user_code, BASIC__file_index file_index, COMPILER__error* error) {
    COMPILER__lexling_start temp_start;
    COMPILER__lexling_end temp_end;

    // setup metadata
    BASIC__line_number current_line_number = 1;

    // setup current character
    BASIC__current current = user_code;

    // lex program
    while (BASIC__check__current_within_range(current)) {
        // skip comments and whitespace
        while (BASIC__check__current_within_range(current) && (COMPILER__calculate__valid_character_range(current, 0, 32) || COMPILER__calculate__valid_character_range(current, '[', '['))) {
            // skip whitespace
            while (BASIC__check__current_within_range(current) && COMPILER__calculate__valid_character_range(current, 0, 32)) {
                // check for new line
                if (COMPILER__calculate__valid_character_range(current, '\n', '\n') || COMPILER__calculate__valid_character_range(current, '\r', '\r')) {
                    // next line
                    current_line_number++;
                }

                // next character
                current.start += sizeof(BASIC__character);
            }

            // skip comments
            if (BASIC__check__current_within_range(current) && COMPILER__calculate__valid_character_range(current, '[', '[')) {
                COMPILER__lexling_depth depth = 1;

                // next character
                current.start += sizeof(BASIC__character);

                // skip past characters
                while (BASIC__check__current_within_range(current) && depth > 0) {
                    // check for new line
                    if (COMPILER__calculate__valid_character_range(current, '\n', '\n') || COMPILER__calculate__valid_character_range(current, '\r', '\r')) {
                        // next line
                        current_line_number++;
                    }
                    // check for opening comment
                    if (COMPILER__calculate__valid_character_range(current, '[', '[')) {
                        // increase depth
                        depth++;
                    }
                    // check for closing comment
                    if (COMPILER__calculate__valid_character_range(current, ']', ']')) {
                        // decrease depth
                        depth--;
                    }

                    // next character
                    current.start += sizeof(BASIC__character);
                }

                // check for unfinished comment
                if (depth > 0) {
                    // set error
                    *error = COMPILER__open__error("Lexing Error: Comment ended with end of file instead of proper closing.", COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current)));

                    goto quit;
                }
            }
        }

        // check for out of range
        if (BASIC__check__current_within_range(current) == BASIC__bt__false) {
            break;
        }

        // check for lexlings
        if (COMPILER__calculate__valid_character_range(current, '(', '(')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__left_parenthesis, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, ')', ')')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__right_parenthesis, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, '{', '{')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__left_curly_bracket, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, '}', '}')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__right_curly_bracket, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_name_character(current)) {
            // get lexling start and setup temp end
            temp_start = current.start;
            temp_end = temp_start - 1;

            // get lexling size
            while (BASIC__check__current_within_range(current) && COMPILER__calculate__valid_name_character(current)) {
                // next character
                current.start += sizeof(BASIC__character);
                temp_end += sizeof(BASIC__character);
            }

            // record lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__name, BASIC__create__buffer(temp_start, temp_end), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, BASIC__create__buffer(temp_start, temp_end)))), error);
            (*lexlings).data.count++;
        } else if (COMPILER__calculate__valid_character_range(current, ':', ':')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__colon, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, ',', ',')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__comma, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, '@', '@')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__at, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, '=', '=')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__equals, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, '!', '!')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__exclamation_point, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, '#', '#')) {
            // add lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__hashtag, BASIC__create__buffer(current.start, current.start), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        } else if (COMPILER__calculate__valid_character_range(current, '"', '"')) {
            BASIC__buffer data;

            // get string start
            data.start = current.start;

            // advance current
            current.start += sizeof(BASIC__character);

            // search for string end
            while (BASIC__check__current_within_range(current) && COMPILER__calculate__valid_character_range(current, '\"', '\"') == BASIC__bt__false) {
                // next character
                current.start += sizeof(BASIC__character);
            }

            // check for end of file
            if (BASIC__check__current_within_range(current) == BASIC__bt__false) {
                // string ended abruptly
                *error = COMPILER__open__error("Lexical Error: String ended at the end of a file and not with a (\").", COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current)));

                goto quit;
            }

            // finish string data
            data.end = current.start;

            // append lexling
            COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__string_literal, data, COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, data))), error);
            (*lexlings).data.count++;

            // next character
            current.start += sizeof(BASIC__character);
        // no lexling found
        } else {
            // open error
            *error = COMPILER__open__error("Lexical Error: Invalid character.", COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current)));

            // quit
            goto quit;
        }

        // check for error
        if (COMPILER__check__error_occured(error)) {
            // return lexlings as they are
            goto quit;
        }
    }

    // append eof lexling
    COMPILER__append(&(*lexlings).data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__end_of_file, BASIC__open__buffer_from_string((u8*)"[EOF]", BASIC__bt__false, BASIC__bt__false), COMPILER__create__character_location(file_index, current_line_number, COMPILER__calculate__character_index(user_code, current))), error);

    // quit
    quit:

    return;
}

// lex a program
COMPILER__lexlings COMPILER__compile__lex(BASIC__buffer included_files, BASIC__bt include_standard_files, BASIC__buffer user_codes, COMPILER__error* error) {
    COMPILER__lexlings output;
    BASIC__current current_file;
    BASIC__file_index file_index;

    // setup output
    output.data = COMPILER__open__counted_list_with_error(sizeof(COMPILER__lexling) * 2048, error);

    // check for error
    if (COMPILER__check__error_occured(error)) {
        // return empty
        return COMPILER__create_null__lexlings();
    }

    // setup file index
    file_index = 0;

    // if standard files are included
    if (include_standard_files == BASIC__bt__true) {
        // lex those files
        // setup current
        current_file = included_files;

        // lex each file
        while (BASIC__check__current_within_range(current_file)) {
            // setup buffer
            BASIC__buffer included_code = *(BASIC__buffer*)current_file.start;

            // lex buffer
            COMPILER__compile__lex_one_buffer(&output, included_code, -1, error);
            if (COMPILER__check__error_occured(error)) {
                goto quit;
            }

            // next lexling buffer
            current_file.start += sizeof(BASIC__buffer);
        }
    }

    // setup current
    current_file = user_codes;

    // for each file made by the user
    while (BASIC__check__current_within_range(current_file)) {
        // setup user code
        BASIC__buffer user_code = *(BASIC__buffer*)current_file.start;

        // lex buffer
        COMPILER__compile__lex_one_buffer(&output, user_code, file_index, error);
        if (COMPILER__check__error_occured(error)) {
            goto quit;
        }

        // next lexling buffer
        current_file.start += sizeof(BASIC__buffer);
        file_index++;
    }

    // quit
    quit:

    // append eofs lexling
    COMPILER__append(&output.data.list, COMPILER__lexling, COMPILER__create__lexling(COMPILER__lt__end_of_files, BASIC__open__buffer_from_string((u8*)"[EOFS]", BASIC__bt__false, BASIC__bt__false), COMPILER__create__character_location(file_index, -1, -1)), error);

    return output;
}

// print lexlings
void COMPILER__debug__print_lexlings(COMPILER__lexlings lexlings) {
    BASIC__current current;
    COMPILER__lexling temp;

    // setup current
    current = lexlings.data.list.buffer;

    // print header
    printf("Lexlings:\n");

    // print each lexling
    while (current.start < lexlings.data.list.buffer.start + lexlings.data.list.filled_index) {
        // get lexling
        temp = COMPILER__read__lexling_from_current(current);

        // next lexling
        current.start += sizeof(COMPILER__lexling);

        // print lexling type
        BASIC__print__tabs(1);
        printf("%lu [ %lu, %lu ] [ file_index: %lu, line_number: %lu, character_index: %lu ] : ", (BASIC__u64)temp.type, (BASIC__u64)temp.value.start, (BASIC__u64)temp.value.end, temp.location.file_index, (BASIC__u64)temp.location.line_number, (BASIC__u64)temp.location.character_index);
        fflush(stdout);

        // print lexling string
        BASIC__print__buffer(temp.value);

        // print new line
        printf("\n");
    }

    return;
}

#endif
