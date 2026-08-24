#ifndef DRAGON__basic
#define DRAGON__basic

/* Include */
// C
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* Define */
// types
typedef uint8_t BASIC__u8;
typedef uint16_t BASIC__u16;
typedef uint32_t BASIC__u32;
typedef uint64_t BASIC__u64;
typedef int8_t BASIC__s8;
typedef int16_t BASIC__s16;
typedef int32_t BASIC__s32;
typedef int64_t BASIC__s64;

// aliases
typedef BASIC__u8 u8;
typedef BASIC__u16 u16;
typedef BASIC__u32 u32;
typedef BASIC__u64 u64;
typedef BASIC__s8 s8;
typedef BASIC__s16 s16;
typedef BASIC__s32 s32;
typedef BASIC__s64 s64;

// pointers
typedef void* BASIC__address;
#define BASIC__define__null_address 0
#define BASIC__define__null_flag 0

// general
typedef BASIC__u8 BASIC__character;
typedef BASIC__u64 BASIC__file_index;
typedef BASIC__u64 BASIC__line_number;
typedef BASIC__u64 BASIC__character_index;
typedef BASIC__u64 BASIC__tab_count;
typedef BASIC__u64 BASIC__length;
typedef BASIC__u64 BASIC__bit_count;
typedef BASIC__u64 BASIC__byte_count;
typedef BASIC__u64 BASIC__digit_count;
#define BASIC__define__bits_in_byte 8
#define BASIC__define__zero_length 0

// boolean type
typedef enum BASIC__bt {
	BASIC__bt__false = 0,
	BASIC__bt__true = 1,
	BASIC__bt__COUNT = 2,
} BASIC__bt;

// file index
#define BASIC__define__null_file_index_ID -1

/* Allocation */
// return memory to OS
void BASIC__close__allocation(BASIC__address start, BASIC__address end) {
	// do useless operation to quiet compiler
	end = end;

	// return memory
	free(start);

	return;
}

// ask OS for memory
BASIC__address BASIC__open__allocation(BASIC__length length) {
	// return allocation
	return (BASIC__address)malloc(length);
}

/* Buffer */
// buffer
typedef struct BASIC__buffer {
	BASIC__address start;
	BASIC__address end;
} BASIC__buffer;

// close buffer
void BASIC__close__buffer(BASIC__buffer buffer) {
	// close allocation
	BASIC__close__allocation(buffer.start, buffer.end);

	return;
}

// create buffer to have specific exact contents
BASIC__buffer BASIC__create__buffer(BASIC__address start, BASIC__address end) {
	BASIC__buffer output;

	// setup output
	output.start = start;
	output.end = end;

	return output;
}

// create buffer in it's standard null setup
BASIC__buffer BASIC__create_null__buffer() {
	// return standard null buffer
	return BASIC__create__buffer(BASIC__define__null_address, BASIC__define__null_address);
}

// calculate buffer length
BASIC__length BASIC__calculate__buffer_length(BASIC__buffer buffer) {
    return (BASIC__length)((u8*)buffer.end - (u8*)buffer.start) + 1;
}

// check to see if the pointers in the buffers are equal
BASIC__bt BASIC__calculate__buffer_addresses_equal(BASIC__buffer a, BASIC__buffer b) {
    return (BASIC__bt)((a.start == b.start) && (a.end == b.end));
}

// check to see if the contents in the buffers are identical
BASIC__bt BASIC__calculate__buffer_contents_equal(BASIC__buffer a, BASIC__buffer b) {
    // check if names are same length
    if (BASIC__calculate__buffer_length(a) != BASIC__calculate__buffer_length(b)) {
        // not same length so not identical
        return BASIC__bt__false;
    }

    // get pointers
    BASIC__address a_current = a.start;
    BASIC__address b_current = b.start;

    // check each character
    while (a_current <= a.end) {
        // check character
        if (*(BASIC__character*)a_current != *(BASIC__character*)b_current) {
            // character not identical, string not identical
            return BASIC__bt__false;
        }

        // next characters
        a_current += sizeof(BASIC__character);
        b_current += sizeof(BASIC__character);
    }

    // no issues found, buffers are identical
    return BASIC__bt__true;
}

