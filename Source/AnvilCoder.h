#ifndef DRAGON__anvil_coder
#define DRAGON__anvil_coder

/* Include */
// C
#include "AnvilSpecifications.h"

/* Instruction Workspace */
// pass type
typedef enum ANVIL__pt {
    ANVIL__pt__get_offsets,
    ANVIL__pt__write_program,
    ANVIL__pt__COUNT,
} ANVIL__pt;

// offsets
typedef ANVIL__u64 ANVIL__offset;

// invalid offset placeholder
#define ANVIL__invalid_offset -1

// instruction creation container
typedef struct ANVIL__workspace {
    ANVIL__pt pass;
    ANVIL__u64 current_program_offset;
    ANVIL__address write_to;
    ANVIL__buffer* program_buffer;
} ANVIL__workspace;

// setup workspace
ANVIL__workspace ANVIL__setup__workspace(ANVIL__buffer* program_buffer_destination) {
    ANVIL__workspace output;

    // setup output
    output.pass = ANVIL__pt__get_offsets;
    output.current_program_offset = 0;
    output.write_to = (*program_buffer_destination).start;
    output.program_buffer = program_buffer_destination;

    return output;
}

// create an offset
ANVIL__offset ANVIL__get__offset(ANVIL__workspace* workspace) {
    // return current offset
    return (*workspace).current_program_offset;
}

// setup pass
void ANVIL__setup__pass(ANVIL__workspace* workspace, ANVIL__pt pass) {
    // setup pass in workspace
    (*workspace).pass = pass;

    // do stuff
    switch ((*workspace).pass) {
    case ANVIL__pt__get_offsets:
        (*workspace).current_program_offset = 0;
        
        break;
    case ANVIL__pt__write_program:
        // allocate program buffer
        (*(*workspace).program_buffer) = ANVIL__open__buffer((*workspace).current_program_offset);

        // setup pass
        (*workspace).current_program_offset = 0;
        (*workspace).write_to = (*(*workspace).program_buffer).start;

        break;
    default:
        break;
    }

    return;
}

/* Appending */
// add an offset to a list
void ANVIL__append__offset(ANVIL__list* list, ANVIL__offset offset, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(ANVIL__offset), memory_error_occured);

    // append data
    (*(ANVIL__offset*)ANVIL__calculate__list_current_address(list)) = offset;

    // increase fill
    (*list).filled_index += sizeof(ANVIL__offset);

    return;
}

/* Write Instruction Scraplets */
// write instruction ID
void ANVIL__write_next__instruction_ID(ANVIL__workspace* workspace, ANVIL__instruction_ID instruction_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__instruction_ID*)(*workspace).write_to) = instruction_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__instruction_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__instruction_ID));

    return;
}

// write flag ID
void ANVIL__write_next__flag_ID(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__flag_ID*)(*workspace).write_to) = flag_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__flag_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__flag_ID));

    return;
}

// write operation ID
void ANVIL__write_next__operation_ID(ANVIL__workspace* workspace, ANVIL__operation_ID operation_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__operation_ID*)(*workspace).write_to) = operation_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__operation_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__operation_ID));

    return;
}

// write cell ID
void ANVIL__write_next__cell_ID(ANVIL__workspace* workspace, ANVIL__cell_ID cell_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__cell_ID*)(*workspace).write_to) = cell_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__cell_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__cell_ID));

    return;
}

// write cell value
void ANVIL__write_next__cell(ANVIL__workspace* workspace, ANVIL__cell cell_value) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__cell*)(*workspace).write_to) = cell_value;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__cell);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__cell));

    return;
}

// write buffer
void ANVIL__write_next__buffer(ANVIL__workspace* workspace, ANVIL__buffer buffer) {
    ANVIL__length buffer_length;

    // set buffer length
    buffer_length = ANVIL__calculate__buffer_length(buffer);

    // write buffer
    if ((*workspace).pass == ANVIL__pt__write_program) {
        // write buffer length
        ANVIL__write__buffer(buffer_length, sizeof(ANVIL__length), (*workspace).write_to);

        // copy buffer
        for (ANVIL__length byte_index = 0; byte_index < buffer_length; byte_index++) {
            // copy character
            ((ANVIL__u8*)(*workspace).write_to)[byte_index + sizeof(ANVIL__length)] = ((ANVIL__u8*)buffer.start)[byte_index];
        }
    }

    // advance
    (*workspace).current_program_offset += buffer_length + sizeof(ANVIL__length);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + buffer_length + sizeof(ANVIL__length));

    return;
}

/* Write Instructions */
// write buffer data
void ANVIL__code__buffer(ANVIL__workspace* workspace, ANVIL__buffer buffer) {
    // write data
    ANVIL__write_next__buffer(workspace, buffer);

    return;
}

