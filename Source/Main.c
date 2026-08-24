// anvil
#include "Anvil.h"

// c
#include <stdio.h>

// print context
void MAIN__print__context(ANVIL__context* context) {
    u64 cell_index;
    u64 row_items;

    // setup variables
    cell_index = 0;

    // print program size
    printf("Program Size: [ %lu ]\n", ((BASIC__u64)(*context).cells[ANVIL__rt__program_end_address] - (BASIC__u64)(*context).cells[ANVIL__rt__program_start_address]));

    // print cells section header
    printf("Cells:\n");

    // print rows
    while (cell_index < ANVIL__rt__TOTAL_COUNT) {
        // set row items
        row_items = 0;

        // print padding
        printf("\t%lu: [", (BASIC__u64)cell_index);

        // print columns
        while (cell_index < ANVIL__rt__TOTAL_COUNT && row_items < 8) {
            // print cell value
            printf(" %lu", (BASIC__u64)(*context).cells[cell_index]);

            // next
            cell_index++;
            row_items++;
        }

        // print padding
        printf(" ]\n");
    }

    // print final padding
    printf("----------\n");

    return;
}

// entry point
int main(int argc, char** argv) {
    BASIC__list files;
    BASIC__bt debug_mode = BASIC__bt__false;
    BASIC__u64 current_argument = 1;
    COMPILER__error error;

    // init error
    error = COMPILER__create_null__error();

    // open files list
    files = COMPILER__open__list_with_error(sizeof(BASIC__buffer) * 32, &error);
    if (COMPILER__check__error_occured(&error)) {
        printf("Error, could not open files list.\n");

        return 1;
    }

    // check if there are enough arguments
    if (argc > 1) {
        // check for debug mode
        if (BASIC__calculate__buffer_contents_equal(BASIC__open__buffer_from_string((u8*)"--debug", BASIC__bt__false, BASIC__bt__false), BASIC__open__buffer_from_string((u8*)argv[current_argument], BASIC__bt__false, BASIC__bt__false))) {
            // enable debug mode
            debug_mode = BASIC__bt__true;

            // skip to next input
            current_argument++;
        }

        // load files
        while (current_argument < (BASIC__u64)argc) {
            // get file
            BASIC__buffer file = BASIC__move__file_to_buffer(BASIC__open__buffer_from_string((u8*)argv[current_argument], BASIC__bt__false, BASIC__bt__true));

            // check for blank file
            if (BASIC__check__empty_buffer(file)) {
                // file could no be opened
                printf("Error, file \"%s\" could not be opened.\n", (char*)BASIC__open__buffer_from_string((u8*)argv[current_argument], BASIC__bt__false, BASIC__bt__true).start);

                goto clean_up;
            }

            // add file
            COMPILER__append(&files, BASIC__buffer, file, &error);
            if (COMPILER__check__error_occured(&error) == BASIC__bt__true) {
                printf("Error, could not add buffer to inputs list.");

                goto clean_up;
            }

            // next argument
            current_argument++;
        }

        // if files were passed
        if (BASIC__check__current_within_range(BASIC__calculate__current_from_list_filled_index(&files)) == BASIC__bt__true) {
            // setup output
            BASIC__buffer program = BASIC__create_null__buffer();
            BASIC__buffer debug_information = BASIC__create_null__buffer();

            // run compiler
            COMPILER__compile__files(BASIC__calculate__list_current_buffer(&files), BASIC__bt__true, BASIC__bt__true, debug_mode, BASIC__bt__true, &program, &debug_information, &error);

            // if error
            if (COMPILER__check__error_occured(&error)) {
                // setup json error
                BASIC__bt json_error_occured = BASIC__bt__false;

                // get message
                BASIC__buffer error_json = COMPILER__serialize__error_json(error, &json_error_occured);
                if (json_error_occured) {
                    printf("Failed to serialize json error, oops.\n");

                    goto clean_up;
                }

                // print error
                fflush(stdout);
                BASIC__print__buffer(error_json);

                // deallocate error message
                BASIC__close__buffer(error_json);
            // no error occured, run code
            } else {
                BASIC__bt memory_error_occured = BASIC__bt__false;

                // setup allocations
                ANVIL__allocations allocations = BASIC__open__allocations(&memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but allocations failed to open.\n");

                    goto close_debug_information;
                }

                // add allocations
                ANVIL__remember__allocation(&allocations, program, &memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but program allocations failed to append.\n");

                    goto close_debug_information;
                }
                ANVIL__remember__allocation(&allocations, debug_information, &memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but debug allocations failed to append.\n");

                    goto close_allocations;
                }

                // setup context
                BASIC__buffer context_buffer = BASIC__open__buffer(sizeof(ANVIL__context));
                *(ANVIL__context*)context_buffer.start = ANVIL__setup__context(program);
                ANVIL__remember__allocation(&allocations, context_buffer, &memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but allocations failed to append.\n");
                
                    goto close_context;
                }

                // print debug
                if (debug_mode == BASIC__bt__true) {
                    // print error json
                    BASIC__print__buffer(debug_information);
                
                    // print header
                    printf("Running program...\n------------------\n");
                }

                // run code
                ANVIL__run__context(&allocations, (ANVIL__context*)context_buffer.start, ANVIL__define__run_forever);

                // print debug
                if (debug_mode == BASIC__bt__true) {
                    printf("\n");
                }

                // close program
                BASIC__close__buffer(program);

                // close context
                close_context:
                BASIC__close__buffer(context_buffer);

                // close allocations
                close_allocations:
                BASIC__close__allocations(&allocations);
            }

            // close debug information
            close_debug_information:
            BASIC__close__buffer(debug_information);
        // if no files
        } else {
            printf("Error, no file paths were passed.\n");
        }

        // clean up
        clean_up:
        if (error.occured == BASIC__bt__true) {
            COMPILER__close__error(error);
        }
        BASIC__current current_file = BASIC__calculate__current_from_list_filled_index(&files);
        while (BASIC__check__current_within_range(current_file)) {
            BASIC__close__buffer(*(BASIC__buffer*)current_file.start);
            current_file.start += sizeof(BASIC__buffer);
        }
        BASIC__close__list(files);
    // not enough args
    } else {
        printf("Error, no arguments were passed.\n");

        return 1;
    }

    // exit
    return 0;
}