BASIC__bt BASIC__calculate__buffer_starts_with_buffer(BASIC__buffer searching_in, BASIC__buffer searching_for) {
    // ensure search is larger than the buffer
    if (BASIC__calculate__buffer_length(searching_in) < BASIC__calculate__buffer_length(searching_for)) {
        // not bigger or equal, thus impossible to contain
        return BASIC__bt__false;
    }

    return BASIC__calculate__buffer_contents_equal(searching_for, BASIC__create__buffer(searching_in.start, searching_in.start + BASIC__calculate__buffer_length(searching_for) - 1));
}

// calculate buffer contains range
BASIC__bt BASIC__calculate__buffer_range_in_buffer_range_inclusive(BASIC__buffer outside, BASIC__buffer inside) {
    // return calculation
    return (outside.start <= inside.start) && (outside.end >= inside.end);
}

// check to see if it is an empty buffer
BASIC__bt BASIC__check__empty_buffer(BASIC__buffer buffer) {
    // return calculation
    return (BASIC__bt)(buffer.start == BASIC__define__null_address);
}

// open buffer
BASIC__buffer BASIC__open__buffer(BASIC__length length) {
	BASIC__buffer output;

	// attempt allocation
	output.start = BASIC__open__allocation(length);

	// set end of buffer according to allocation success
	if (output.start != BASIC__define__null_address) {
		output.end = (BASIC__address)((((BASIC__u64)output.start) + length) - 1);
	} else {
		output.end = BASIC__define__null_address;
	}

	return output;
}

// check if buffers are the same size
BASIC__bt BASIC__calculate__are_buffers_same_size(BASIC__buffer a, BASIC__buffer b) {
    // return calculation
    return (BASIC__bt)((a.end - a.start) == (b.end - b.start));
}

// copy buffer
void BASIC__copy__buffer(BASIC__buffer source, BASIC__buffer destination, BASIC__bt* error) {
    // check for invalid buffer
    if (BASIC__calculate__are_buffers_same_size(source, destination) == BASIC__bt__true && source.start <= source.end && destination.start <= destination.end) {
        // copy buffer
        for (BASIC__length byte_index = 0; byte_index < (BASIC__length)(destination.end - destination.start) + 1; byte_index++) {
            // copy byte
            ((BASIC__u8*)destination.start)[byte_index] = ((BASIC__u8*)source.start)[byte_index];
        }
    } else {
        // set error
        *error = BASIC__bt__true;
    }

    return;
}

// copy buffer backwards
void BASIC__copy__buffer__backwards(BASIC__buffer source, BASIC__buffer destination, BASIC__bt* error) {
    // check for invalid buffer
    if (BASIC__calculate__are_buffers_same_size(source, destination) == BASIC__bt__true && source.start <= source.end  && destination.start <= destination.end) {
        // copy buffer
        for (BASIC__length byte_index = (BASIC__length)(destination.end - destination.start) + 1; byte_index > 0; byte_index--) {
            // copy byte
            ((BASIC__u8*)destination.start)[byte_index - 1] = ((BASIC__u8*)source.start)[byte_index - 1];
        }
    } else {
        // set error
        *error = BASIC__bt__true;
    }

    return;
}

// create or open a buffer from a string literal (can either duplicate buffer or simply reference original) (can opt out of null termination)
BASIC__buffer BASIC__open__buffer_from_string(u8* string, BASIC__bt duplicate, BASIC__bt null_terminate) {
    BASIC__buffer output;
    BASIC__length length;

    // setup length
    length = 0;

    // get buffer length
    while (string[length] != 0) {
        length++;
    }

    // optionally append null termination
    if (null_terminate == BASIC__bt__true) {
        length++;
    }

    // reference or duplicate
    if (duplicate == BASIC__bt__true) {
        // attempt allocation
        output = BASIC__open__buffer(length);

        // check for null allocation
        if (output.start == BASIC__define__null_address) {
            // return empty buffer
            return output;
        }

        // copy buffer byte by byte
        for (BASIC__length byte_index = 0; byte_index < length; byte_index++) {
            // copy byte
            ((BASIC__u8*)output.start)[byte_index] = string[byte_index];
        }
    } else {
        // setup duplicate output
        output.start = string;
        output.end = string + length - 1;
    }

    return output;
}