// write stop instruction
void ANVIL__code__stop(ANVIL__workspace* workspace) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__stop);

    return;
}

// write write cell instruction
void ANVIL__code__write_cell(ANVIL__workspace* workspace, ANVIL__cell value, ANVIL__cell_ID value_destination) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__write_cell);
    ANVIL__write_next__cell(workspace, value);
    ANVIL__write_next__cell_ID(workspace, value_destination);

    return;
}

// write operate instruction
void ANVIL__code__operate(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID, ANVIL__operation_ID operation_ID, ANVIL__cell_ID input_0, ANVIL__cell_ID input_1, ANVIL__cell_ID input_2, ANVIL__cell_ID output_0) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__operate);
    ANVIL__write_next__flag_ID(workspace, flag_ID);
    ANVIL__write_next__operation_ID(workspace, operation_ID);
    ANVIL__write_next__cell_ID(workspace, input_0);
    ANVIL__write_next__cell_ID(workspace, input_1);
    ANVIL__write_next__cell_ID(workspace, input_2);
    ANVIL__write_next__cell_ID(workspace, output_0);

    return;
}

// write request memory instruction
void ANVIL__code__request_memory(ANVIL__workspace* workspace, ANVIL__cell_ID allocation_size, ANVIL__cell_ID allocation_start, ANVIL__cell_ID allocation_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__request_memory);
    ANVIL__write_next__cell_ID(workspace, allocation_size);
    ANVIL__write_next__cell_ID(workspace, allocation_start);
    ANVIL__write_next__cell_ID(workspace, allocation_end);

    return;
}

// write return memory instruction
void ANVIL__code__return_memory(ANVIL__workspace* workspace, ANVIL__cell_ID allocation_start, ANVIL__cell_ID allocation_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__return_memory);
    ANVIL__write_next__cell_ID(workspace, allocation_start);
    ANVIL__write_next__cell_ID(workspace, allocation_end);

    return;
}

// write address to cell instruction
void ANVIL__code__address_to_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID, ANVIL__cell_ID source_address, ANVIL__cell_ID byte_count, ANVIL__cell_ID destination_cell) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__address_to_cell);
    ANVIL__write_next__flag_ID(workspace, flag_ID);
    ANVIL__write_next__cell_ID(workspace, source_address);
    ANVIL__write_next__cell_ID(workspace, byte_count);
    ANVIL__write_next__cell_ID(workspace, destination_cell);

    return;
}

// write cell to address instruction
void ANVIL__code__cell_to_address(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID, ANVIL__cell_ID source_cell, ANVIL__cell_ID byte_count, ANVIL__cell_ID destination_address) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__cell_to_address);
    ANVIL__write_next__flag_ID(workspace, flag_ID);
    ANVIL__write_next__cell_ID(workspace, source_cell);
    ANVIL__write_next__cell_ID(workspace, byte_count);
    ANVIL__write_next__cell_ID(workspace, destination_address);

    return;
}

// write file to buffer instruction
void ANVIL__code__file_to_buffer(ANVIL__workspace* workspace, ANVIL__cell_ID file_name_start, ANVIL__cell_ID file_name_end, ANVIL__cell_ID file_data_start, ANVIL__cell_ID file_data_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__file_to_buffer);
    ANVIL__write_next__cell_ID(workspace, file_name_start);
    ANVIL__write_next__cell_ID(workspace, file_name_end);
    ANVIL__write_next__cell_ID(workspace, file_data_start);
    ANVIL__write_next__cell_ID(workspace, file_data_end);

    return;
}

// write buffer to file instruction
void ANVIL__code__buffer_to_file(ANVIL__workspace* workspace, ANVIL__cell_ID file_data_start, ANVIL__cell_ID file_data_end, ANVIL__cell_ID file_name_start, ANVIL__cell_ID file_name_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__buffer_to_file);
    ANVIL__write_next__cell_ID(workspace, file_data_start);
    ANVIL__write_next__cell_ID(workspace, file_data_end);
    ANVIL__write_next__cell_ID(workspace, file_name_start);
    ANVIL__write_next__cell_ID(workspace, file_name_end);

    return;
}

// write delete file instruction
void ANVIL__code__delete_file(ANVIL__workspace* workspace, ANVIL__cell_ID file_path_start, ANVIL__cell_ID file_path_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__delete_file);
    ANVIL__write_next__cell_ID(workspace, file_path_start);
    ANVIL__write_next__cell_ID(workspace, file_path_end);

    return;
}

