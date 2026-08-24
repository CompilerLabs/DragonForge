#ifndef DRAGON__interpreter_specifications
#define DRAGON__interpreter_specifications

/* Include */
#include "Basic.h"
#include <time.h>

/* Specifications */
// instructions
typedef BASIC__address ANVIL__cell;
typedef BASIC__u8 ANVIL__instruction_ID;
typedef BASIC__u8 ANVIL__flag_ID;
typedef BASIC__u8 ANVIL__operation_ID;
typedef BASIC__u16 ANVIL__cell_ID;
typedef BASIC__u64 ANVIL__instruction_count;
#define ANVIL__define__run_forever (BASIC__u64)-1
#define ANVIL__define__input_string_max_length 256 // is part of fgets instruction and nothing else
#define ANVIL__define__max_allocation_size 0x100000000

// cell related types
typedef BASIC__u64 ANVIL__cell_count;
typedef ANVIL__cell_count ANVIL__cell_index;
typedef BASIC__u64 ANVIL__cell_integer_value;
#define ANVIL__unused_cell_ID 0

// invalids
#define ANVIL__define__null_offset_ID -1
#define ANVIL__define__null_call_ID -1
#define ANVIL__define__invalid_abstraction_call_ID -1

// scraplet types
typedef enum ANVIL__st {
    // scraplet types
    ANVIL__st__instruction_ID,
    ANVIL__st__flag_ID,
    ANVIL__st__operation_ID,
    ANVIL__st__cell_ID,
    ANVIL__st__cell,

    // count
    ANVIL__st__COUNT,
} ANVIL__st;

// context cell organization defines
// cell type
typedef enum ANVIL__rt {
    // start of defined values
    ANVIL__rt__START = 0,

    // basic cells
    ANVIL__rt__program_start_address = ANVIL__rt__START,
    ANVIL__rt__program_end_address,
    ANVIL__rt__program_current_address,
    ANVIL__rt__error_code,
    ANVIL__rt__flags_0,
    ANVIL__rt__flags_1,
    ANVIL__rt__flags_2,
    ANVIL__rt__flags_3,
    ANVIL__rt__address_catch_toggle,

    // end of defined cells
    ANVIL__rt__END,

    // count
    ANVIL__rt__RESERVED_COUNT = ANVIL__rt__END - ANVIL__rt__START,

    // statistics
    ANVIL__rt__FIRST_ID = ANVIL__rt__START,
    ANVIL__rt__LAST_ID = 65535, // NUMBER DOES NOT CHANGE!
    ANVIL__rt__TOTAL_COUNT = ANVIL__rt__LAST_ID + 1,
} ANVIL__rt;

// context
typedef struct ANVIL__context {
    ANVIL__cell cells[ANVIL__rt__TOTAL_COUNT];
} ANVIL__context;

// instruction type and instruction ID defines (type number is also ID)
typedef enum ANVIL__it {
    // start of defined instructions
    ANVIL__it__START = 0,

    // defined instructions
    ANVIL__it__stop = ANVIL__it__START, // returns context to caller
    ANVIL__it__write_cell, // overwrites entire cell with hard coded value
    ANVIL__it__operate, // inter-cell operations
    ANVIL__it__request_memory, // allocate request
    ANVIL__it__return_memory, // deallocate request
    ANVIL__it__address_to_cell, // read memory into cell
    ANVIL__it__cell_to_address, // write cell to memory
    ANVIL__it__file_to_buffer, // get file into buffer
    ANVIL__it__buffer_to_file, // write buffer to disk
    ANVIL__it__delete_file, // deletes a file
    ANVIL__it__buffer_to_buffer__low_to_high, // copy data from one buffer to another of the same size in the low to high direction
    ANVIL__it__buffer_to_buffer__high_to_low, // copy data from one buffer to another of the same size in the high to low direction
    ANVIL__it__compile, // compile one or more source files to an anvil program
    ANVIL__it__run, // run context (both until finished and run one instruction)
    ANVIL__it__get_time, // get time in seconds and nano seconds
    ANVIL__it__debug__putchar, // print one character to stdout
    ANVIL__it__debug__fgets, // read one line from stdin
    ANVIL__it__debug__mark_data_section, // mark a section of data (NOP)
    ANVIL__it__debug__mark_code_section, // mark a section of code (NOP)
    ANVIL__it__debug__get_current_context, // gets the current context as a buffer
    ANVIL__it__debug__search_for_allocation, // takes a buffer and searches for an allocation it fits inside of, if not found, null buffer

    // end of defined instruction types
    ANVIL__it__END,

    // count
    ANVIL__it__COUNT = ANVIL__it__END - ANVIL__it__START,
} ANVIL__it;