// read buffer
BASIC__u64 BASIC__read__buffer(BASIC__address source, BASIC__length byte_amount) {
	BASIC__u64 output;

	// setup output
	output = 0;

	// read buffer
	for (BASIC__u64 byte_index = 0; byte_index < byte_amount; byte_index += 1) {
		// get byte
		((BASIC__u8*)&output)[byte_index] = ((BASIC__u8*)source)[byte_index];
	}

	// return output
	return output;
}

// write buffer
void BASIC__write__buffer(BASIC__u64 source, BASIC__length byte_amount, BASIC__address destination) {
	// write data to buffer
	for (BASIC__length byte_index = 0; byte_index < byte_amount; byte_index += 1) {
		// write byte
		((BASIC__u8*)destination)[byte_index] = ((BASIC__u8*)&source)[byte_index];
	}
	
	return;
}

// append null termination
BASIC__buffer BASIC__add__null_termination_to_file_path(BASIC__buffer file_path, BASIC__bt* error_occured) {
    BASIC__buffer output;

    // allocate buffer
    output = BASIC__open__buffer(BASIC__calculate__buffer_length(file_path) + 1);

    // copy buffer
    BASIC__copy__buffer(file_path, BASIC__create__buffer(output.start, output.end - 1), error_occured);

    // append null termination
    *((BASIC__character*)output.end) = 0;

    return output;
}

// create buffer from file
BASIC__buffer BASIC__move__file_to_buffer(BASIC__buffer file_path) {
	BASIC__buffer output;
	FILE* file_handle;
	BASIC__u64 file_size;
    BASIC__buffer null_terminated_file_path = file_path;
    BASIC__bt error_occured = BASIC__bt__false;

    // check for null termination
    if (*(BASIC__character*)file_path.end != 0) {
        // setup null termination
        null_terminated_file_path = BASIC__add__null_termination_to_file_path(file_path, &error_occured);
    }

	// open file
	file_handle = fopen((const char*)null_terminated_file_path.start, "rb");

	// check if the file opened
	if (file_handle == 0) {
		// if not, return empty buffer
		output = BASIC__create_null__buffer();

        goto quit_no_file_handle;
	}

	// get file size
	fseek(file_handle, 0, SEEK_END);
	file_size = ftell(file_handle);
	fseek(file_handle, 0, SEEK_SET);

	// allocate buffer
	output = BASIC__open__buffer(file_size);

	// check if buffer allocated
	if (output.start == BASIC__define__null_address) {
		// exit
        goto quit;
	}

	// read file into buffer
	fread(output.start, file_size, 1, file_handle);

    // return offset
    quit:

	// close file handle
	fclose(file_handle);

    // quit no file handle
    quit_no_file_handle:

    // close null file path if necessary
    if (*(BASIC__character*)file_path.end != 0) {
        BASIC__close__buffer(null_terminated_file_path);
    }

	// return buffer
	return output;
}

// create file from buffer
void BASIC__move__buffer_to_file(BASIC__bt* error, BASIC__buffer file_path, BASIC__buffer data) {
	FILE* file_handle;
    BASIC__buffer null_terminated_file_path = file_path;

    // check for null termination
    if (*(BASIC__character*)file_path.end != 0) {
        // setup null termination
        null_terminated_file_path = BASIC__add__null_termination_to_file_path(file_path, error);
    }

    // setup error to no error to start
    *error = BASIC__bt__false;

	// open file
	file_handle = fopen((const char*)null_terminated_file_path.start, "w+b");

	// check if the file opened
	if (file_handle == 0) {
		// if not, return error
        *error = BASIC__bt__true;

		goto quit;
	}

	// write buffer to file
	fwrite(data.start, BASIC__calculate__buffer_length(data), 1, file_handle);

	// close file handle
	fclose(file_handle);

    // exit offset
    quit:

    // close null file path if necessary
    if (*(BASIC__character*)file_path.end != 0) {
        BASIC__close__buffer(null_terminated_file_path);
    }

	// return
	return;
}

