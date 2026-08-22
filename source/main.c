// anvil
#include "anvil.h"

// c
#include <stdio.h>

// print context
void MAIN__print__context(ANVIL__context* context) {
    u64 cell_index;
    u64 row_items;

    // setup variables
    cell_index = 0;

    // print program size
    printf("Program Size: [ %lu ]\n", ((ANVIL__u64)(*context).cells[ANVIL__rt__program_end_address] - (ANVIL__u64)(*context).cells[ANVIL__rt__program_start_address]));

    // print cells section header
    printf("Cells:\n");

    // print rows
    while (cell_index < ANVIL__rt__TOTAL_COUNT) {
        // set row items
        row_items = 0;

        // print padding
        printf("\t%lu: [", (ANVIL__u64)cell_index);

        // print columns
        while (cell_index < ANVIL__rt__TOTAL_COUNT && row_items < 8) {
            // print cell value
            printf(" %lu", (ANVIL__u64)(*context).cells[cell_index]);

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
    ANVIL__list files;
    ANVIL__bt debug_mode = ANVIL__bt__false;
    ANVIL__u64 current_argument = 1;
    COMPILER__error error;

    // init error
    error = COMPILER__create_null__error();

    // open files list
    files = COMPILER__open__list_with_error(sizeof(ANVIL__buffer) * 32, &error);
    if (COMPILER__check__error_occured(&error)) {
        printf("Error, could not open files list.\n");

        return 1;
    }

    // check if there are enough arguments
    if (argc > 1) {
        // check for debug mode
        if (ANVIL__calculate__buffer_contents_equal(ANVIL__open__buffer_from_string((u8*)"--debug", ANVIL__bt__false, ANVIL__bt__false), ANVIL__open__buffer_from_string((u8*)argv[current_argument], ANVIL__bt__false, ANVIL__bt__false))) {
            // enable debug mode
            debug_mode = ANVIL__bt__true;

            // skip to next input
            current_argument++;
        }

        // load files
        while (current_argument < (ANVIL__u64)argc) {
            // get file
            ANVIL__buffer file = ANVIL__move__file_to_buffer(ANVIL__open__buffer_from_string((u8*)argv[current_argument], ANVIL__bt__false, ANVIL__bt__true));

            // check for blank file
            if (ANVIL__check__empty_buffer(file)) {
                // file could no be opened
                printf("Error, file \"%s\" could not be opened.\n", (char*)ANVIL__open__buffer_from_string((u8*)argv[current_argument], ANVIL__bt__false, ANVIL__bt__true).start);

                goto clean_up;
            }

            // add file
            COMPILER__append(&files, ANVIL__buffer, file, &error);
            if (COMPILER__check__error_occured(&error) == ANVIL__bt__true) {
                printf("Error, could not add buffer to inputs list.");

                goto clean_up;
            }

            // next argument
            current_argument++;
        }

        // if files were passed
        if (ANVIL__check__current_within_range(ANVIL__calculate__current_from_list_filled_index(&files)) == ANVIL__bt__true) {
            // setup output
            ANVIL__buffer program = ANVIL__create_null__buffer();
            ANVIL__buffer debug_information = ANVIL__create_null__buffer();

            // run compiler
            COMPILER__compile__files(ANVIL__calculate__list_current_buffer(&files), ANVIL__bt__true, ANVIL__bt__true, debug_mode, ANVIL__bt__true, &program, &debug_information, &error);

            // if error
            if (COMPILER__check__error_occured(&error)) {
                // setup json error
                ANVIL__bt json_error_occured = ANVIL__bt__false;

                // get message
                ANVIL__buffer error_json = COMPILER__serialize__error_json(error, &json_error_occured);
                if (json_error_occured) {
                    printf("Failed to serialize json error, oops.\n");

                    goto clean_up;
                }

                // print error
                fflush(stdout);
                ANVIL__print__buffer(error_json);

                // deallocate error message
                ANVIL__close__buffer(error_json);
            // no error occured, run code
            } else {
                ANVIL__bt memory_error_occured = ANVIL__bt__false;

                // setup allocations
                ANVIL__allocations allocations = ANVIL__open__allocations(&memory_error_occured);
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
                ANVIL__buffer context_buffer = ANVIL__open__buffer(sizeof(ANVIL__context));
                *(ANVIL__context*)context_buffer.start = ANVIL__setup__context(program);
                ANVIL__remember__allocation(&allocations, context_buffer, &memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but allocations failed to append.\n");
                
                    goto close_context;
                }

                // print debug
                if (debug_mode == ANVIL__bt__true) {
                    // print error json
                    ANVIL__print__buffer(debug_information);
                
                    // print header
                    printf("Running program...\n------------------\n");
                }

                // run code
                ANVIL__run__context(&allocations, (ANVIL__context*)context_buffer.start, ANVIL__define__run_forever);

                // print debug
                if (debug_mode == ANVIL__bt__true) {
                    printf("\n");
                }

                // close program
                ANVIL__close__buffer(program);

                // close context
                close_context:
                ANVIL__close__buffer(context_buffer);

                // close allocations
                close_allocations:
                ANVIL__close__allocations(&allocations);
            }

            // close debug information
            close_debug_information:
            ANVIL__close__buffer(debug_information);
        // if no files
        } else {
            printf("Error, no file paths were passed.\n");
        }

        // clean up
        clean_up:
        if (error.occured == ANVIL__bt__true) {
            COMPILER__close__error(error);
        }
        ANVIL__current current_file = ANVIL__calculate__current_from_list_filled_index(&files);
        while (ANVIL__check__current_within_range(current_file)) {
            ANVIL__close__buffer(*(ANVIL__buffer*)current_file.start);
            current_file.start += sizeof(ANVIL__buffer);
        }
        ANVIL__close__list(files);
    // not enough args
    } else {
        printf("Error, no arguments were passed.\n");

        return 1;
    }

    // exit
    return 0;
}
