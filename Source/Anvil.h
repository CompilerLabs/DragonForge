#ifndef DRAGON__anvil
#define DRAGON__anvil

/* Include */
#include "DragonForge/Compiler/Compiler.h"

/* Allocations */
// allocations
typedef struct ANVIL__allocations {
    BASIC__list buffers; // BASIC__buffer
} ANVIL__allocations;

// find an allocation
BASIC__list_filled_index ANVIL__find__allocation(ANVIL__allocations* allocations, BASIC__buffer allocation, BASIC__bt* found) {
    BASIC__list_filled_index output = 0;
    BASIC__buffer current;
    
    // setup current
    current = BASIC__calculate__list_current_buffer(&((*allocations).buffers));

    // check for valid allocation
    while (current.start <= current.end) {
        // check one allocation
        if (BASIC__calculate__buffer_addresses_equal(*(BASIC__buffer*)current.start, allocation)) {
            // allocation is valid
            *found = BASIC__bt__true;

            return output;
        }

        // next allocation
        current.start += sizeof(BASIC__buffer);
        output += sizeof(BASIC__buffer);
    }

    // not valid
    *found = BASIC__bt__false;

    return output;
}

// add a buffer
void ANVIL__remember__allocation(ANVIL__allocations* allocations, BASIC__buffer allocation, BASIC__bt* list_error_occured) {
    // append allocation
    BASIC__list__append__buffer(&((*allocations).buffers), allocation, list_error_occured);

    return;
}

// remove a buffer
void ANVIL__forget__allocation(ANVIL__allocations* allocations, BASIC__buffer allocation, BASIC__bt* buffer_did_not_exist) {
    BASIC__bt found = BASIC__bt__false;

    // find allocation index
    BASIC__list_filled_index start_index = ANVIL__find__allocation(allocations, allocation, &found);

    // if found
    if (found == BASIC__bt__true) {
        // erase from list
        BASIC__list__erase__space(&((*allocations).buffers), start_index, start_index + sizeof(BASIC__buffer) - 1);

        // set error
        *buffer_did_not_exist = BASIC__bt__false;
    // not found, so not removed
    } else {
        // set error
        *buffer_did_not_exist = BASIC__bt__true;
    }

    return;
}

// search for and return a valid allocation buffer from another buffer
BASIC__buffer ANVIL__search__valid_allocation_from_sub_buffer(ANVIL__allocations* allocations, BASIC__buffer range, BASIC__bt* found) {
    BASIC__buffer current;
    
    // setup current
    current = BASIC__calculate__list_current_buffer(&(*allocations).buffers);

    // check for valid allocation range
    while (current.start <= current.end) {
        // check one allocation
        if (BASIC__calculate__buffer_range_in_buffer_range_inclusive(*(BASIC__buffer*)current.start, range)) {
            // allocation is valid
            *found = BASIC__bt__true;
            return *(BASIC__buffer*)current.start;
        }
        
        // next allocation
        current.start += sizeof(BASIC__buffer);
    }

    // allocation not valid
    *found = BASIC__bt__false;
    return BASIC__create_null__buffer();
}

// check to see if an address is valid
BASIC__bt ANVIL__check__valid_address_range_in_allocations(BASIC__bt catch_addresses, ANVIL__allocations* allocations, BASIC__buffer range) {
    BASIC__buffer current;

    // check if not catching addresses
    if (catch_addresses == BASIC__bt__false) {
        return BASIC__bt__true;
    }
    
    // setup current
    current = BASIC__calculate__list_current_buffer(&(*allocations).buffers);

    // check for valid allocation range
    while (current.start <= current.end) {
        // check one allocation
        if (BASIC__calculate__buffer_range_in_buffer_range_inclusive(*(BASIC__buffer*)current.start, range)) {
            // allocation is valid
            return BASIC__bt__true;
        }
        
        // next allocation
        current.start += sizeof(BASIC__buffer);
    }

    return BASIC__bt__false;
}

// open allocations
ANVIL__allocations BASIC__open__allocations(BASIC__bt* memory_error_occured) {
    ANVIL__allocations output;

    // setup output
    output.buffers = BASIC__open__list(sizeof(BASIC__buffer) * 256, memory_error_occured);

    return output;
}

// close allocations (does NOT clear actual allocations)
void BASIC__close__allocations(ANVIL__allocations* allocations) {
    // clean up
    BASIC__close__list((*allocations).buffers);

    return;
}

// print allocations
void ANVIL__print__allocations(ANVIL__allocations allocations) {
    // print header
    printf("Allocations:\n");

    // setup current
    BASIC__current current_allocation = BASIC__calculate__current_from_list_filled_index(&allocations.buffers);

    // for each allocation
    while (BASIC__check__current_within_range(current_allocation)) {
        // get allocation
        BASIC__buffer allocation = *(BASIC__buffer*)current_allocation.start;

        // print allocation
        printf("\t[ %lu %lu ] (%li)\n", (ANVIL__cell_integer_value)allocation.start, (ANVIL__cell_integer_value)allocation.end, (ANVIL__cell_integer_value)allocation.end - (ANVIL__cell_integer_value)allocation.start + 1);

        // next allocation
        current_allocation.start += sizeof(BASIC__buffer);
    }

    return;
}

/* Helper Functions */
// convert instruction type to instruction length type
ANVIL__ilt ANVIL__convert__it_to_ilt(ANVIL__it instruction) {
    ANVIL__ilt lengths[] = {
        ANVIL__ilt__stop,
        ANVIL__ilt__write_cell,
        ANVIL__ilt__operate,
        ANVIL__ilt__request_memory,
        ANVIL__ilt__return_memory,
        ANVIL__ilt__address_to_cell,
        ANVIL__ilt__cell_to_address,
        ANVIL__ilt__file_to_buffer,
        ANVIL__ilt__buffer_to_file,
        ANVIL__ilt__delete_file,
        ANVIL__ilt__buffer_to_buffer__low_to_high,
        ANVIL__ilt__buffer_to_buffer__high_to_low,
        ANVIL__ilt__compile,
        ANVIL__ilt__run,
        ANVIL__ilt__get_time,
        ANVIL__ilt__debug__putchar,
        ANVIL__ilt__debug__fgets,
        ANVIL__ilt__debug__mark_data_section,
        ANVIL__ilt__debug__mark_code_section,
        ANVIL__ilt__debug__get_current_context,
        ANVIL__ilt__debug__search_for_allocation,
    };

    return lengths[instruction];
}