// delete a file
void BASIC__delete__file(BASIC__bt* error, BASIC__buffer file_path) {
    BASIC__buffer null_terminated_file_path = file_path;

    // check for null termination
    if (*(BASIC__character*)file_path.end != 0) {
        // setup null termination
        null_terminated_file_path = BASIC__add__null_termination_to_file_path(file_path, error);
    }

    // free file
    remove(null_terminated_file_path.start);

    // close null file path if necessary
    if (*(BASIC__character*)file_path.end != 0) {
        BASIC__close__buffer(null_terminated_file_path);
    }

    return;
}

// print buffer
void BASIC__print__buffer(BASIC__buffer buffer) {
    // print character by character
    for (BASIC__address character = buffer.start; character <= buffer.end; character += sizeof(BASIC__character)) {
        // print character
        putchar(*(BASIC__character*)character);
    }

    return;
}

/* List */
// list types
typedef BASIC__u64 BASIC__list_filled_index;
typedef BASIC__u64 BASIC__list_increase;

// list object
typedef struct BASIC__list {
    BASIC__buffer buffer;
    BASIC__list_filled_index filled_index;
    BASIC__list_increase increase;
} BASIC__list;

// create a list
BASIC__list BASIC__create__list(BASIC__buffer buffer, BASIC__list_filled_index filled_index, BASIC__list_increase increase) {
    BASIC__list output;

    // setup output
    output.buffer = buffer;
    output.filled_index = filled_index;
    output.increase = increase;

    return output;
}

// create a null list
BASIC__list BASIC__create_null__list() {
    // return empty list
    return BASIC__create__list(BASIC__create_null__buffer(), 0, 0);
}

// open a list
BASIC__list BASIC__open__list(BASIC__list_increase increase, BASIC__bt* error_occured) {
    BASIC__list output;
    BASIC__buffer allocation;

    // allocate list
    allocation = BASIC__open__buffer(increase);

    // check list validity
    if (allocation.start == BASIC__define__null_address) {
        // set error
        *error_occured = BASIC__bt__true;

        // return empty
        return BASIC__create_null__list();
    // list is valid
    } else {
        // set error to false
        *error_occured = BASIC__bt__false;
    }

    // setup output
    output.buffer = allocation;
    output.filled_index = 0;
    output.increase = increase;

    return output;
}

// destroy a list
void BASIC__close__list(BASIC__list list) {
    // free buffer
    BASIC__close__buffer(BASIC__create__buffer(list.buffer.start, list.buffer.end));

    return;
}

// expand a list
void BASIC__list__expand(BASIC__list* list, BASIC__bt* error_occured) {
    BASIC__list_filled_index new_size;
    BASIC__buffer new_allocation;

    // calculate new buffer size
    new_size = ((BASIC__u64)(*list).buffer.end - (BASIC__u64)(*list).buffer.start + 1) + (*list).increase;

    // request new memory
    new_allocation = BASIC__open__buffer(new_size);

    // check for failure
    if (new_allocation.start == BASIC__define__null_address) {
        // set error
        *error_occured = BASIC__bt__true;

        // return unmodified list
        return;
    }

    // copy old data to new list
    for (BASIC__list_filled_index i = 0; i < (*list).filled_index; i++) {
        // copy one byte
        ((BASIC__u8*)new_allocation.start)[i] = ((BASIC__u8*)(*list).buffer.start)[i];
    }

    // free old buffer
    BASIC__close__buffer((*list).buffer);

    // setup new list allocation
    (*list).buffer = new_allocation;

    return;
}

// request space for the list
void BASIC__list__request__space(BASIC__list* list, BASIC__byte_count byte_count, BASIC__bt* error_occured) {
    // expand the list until there is enough space
    while (((BASIC__u64)(*list).buffer.end - (BASIC__u64)(*list).buffer.start + 1) < ((*list).filled_index + byte_count)) {
        // expand the list
        BASIC__list__expand(list, error_occured);

        // check for error
        if (*error_occured == BASIC__bt__true) {
            // return last modified list
            return;
        }
    }

    return;
}

// add index to address
BASIC__address BASIC__calculate__address_from_buffer_index(BASIC__address start, BASIC__list_filled_index index) {
    return start + index;
}

// calculate the tip of the list
BASIC__address BASIC__calculate__list_current_address(BASIC__list* list) {
    return BASIC__calculate__address_from_buffer_index((*list).buffer.start, (*list).filled_index);
}

