#ifndef DRAGON__compiler__generator
#define DRAGON__compiler__generator

/* Include */
#include "Accounter.h"
#include "CompilerSpecifications.h"

/* Generation */
// one function
typedef struct COMPILER__generation_function {
    // offsets
    ANVIL__offset offset__function_start;
    ANVIL__offset offset__function_return;
    ANVIL__offset offset__function_data;
    ANVIL__counted_list statement_offsets; // ANVIL__offset
    ANVIL__counted_list data_offsets; // ANVIL__offset

    // cell ranges
    COMPILER__cell_range cells__inputs; // function io inputs
    COMPILER__cell_range cells__outputs; // function io outputs
    COMPILER__cell_range cells__workspace; // all workspace cells

    // data
    ANVIL__counted_list data__user_defined_strings; // copied from accounting, DO NOT FREE!
} COMPILER__generation_function;

// one program's workspace
// (program buffer is not in workspace as it needs to be freed separately)
typedef struct COMPILER__generation_workspace {
    // anvil two pass workspace
    ANVIL__workspace workspace;

    // functions
    ANVIL__counted_list user_defined_functions; // COMPILER__generation_function
} COMPILER__generation_workspace;

// open generation function
COMPILER__generation_function COMPILER__open__generation_function(COMPILER__accountling_function accountlings, COMPILER__error* error) {
    COMPILER__generation_function output;

    // open offsets
    output.data_offsets = COMPILER__open__counted_list_with_error(sizeof(ANVIL__offset) * accountlings.strings.count, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    for (COMPILER__offset_index index = 0; index < accountlings.strings.count; index++) {
        // append blank offset
        ((ANVIL__offset*)output.data_offsets.list.buffer.start)[index] = ANVIL__define__null_offset_ID;
    }
    output.statement_offsets = COMPILER__open__counted_list_with_error(sizeof(ANVIL__offset) * accountlings.offsets.count, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    for (COMPILER__offset_index index = 0; index < accountlings.offsets.count; index++) {
        // append blank offset
        ((ANVIL__offset*)output.statement_offsets.list.buffer.start)[index] = ANVIL__define__null_offset_ID;
    }

    // setup cells
    output.cells__workspace.start = (ANVIL__u64)ANVIL__srt__start__workspace;
    output.cells__workspace.end = accountlings.next_available_workspace_cell - 1;

    return output;
}

// close generation fnuction
void COMPILER__close__generation_function(COMPILER__generation_function function) {
    // close lists
    ANVIL__close__counted_list(function.data_offsets);
    ANVIL__close__counted_list(function.statement_offsets);

    return;
}

// open generation workspace
COMPILER__generation_workspace COMPILER__open__generation_workspace(ANVIL__buffer* program_buffer, COMPILER__accountling_program accountlings, COMPILER__error* error) {
    COMPILER__generation_workspace output;

    // open anvil workspace
    output.workspace = ANVIL__setup__workspace(program_buffer);

    // open user defined functions
    output.user_defined_functions = COMPILER__open__counted_list_with_error(sizeof(COMPILER__generation_function) * accountlings.functions.headers.category[COMPILER__afht__user_defined].count, error);
    if (COMPILER__check__error_occured(error)) {
        goto failure;
    }
    output.user_defined_functions.count = accountlings.functions.bodies.count;
    for (COMPILER__function_index index = 0; index < accountlings.functions.headers.category[COMPILER__afht__user_defined].count; index++) {
        // open one function
        ((COMPILER__generation_function*)output.user_defined_functions.list.buffer.start)[index] = COMPILER__open__generation_function(((COMPILER__accountling_function*)accountlings.functions.bodies.list.buffer.start)[index], error);
        if (COMPILER__check__error_occured(error)) {
            goto failure;
        }
    }

    // success
    return output;

    // failure to create, clean up object
    failure:
    return output;
}

// close generation workspace
void COMPILER__close__generation_workspace(COMPILER__generation_workspace workspace) {
    // close function bodies
    for (COMPILER__function_index index = 0; index < workspace.user_defined_functions.count; index++) {
        // close function
        COMPILER__close__generation_function(((COMPILER__generation_function*)workspace.user_defined_functions.list.buffer.start)[index]);
    }
    ANVIL__close__counted_list(workspace.user_defined_functions);

    return;
}

// variable macro
#define COMPILER__generate__use_variable(value) COMPILER__account__functions__get_variable_by_variable_argument(accountling_function.variables, statement.value)

// generate user defined function statement
void COMPILER__generate__user_defined_function_scope(COMPILER__generation_workspace* workspace, COMPILER__accountling_function accountling_function, COMPILER__function_index user_defined_function_index, COMPILER__accountling_scope scope, COMPILER__scope_index current_scope_index, COMPILER__error* error) {
    // setup helper variables
    COMPILER__generation_function* function = &((COMPILER__generation_function*)(*workspace).user_defined_functions.list.buffer.start)[user_defined_function_index];
    COMPILER__accountling_scope_header* scope_header = &(((COMPILER__accountling_scope_header*)accountling_function.scope_headers.list.buffer.start)[current_scope_index]);
    ANVIL__workspace* anvil = &(*workspace).workspace;

    // setup starting offset
    (*scope_header).starting_offset = ANVIL__get__offset(anvil);

    // for each statement
    for (COMPILER__statement_index index = 0; index < scope.statements.count; index++) {
        // define variables
        ANVIL__cell_ID pack__output_to;
        ANVIL__cell_ID user_defined_function_call__current_function_io_register;

        // get statement
        COMPILER__accountling_statement statement = ((COMPILER__accountling_statement*)scope.statements.list.buffer.start)[index];

        // build statement
        // if statement is function call
        switch (statement.statement_type) {
        case COMPILER__ast__predefined__execution__stop:
            ANVIL__code__stop(anvil);
            
            break;
        case COMPILER__ast__predefined__set__cell:
            ANVIL__code__write_cell(anvil, (ANVIL__cell)statement.set_cell__raw_value, COMPILER__generate__use_variable(set_cell__variable_argument).cells.start);

            break;
        case COMPILER__ast__predefined__set__string:
            ANVIL__code__write_cell(anvil, (ANVIL__cell)((ANVIL__offset*)(*function).data_offsets.list.buffer.start)[statement.set_string__string_value_index], ANVIL__srt__temp__offset);
            ANVIL__code__retrieve_embedded_buffer(anvil, ANVIL__sft__always_run, ANVIL__srt__temp__offset, COMPILER__generate__use_variable(set_string__variable_argument).cells.start, COMPILER__generate__use_variable(set_string__variable_argument).cells.end);

            break;
        case COMPILER__ast__predefined__print__character:
            ANVIL__code__debug__putchar(anvil, COMPILER__generate__use_variable(print__variable_argument).cells.start);

            break;
        case COMPILER__ast__predefined__copy__anything:
            // setup copies
            for (ANVIL__cell_index cell_index = 0; cell_index <= (ANVIL__cell_index)(COMPILER__generate__use_variable(copy__input).cells.end - COMPILER__generate__use_variable(copy__input).cells.start); cell_index++) {
                // setup cell copy
                ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(copy__input).cells.start + cell_index, COMPILER__generate__use_variable(copy__output).cells.start + cell_index);
            }

            break;
        case COMPILER__ast__predefined__pack__anything:
            // setup variables
            pack__output_to = COMPILER__generate__use_variable(pack__output).cells.start;

            // pack variables
            for (COMPILER__variable_index variable_index = 0; variable_index < statement.pack__inputs.count; variable_index++) {
                // get one input variable
                COMPILER__accountling_variable packer_input = COMPILER__account__functions__get_variable_by_variable_argument(accountling_function.variables, ((COMPILER__accountling_variable_argument*)statement.pack__inputs.list.buffer.start)[variable_index]);
                
                // pass variables for one structure
                for (ANVIL__cell_index cell_index = packer_input.cells.start; cell_index <= packer_input.cells.end; cell_index++) {
                    // pass one cell
                    ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, cell_index, pack__output_to);

                    // next output cell
                    pack__output_to++;
                }
            }

            break;
        case COMPILER__ast__predefined__pack_null__anything:
            // pack cells
            for (ANVIL__cell_index cell_index = COMPILER__generate__use_variable(pack__output).cells.start; cell_index <= COMPILER__generate__use_variable(pack__output).cells.end; cell_index++) {
                // pass one cell
                ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, ANVIL__srt__constant__0, cell_index);
            }

            break;
        case COMPILER__ast__predefined__pack_increment__anything:
            // setup starting value
            ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(pack_increment_start).cells.start, ANVIL__srt__temp__write);

            // pack cells
            for (ANVIL__cell_index cell_index = COMPILER__generate__use_variable(pack__output).cells.start; cell_index <= COMPILER__generate__use_variable(pack__output).cells.end; cell_index++) {
                // write one cell
                ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, ANVIL__srt__temp__write, cell_index);

                // increment write value
                ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_add, ANVIL__srt__temp__write, COMPILER__generate__use_variable(pack_increment_by).cells.start, ANVIL__unused_cell_ID, ANVIL__srt__temp__write);
            }

            break;
        case COMPILER__ast__predefined__jump__top:
            // code jump
            ANVIL__code__operate__jump__static(anvil, ANVIL__sft__always_run, ANVIL__srt__constant__false, COMPILER__generate__use_variable(jump__variable_argument).cells.start, ANVIL__srt__constant__false, ANVIL__sft__always_run, ((COMPILER__accountling_scope_header*)accountling_function.scope_headers.list.buffer.start)[statement.scope_index].starting_offset);

            break;
        case COMPILER__ast__predefined__jump__bottom:
            // code jump
            ANVIL__code__operate__jump__static(anvil, ANVIL__sft__always_run, ANVIL__srt__constant__false, COMPILER__generate__use_variable(jump__variable_argument).cells.start, ANVIL__srt__constant__false, ANVIL__sft__always_run, ((COMPILER__accountling_scope_header*)accountling_function.scope_headers.list.buffer.start)[statement.scope_index].ending_offset);

            break;
        case COMPILER__ast__predefined__jump__offset:
            // code jump
            ANVIL__code__operate__jump__static(anvil, ANVIL__sft__always_run, ANVIL__srt__constant__false, COMPILER__generate__use_variable(jump__variable_argument).cells.start, ANVIL__srt__constant__false, ANVIL__sft__always_run, ((ANVIL__offset*)(((COMPILER__generation_function*)(*workspace).user_defined_functions.list.buffer.start)[user_defined_function_index].statement_offsets.list.buffer.start))[statement.offset_index]);

            break;
        case COMPILER__ast__predefined__bits_or:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_or, COMPILER__generate__use_variable(bit_operation__first_argument).cells.start, COMPILER__generate__use_variable(bit_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(bit_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__bits_invert:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_invert, COMPILER__generate__use_variable(bit_operation__first_argument).cells.start, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(bit_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__bits_and:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_and, COMPILER__generate__use_variable(bit_operation__first_argument).cells.start, COMPILER__generate__use_variable(bit_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(bit_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__bits_xor:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_xor, COMPILER__generate__use_variable(bit_operation__first_argument).cells.start, COMPILER__generate__use_variable(bit_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(bit_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__bits_shift_higher:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_shift_higher, COMPILER__generate__use_variable(bit_operation__first_argument).cells.start, COMPILER__generate__use_variable(bit_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(bit_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__bits_shift_lower:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_shift_lower, COMPILER__generate__use_variable(bit_operation__first_argument).cells.start, COMPILER__generate__use_variable(bit_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(bit_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__bits_overwrite:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_overwrite, COMPILER__generate__use_variable(bit_operation__first_argument).cells.start, COMPILER__generate__use_variable(bit_operation__second_argument).cells.start, COMPILER__generate__use_variable(bit_operation__third_argument).cells.start, COMPILER__generate__use_variable(bit_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__check__integer_within_range:
            // setup inversion flag
            //ANVIL__code__write_cell(anvil, (ANVIL__cell)ANIVL__sft__temp_1, ANVIL__srt__temp__flag_ID_1);
            ANVIL__code__write_cell(anvil, (ANVIL__cell)ANIVL__sft__temp_2, ANVIL__srt__temp__flag_ID_2);
            //ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__flag_set, , ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__srt__temp__flag_ID_1);

            // do comparison
            ANVIL__code__operate__flag(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(within_range__lower_bound).cells.start, COMPILER__generate__use_variable(within_range__checking).cells.start, COMPILER__generate__use_variable(within_range__higher_bound).cells.start, ANVIL__sft__never_run, ANVIL__srt__temp__flag_ID_2);

            // get results
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__flag_get, ANVIL__srt__temp__flag_ID_2, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(within_range__normal_result).cells.start);
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__bits_xor, COMPILER__generate__use_variable(within_range__normal_result).cells.start, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(within_range__inverted_result).cells.start);

            break;
        case COMPILER__ast__predefined__integer__addition:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_add, COMPILER__generate__use_variable(integer_operation__first_argument).cells.start, COMPILER__generate__use_variable(integer_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(integer_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__integer__subtraction:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_subtract, COMPILER__generate__use_variable(integer_operation__first_argument).cells.start, COMPILER__generate__use_variable(integer_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(integer_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__integer__multiplication:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_multiply, COMPILER__generate__use_variable(integer_operation__first_argument).cells.start, COMPILER__generate__use_variable(integer_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(integer_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__integer__division:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_division, COMPILER__generate__use_variable(integer_operation__first_argument).cells.start, COMPILER__generate__use_variable(integer_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(integer_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__integer__modulous:
            // do calculation
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_modulous, COMPILER__generate__use_variable(integer_operation__first_argument).cells.start, COMPILER__generate__use_variable(integer_operation__second_argument).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(integer_operation__output_argument).cells.start);

            break;
        case COMPILER__ast__predefined__buffer__request:
            ANVIL__code__request_memory(anvil, COMPILER__generate__use_variable(buffer_acquisition__buffer_length).cells.start, COMPILER__generate__use_variable(buffer_acquisition__buffer_result).cells.start, COMPILER__generate__use_variable(buffer_acquisition__buffer_result).cells.end);

            break;
        case COMPILER__ast__predefined__buffer__return:
            ANVIL__code__return_memory(anvil, COMPILER__generate__use_variable(buffer_acquisition__buffer_return).cells.start, COMPILER__generate__use_variable(buffer_acquisition__buffer_return).cells.end);

            break;
        case COMPILER__ast__predefined__structure__size:
            ANVIL__code__write_cell(anvil, (ANVIL__cell)(ANVIL__u64)((COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.end - COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.start + 1) * sizeof(ANVIL__cell)), COMPILER__generate__use_variable(structure_buffer_mover__size).cells.start);

            break;
        case COMPILER__ast__predefined__structure__structure_to_buffer:
            // calculate addresses
            ANVIL__code__calculate__addresses_for_cell_range_from_context(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.start, COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.end, ANVIL__srt__temp__buffer_1__start, ANVIL__srt__temp__buffer_1__end);

            // perform write
            ANVIL__code__buffer_to_buffer__low_to_high(anvil, ANVIL__srt__temp__buffer_1__start, ANVIL__srt__temp__buffer_1__end, COMPILER__generate__use_variable(structure_buffer_mover__buffer).cells.start, COMPILER__generate__use_variable(structure_buffer_mover__buffer).cells.end);

            // calculate advancement
            ANVIL__code__write_cell(anvil, (ANVIL__cell)(ANVIL__u64)((COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.end - COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.start + 1) * sizeof(ANVIL__cell)), ANVIL__srt__temp__length);
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_add, COMPILER__generate__use_variable(structure_buffer_mover__buffer).cells.start, ANVIL__srt__temp__length, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(structure_buffer_mover__advancement).cells.start);

            break;
        case COMPILER__ast__predefined__structure__buffer_to_structure:
            // calculate addresses
            ANVIL__code__calculate__addresses_for_cell_range_from_context(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.start, COMPILER__generate__use_variable(structure_buffer_mover__structure).cells.end, ANVIL__srt__temp__buffer_1__start, ANVIL__srt__temp__buffer_1__end);

            // perform write
            ANVIL__code__buffer_to_buffer__low_to_high(anvil, COMPILER__generate__use_variable(structure_buffer_mover__buffer).cells.start, COMPILER__generate__use_variable(structure_buffer_mover__buffer).cells.end, ANVIL__srt__temp__buffer_1__start, ANVIL__srt__temp__buffer_1__end);

            break;
        case COMPILER__ast__predefined__mover__address_to_cell:
            ANVIL__code__address_to_cell(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(cell_address_mover__address).cells.start, COMPILER__generate__use_variable(cell_address_mover__byte_count).cells.start, COMPILER__generate__use_variable(cell_address_mover__cell).cells.start);
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_add, COMPILER__generate__use_variable(cell_address_mover__address).cells.start, COMPILER__generate__use_variable(cell_address_mover__byte_count).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(cell_address_mover__advancement).cells.start);

            break;
        case COMPILER__ast__predefined__mover__cell_to_address:
            ANVIL__code__cell_to_address(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(cell_address_mover__cell).cells.start, COMPILER__generate__use_variable(cell_address_mover__byte_count).cells.start, COMPILER__generate__use_variable(cell_address_mover__address).cells.start);
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_add, COMPILER__generate__use_variable(cell_address_mover__address).cells.start, COMPILER__generate__use_variable(cell_address_mover__byte_count).cells.start, ANVIL__unused_cell_ID, COMPILER__generate__use_variable(cell_address_mover__advancement).cells.start);

            break;
        case COMPILER__ast__predefined__mover__file_to_buffer:
            ANVIL__code__file_to_buffer(anvil, COMPILER__generate__use_variable(file_buffer_mover__file_path).cells.start, COMPILER__generate__use_variable(file_buffer_mover__file_path).cells.end, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.start, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.end);

            break;
        case COMPILER__ast__predefined__mover__buffer_to_file:
            ANVIL__code__buffer_to_file(anvil, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.start, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.end, COMPILER__generate__use_variable(file_buffer_mover__file_path).cells.start, COMPILER__generate__use_variable(file_buffer_mover__file_path).cells.end);
            
            break;
        case COMPILER__ast__predefined__mover__buffer_copy__low_to_high:
            ANVIL__code__buffer_to_buffer__low_to_high(anvil, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.start, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.end, COMPILER__generate__use_variable(file_buffer_mover__buffer_1).cells.start, COMPILER__generate__use_variable(file_buffer_mover__buffer_1).cells.end);
            
            break;
        case COMPILER__ast__predefined__mover__buffer_copy__high_to_low:
            ANVIL__code__buffer_to_buffer__high_to_low(anvil, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.start, COMPILER__generate__use_variable(file_buffer_mover__buffer_0).cells.end, COMPILER__generate__use_variable(file_buffer_mover__buffer_1).cells.start, COMPILER__generate__use_variable(file_buffer_mover__buffer_1).cells.end);
            
            break;
        case COMPILER__ast__predefined__delete_file:
            ANVIL__code__delete_file(anvil, COMPILER__generate__use_variable(file_buffer_mover__file_path).cells.start, COMPILER__generate__use_variable(file_buffer_mover__file_path).cells.start + 1);

            break;
        case COMPILER__ast__predefined__allocation__find:
            ANVIL__code__debug__search_for_allocation(anvil, COMPILER__generate__use_variable(allocation__source_buffer).cells.start, COMPILER__generate__use_variable(allocation__source_buffer).cells.end, COMPILER__generate__use_variable(allocation__was_found).cells.start, COMPILER__generate__use_variable(allocation__found_buffer).cells.start, COMPILER__generate__use_variable(allocation__found_buffer).cells.end);
            
            break;
        case COMPILER__ast__predefined__compilation__compile:
            // compile
            ANVIL__code__compile(
                anvil,
                COMPILER__generate__use_variable(compile__user_code_buffers).cells.start,
                COMPILER__generate__use_variable(compile__user_code_buffers).cells.end,
                COMPILER__generate__use_variable(compile__settings).cells.start,
                COMPILER__generate__use_variable(compile__settings).cells.start + 1,
                COMPILER__generate__use_variable(compile__settings).cells.start + 2,
                COMPILER__generate__use_variable(compile__anvil_code).cells.start,
                COMPILER__generate__use_variable(compile__anvil_code).cells.end,
                COMPILER__generate__use_variable(compile__debug_information).cells.start,
                COMPILER__generate__use_variable(compile__debug_information).cells.end,
                COMPILER__generate__use_variable(compile__error_information).cells.start,
                COMPILER__generate__use_variable(compile__error_information).cells.start + 1,
                COMPILER__generate__use_variable(compile__error_information).cells.start + 2,
                COMPILER__generate__use_variable(compile__error_information).cells.start + 3,
                COMPILER__generate__use_variable(compile__error_information).cells.start + 4,
                COMPILER__generate__use_variable(compile__error_information).cells.start + 5
            );

            break;
        case COMPILER__ast__predefined__context__install_program:
            // write buffer start
            ANVIL__code__cell_to_address(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(context__program_buffer).cells.start, ANVIL__srt__constant__cell_byte_size, COMPILER__generate__use_variable(context__context_buffer).cells.start);

            // advance
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_add, COMPILER__generate__use_variable(context__context_buffer).cells.start, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__temp__address);

            // write buffer end
            ANVIL__code__cell_to_address(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(context__program_buffer).cells.end, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__temp__address);

            // advance
            ANVIL__code__operate(anvil, ANVIL__sft__always_run, ANVIL__ot__integer_add, ANVIL__srt__temp__address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__temp__address);

            // write execution current
            ANVIL__code__cell_to_address(anvil, ANVIL__sft__always_run, COMPILER__generate__use_variable(context__program_buffer).cells.start, ANVIL__srt__constant__cell_byte_size, ANVIL__srt__temp__address);

            break;
        case COMPILER__ast__predefined__context__run:
            ANVIL__code__run(anvil, COMPILER__generate__use_variable(context__context_buffer).cells.start, COMPILER__generate__use_variable(context__context_buffer).cells.start + 1, COMPILER__generate__use_variable(context__instruction_count).cells.start);

            break;
        case COMPILER__ast__predefined__time__get_current_time:
            ANVIL__code__get_time(anvil, COMPILER__generate__use_variable(time__get_time_data).cells.start, COMPILER__generate__use_variable(time__get_time_data).cells.start + 1);
            
            break;
        case COMPILER__ast__user_defined_function_call:
            // setup counter
            user_defined_function_call__current_function_io_register = ANVIL__srt__start__function_io;

            // pass inputs
            for (COMPILER__variable_index input_variables = 0; input_variables < statement.function_call__inputs.count; input_variables++) {
                // get variable
                COMPILER__accountling_variable variable = COMPILER__account__functions__get_variable_by_variable_argument(accountling_function.variables, ((COMPILER__accountling_variable_argument*)statement.function_call__inputs.list.buffer.start)[input_variables]);

                // pass one variable
                for (ANVIL__cell_index input_cell = variable.cells.start; input_cell <= variable.cells.end; input_cell++) {
                    // code cell passing
                    ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, input_cell, user_defined_function_call__current_function_io_register);

                    // next cell
                    user_defined_function_call__current_function_io_register++;
                }
            }

            // call function
            ANVIL__code__inner_call__static(anvil, ANVIL__sft__always_run, ((COMPILER__generation_function*)(*workspace).user_defined_functions.list.buffer.start)[statement.function_call__calling_function_header_index].offset__function_start);

            // setup counter
            user_defined_function_call__current_function_io_register = ANVIL__srt__start__function_io;

            // get outputs
            for (COMPILER__variable_index output_variables = 0; output_variables < statement.function_call__outputs.count; output_variables++) {
                // get variable
                COMPILER__accountling_variable variable = COMPILER__account__functions__get_variable_by_variable_argument(accountling_function.variables, ((COMPILER__accountling_variable_argument*)statement.function_call__outputs.list.buffer.start)[output_variables]);

                // pass one variable
                for (ANVIL__cell_index output_cell = variable.cells.start; output_cell <= variable.cells.end; output_cell++) {
                    // code cell passing
                    ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, user_defined_function_call__current_function_io_register, output_cell);

                    // next cell
                    user_defined_function_call__current_function_io_register++;
                }
            }

            break;
        case COMPILER__ast__offset:
            // set offset
            ((ANVIL__offset*)(((COMPILER__generation_function*)(*workspace).user_defined_functions.list.buffer.start)[user_defined_function_index].statement_offsets.list.buffer.start))[statement.offset_index] = ANVIL__get__offset(anvil);

            break;
        case COMPILER__ast__scope:
            // setup conditional jump
            ANVIL__code__operate__jump__static(anvil, ANVIL__sft__always_run, ANVIL__srt__constant__false, COMPILER__generate__use_variable(scope_data.condition).cells.start, ANVIL__srt__constant__false, ANVIL__sft__never_run, ((COMPILER__accountling_scope_header*)accountling_function.scope_headers.list.buffer.start)[statement.scope_index].ending_offset);

            // build scope
            COMPILER__generate__user_defined_function_scope(workspace, accountling_function, user_defined_function_index, statement.scope_data, statement.scope_index, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }

            break;
        default:
            break;
        }
    }

    // setup statement ending offset
    (*scope_header).ending_offset = ANVIL__get__offset(anvil);

    return;
}

// generate user defined function
void COMPILER__generate__user_defined_function(COMPILER__generation_workspace* workspace, COMPILER__accountling_function accountling_function, COMPILER__function_index user_defined_function_index, COMPILER__error* error) {
    // setup helper variables
    COMPILER__generation_function* function = &((COMPILER__generation_function*)(*workspace).user_defined_functions.list.buffer.start)[user_defined_function_index];
    ANVIL__workspace* anvil = &(*workspace).workspace;
    ANVIL__cell_ID current_function_io_register;

    // setup function start offset
    (*function).offset__function_start = ANVIL__get__offset(anvil);

    // setup function prologue
    ANVIL__code__preserve_workspace(anvil, ANVIL__sft__always_run, (*function).cells__workspace.start, (*function).cells__workspace.end);

    // setup function io index
    current_function_io_register = ANVIL__srt__start__function_io;

    // get inputs
    for (COMPILER__variable_index input_variables = 0; input_variables < accountling_function.function_inputs.count; input_variables++) {
        // get variable
        COMPILER__accountling_variable variable = COMPILER__account__functions__get_variable_by_variable_argument(accountling_function.variables, ((COMPILER__accountling_variable_argument*)accountling_function.function_inputs.list.buffer.start)[input_variables]);

        // pass one variable
        for (ANVIL__cell_index input_cell = variable.cells.start; input_cell <= variable.cells.end; input_cell++) {
            // code cell passing
            ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, current_function_io_register, input_cell);

            // next cell
            current_function_io_register++;
        }
    }

    // generate function body code
    COMPILER__generate__user_defined_function_scope(workspace, accountling_function, user_defined_function_index, accountling_function.scope, 0, error);
    if (COMPILER__check__error_occured(error)) {
        return;
    }

    // setup function return offset
    (*function).offset__function_return = ANVIL__get__offset(anvil);

    // setup function io index
    current_function_io_register = ANVIL__srt__start__function_io;

    // pass outputs
    for (COMPILER__variable_index output_variables = 0; output_variables < accountling_function.function_outputs.count; output_variables++) {
        // get variable
        COMPILER__accountling_variable variable = COMPILER__account__functions__get_variable_by_variable_argument(accountling_function.variables, ((COMPILER__accountling_variable_argument*)accountling_function.function_outputs.list.buffer.start)[output_variables]);

        // pass one variable
        for (ANVIL__cell_index output_cell = variable.cells.start; output_cell <= variable.cells.end; output_cell++) {
            // code cell passing
            ANVIL__code__cell_to_cell(anvil, ANVIL__sft__always_run, output_cell, current_function_io_register);

            // next cell
            current_function_io_register++;
        }
    }

    // setup function epilogue
    ANVIL__code__restore_workspace(anvil, ANVIL__sft__always_run, (*function).cells__workspace.start, (*function).cells__workspace.end);

    // return to caller
    ANVIL__code__jump__explicit(anvil, ANVIL__sft__always_run, ANVIL__srt__return_address);

    // setup data offset
    (*function).offset__function_data = ANVIL__get__offset(anvil);

    // write strings
    for (COMPILER__string_index index = 0; index < accountling_function.strings.count; index++) {
        // mark data section
        ANVIL__code__debug__mark_data_section(anvil, (ANVIL__cell)ANVIL__calculate__buffer_length(((ANVIL__buffer*)accountling_function.strings.list.buffer.start)[index]));

        // setup offset
        ((ANVIL__offset*)(*function).data_offsets.list.buffer.start)[index] = ANVIL__get__offset(anvil);

        // embed string
        ANVIL__code__buffer(anvil, ((ANVIL__buffer*)accountling_function.strings.list.buffer.start)[index]);
    }

    return;
}

// generate program
void COMPILER__generate__program(ANVIL__buffer* final_program, COMPILER__accountling_program program, ANVIL__bt generate_kickstarter, ANVIL__stack_size stack_size, COMPILER__error* error) {
    COMPILER__generation_workspace workspace;

    // open generation workspace
    workspace = COMPILER__open__generation_workspace(final_program, program, error);
    if (COMPILER__check__error_occured(error)) {
        return;
    }

    // forge program in two passes
    for (ANVIL__pt pass = ANVIL__pt__get_offsets; pass < ANVIL__pt__COUNT; pass++) {
        // setup pass
        ANVIL__setup__pass(&workspace.workspace, pass);

        // build program
        // if kickstarter enabled
        if (generate_kickstarter == ANVIL__bt__true) {
            // setup kickstarter
            ANVIL__code__start(&workspace.workspace, stack_size, ((COMPILER__generation_function*)workspace.user_defined_functions.list.buffer.start)[program.entry_point].offset__function_start);
        }

        // build each user defined abstraction
        for (COMPILER__function_index index = 0; index < program.functions.headers.category[COMPILER__afht__user_defined].count; index++) {
            // build function
            COMPILER__generate__user_defined_function(&workspace, ((COMPILER__accountling_function*)program.functions.bodies.list.buffer.start)[index], index, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }
        }
    }

    // close workspace
    COMPILER__close__generation_workspace(workspace);

    return;
}

#endif