// write buffer to buffer instruction
void ANVIL__code__buffer_to_buffer__low_to_high(ANVIL__workspace* workspace, ANVIL__cell_ID source_start, ANVIL__cell_ID source_end, ANVIL__cell_ID destination_start, ANVIL__cell_ID destination_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__buffer_to_buffer__low_to_high);
    ANVIL__write_next__cell_ID(workspace, source_start);
    ANVIL__write_next__cell_ID(workspace, source_end);
    ANVIL__write_next__cell_ID(workspace, destination_start);
    ANVIL__write_next__cell_ID(workspace, destination_end);

    return;
}

// write buffer to buffer instruction
void ANVIL__code__buffer_to_buffer__high_to_low(ANVIL__workspace* workspace, ANVIL__cell_ID source_start, ANVIL__cell_ID source_end, ANVIL__cell_ID destination_start, ANVIL__cell_ID destination_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__buffer_to_buffer__high_to_low);
    ANVIL__write_next__cell_ID(workspace, source_start);
    ANVIL__write_next__cell_ID(workspace, source_end);
    ANVIL__write_next__cell_ID(workspace, destination_start);
    ANVIL__write_next__cell_ID(workspace, destination_end);

    return;
}

// write compile instruction
void ANVIL__code__compile(ANVIL__workspace* workspace, ANVIL__cell_ID user_codes_buffers_buffer_start, ANVIL__cell_ID user_codes_buffers_buffer_end, ANVIL__cell_ID include_standard, ANVIL__cell_ID debug_enabled, ANVIL__cell_ID generate_kickstarter, ANVIL__cell_ID output_program_start, ANVIL__cell_ID output_program_end, ANVIL__cell_ID output_debug_start, ANVIL__cell_ID output_debug_end, ANVIL__cell_ID error__error_occured, ANVIL__cell_ID error__message_start, ANVIL__cell_ID error__message_end, ANVIL__cell_ID error__character_location__file_index, ANVIL__cell_ID error__character_location__line_number, ANVIL__cell_ID error__character_location__character_index) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__compile);
    ANVIL__write_next__cell_ID(workspace, user_codes_buffers_buffer_start);
    ANVIL__write_next__cell_ID(workspace, user_codes_buffers_buffer_end);
    ANVIL__write_next__cell_ID(workspace, include_standard);
    ANVIL__write_next__cell_ID(workspace, debug_enabled);
    ANVIL__write_next__cell_ID(workspace, generate_kickstarter);
    ANVIL__write_next__cell_ID(workspace, output_program_start);
    ANVIL__write_next__cell_ID(workspace, output_program_end);
    ANVIL__write_next__cell_ID(workspace, output_debug_start);
    ANVIL__write_next__cell_ID(workspace, output_debug_end);
    ANVIL__write_next__cell_ID(workspace, error__error_occured);
    ANVIL__write_next__cell_ID(workspace, error__message_start);
    ANVIL__write_next__cell_ID(workspace, error__message_end);
    ANVIL__write_next__cell_ID(workspace, error__character_location__file_index);
    ANVIL__write_next__cell_ID(workspace, error__character_location__line_number);
    ANVIL__write_next__cell_ID(workspace, error__character_location__character_index);

    return;
}

// write run instruction
void ANVIL__code__run(ANVIL__workspace* workspace, ANVIL__cell_ID context_buffer_start, ANVIL__cell_ID context_buffer_end, ANVIL__cell_ID instruction_count) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__run);
    ANVIL__write_next__cell_ID(workspace, context_buffer_start);
    ANVIL__write_next__cell_ID(workspace, context_buffer_end);
    ANVIL__write_next__cell_ID(workspace, instruction_count);

    return;
}

// get time instruction
void ANVIL__code__get_time(ANVIL__workspace* workspace, ANVIL__cell_ID time_in_seconds, ANVIL__cell_ID time_in_nanoseconds) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__get_time);
    ANVIL__write_next__cell_ID(workspace, time_in_seconds);
    ANVIL__write_next__cell_ID(workspace, time_in_nanoseconds);

    return;
}

// write debug putchar instruction
void ANVIL__code__debug__putchar(ANVIL__workspace* workspace, ANVIL__cell_ID printing_cell_ID) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__putchar);
    ANVIL__write_next__cell_ID(workspace, printing_cell_ID);

    return;
}

// write debug fgets instruction
void ANVIL__code__debug__fgets(ANVIL__workspace* workspace, ANVIL__cell_ID buffer_start_ID, ANVIL__cell_ID buffer_end_ID) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__fgets);
    ANVIL__write_next__cell_ID(workspace, buffer_start_ID);
    ANVIL__write_next__cell_ID(workspace, buffer_end_ID);

    return;
}