// calculate the current buffer
BASIC__buffer BASIC__calculate__list_current_buffer(BASIC__list* list) {
    return BASIC__create__buffer(((*list).buffer.start), BASIC__calculate__list_current_address(list) - 1);
}

// macro to create custom appender


// add a buffer to a list
void BASIC__list__append__buffer(BASIC__list* list, BASIC__buffer buffer, BASIC__bt* memory_error_occured) {
    // request space
    BASIC__list__request__space(list, sizeof(BASIC__buffer), memory_error_occured);

    // append data
    (*(BASIC__buffer*)BASIC__calculate__list_current_address(list)) = buffer;

    // increase fill
    (*list).filled_index += sizeof(BASIC__buffer);

    return;
}

// add a buffer's data to a list
void BASIC__list__append__buffer_data(BASIC__list* list, BASIC__buffer buffer, BASIC__bt* memory_error_occured) {
    BASIC__length buffer_length;
    BASIC__address buffer_old_end;

    // calculate buffer length
    buffer_length = BASIC__calculate__buffer_length(buffer);

    // request space
    BASIC__list__request__space(list, buffer_length, memory_error_occured);

    // calculate old buffer end
    buffer_old_end = (*list).buffer.start + (*list).filled_index - 1;

    // append data
    BASIC__copy__buffer(buffer, BASIC__create__buffer(buffer_old_end + 1, buffer_old_end + 1 + buffer_length - 1), memory_error_occured);

    // increase fill
    (*list).filled_index += buffer_length;

    return;
}

// macro for appending anything to a list
#define BASIC__list__append(list_address, type, data, error_occured) { BASIC__list__request__space(list_address, sizeof(type), error_occured); (*(type *)BASIC__calculate__list_current_address(list_address)) = data; (*list_address).filled_index += sizeof(type); }

// remove a slice of data from a list
void BASIC__list__erase__space(BASIC__list* list, BASIC__list_filled_index range_start_index, BASIC__list_filled_index range_end_index) {
    BASIC__buffer old_right;
    BASIC__buffer new_right;
    BASIC__bt error;

    // get new right buffer
    old_right = BASIC__create__buffer(BASIC__calculate__address_from_buffer_index((*list).buffer.start, range_end_index), BASIC__calculate__list_current_address(list));
    new_right = BASIC__create__buffer(old_right.start - (range_end_index - range_start_index + 1), old_right.end - (range_end_index - range_start_index + 1));

    // move data from left to right filling in the gap
    BASIC__copy__buffer(old_right, new_right, &error);

    // should not happen but handled anyways
    if (error == BASIC__bt__true) {
        // tell user
        printf("Internal Error: Buffer could not erase data.\n");
    // buffer was clipped, change filled index
    } else {
        // change filled index
        (*list).filled_index -= range_end_index - range_start_index + 1;
    }

    return;
}

// check if two lists are filled up to the same amount
BASIC__bt BASIC__calculate__lists_have_same_fill_size(BASIC__list* a, BASIC__list* b) {
    return (BASIC__bt)((*a).filled_index == (*b).filled_index);
}

// take a list and make a standalone buffer
BASIC__buffer BASIC__list__open_buffer_from_list(BASIC__list* list, BASIC__bt* memory_error_occured) {
    BASIC__buffer output;

    // allocate output
    output = BASIC__open__buffer((*list).filled_index);

    // if buffer did not open
    if (BASIC__check__empty_buffer(output) == BASIC__bt__true) {
        // set error
        *memory_error_occured = BASIC__bt__true;
    // if buffer opened
    } else {
        // copy data from list to buffer
        BASIC__copy__buffer(BASIC__calculate__list_current_buffer(list), output, memory_error_occured);
    }

    return output;
}

// check if a list is uninitialized
BASIC__bt BASIC__check__empty_list(BASIC__list list) {
    return BASIC__check__empty_buffer(list.buffer);
}

/* Counted List */
// structure
typedef struct BASIC__counted_list {
    BASIC__list list;
    BASIC__length count;
} BASIC__counted_list;

// create structure
BASIC__counted_list BASIC__create__counted_list(BASIC__list list, BASIC__length count) {
    BASIC__counted_list output;

    // setup output
    output.list = list;
    output.count = count;

    return output;
}