// instruction length types
typedef enum ANVIL__ilt {
    ANVIL__ilt__stop = sizeof(ANVIL__instruction_ID),
    ANVIL__ilt__write_cell = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell) + sizeof(ANVIL__cell_ID),
    ANVIL__ilt__operate = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__flag_ID) + sizeof(ANVIL__operation_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__request_memory = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__return_memory = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 2),
    ANVIL__ilt__address_to_cell = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__flag_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__cell_to_address = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__flag_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__file_to_buffer = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__buffer_to_file = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__delete_file = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 2),
    ANVIL__ilt__buffer_to_buffer__low_to_high = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__buffer_to_buffer__high_to_low = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__compile = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 15),
    ANVIL__ilt__run = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__get_time = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 2),
    ANVIL__ilt__debug__putchar = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell_ID),
    ANVIL__ilt__debug__fgets = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 2),
    ANVIL__ilt__debug__mark_data_section = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell),
    ANVIL__ilt__debug__mark_code_section = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell),
    ANVIL__ilt__debug__get_current_context = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 2),
    ANVIL__ilt__debug__search_for_allocation = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 5),
} ANVIL__ilt;

// error codes
typedef enum ANVIL__et {
    // error codes
    ANVIL__et__no_error = 0,
    ANVIL__et__invalid_instruction_ID,
    ANVIL__et__divide_by_zero,
    ANVIL__et__modulous_by_zero,
    ANVIL__et__file_not_found,
    ANVIL__et__file_not_created,
    ANVIL__et__invalid_address__address_to_cell,
    ANVIL__et__invalid_address__cell_to_address,
    ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer,
    ANVIL__et__invalid_allocation__allocation_does_not_exist,
    ANVIL__et__invalid_allocation__requested_memory_too_large,
    ANVIL__et__invalid_address_range,
    ANVIL__et__buffer_to_buffer__buffers_are_different_sizes,
    ANVIL__et__compile__compilation_error,
    ANVIL__et__compile__invalid_input_configuration,
    ANVIL__et__program_ran_out_of_instructions,
    ANVIL__et__program_ran_out_of_instruction_parameters,

    // count
    ANVIL__et__COUNT,
} ANVIL__et;

// operation types
typedef enum ANVIL__ot {
    // start
    ANVIL__ot__START = 0,

    // copy
    ANVIL__ot__cell_to_cell = ANVIL__ot__START, // copies one cell to another without transformation

    // binary operations
    ANVIL__ot__bits_or,
    ANVIL__ot__bits_invert,
    ANVIL__ot__bits_and,
    ANVIL__ot__bits_xor,
    ANVIL__ot__bits_shift_higher,
    ANVIL__ot__bits_shift_lower,
    ANVIL__ot__bits_overwrite,

    // arithmetic operations
    ANVIL__ot__integer_add,
    ANVIL__ot__integer_subtract,
    ANVIL__ot__integer_multiply,
    ANVIL__ot__integer_division,
    ANVIL__ot__integer_modulous,

    // comparison operations
    ANVIL__ot__integer_within_range, // equivalent to (range_start <= integer_n && integer_n <= range_end) -> boolean

    // flag operations
    ANVIL__ot__flag_or,
    ANVIL__ot__flag_invert,
    ANVIL__ot__flag_and,
    ANVIL__ot__flag_xor,
    ANVIL__ot__flag_get,
    ANVIL__ot__flag_set,

    // end
    ANVIL__ot__END = ANVIL__ot__flag_set,

    // count
    ANVIL__ot__COUNT = ANVIL__ot__END - ANVIL__ot__START + 1,
} ANVIL__ot;

// should the next instruction process
// next instruction type
typedef enum ANVIL__nit {
    // next instruction types
    ANVIL__nit__return_context,
    ANVIL__nit__next_instruction,

    // count
    ANVIL__nit__COUNT,
} ANVIL__nit;

#endif