// write debug mark data section instruction
void ANVIL__code__debug__mark_data_section(ANVIL__workspace* workspace, ANVIL__cell buffer_length) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__mark_data_section);
    ANVIL__write_next__cell(workspace, buffer_length);

    return;
}

// write debug mark code section instruction
void ANVIL__code__debug__mark_code_section(ANVIL__workspace* workspace, ANVIL__cell code_buffer_length) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__mark_code_section);
    ANVIL__write_next__cell(workspace, code_buffer_length);

    return;
}

// write debug get current context instruction
void ANVIL__code__debug__get_current_context(ANVIL__workspace* workspace, ANVIL__cell_ID buffer_start, ANVIL__cell_ID buffer_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__get_current_context);
    ANVIL__write_next__cell_ID(workspace, buffer_start);
    ANVIL__write_next__cell_ID(workspace, buffer_end);

    return;
}

// write debug search for allocation instruction
void ANVIL__code__debug__search_for_allocation(ANVIL__workspace* workspace, ANVIL__cell_ID source_buffer_start, ANVIL__cell_ID source_buffer_end, ANVIL__cell_ID was_found, ANVIL__cell_ID found_buffer_start, ANVIL__cell_ID found_buffer_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__search_for_allocation);
    ANVIL__write_next__cell_ID(workspace, source_buffer_start);
    ANVIL__write_next__cell_ID(workspace, source_buffer_end);
    ANVIL__write_next__cell_ID(workspace, was_found);
    ANVIL__write_next__cell_ID(workspace, found_buffer_start);
    ANVIL__write_next__cell_ID(workspace, found_buffer_end);

    return;
}

/* Stack ABI Defines */
// types
typedef u64 ANVIL__preserve;
typedef ANVIL__preserve ANVIL__preserve__start;
typedef ANVIL__preserve ANVIL__preserve__end;

// stack cell types
typedef enum ANVIL__srt {
    // start of cells
    ANVIL__srt__START = ANVIL__rt__END,

    // constant cells
    ANVIL__srt__constant__0 = ANVIL__srt__START,
    ANVIL__srt__constant__1,
    ANVIL__srt__constant__2,
    ANVIL__srt__constant__3,
    ANVIL__srt__constant__4,
    ANVIL__srt__constant__5,
    ANVIL__srt__constant__6,
    ANVIL__srt__constant__7,
    ANVIL__srt__constant__8,
    ANVIL__srt__constant__16,
    ANVIL__srt__constant__32,
    ANVIL__srt__constant__64,
    ANVIL__srt__constant__cell_byte_size,
    ANVIL__srt__constant__context_byte_size,
    ANVIL__srt__constant__return_address_offset_creation_size,

    // context io cells
    ANVIL__srt__input_buffer_start,
    ANVIL__srt__input_buffer_end,
    ANVIL__srt__output_result_buffer_start,
    ANVIL__srt__output_result_buffer_end,
    ANVIL__srt__output_error_buffer_start,
    ANVIL__srt__output_error_buffer_end,

    // stack cells
    ANVIL__srt__stack__start_address,
    ANVIL__srt__stack__end_address,
    ANVIL__srt__stack__current_address,

    // control flow cells
    ANVIL__srt__return_address,
    ANVIL__srt__always_run__flag_ID,
    ANVIL__srt__never_run__flag_ID,

    // anvil data cells
    ANVIL__srt__anvil_scraplet__instruction_ID__byte_size,
    ANVIL__srt__anvil_scraplet__flag_ID__byte_size,
    ANVIL__srt__anvil_scraplet__operation_ID__byte_size,
    ANVIL__srt__anvil_scraplet__cell_ID__byte_size,
    ANVIL__srt__anvil_scraplet__cell__byte_size,
    ANVIL__srt__anvil_scraplet__operation_ID_value__START,
    ANVIL__srt__anvil_scraplet__operation_ID_value__END = ANVIL__srt__anvil_scraplet__operation_ID_value__START + ANVIL__ot__COUNT,

    // temporary cells
    ANVIL__srt__temp__write = ANVIL__srt__anvil_scraplet__operation_ID_value__END,
    ANVIL__srt__temp__offset,
    ANVIL__srt__temp__address,
    ANVIL__srt__temp__flag,
    ANVIL__srt__temp__flag_ID_0,
    ANVIL__srt__temp__flag_ID_1,
    ANVIL__srt__temp__flag_ID_2,
    ANVIL__srt__temp__byte_count,
    ANVIL__srt__temp__length,
    ANVIL__srt__temp__buffer_0__start,
    ANVIL__srt__temp__buffer_0__end,
    ANVIL__srt__temp__buffer_1__start,
    ANVIL__srt__temp__buffer_1__end,
    ANVIL__srt__temp__cell_ID,

    // end of cells
    ANVIL__srt__END,

    // aliases
    ANVIL__srt__constant__true = ANVIL__srt__constant__1,
    ANVIL__srt__constant__false = ANVIL__srt__constant__0,
    ANVIL__srt__constant__always_run__value = ANVIL__srt__constant__true,
    ANVIL__srt__constant__never_run__value = ANVIL__srt__constant__false,
    ANVIL__srt__constant__bits_in_byte = ANVIL__srt__constant__8,
    ANVIL__srt__constant__ascii_character_byte_size = ANVIL__srt__constant__1,

    // locations
    ANVIL__srt__start__workspace = 8192,
    ANVIL__srt__start__function_io = ANVIL__rt__TOTAL_COUNT - 8192,

    // count
    ANVIL__srt__COUNT = ANVIL__srt__END - ANVIL__srt__START,
    ANVIL__srt__workspace__COUNT = ANVIL__srt__start__function_io - ANVIL__srt__start__workspace,
} ANVIL__srt;