// create null structure
BASIC__counted_list BASIC__create_null__counted_list() {
    // return empty
    return BASIC__create__counted_list(BASIC__create_null__list(), BASIC__define__zero_length);
}

// close counted list
void BASIC__close__counted_list(BASIC__counted_list counted_list) {
    // close internals
    BASIC__close__list(counted_list.list);

    return;
}

// open counted list
BASIC__counted_list BASIC__open__counted_list(BASIC__list_increase increase, BASIC__bt* error_occured) {
    return BASIC__create__counted_list(BASIC__open__list(increase, error_occured), 0);
}

/* Current */
// define
typedef BASIC__buffer BASIC__current;

// check if a current buffer is still valid
BASIC__bt BASIC__check__current_within_range(BASIC__current current) {
    return (current.start <= current.end);
}

// calculate a current buffer from a list // NOTE: buffer cannot be null or calculation fails!
BASIC__current BASIC__calculate__current_from_list_filled_index(BASIC__list* list) {
    return BASIC__create__buffer((*list).buffer.start, (*list).buffer.start + (*list).filled_index - 1);
}

// check for a character at a current
BASIC__bt BASIC__check__character_range_at_current(BASIC__current current, BASIC__character range_start, BASIC__character range_end) {
    return ((*(BASIC__character*)current.start) >= range_start) && ((*(BASIC__character*)current.start) <= range_end);
}

// calculate the amounnt of items in one list (assumes all items are same size!)
BASIC__list_filled_index BASIC__calculate__list_content_count(BASIC__list list, size_t item_size) {
    return list.filled_index / item_size;
}

/* Essentials */
BASIC__u64 BASIC__calculate__exponent(BASIC__u64 base, BASIC__u64 exponent) {
    BASIC__u64 output = 1;

    // if zero
    if (exponent == 0) {
        return output;
    }

    // calculate number
    while (exponent > 0) {
        // multiply
        output *= base;

        // next exponent
        exponent--;
    }

    return output;
}

// print tabs
void BASIC__print__tabs(BASIC__tab_count tab_count) {
    // print tabs
    while (tab_count > 0) {
        // print tab
        printf("|   ");

        // next tab
        tab_count--;
    }

    return;
}

// calculate character count for binary number to decimal string
BASIC__digit_count BASIC__calculate__digit_count(BASIC__u64 base, BASIC__u64 value) {
    // check for zero value
    if (value == 0) {
        return 1;
    }

    // check for invalid denominator
    if (base < 2) {
        return 0;
    }

    // calculate count
    // setup count
    BASIC__digit_count count = 0;

    // loop through bases
    while (value > 0) {
        // divide
        value /= base;

        // count
        count++;
    }

    return count;
}

// convert an integer into a base numbered number
BASIC__buffer BASIC__cast__integer_to_unsigned_text_value(BASIC__u64 value, BASIC__u64 base, BASIC__buffer digits, BASIC__bt direction /* false is lower to higher, true is higher to lower */) {
    // calculate digit count
    BASIC__digit_count digit_count = BASIC__calculate__digit_count(base, value);

    // allocate string
    BASIC__buffer output = BASIC__open__buffer(digit_count);

    // if lower to higher
    if (direction == BASIC__bt__false) {
        // for each character
        for (BASIC__digit_count digit_index = 0; digit_index < digit_count; digit_index++) {
            // calculate & write digit
            ((BASIC__u8*)output.start)[digit_index] = (value % base) + (*(BASIC__u8*)digits.start);

            // next digit
            value /= base;
        }
    }

    // if higher to lower
    if (direction == BASIC__bt__true) {
        // for each character
        for (BASIC__digit_count digit_index = digit_count; digit_index > 0; digit_index--) {
            // calculate & write digit
            ((BASIC__u8*)output.start)[digit_index - 1] = (value % base) + (*(BASIC__u8*)digits.start);

            // next digit
            value /= base;
        }
    }

    return output;
}

// cast integer to unsigned base 10 integer
BASIC__buffer BASIC__cast__integer_to_unsigned_base_10(BASIC__u64 value) {
    return BASIC__cast__integer_to_unsigned_text_value(value, 10, BASIC__open__buffer_from_string((u8*)"0123456789", BASIC__bt__false, BASIC__bt__false), BASIC__bt__true);
}

#endif