// get a pointer from a context to a cell inside that context
ANVIL__cell* ANVIL__get__cell_address_from_context(ANVIL__context* context, ANVIL__cell_ID cell_ID) {
    // return data
    return &((*context).cells[cell_ID]);
}

// get cell value from context
ANVIL__cell ANVIL__get__cell_from_context(ANVIL__context* context, ANVIL__cell_ID cell_ID) {
    // return data
    return *ANVIL__get__cell_address_from_context(context, cell_ID);
}

// set cell value by pointer
void ANVIL__set__cell_by_address(ANVIL__cell* destination_cell, ANVIL__cell value) {
    // set value
    *destination_cell = value;

    return;
}

// set value in error code cell
void ANVIL__set__error_code_cell(ANVIL__context* context, ANVIL__et error_code) {
    // set error
    ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, ANVIL__rt__error_code), (ANVIL__cell)error_code);

    return;
}

// get flag (value)
BASIC__bt ANVIL__get__flag_from_context(ANVIL__context* context, ANVIL__flag_ID flag_ID) {
    BASIC__u8 masks[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    // return bit
    return (BASIC__bt)((((BASIC__u8*)&((*context).cells[ANVIL__rt__flags_0]))[flag_ID / 8] & masks[flag_ID % 8]) > 0);
}

// set flag (value)
void ANVIL__set__flag_in_context(ANVIL__context* context, ANVIL__flag_ID flag_ID, BASIC__bt value) {
    BASIC__u8 masks[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    // clear bit
    ((((BASIC__u8*)&((*context).cells[ANVIL__rt__flags_0]))[flag_ID / 8] &= (~masks[flag_ID % 8])));

    // set bit
    ((BASIC__u8*)&((*context).cells[ANVIL__rt__flags_0]))[flag_ID / 8] |= (value << (flag_ID % 8));

    return;
}

// read item from buffer and advance read address
BASIC__u64 ANVIL__read_next__buffer_item(ANVIL__cell* address, BASIC__byte_count byte_count) {
    BASIC__u64 output;

    // read data from buffer
    output = BASIC__read__buffer(*address, byte_count);

    // advance pointer
    *address = (BASIC__address)(*((u8**)address) + byte_count);

    // return data
    return output;
}

// get cell value and advance
ANVIL__cell ANVIL__read_next__cell(ANVIL__cell* address) {
    // read data
    return (ANVIL__cell)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__cell));
}

// get instruction ID and advance
ANVIL__instruction_ID ANVIL__read_next__instruction_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__instruction_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__instruction_ID));
}

// get flag ID and advance
ANVIL__flag_ID ANVIL__read_next__flag_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__flag_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__flag_ID));
}

// get operation ID and advance
ANVIL__operation_ID ANVIL__read_next__operation_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__operation_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__operation_ID));
}

// get cell ID and advance
ANVIL__cell_ID ANVIL__read_next__cell_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__cell_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__cell_ID));
}

/* Setup Alloy Code */
// create a skeleton context
ANVIL__context ANVIL__setup__context(BASIC__buffer program) {
    ANVIL__context output;

    // setup program and execution cells
    output.cells[ANVIL__rt__program_start_address] = (ANVIL__cell)program.start;
    output.cells[ANVIL__rt__program_end_address] = (ANVIL__cell)program.end;
    output.cells[ANVIL__rt__program_current_address] = output.cells[ANVIL__rt__program_start_address];

    return output;
}

/* Run Alloy Code */
// predefined run context header
void ANVIL__run__context(ANVIL__allocations* allocations, ANVIL__context* context, ANVIL__instruction_count instruction_count);

// check compiler instruction inputs
BASIC__bt COMPILER__check__valid_compilation_instruction_inputs(ANVIL__allocations* allocations, BASIC__bt catch_addresses, BASIC__buffer input_buffer) {
    // don't validate if catch addresses is disabled
    if (catch_addresses == BASIC__bt__false) {
        // ignore validation
        return BASIC__bt__true;
    }

    // check for valid buffer of buffers
    // check for allocation
    if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, input_buffer) == BASIC__bt__false) {
        // invalid
        return BASIC__bt__false;
    }

    // check for valid buffer of buffers
    // check size
    if ((BASIC__calculate__buffer_length(input_buffer) % sizeof(BASIC__buffer)) != 0) {
        // invalid
        return BASIC__bt__false;
    }

    // check each user code buffer for address validity
    // setup current
    BASIC__current current = input_buffer;

    // for each sub buffer
    while (BASIC__check__current_within_range(current) == BASIC__bt__true) {
        // get allocation
        BASIC__buffer temp_buffer = *(BASIC__buffer*)current.start;

        // validate buffer
        BASIC__bt valid = ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, temp_buffer);
        if (valid == BASIC__bt__false) {
            // invalid buffer
            return BASIC__bt__false;
        }

        // next allocation
        current.start += sizeof(BASIC__buffer);
    }

    // no issues found, validated
    return BASIC__bt__true;
}