// stack instruction types
typedef enum ANVIL__sit {
    // start
    ANVIL__sit__START = ANVIL__it__END,

    // instructions
    ANVIL__sit__cell_to_cell = ANVIL__sit__START,
    ANVIL__sit__push_cell,
    ANVIL__sit__pop_cell,
    ANVIL__sit__calculate_dynamically__offset_address,
    ANVIL__sit__calculate_statically__offset_address,
    ANVIL__sit__jump__explicit,
    ANVIL__sit__jump__static,
    ANVIL__sit__create_return_address__directly_after_jump,
    ANVIL__sit__call__explicit,
    ANVIL__sit__call__static,
    ANVIL__sit__start,
    ANVIL__sit__preserve_workspace,
    ANVIL__sit__restore_workspace,
    ANVIL__sit__operate__flag,
    ANVIL__sit__operate__jump__explicit,
    ANVIL__sit__operate__jump__dynamic,
    ANVIL__sit__operate__jump__static,
    ANVIL__sit__setup__context,

    // end
    ANVIL__sit__END,

    // counts
    ANVIL__sit__COUNT = ANVIL__sit__END - ANVIL__sit__START,
    ANVIL__sit__TOTAL_COUNT = ANVIL__sit__END - ANVIL__it__START,
} ANVIL__sit;

// stack instruction length types
typedef enum ANVIL__silt {
    ANVIL__silt__cell_to_cell = ANVIL__ilt__operate,
    ANVIL__silt__push_cell = ANVIL__ilt__cell_to_address + ANVIL__ilt__operate,
    ANVIL__silt__pop_cell = ANVIL__ilt__operate + ANVIL__ilt__address_to_cell,
    ANVIL__silt__calculate_dynamically__offset_address = ANVIL__ilt__operate,
    ANVIL__silt__calculate_statically__offset_address = ANVIL__ilt__write_cell + ANVIL__silt__calculate_dynamically__offset_address,
    ANVIL__silt__jump__explicit = ANVIL__silt__cell_to_cell,
    ANVIL__silt__jump__static = ANVIL__silt__calculate_statically__offset_address + ANVIL__silt__jump__explicit,
} ANVIL__silt;

// stack flag types
typedef enum ANVIL__sft {
    // start of flags
    ANVIL__sft__START = 0,

    // flags
    ANVIL__sft__always_run = ANVIL__sft__START,
    ANVIL__sft__never_run,
    ANVIL__sft__temp_0,
    ANIVL__sft__temp_1,
    ANIVL__sft__temp_2,

    // end of stack flags
    ANVIL__sft__END,
} ANVIL__sft;

// stack size
typedef u64 ANVIL__stack_size;

/* Context IO */
// pass input
void ANVIL__set__input(ANVIL__context* context, ANVIL__buffer input) {
    // write data to cells
    (*context).cells[ANVIL__srt__input_buffer_start] = (ANVIL__cell)input.start;
    (*context).cells[ANVIL__srt__input_buffer_end] = (ANVIL__cell)input.end;

    return;
}

/* Stack ABI Code */
// move one cell to the next
void ANVIL__code__cell_to_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID source, ANVIL__cell_ID destination) {
    // write code
    ANVIL__code__operate(workspace, flag, ANVIL__ot__cell_to_cell, source, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, destination);

    return;
}

// push a cell onto the stack
void ANVIL__code__push_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID source_cell) {
    // write data to stack
    ANVIL__code__cell_to_address(workspace, flag, source_cell, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__stack__current_address);

    // increase stack pointer
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__srt__stack__current_address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__stack__current_address);

    return;
}

// pop a cell from the stack
void ANVIL__code__pop_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID destination_cell) {
    // decrease stack pointer
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_subtract, ANVIL__srt__stack__current_address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__stack__current_address);

    // read data from stack
    ANVIL__code__address_to_cell(workspace, flag, ANVIL__srt__stack__current_address, ANVIL__srt__constant__cell_byte_size, destination_cell);

    return;
}

// calculate a buffer's length
void ANVIL__code__calculate__buffer_length(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID buffer_start, ANVIL__cell_ID buffer_end, ANVIL__cell_ID buffer_length) {
    // subtract end and start
	ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_subtract, buffer_end, buffer_start, ANVIL__unused_cell_ID, buffer_length);

    // convert difference to length
	ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, buffer_length, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, buffer_length);

    return;
}

// calculate a buffers end address from the start and length
void ANVIL__code__calculate__buffer_end_address(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID buffer_start, ANVIL__cell_ID buffer_length, ANVIL__cell_ID buffer_end) {
    // add length to value
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, buffer_start, buffer_length, ANVIL__unused_cell_ID, buffer_end);

    // adjust length to buffer end pointer
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_subtract, buffer_end, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, buffer_end);

    return;
}

// calculate an address from the program start and an offset cell
void ANVIL__code__calculate_dynamically__offset_address(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID offset_cell, ANVIL__cell_ID destination) {
    // calculate address
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__rt__program_start_address, offset_cell, ANVIL__unused_cell_ID, destination);

    return;
}

// calculate an address from the program start and an offset
void ANVIL__code__calculate_statically__offset_address(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__offset offset, ANVIL__cell_ID destination) {
    // write temp
    ANVIL__code__write_cell(workspace, (ANVIL__cell)offset, ANVIL__srt__temp__offset);

    // calculate address
    ANVIL__code__calculate_dynamically__offset_address(workspace, flag, ANVIL__srt__temp__offset, destination);

    return;
}

// jump to a specific address
void ANVIL__code__jump__explicit(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID address) {
    // jump
    ANVIL__code__cell_to_cell(workspace, flag, address, ANVIL__rt__program_current_address);

    return;
}

// jump to an offset calculated address
void ANVIL__code__jump__static(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__offset offset) {
    // calculate offset
    ANVIL__code__calculate_statically__offset_address(workspace, flag, offset, ANVIL__srt__temp__address);

    // jump
    ANVIL__code__jump__explicit(workspace, flag, ANVIL__srt__temp__address);

    return;
}

// create return address
void ANVIL__code__create_return_address__directly_after_jump(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID destination) {
    // create offset
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__rt__program_current_address, ANVIL__srt__constant__return_address_offset_creation_size, ANVIL__unused_cell_ID, destination);

    return;
}

// call function explicitly
void ANVIL__code__inner_call__explicit(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID address) {
    // preserve return address
    ANVIL__code__push_cell(workspace, flag, ANVIL__srt__return_address);

    // setup new return address
    ANVIL__code__create_return_address__directly_after_jump(workspace, flag, ANVIL__srt__return_address);

    // jump
    ANVIL__code__jump__explicit(workspace, flag, address);

    // restore return address
    ANVIL__code__pop_cell(workspace, flag, ANVIL__srt__return_address);

    return;
}

// call function statically
void ANVIL__code__inner_call__static(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__offset offset) {
    // calculate address
    ANVIL__code__calculate_statically__offset_address(workspace, flag, offset, ANVIL__srt__temp__address);

    // call function with offset set
    ANVIL__code__inner_call__explicit(workspace, flag, ANVIL__srt__temp__address);

    return;
}