// process operation (assumes flag was checked)
ANVIL__nit ANVIL__run__operation(ANVIL__context* context, ANVIL__ot operation_type, ANVIL__cell_ID input_0, ANVIL__cell_ID input_1, ANVIL__cell_ID input_2, ANVIL__cell_ID output_0) {
    ANVIL__cell_integer_value temp_input_0;
    ANVIL__cell_integer_value temp_input_1;
    ANVIL__cell_integer_value temp_input_2;
    ANVIL__cell_integer_value temp_result;

    // do operation based on type
    switch (operation_type) {
    // cell to cell
    case ANVIL__ot__cell_to_cell:
        // set value
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), ANVIL__get__cell_from_context(context, input_0));

        break;
    // binary or
    case ANVIL__ot__bits_or:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 | temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary invert
    case ANVIL__ot__bits_invert:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);

        // perform operation
        temp_result = ~temp_input_0;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary and
    case ANVIL__ot__bits_and:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 & temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary xor
    case ANVIL__ot__bits_xor:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 ^ temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary bit shift higher
    case ANVIL__ot__bits_shift_higher:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 << temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary bit shift lower
    case ANVIL__ot__bits_shift_lower:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 >> temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary overwrite bits
    case ANVIL__ot__bits_overwrite:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // mask (positive bits are the ones being overwritten!)
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // old bits
        temp_input_2 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_2); // new bits

        // perform operation
        temp_result = (~temp_input_0) & temp_input_1;
        temp_result = temp_result | (temp_input_2 & temp_input_0);

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer addition
    case ANVIL__ot__integer_add:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 + temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer subtraction
    case ANVIL__ot__integer_subtract:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 - temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer multiplication
    case ANVIL__ot__integer_multiply:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 * temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer division
    case ANVIL__ot__integer_division:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // check if division is possible
        if (temp_input_1 != 0) {
            // compute result
            temp_result = temp_input_0 / temp_input_1;
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__divide_by_zero);

            // set blank output
            temp_result = 0;
        }

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer modulous
    case ANVIL__ot__integer_modulous:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // check if modulous is possible
        if (temp_input_1 != 0) {
            // compute result
            temp_result = temp_input_0 % temp_input_1;
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__modulous_by_zero);

            // set blank output
            temp_result = 0;
        }

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer range check
    case ANVIL__ot__integer_within_range:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // range start
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // value to be checked
        temp_input_2 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_2); // range end

        // check range
        temp_result = (ANVIL__cell_integer_value)((temp_input_0 <= temp_input_1) && (temp_input_1 <= temp_input_2));

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // flag or
    case ANVIL__ot__flag_or:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // second flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // 'or' flags into new flag
        ANVIL__set__flag_in_context(context, temp_result, (BASIC__bt)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) | ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_1)));

        break;
    // flag invert
    case ANVIL__ot__flag_invert:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // invert flag
        ANVIL__set__flag_in_context(context, temp_result, (BASIC__bt)!(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0)));

        break;
    // flag and
    case ANVIL__ot__flag_and:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // second flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // 'and' flags into new flag
        ANVIL__set__flag_in_context(context, temp_result, (BASIC__bt)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) & ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_1)));

        break;
    // flag xor
    case ANVIL__ot__flag_xor:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // second flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // 'xor' flags into new flag
        ANVIL__set__flag_in_context(context, temp_result, (BASIC__bt)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) != ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_1)));

        break;
    // flag get
    case ANVIL__ot__flag_get:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // flag address

        // get flag
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)(BASIC__u64)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) > 0));

        break;
    // flag set
    case ANVIL__ot__flag_set:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // flag value
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // destination flag ID

        // set flag
        ANVIL__set__flag_in_context(context, (ANVIL__flag_ID)temp_result, (BASIC__bt)temp_input_0);

        break;
    // operation ID was invalid
    default:
        // return failed instruction
        return ANVIL__nit__return_context;
    }

    return ANVIL__nit__next_instruction;
}