// kickstart program (assumes program variables are set!)
void ANVIL__code__start(ANVIL__workspace* workspace, ANVIL__stack_size stack_size, ANVIL__offset jump_to) {
    // setup code marker
    ANVIL__code__debug__mark_code_section(workspace, 0);

    // setup error cell
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__et__no_error, ANVIL__rt__error_code);

    // setup address catcher (default is enabled)
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__bt__true, ANVIL__rt__address_catch_toggle);

    // setup flag cells
    ANVIL__code__write_cell(workspace, (ANVIL__cell)1, ANVIL__rt__flags_0);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__rt__flags_1);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__rt__flags_2);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__rt__flags_3);

    // setup constants
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__srt__constant__0);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)1, ANVIL__srt__constant__1);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)2, ANVIL__srt__constant__2);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)3, ANVIL__srt__constant__3);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)4, ANVIL__srt__constant__4);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)5, ANVIL__srt__constant__5);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)6, ANVIL__srt__constant__6);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)7, ANVIL__srt__constant__7);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)8, ANVIL__srt__constant__8);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)16, ANVIL__srt__constant__16);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)32, ANVIL__srt__constant__32);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)64, ANVIL__srt__constant__64);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__cell), ANVIL__srt__constant__cell_byte_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__context), ANVIL__srt__constant__context_byte_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__silt__jump__explicit, ANVIL__srt__constant__return_address_offset_creation_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__sft__always_run, ANVIL__srt__always_run__flag_ID);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__sft__never_run, ANVIL__srt__never_run__flag_ID);

    // setup anvil scraplet constants
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__instruction_ID), ANVIL__srt__anvil_scraplet__instruction_ID__byte_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__flag_ID), ANVIL__srt__anvil_scraplet__flag_ID__byte_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__operation_ID), ANVIL__srt__anvil_scraplet__operation_ID__byte_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__cell_ID), ANVIL__srt__anvil_scraplet__cell_ID__byte_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__cell), ANVIL__srt__anvil_scraplet__cell__byte_size);

    // generate operation ID scraplet constants
    for (ANVIL__cell_index operation_ID_list_start = ANVIL__srt__anvil_scraplet__operation_ID_value__START; operation_ID_list_start <= ANVIL__srt__anvil_scraplet__operation_ID_value__END; operation_ID_list_start++) {
        // write cell
        ANVIL__code__write_cell(workspace, (ANVIL__cell)(ANVIL__ot__START + operation_ID_list_start), operation_ID_list_start);
    }

    // setup outputs
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, ANVIL__srt__output_result_buffer_start);
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, ANVIL__srt__output_result_buffer_end);
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, ANVIL__srt__output_error_buffer_start);
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, ANVIL__srt__output_error_buffer_end);

    // setup stack
    ANVIL__code__write_cell(workspace, (ANVIL__cell)stack_size, ANVIL__srt__temp__write);
    ANVIL__code__request_memory(workspace, ANVIL__srt__temp__write, ANVIL__srt__stack__start_address, ANVIL__srt__stack__end_address);
    ANVIL__code__cell_to_cell(workspace, (ANVIL__flag_ID)ANVIL__sft__always_run, ANVIL__srt__stack__start_address, ANVIL__srt__stack__current_address);

    // jump to main
    ANVIL__code__inner_call__static(workspace, ANVIL__sft__always_run, jump_to);

    // deallocate stack
    ANVIL__code__return_memory(workspace, ANVIL__srt__stack__start_address, ANVIL__srt__stack__end_address);

    // quit program
    ANVIL__code__stop(workspace);

    return;
}

// preserve workspace
void ANVIL__code__preserve_workspace(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__preserve__start preserve_start, ANVIL__preserve__end preserve_end) {
    // preserve flags
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_0);
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_1);
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_2);
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_3);

    // preserve error code
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__error_code);

    // preserve workspace cells
    for (ANVIL__preserve i = preserve_start; i <= preserve_end; i++) {
        // preserve cell
        ANVIL__code__push_cell(workspace, flag, i);
    }

    return;
}

// restore workspace
void ANVIL__code__restore_workspace(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__preserve__start preserve_start, ANVIL__preserve__end preserve_end) {
    // restore workspace cells
    for (ANVIL__preserve i = preserve_end; i >= preserve_start; i--) {
        // restore cell
        ANVIL__code__pop_cell(workspace, flag, i);
    }

    // restore error code
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__error_code);

    // restore flags
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_3);
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_2);
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_1);
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_0);

    return;
}

// operate boolean
void ANVIL__code__operate__boolean(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID resulting_boolean) {
    // get comparison result set to variable
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_within_range, lower_bound, value, upper_bound, resulting_boolean);

    // invert boolean
    ANVIL__code__operate(workspace, invert_result, ANVIL__ot__bits_xor, resulting_boolean, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, resulting_boolean);

    return;
}

// operate flag
void ANVIL__code__operate__flag(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID output_flag_ID) {
    // get comparison result set to variable
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_within_range, lower_bound, value, upper_bound, ANVIL__srt__temp__flag);

    // write flag
    ANVIL__code__operate(workspace, flag, ANVIL__ot__flag_set, ANVIL__srt__temp__flag, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, output_flag_ID);

    // invert flag
    ANVIL__code__operate(workspace, invert_result, ANVIL__ot__flag_invert, output_flag_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, output_flag_ID);

    return;
}

// operate jump explicitly
void ANVIL__code__operate__jump__explicit(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID jump_address) {
    // setup flag temp
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__sft__temp_0, ANVIL__srt__temp__flag_ID_0);

    // perform comparison
    ANVIL__code__operate__flag(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__flag_ID_0);

    // attempt jump
    ANVIL__code__jump__explicit(workspace, ANVIL__sft__temp_0, jump_address);

    return;
}