// process instruction
ANVIL__nit ANVIL__run__instruction(ANVIL__allocations* allocations, ANVIL__context* context) {
    // execution current read address
    ANVIL__cell* execution_read_address;

    // address catching toggle
    BASIC__bt catch_addresses;

    // instruction ID
    ANVIL__it instruction_ID;

    // write cell temps
    ANVIL__cell write_cell__cell;
    ANVIL__cell_ID write_cell__cell_ID;

    // operate temps
    ANVIL__flag_ID operate__flag_ID;
    ANVIL__operation_ID operate__operation_ID;
    ANVIL__cell_ID operate__input_cell_ID_0;
    ANVIL__cell_ID operate__input_cell_ID_1;
    ANVIL__cell_ID operate__input_cell_ID_2;
    ANVIL__cell_ID operate__output_cell_ID_0;

    // request memory temps
    ANVIL__cell_ID request_memory__allocation_size;
    ANVIL__cell_ID request_memory__allocation_start;
    ANVIL__cell_ID request_memory__allocation_end;
    BASIC__buffer request_memory__allocation;
    BASIC__bt request_memory__buffer_appending_error = BASIC__bt__false;

    // return memory temps
    ANVIL__cell_ID return_memory__allocation_start;
    ANVIL__cell_ID return_memory__allocation_end;
    BASIC__buffer return_memory__allocation;
    BASIC__bt return_memory__allocation_does_not_exist = BASIC__bt__false;

    // address to cell temps
    ANVIL__flag_ID address_to_cell__flag_ID;
    BASIC__length address_to_cell__source_cell_ID;
    BASIC__length address_to_cell__byte_count_cell_ID;
    BASIC__length address_to_cell__destination_cell_ID;

    // cell to address temps
    ANVIL__flag_ID cell_to_address__flag_ID;
    BASIC__length cell_to_address__source_cell_ID;
    BASIC__length cell_to_address__byte_count_cell_ID;
    BASIC__length cell_to_address__destination_cell_ID;

    // file to buffer temps
    ANVIL__cell_ID file_to_buffer__file_name_start;
    ANVIL__cell_ID file_to_buffer__file_name_end;
    ANVIL__cell_ID file_to_buffer__file_data_start;
    ANVIL__cell_ID file_to_buffer__file_data_end;
    BASIC__buffer file_to_buffer__file_name;
    BASIC__buffer file_to_buffer__file_data;
    BASIC__bt file_to_buffer__buffer_appending_error = BASIC__bt__false;

    // buffer to file temps
    ANVIL__cell_ID buffer_to_file__file_data_start;
    ANVIL__cell_ID buffer_to_file__file_data_end;
    ANVIL__cell_ID buffer_to_file__file_name_start;
    ANVIL__cell_ID buffer_to_file__file_name_end;
    BASIC__buffer buffer_to_file__file_data;
    BASIC__buffer buffer_to_file__file_name;
    BASIC__bt buffer_to_file__error = BASIC__bt__false;

    // delete file temps
    ANVIL__cell_ID delete_file__file_path_start;
    ANVIL__cell_ID delete_file__file_path_end;
    BASIC__buffer delete_file__file_path;
    BASIC__bt delete_file__error = BASIC__bt__false;

    // buffer to buffer temps
    ANVIL__cell_ID buffer_to_buffer__source_start;
    ANVIL__cell_ID buffer_to_buffer__source_end;
    ANVIL__cell_ID buffer_to_buffer__destination_start;
    ANVIL__cell_ID buffer_to_buffer__destination_end;
    BASIC__buffer buffer_to_buffer__source;
    BASIC__buffer buffer_to_buffer__destination;
    BASIC__bt buffer_to_buffer__error = BASIC__bt__false;

    // compile temps
    ANVIL__cell_ID compile__user_code_buffers_buffer_start;
    ANVIL__cell_ID compile__user_code_buffers_buffer_end;
    ANVIL__cell_ID compile__include_standard;
    ANVIL__cell_ID compile__debug_enabled;
    ANVIL__cell_ID compile__generate_kickstarter;
    ANVIL__cell_ID compile__output_start;
    ANVIL__cell_ID compile__output_end;
    ANVIL__cell_ID compile__debug_start;
    ANVIL__cell_ID compile__debug_end;
    ANVIL__cell_ID compile__error__occured;
    ANVIL__cell_ID compile__error__message_start;
    ANVIL__cell_ID compile__error__message_end;
    ANVIL__cell_ID compile__error__character_location__file_index;
    ANVIL__cell_ID compile__error__character_location__line_number;
    ANVIL__cell_ID compile__error__character_location__character_index;
    BASIC__buffer compile__output_program;
    BASIC__buffer compile__debug_information;
    BASIC__buffer compile__output_error_message;
    COMPILER__error compile__error;
    BASIC__bt compile__buffer_appending_error = BASIC__bt__false;

    // run temps
    ANVIL__cell_ID run__context_buffer_start;
    ANVIL__cell_ID run__context_buffer_end;
    ANVIL__cell_ID run__instruction_count;

    // get time temps
    ANVIL__cell_ID get_time__seconds;
    ANVIL__cell_ID get_time__nanoseconds;
    struct timespec get_time__data_destination;

    // debug putchar temps
    ANVIL__cell_ID debug__putchar__printing_cell_ID;

    // debug fgets temps
    ANVIL__cell_ID debug__fgets__buffer_address_start;
    ANVIL__cell_ID debug__fgets__buffer_address_end;
    BASIC__u8 debug__fgets__temporary_string[ANVIL__define__input_string_max_length];
    BASIC__buffer debug__fgets__buffer;
    BASIC__length debug__fgets__buffer_length;
    BASIC__bt debug__fgets__buffer_appending_error = BASIC__bt__false;

    // debug mark data section temps
    ANVIL__cell debug__mark_data_section__section_length;

    // debug mark code section temps
    ANVIL__cell debug__mark_code_section__section_length;

    // debug get current context
    ANVIL__cell_ID get_current_context__buffer_start;
    ANVIL__cell_ID get_current_context__buffer_end;

    // debug search for allocation
    ANVIL__cell_ID search_for_allocation__source_buffer_start;
    ANVIL__cell_ID search_for_allocation__source_buffer_end;
    BASIC__buffer search_for_allocation__source_buffer;
    ANVIL__cell_ID search_for_allocation__was_found;
    BASIC__bt search_for_allocation__was_found_boolean;
    ANVIL__cell_ID search_for_allocation__found_buffer_start;
    ANVIL__cell_ID search_for_allocation__found_buffer_end;
    BASIC__buffer search_for_allocation__found_buffer;

    // setup address catcher toggle
    catch_addresses = (BASIC__bt)(BASIC__u64)ANVIL__get__cell_from_context(context, ANVIL__rt__address_catch_toggle);

    // setup execution read address
    execution_read_address = ANVIL__get__cell_address_from_context(context, ANVIL__rt__program_current_address);

    // check for no more instructions
    if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, BASIC__create__buffer((*context).cells[ANVIL__rt__program_current_address], (*context).cells[ANVIL__rt__program_current_address] + sizeof(ANVIL__it) - 1)) == BASIC__bt__false) {
        // set error code
        ANVIL__set__error_code_cell(context, ANVIL__et__program_ran_out_of_instructions);

        // allocation does not exist, quit
        return ANVIL__nit__return_context;
    }

    // get instruction ID from program
    instruction_ID = (ANVIL__it)ANVIL__read_next__instruction_ID(execution_read_address);

    // check for no more instruction parameters
    if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, BASIC__create__buffer((*context).cells[ANVIL__rt__program_current_address], (*context).cells[ANVIL__rt__program_current_address] + ANVIL__convert__it_to_ilt(instruction_ID) - sizeof(ANVIL__it) - 1)) == BASIC__bt__false) {
        // set error code
        ANVIL__set__error_code_cell(context, ANVIL__et__program_ran_out_of_instruction_parameters);

        // allocation does not exist, quit
        return ANVIL__nit__return_context;
    }

    // DEBUG
    // printf("[%lu]: instruction_ID: %lu\n", (BASIC__u64)(*execution_read_address) - 1, (BASIC__u64)instruction_ID);

    // check for valid instruction ID
    if (instruction_ID >= ANVIL__it__COUNT) {
        // set error
        ANVIL__set__error_code_cell(context, ANVIL__et__invalid_instruction_ID);

        // return exit context
        return ANVIL__nit__return_context;
    }

    // process instruction accordingly
    switch (instruction_ID) {
    // if context should stop
    case ANVIL__it__stop:
        // return exit context
        return ANVIL__nit__return_context;
    // overwrite cell value
    case ANVIL__it__write_cell:
        // get parameters
        write_cell__cell = ANVIL__read_next__cell(execution_read_address);
        write_cell__cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // do action
        (*context).cells[write_cell__cell_ID] = write_cell__cell;

        break;
    // operate between cells
    case ANVIL__it__operate:
        // get parameters
        operate__flag_ID = ANVIL__read_next__flag_ID(execution_read_address);
        operate__operation_ID = ANVIL__read_next__operation_ID(execution_read_address);
        operate__input_cell_ID_0 = ANVIL__read_next__cell_ID(execution_read_address);
        operate__input_cell_ID_1 = ANVIL__read_next__cell_ID(execution_read_address);
        operate__input_cell_ID_2 = ANVIL__read_next__cell_ID(execution_read_address);
        operate__output_cell_ID_0 = ANVIL__read_next__cell_ID(execution_read_address);

        // if flag enabled
        if (ANVIL__get__flag_from_context(context, operate__flag_ID) == BASIC__bt__true) {
            // perform operation
            return ANVIL__run__operation(context, (ANVIL__ot)operate__operation_ID, operate__input_cell_ID_0, operate__input_cell_ID_1, operate__input_cell_ID_2, operate__output_cell_ID_0);
        }

        break;
    // ask os for new buffer
    case ANVIL__it__request_memory:
        // get parameters
        request_memory__allocation_size = ANVIL__read_next__cell_ID(execution_read_address);
        request_memory__allocation_start = ANVIL__read_next__cell_ID(execution_read_address);
        request_memory__allocation_end = ANVIL__read_next__cell_ID(execution_read_address);

        // do action
        if ((BASIC__length)(*context).cells[request_memory__allocation_size] <= ANVIL__define__max_allocation_size) {
            // allocate
            request_memory__allocation = BASIC__open__buffer((BASIC__length)(*context).cells[request_memory__allocation_size]);
        } else {
            // set buffer
            request_memory__allocation = BASIC__create_null__buffer();
            
            // set internal error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_allocation__requested_memory_too_large);
        }

        // add allocation if successful
        if (BASIC__check__empty_buffer(request_memory__allocation) == BASIC__bt__false) {
            // remember allocation
            ANVIL__remember__allocation(allocations, request_memory__allocation, &request_memory__buffer_appending_error);

            // if buffer could not be added
            if (request_memory__buffer_appending_error == BASIC__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                BASIC__close__buffer(request_memory__allocation);

                // setup empty buffer
                request_memory__allocation = BASIC__create_null__buffer();
            }
        }

        // set cell data
        (*context).cells[request_memory__allocation_start] = request_memory__allocation.start;
        (*context).cells[request_memory__allocation_end] = request_memory__allocation.end;

        break;
    // return buffer to OS
    case ANVIL__it__return_memory:
        // get parameters
        return_memory__allocation_start = ANVIL__read_next__cell_ID(execution_read_address);
        return_memory__allocation_end = ANVIL__read_next__cell_ID(execution_read_address);

        // get parameters
        return_memory__allocation.start = (*context).cells[return_memory__allocation_start];
        return_memory__allocation.end = (*context).cells[return_memory__allocation_end];

        // remove buffer from valid allocation list
        ANVIL__forget__allocation(allocations, return_memory__allocation, &return_memory__allocation_does_not_exist);

        // if allocation existed
        if (return_memory__allocation_does_not_exist == BASIC__bt__false) {
            // deallocate
            BASIC__close__buffer(return_memory__allocation);
        // allocation did not exist
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // take data from an address and put it into a cell
    case ANVIL__it__address_to_cell:
        // get parameters
        address_to_cell__flag_ID = ANVIL__read_next__flag_ID(execution_read_address);
        address_to_cell__source_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        address_to_cell__byte_count_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        address_to_cell__destination_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // if valid address range
        if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, BASIC__create__buffer((BASIC__address)(*context).cells[address_to_cell__source_cell_ID], (BASIC__address)(*context).cells[address_to_cell__source_cell_ID] + (((BASIC__length)(*context).cells[address_to_cell__byte_count_cell_ID])) - 1))) {
            // if flag enabled
            if (ANVIL__get__flag_from_context(context, address_to_cell__flag_ID) == BASIC__bt__true) {
                // read data into cell
                (*context).cells[address_to_cell__destination_cell_ID] = (ANVIL__cell)BASIC__read__buffer((BASIC__address)(*context).cells[address_to_cell__source_cell_ID], ((BASIC__length)(*context).cells[address_to_cell__byte_count_cell_ID]));
            }
        // invalid address range
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_address_range);

            // setup blank value
            (*context).cells[address_to_cell__destination_cell_ID] = BASIC__define__null_address;
        }

        break;
    // take data from a cell and put it at an address
    case ANVIL__it__cell_to_address:
        // get parameters
        cell_to_address__flag_ID = ANVIL__read_next__flag_ID(execution_read_address);
        cell_to_address__source_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        cell_to_address__byte_count_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        cell_to_address__destination_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // if valid address range
        if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, BASIC__create__buffer((*context).cells[cell_to_address__destination_cell_ID], (*context).cells[cell_to_address__destination_cell_ID] + (((BASIC__length)(*context).cells[cell_to_address__byte_count_cell_ID])) - 1))) {
            // if flag enabled
            if (ANVIL__get__flag_from_context(context, cell_to_address__flag_ID) == BASIC__bt__true) {
                // write data to an address
                BASIC__write__buffer((BASIC__u64)(*context).cells[cell_to_address__source_cell_ID], ((BASIC__length)(*context).cells[cell_to_address__byte_count_cell_ID]), (*context).cells[cell_to_address__destination_cell_ID]);
            }
        // invalid address range
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_address_range);
        }

        break;
    // take data from a file and create a buffer with it
    case ANVIL__it__file_to_buffer:
        // get parameters
        file_to_buffer__file_name_start = ANVIL__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_name_end = ANVIL__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_data_start = ANVIL__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_data_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup temps
        file_to_buffer__file_name.start = (*context).cells[file_to_buffer__file_name_start];
        file_to_buffer__file_name.end = (*context).cells[file_to_buffer__file_name_end];

        // get data from file
        file_to_buffer__file_data = BASIC__move__file_to_buffer(file_to_buffer__file_name);

        // check for errors
        if (file_to_buffer__file_data.start == BASIC__define__null_address) {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__file_not_found);
        // append buffer as valid allocation
        } else {
            // remember allocation
            ANVIL__remember__allocation(allocations, file_to_buffer__file_data, &file_to_buffer__buffer_appending_error);

            // if buffer could not be added
            if (file_to_buffer__buffer_appending_error == BASIC__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                BASIC__close__buffer(file_to_buffer__file_data);

                // setup empty buffer
                file_to_buffer__file_data = BASIC__create_null__buffer();
            }
        }

        // write data to cells
        (*context).cells[file_to_buffer__file_data_start] = file_to_buffer__file_data.start;
        (*context).cells[file_to_buffer__file_data_end] = file_to_buffer__file_data.end;

        break;
    // take a buffer and overwrite a file with it
    case ANVIL__it__buffer_to_file:
        // get parameters
        buffer_to_file__file_data_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_data_end = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_name_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_name_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup temps
        buffer_to_file__file_data.start = (*context).cells[buffer_to_file__file_data_start];
        buffer_to_file__file_data.end = (*context).cells[buffer_to_file__file_data_end];
        buffer_to_file__file_name.start = (*context).cells[buffer_to_file__file_name_start];
        buffer_to_file__file_name.end = (*context).cells[buffer_to_file__file_name_end];

        // if source allocations exists
        if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_file__file_data) && ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_file__file_name)) {
            // create file
            BASIC__move__buffer_to_file(&buffer_to_file__error, buffer_to_file__file_name, buffer_to_file__file_data);

            // check for errors
            if (buffer_to_file__error == BASIC__bt__true) {
                // set error
                ANVIL__set__error_code_cell(context, ANVIL__et__file_not_created);
            }
        // if any allocations do not exist
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // take a file path and delete it
    case ANVIL__it__delete_file:
        // get parameters
        delete_file__file_path_start = ANVIL__read_next__cell_ID(execution_read_address);
        delete_file__file_path_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup temps
        delete_file__file_path.start = (*context).cells[delete_file__file_path_start];
        delete_file__file_path.end = (*context).cells[delete_file__file_path_end];

        // remove file
        BASIC__delete__file(&delete_file__error, delete_file__file_path);

        // check for error
        if (delete_file__error == BASIC__bt__true) {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__file_not_found);
        }

        break;
    // copy one buffer of data from one area to another of equal size copying bytes low to high
    case ANVIL__it__buffer_to_buffer__low_to_high:
        // get parameters
        buffer_to_buffer__source_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__source_end = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup temps
        buffer_to_buffer__source.start = (*context).cells[buffer_to_buffer__source_start];
        buffer_to_buffer__source.end = (*context).cells[buffer_to_buffer__source_end];
        buffer_to_buffer__destination.start = (*context).cells[buffer_to_buffer__destination_start];
        buffer_to_buffer__destination.end = (*context).cells[buffer_to_buffer__destination_end];

        // if both allocations exist
        if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__source) && ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__destination)) {
            // perform copy
            BASIC__copy__buffer(buffer_to_buffer__source, buffer_to_buffer__destination, &buffer_to_buffer__error);

            // check for error
            if (buffer_to_buffer__error == BASIC__bt__true) {
                ANVIL__set__error_code_cell(context, ANVIL__et__buffer_to_buffer__buffers_are_different_sizes);
            }
        // if one allocation does not exist
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // copy one buffer of data from one area to another of equal size copying bytes low to high
    case ANVIL__it__buffer_to_buffer__high_to_low:
        // get parameters
        buffer_to_buffer__source_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__source_end = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup temps
        buffer_to_buffer__source.start = (*context).cells[buffer_to_buffer__source_start];
        buffer_to_buffer__source.end = (*context).cells[buffer_to_buffer__source_end];
        buffer_to_buffer__destination.start = (*context).cells[buffer_to_buffer__destination_start];
        buffer_to_buffer__destination.end = (*context).cells[buffer_to_buffer__destination_end];

        // if both allocations exist
        if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__source) && ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__destination)) {
            // perform copy
            BASIC__copy__buffer__backwards(buffer_to_buffer__source, buffer_to_buffer__destination, &buffer_to_buffer__error);

            // check for error
            if (buffer_to_buffer__error == BASIC__bt__true) {
                ANVIL__set__error_code_cell(context, ANVIL__et__buffer_to_buffer__buffers_are_different_sizes);
            }
        // if one allocation does not exist
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // compile program
    case ANVIL__it__compile:
        // get parameters
        compile__user_code_buffers_buffer_start = ANVIL__read_next__cell_ID(execution_read_address);
        compile__user_code_buffers_buffer_end = ANVIL__read_next__cell_ID(execution_read_address);
        compile__include_standard = ANVIL__read_next__cell_ID(execution_read_address);
        compile__debug_enabled = ANVIL__read_next__cell_ID(execution_read_address);
        compile__generate_kickstarter = ANVIL__read_next__cell_ID(execution_read_address);
        compile__output_start = ANVIL__read_next__cell_ID(execution_read_address);
        compile__output_end = ANVIL__read_next__cell_ID(execution_read_address);
        compile__debug_start = ANVIL__read_next__cell_ID(execution_read_address);
        compile__debug_end = ANVIL__read_next__cell_ID(execution_read_address);
        compile__error__occured = ANVIL__read_next__cell_ID(execution_read_address);
        compile__error__message_start = ANVIL__read_next__cell_ID(execution_read_address);
        compile__error__message_end = ANVIL__read_next__cell_ID(execution_read_address);
        compile__error__character_location__file_index = ANVIL__read_next__cell_ID(execution_read_address);
        compile__error__character_location__line_number = ANVIL__read_next__cell_ID(execution_read_address);
        compile__error__character_location__character_index = ANVIL__read_next__cell_ID(execution_read_address);

        // null init error
        compile__error = COMPILER__create_null__error();

        // validate input buffers
        // validate
        BASIC__bt compile__valid_inputs = COMPILER__check__valid_compilation_instruction_inputs(
            allocations,
            catch_addresses,
            BASIC__create__buffer(
                (*context).cells[compile__user_code_buffers_buffer_start],
                (*context).cells[compile__user_code_buffers_buffer_end]
            )
        );

        // if valid
        if (compile__valid_inputs == BASIC__bt__true) {
            // run compiler
            COMPILER__compile__files(
                BASIC__create__buffer(
                    (*context).cells[compile__user_code_buffers_buffer_start],
                    (*context).cells[compile__user_code_buffers_buffer_end]
                ),
                (BASIC__bt)(ANVIL__cell_integer_value)(*context).cells[compile__include_standard],
                (BASIC__bt)(ANVIL__cell_integer_value)(*context).cells[compile__generate_kickstarter],
                (BASIC__bt)(ANVIL__cell_integer_value)(*context).cells[compile__debug_enabled],
                (BASIC__bt)(ANVIL__cell_integer_value)(*context).cells[compile__debug_enabled],
                &compile__output_program,
                &compile__debug_information,
                &compile__error
            );
        // invalid
        } else {
            // set internal error
            ANVIL__set__error_code_cell(context, ANVIL__et__compile__invalid_input_configuration);

            // zero out everything
            (*context).cells[compile__output_start] = BASIC__define__null_address;
            (*context).cells[compile__output_end] = BASIC__define__null_address;
            (*context).cells[compile__error__occured] = (ANVIL__cell)(ANVIL__cell_integer_value)BASIC__bt__true;
            (*context).cells[compile__error__message_start] = BASIC__define__null_address;
            (*context).cells[compile__error__message_end] = BASIC__define__null_address;
            (*context).cells[compile__error__character_location__file_index] = (ANVIL__cell)BASIC__define__null_file_index_ID;
            (*context).cells[compile__error__character_location__line_number] = (ANVIL__cell)BASIC__define__null_address;
            (*context).cells[compile__error__character_location__character_index] = (ANVIL__cell)BASIC__define__null_address;

            break;
        }

        // get temps
        compile__output_error_message = compile__error.message;

        // check if program was created
        if (COMPILER__check__error_occured(&compile__error) == BASIC__bt__false) {
            // add new buffer to allocations
            ANVIL__remember__allocation(allocations, compile__output_program, &compile__buffer_appending_error);

            // if buffer could not be added
            if (compile__buffer_appending_error == BASIC__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                BASIC__close__buffer(compile__output_program);

                // setup empty buffer
                compile__output_program = BASIC__create_null__buffer();
            }
        // program was not created
        } else {
            // setup empty buffer
            compile__output_program = BASIC__create_null__buffer();

            // set error code
            ANVIL__set__error_code_cell(context, ANVIL__et__compile__compilation_error);

            // add error message to allocations
            ANVIL__remember__allocation(allocations, compile__output_error_message, &compile__buffer_appending_error);

            // if buffer could not be added
            if (compile__buffer_appending_error == BASIC__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                BASIC__close__buffer(compile__output_error_message);

                // setup empty buffer
                compile__output_error_message = BASIC__create_null__buffer();
            }
        }

        // setup outputs
        (*context).cells[compile__output_start] = compile__output_program.start;
        (*context).cells[compile__output_end] = compile__output_program.end;
        (*context).cells[compile__error__occured] = (ANVIL__cell)(ANVIL__cell_integer_value)compile__error.occured;
        (*context).cells[compile__error__message_start] = compile__output_error_message.start;
        (*context).cells[compile__error__message_end] = compile__output_error_message.end;
        (*context).cells[compile__error__character_location__file_index] = (ANVIL__cell)compile__error.location.file_index;
        (*context).cells[compile__error__character_location__line_number] = (ANVIL__cell)compile__error.location.line_number;
        (*context).cells[compile__error__character_location__character_index] = (ANVIL__cell)compile__error.location.character_index;

        break;
    // run a context like a program
    case ANVIL__it__run:
        // get parameters
        run__context_buffer_start = ANVIL__read_next__cell_ID(execution_read_address);
        run__context_buffer_end = ANVIL__read_next__cell_ID(execution_read_address);
        run__instruction_count = ANVIL__read_next__cell_ID(execution_read_address);

        // useless operation to quiet compiler
        run__context_buffer_end = run__context_buffer_end;

        // if context buffer is valid
        if (ANVIL__check__valid_address_range_in_allocations(catch_addresses, allocations, BASIC__create__buffer((ANVIL__context*)(*context).cells[run__context_buffer_start], (ANVIL__context*)(*context).cells[run__context_buffer_end]))) {
            // run context
            ANVIL__run__context(allocations, (ANVIL__context*)(*context).cells[run__context_buffer_start], (u64)(*context).cells[run__instruction_count]);
        // error
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_address_range);
        }

        break;
    // get current time
    case ANVIL__it__get_time:
        // get parameters
        get_time__seconds = ANVIL__read_next__cell_ID(execution_read_address);
        get_time__nanoseconds = ANVIL__read_next__cell_ID(execution_read_address);

        // get time of day
        clock_gettime(CLOCK_REALTIME, &get_time__data_destination);

        // set variables
        (*context).cells[get_time__seconds] = (ANVIL__cell)get_time__data_destination.tv_sec;
        (*context).cells[get_time__nanoseconds] = (ANVIL__cell)get_time__data_destination.tv_nsec;

        break;
    // print one char to stdout
    case ANVIL__it__debug__putchar:
        // get parameters
        debug__putchar__printing_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // print
        putchar((u8)(u64)((*context).cells[debug__putchar__printing_cell_ID]));

        // flush stream for full update
        fflush(stdout);

        break;
    // read one string from stdin
    case ANVIL__it__debug__fgets:
        // get parameters
        debug__fgets__buffer_address_start = ANVIL__read_next__cell_ID(execution_read_address);
        debug__fgets__buffer_address_end = ANVIL__read_next__cell_ID(execution_read_address);

        // zero out temporaries
        for (BASIC__length i = 0; i < ANVIL__define__input_string_max_length; i++) {
            // zero out character
            debug__fgets__temporary_string[i] = 0;
        }
        debug__fgets__buffer_length = 0;

        // read string
        fgets((char*)debug__fgets__temporary_string, ANVIL__define__input_string_max_length, stdin);

        // read buffer for string size
        while (debug__fgets__buffer_length < ANVIL__define__input_string_max_length && debug__fgets__temporary_string[debug__fgets__buffer_length] != 0) {
            // increment length
            debug__fgets__buffer_length++;
        }

        // create buffer based on string length
        debug__fgets__buffer = BASIC__open__buffer(debug__fgets__buffer_length);

        // if buffer is not empty
        if (BASIC__check__empty_buffer(debug__fgets__buffer) == BASIC__bt__false) {
            // copy data into buffer
            for (BASIC__u64 i = 0; i < debug__fgets__buffer_length; i++) {
                // write character
                BASIC__write__buffer((u8)debug__fgets__temporary_string[i], sizeof(BASIC__u8), (BASIC__u8*)debug__fgets__buffer.start + (i * sizeof(BASIC__u8)));
            }

            // remember allocation
            ANVIL__remember__allocation(allocations, debug__fgets__buffer, &debug__fgets__buffer_appending_error);

            // if buffer could not be added
            if (debug__fgets__buffer_appending_error == BASIC__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                BASIC__close__buffer(debug__fgets__buffer);

                // setup empty buffer
                debug__fgets__buffer = BASIC__create_null__buffer();
            }
        }

        // setup cells
        (*context).cells[debug__fgets__buffer_address_start] = debug__fgets__buffer.start;
        (*context).cells[debug__fgets__buffer_address_end] = debug__fgets__buffer.end;

        break;
    // mark section of data
    case ANVIL__it__debug__mark_data_section:
        // get parameters
        debug__mark_data_section__section_length = ANVIL__read_next__cell(execution_read_address);

        // skip over data section
        (*context).cells[ANVIL__rt__program_current_address] = (BASIC__address)((u64)(*context).cells[ANVIL__rt__program_current_address] + (u64)debug__mark_data_section__section_length);

        break;
    // mark section of code
    case ANVIL__it__debug__mark_code_section:
        // instruction does nothing but mark code space, so get length and do nothing
        debug__mark_code_section__section_length = ANVIL__read_next__cell(execution_read_address);

        // useless operation to quiet compiler
        debug__mark_code_section__section_length = debug__mark_code_section__section_length;

        break;
    // get current context as a buffer
    case ANVIL__it__debug__get_current_context:
        // get parameters
        get_current_context__buffer_start = ANVIL__read_next__cell_ID(execution_read_address);
        get_current_context__buffer_end = ANVIL__read_next__cell_ID(execution_read_address);
        
        // setup output
        (*context).cells[get_current_context__buffer_start] = (void*)ANVIL__get__cell_address_from_context(context, 0);
        (*context).cells[get_current_context__buffer_end] = ((void*)ANVIL__get__cell_address_from_context(context, 0)) + sizeof(ANVIL__context) - 1;

        break;
    // search for an allocation given a buffer
    case ANVIL__it__debug__search_for_allocation:
        // get parameters
        search_for_allocation__source_buffer_start = ANVIL__read_next__cell_ID(execution_read_address);
        search_for_allocation__source_buffer_end = ANVIL__read_next__cell_ID(execution_read_address);
        search_for_allocation__was_found = ANVIL__read_next__cell_ID(execution_read_address);
        search_for_allocation__found_buffer_start = ANVIL__read_next__cell_ID(execution_read_address);
        search_for_allocation__found_buffer_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup inputs
        search_for_allocation__source_buffer.start = (*context).cells[search_for_allocation__source_buffer_start];
        search_for_allocation__source_buffer.end = (*context).cells[search_for_allocation__source_buffer_end];

        // search for allocation
        search_for_allocation__found_buffer = ANVIL__search__valid_allocation_from_sub_buffer(allocations, search_for_allocation__source_buffer, &search_for_allocation__was_found_boolean);

        // pass outputs
        (*context).cells[search_for_allocation__was_found] = (ANVIL__cell)(BASIC__u64)search_for_allocation__was_found_boolean;
        (*context).cells[search_for_allocation__found_buffer_start] = search_for_allocation__found_buffer.start;
        (*context).cells[search_for_allocation__found_buffer_end] = search_for_allocation__found_buffer.end;

        break;
    // in case instruction ID was invalid
    default:
        // set error
        ANVIL__set__error_code_cell(context, ANVIL__et__invalid_instruction_ID);

        // return exit context
        return ANVIL__nit__return_context;
    }

    // return next instruction by default
    return ANVIL__nit__next_instruction;
}

// run context
void ANVIL__run__context(ANVIL__allocations* allocations, ANVIL__context* context, ANVIL__instruction_count instruction_count) {
    ANVIL__nit next_instruction_action;

    // if an infinite amount of instructions can execute
    if (instruction_count == ANVIL__define__run_forever) {
        // run instructions
        while (1) {
            // run instruction
            next_instruction_action = ANVIL__run__instruction(allocations, context);

            // if quit
            if (next_instruction_action == ANVIL__nit__return_context) {
                return;
            }

            // assuming ANVIL__nit__next_instruction
            continue;
        }
    // if a finite amount of instructions can execute
    } else {
        for (ANVIL__instruction_count i = 0; i < instruction_count; i++) {
            // run instruction
            next_instruction_action = ANVIL__run__instruction(allocations, context);

            // check for early quitting
            if (next_instruction_action == ANVIL__nit__return_context) {
                return;
            }
        }
    }

    // execution ended without error
    return;
}

#endif