// operate jump dynamically
void ANVIL__code__operate__jump__dynamic(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID jump_offset_cell) {
    // calculate offset
    ANVIL__code__calculate_dynamically__offset_address(workspace, flag, jump_offset_cell, ANVIL__srt__temp__address);

    // attempt jump
    ANVIL__code__operate__jump__explicit(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__address);

    return;
}

// operate jump statically
void ANVIL__code__operate__jump__static(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__offset jump_offset) {
    // write offset to cell
    ANVIL__code__write_cell(workspace, (ANVIL__cell)jump_offset, ANVIL__srt__temp__offset);

    // attempt jump
    ANVIL__code__operate__jump__dynamic(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__offset);

    return;
}

// retrieve an embedded buffer
void ANVIL__code__retrieve_embedded_buffer(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID program_offset, ANVIL__cell_ID buffer_start, ANVIL__cell_ID buffer_end) {
    // calculate buffer start
    ANVIL__code__calculate_dynamically__offset_address(workspace, flag, program_offset, ANVIL__srt__temp__address);

    // get buffer length
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__length), ANVIL__srt__temp__byte_count);
    ANVIL__code__address_to_cell(workspace, flag, ANVIL__srt__temp__address, ANVIL__srt__temp__byte_count, ANVIL__srt__temp__length);

    // calculate buffer data start
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__srt__temp__address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, buffer_start);

    // calculate buffer data end
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, buffer_start, ANVIL__srt__temp__length, ANVIL__unused_cell_ID, ANVIL__srt__temp__address);
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_subtract, ANVIL__srt__temp__address, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, buffer_end);

    return;
}

// setup context
void ANVIL__code__setup__context(ANVIL__workspace* workspace, ANVIL__cell_ID program_buffer_start, ANVIL__cell_ID program_buffer_end, ANVIL__cell_ID context_buffer_start, ANVIL__cell_ID context_buffer_end) {
    // setup allocation size
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__context), ANVIL__srt__temp__write);

    // allocate context
    ANVIL__code__request_memory(workspace, ANVIL__srt__temp__write, context_buffer_start, context_buffer_end);

    // setup skeleton context
    // setup buffer start
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, context_buffer_start, ANVIL__srt__temp__address);
    ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, program_buffer_start, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__temp__address);

    // setup current address
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, ANVIL__srt__temp__address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__temp__address);
    ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, program_buffer_end, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__temp__address);

    // setup end address
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, ANVIL__srt__temp__address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__temp__address);
    ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, program_buffer_start, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__temp__address);
    
    return;
}

// calculate buffer for cell range
void ANVIL__code__calculate__addresses_for_cell_range_from_context(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID range_start, ANVIL__cell_ID range_end, ANVIL__cell_ID buffer_start, ANVIL__cell_ID buffer_end) {
    // get cell address range from context
    ANVIL__code__debug__get_current_context(workspace, ANVIL__srt__temp__buffer_0__start, ANVIL__srt__temp__buffer_0__end);

    // setup starting address
    ANVIL__code__write_cell(workspace, (ANVIL__cell)(ANVIL__u64)range_start, ANVIL__srt__temp__cell_ID);
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_multiply, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__temp__cell_ID, ANVIL__unused_cell_ID, ANVIL__srt__temp__length);
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__srt__temp__buffer_0__start, ANVIL__srt__temp__length, ANVIL__unused_cell_ID, buffer_start);

    // setup end address
    ANVIL__code__write_cell(workspace, (ANVIL__cell)(ANVIL__u64)range_end + 1, ANVIL__srt__temp__cell_ID);
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_multiply, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__temp__cell_ID, ANVIL__unused_cell_ID, ANVIL__srt__temp__length);
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_subtract, ANVIL__srt__temp__length, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, ANVIL__srt__temp__length);
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__srt__temp__buffer_0__start, ANVIL__srt__temp__length, ANVIL__unused_cell_ID, buffer_end);

    return;
}

// debug, print buffer inline
void ANVIL__code__debug__print_buffer_inline(ANVIL__workspace* workspace, ANVIL__buffer buffer) {
    // print character by character
    for (ANVIL__u64 character = 0; character < ANVIL__calculate__buffer_length(buffer); character++) {
        // write character to cell
        ANVIL__code__write_cell(workspace, (ANVIL__cell)(ANVIL__u64)(((ANVIL__character*)buffer.start)[character]), ANVIL__srt__temp__write);

        // print character
        ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);
    }

    return;
}

#endif
