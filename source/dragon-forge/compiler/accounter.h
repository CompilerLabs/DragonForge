#ifndef DRAGON__compiler__accounter
#define DRAGON__compiler__accounter

/* Include */
#include "compiler_specifications.h"
#include "parser.h"

/* Account */
// one structure member
typedef struct COMPILER__accountling_structure_member {
    COMPILER__structure_index structure_ID; // the ID of the member's type
    COMPILER__namespace name; // the namespace of the member
    ANVIL__bt predefined; // if the member is built in statically
} COMPILER__accountling_structure_member;

// close a predefined accountling structure member
void COMPILER__close__accountling_structure_member(COMPILER__accountling_structure_member member) {
    // close namespace
    if (member.predefined == ANVIL__bt__true) {
        COMPILER__close__parsling_namespace(member.name);
    }

    return;
}

// append accountling structure member
void COMPILER__append__accountling_structure_member(ANVIL__list* list, COMPILER__accountling_structure_member data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_structure_member), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_structure_member*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_structure_member);

    return;
}

// one structure
typedef struct COMPILER__accountling_structure {
    COMPILER__namespace name; // copied from parser when user defined, opened explicitly when predefined
    ANVIL__counted_list members; // COMPILER__accountling_structure_member
    ANVIL__cell_count cell_count; // the count of all cells
    ANVIL__bt predefined;
} COMPILER__accountling_structure;

// close one structure
void COMPILER__close__accountling_structure(COMPILER__accountling_structure structure) {
    // close name
    if (structure.predefined == ANVIL__bt__true) {
        COMPILER__close__parsling_namespace(structure.name);
    }

    // set current
    ANVIL__current current_member = ANVIL__calculate__current_from_list_filled_index(&structure.members.list);

    // for each member
    while (ANVIL__check__current_within_range(current_member)) {
        // get member
        COMPILER__accountling_structure_member member = *(COMPILER__accountling_structure_member*)current_member.start;

        // close member
        COMPILER__close__accountling_structure_member(member);

        // next member
        current_member.start += sizeof(COMPILER__accountling_structure_member);
    }

    // close members
    ANVIL__close__counted_list(structure.members);

    return;
}

// append accountling structure
void COMPILER__append__accountling_structure(ANVIL__list* list, COMPILER__accountling_structure data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_structure), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_structure*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_structure);

    return;
}

// all structures
typedef struct COMPILER__accountling_structures {
    ANVIL__counted_list name_table; // COMPILER__namespace
    ANVIL__counted_list data_table; // COMPILER__accountling_structure
} COMPILER__accountling_structures;

// close accountling structures
void COMPILER__close__accountling_structures(COMPILER__accountling_structures structures) {
    // close name table
    // close each predefined name
    ANVIL__current current_table_name = ANVIL__calculate__current_from_list_filled_index(&structures.name_table.list);
    COMPILER__structure_index name_index = 0;

    // for each name
    while (ANVIL__check__current_within_range(current_table_name) && name_index < COMPILER__ptt__USER_DEFINED_START) {
        // close name
        COMPILER__close__parsling_namespace(*(COMPILER__namespace*)current_table_name.start);

        // next name
        current_table_name.start += sizeof(COMPILER__namespace);
        name_index++;
    }

    // close list
    ANVIL__close__counted_list(structures.name_table);

    // close structures
    // setup current
    ANVIL__current current_structure = ANVIL__calculate__current_from_list_filled_index(&structures.data_table.list);

    // for each structure
    while (ANVIL__check__current_within_range(current_structure)) {
        // get structure
        COMPILER__accountling_structure structure = *(COMPILER__accountling_structure*)current_structure.start;

        // close structure
        COMPILER__close__accountling_structure(structure);

        // next structure
        current_structure.start += sizeof(COMPILER__accountling_structure);
    }

    // close data table
    ANVIL__close__counted_list(structures.data_table);

    return;
}

// one function header
typedef struct COMPILER__accountling_function_header {
    COMPILER__namespace name;
    ANVIL__counted_list input_types; // COMPILER__structure_index
    ANVIL__counted_list output_types; // COMPILER__structure_index
} COMPILER__accountling_function_header;

// append accountling function header
void COMPILER__append__accountling_function_header(ANVIL__list* list, COMPILER__accountling_function_header data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_function_header), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_function_header*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_function_header);

    return;
}

// one function header index
typedef struct COMPILER__accountling_function_header_location {
    COMPILER__afht category_index;
    COMPILER__function_header_index header_index;
} COMPILER__accountling_function_header_location;

// create custom index
COMPILER__accountling_function_header_location COMPILER__create__accountling_function_header_location(COMPILER__afht category_index, COMPILER__function_header_index header_index) {
    COMPILER__accountling_function_header_location output;

    // setup output
    output.category_index = category_index;
    output.header_index = header_index;

    return output;
}

// all function headers
typedef struct COMPILER__accountling_function_headers {
    ANVIL__counted_list category[COMPILER__afht__COUNT]; // all headers categories, COMPILER__accountling_function_header, COMPILER__afht
} COMPILER__accountling_function_headers;

// open function headers
COMPILER__accountling_function_headers COMPILER__open__accountling_function_headers(COMPILER__error* error) {
    COMPILER__accountling_function_headers output;

    // null init
    output.category[COMPILER__afht__predefined] = ANVIL__create_null__counted_list();
    output.category[COMPILER__afht__user_defined] = ANVIL__create_null__counted_list();
    
    // open all lists
    output.category[COMPILER__afht__predefined] = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_function_header) * 16, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    output.category[COMPILER__afht__user_defined] = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_function_header) * 16, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    
    return output;
}

// close function headers counted list
void COMPILER__close__accountling_function_headers__header_list(ANVIL__counted_list headers, ANVIL__bt close_namespaces) {
    // check for empty list
    if (ANVIL__check__empty_list(headers.list) == ANVIL__bt__true) {
        // nothing to close, quit
        return;
    }

    // close all headers
    // setup current
    ANVIL__current current_function_header = ANVIL__calculate__current_from_list_filled_index(&headers.list);

    // for each header
    while (ANVIL__check__current_within_range(current_function_header)) {
        // get header
        COMPILER__accountling_function_header function_header = *(COMPILER__accountling_function_header*)current_function_header.start;

        // close header
        // if namespace should be closed
        if (close_namespaces == ANVIL__bt__true) {
            // close name
            COMPILER__close__parsling_namespace(function_header.name);
        }

        // close io
        ANVIL__close__counted_list(function_header.input_types);
        ANVIL__close__counted_list(function_header.output_types);

        // next header
        current_function_header.start += sizeof(COMPILER__accountling_function_header);
    }

    // close header buffer
    ANVIL__close__counted_list(headers);

    return;
}

// close function headers
void COMPILER__close__accountling_function_headers(COMPILER__accountling_function_headers headers) {
    // close lists
    COMPILER__close__accountling_function_headers__header_list(headers.category[COMPILER__afht__predefined], ANVIL__bt__true);
    COMPILER__close__accountling_function_headers__header_list(headers.category[COMPILER__afht__user_defined], ANVIL__bt__false);

    return;
}

// one accountling variable argument
typedef struct COMPILER__accountling_variable_argument {
    COMPILER__avat type;
    COMPILER__variable_index index;
} COMPILER__accountling_variable_argument;

// create variable argument
COMPILER__accountling_variable_argument COMPILER__create__accountling_variable_argument(COMPILER__avat type, COMPILER__variable_index index) {
    COMPILER__accountling_variable_argument output;

    output.type = type;
    output.index = index;

    return output;
}

// create null variable argument
COMPILER__accountling_variable_argument COMPILER__create_null__accountling_variable_argument() {
    return COMPILER__create__accountling_variable_argument(COMPILER__avat__COUNT, -1);
}

// append accountling function header
void COMPILER__append__accountling_variable_argument(ANVIL__list* list, COMPILER__accountling_variable_argument data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_variable_argument), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_variable_argument*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_variable_argument);

    return;
}

// one function scope
typedef struct COMPILER__accountling_scope {
    COMPILER__accountling_variable_argument condition;
    ANVIL__counted_list statements; // COMPILER__accountling_statement
} COMPILER__accountling_scope;

// one statement
typedef struct COMPILER__accountling_statement {
    // type
    COMPILER__ast statement_type;
    COMPILER__accountling_function_header_location associated_header;

    // set cell data
    ANVIL__cell_integer_value set_cell__raw_value;
    COMPILER__accountling_variable_argument set_cell__variable_argument;

    // set string data
    COMPILER__string_index set_string__string_value_index;
    COMPILER__accountling_variable_argument set_string__variable_argument;

    // printing data
    COMPILER__accountling_variable_argument print__variable_argument;

    // offset data
    COMPILER__accountling_variable_argument jump__variable_argument;
    COMPILER__offset_index offset_index; // if the statement is a regular offset, use this

    // scope data
    COMPILER__scope_index scope_index;
    COMPILER__accountling_scope scope_data; // COMPILER__accountling_statement

    // copy data
    COMPILER__accountling_variable_argument copy__input;
    COMPILER__accountling_variable_argument copy__output;

    // packer data
    ANVIL__counted_list pack__inputs; // COMPILER__accountling_variable_argument
    COMPILER__accountling_variable_argument pack_increment_start;
    COMPILER__accountling_variable_argument pack_increment_by;
    COMPILER__accountling_variable_argument pack__output;

    // within range data
    COMPILER__accountling_variable_argument within_range__lower_bound;
    COMPILER__accountling_variable_argument within_range__checking;
    COMPILER__accountling_variable_argument within_range__higher_bound;
    COMPILER__accountling_variable_argument within_range__normal_result;
    COMPILER__accountling_variable_argument within_range__inverted_result;

    // integer operations data
    COMPILER__accountling_variable_argument integer_operation__first_argument;
    COMPILER__accountling_variable_argument integer_operation__second_argument;
    COMPILER__accountling_variable_argument integer_operation__output_argument;

    // bit operations
    COMPILER__accountling_variable_argument bit_operation__first_argument;
    COMPILER__accountling_variable_argument bit_operation__second_argument;
    COMPILER__accountling_variable_argument bit_operation__third_argument;
    COMPILER__accountling_variable_argument bit_operation__output_argument;

    // buffer acquisition data
    COMPILER__accountling_variable_argument buffer_acquisition__buffer_length;
    COMPILER__accountling_variable_argument buffer_acquisition__buffer_result;
    COMPILER__accountling_variable_argument buffer_acquisition__buffer_return;

    // cell address movers (variables for both address->cell and cell->address)
    COMPILER__accountling_variable_argument cell_address_mover__cell;
    COMPILER__accountling_variable_argument cell_address_mover__byte_count;
    COMPILER__accountling_variable_argument cell_address_mover__address;
    COMPILER__accountling_variable_argument cell_address_mover__advancement;

    // file buffer movers (variables for file->buffer, buffer->file & buffer->buffer)
    COMPILER__accountling_variable_argument file_buffer_mover__file_path;
    COMPILER__accountling_variable_argument file_buffer_mover__buffer_0;
    COMPILER__accountling_variable_argument file_buffer_mover__buffer_1;

    // buffer structure movers & general structure data
    COMPILER__accountling_variable_argument structure_buffer_mover__structure;
    COMPILER__accountling_variable_argument structure_buffer_mover__buffer;
    COMPILER__accountling_variable_argument structure_buffer_mover__size;
    COMPILER__accountling_variable_argument structure_buffer_mover__advancement;

    // list functions
    COMPILER__accountling_variable_argument list__increase;
    COMPILER__accountling_variable_argument list__input_list;
    COMPILER__accountling_variable_argument list__output_list;
    COMPILER__accountling_variable_argument list__output_buffer;
    COMPILER__accountling_variable_argument list__append_data;

    // compiler assorted functions
    COMPILER__accountling_variable_argument compile__user_code_buffers;
    COMPILER__accountling_variable_argument compile__settings;
    COMPILER__accountling_variable_argument compile__anvil_code;
    COMPILER__accountling_variable_argument compile__debug_information;
    COMPILER__accountling_variable_argument compile__error_information;

    // context assorted functions
    COMPILER__accountling_variable_argument context__context_buffer;
    COMPILER__accountling_variable_argument context__program_buffer;
    COMPILER__accountling_variable_argument context__result_buffer;
    COMPILER__accountling_variable_argument context__error_buffer;
    COMPILER__accountling_variable_argument context__instruction_count;

    // time functions
    COMPILER__accountling_variable_argument time__get_time_data;

    // allocation functions
    COMPILER__accountling_variable_argument allocation__source_buffer;
    COMPILER__accountling_variable_argument allocation__was_found;
    COMPILER__accountling_variable_argument allocation__found_buffer;

    // user defined function call inputs and outputs
    COMPILER__function_header_index function_call__calling_function_header_index;
    ANVIL__counted_list function_call__inputs; // COMPILER__accountling_variable_argument
    ANVIL__counted_list function_call__outputs; // COMPILER__accountling_variable_argument
} COMPILER__accountling_statement;

// create null accountling statement
COMPILER__accountling_statement COMPILER__create_null__accountling_statement() {
    COMPILER__accountling_statement output;

    // setup output
    output.statement_type = COMPILER__ast__invalid;
    output.associated_header= COMPILER__create__accountling_function_header_location(0, 0);
    output.set_cell__raw_value = -1;
    output.set_cell__variable_argument = COMPILER__create_null__accountling_variable_argument();
    output.set_string__string_value_index = -1;
    output.set_string__variable_argument = COMPILER__create_null__accountling_variable_argument();
    output.print__variable_argument = COMPILER__create_null__accountling_variable_argument();
    output.offset_index = -1;
    output.scope_index = -1;
    output.scope_data.statements = ANVIL__create_null__counted_list();
    output.function_call__inputs = ANVIL__create_null__counted_list();
    output.function_call__outputs = ANVIL__create_null__counted_list();

    return output;
}

// append accountling statement
void COMPILER__append__accountling_statement(ANVIL__list* list, COMPILER__accountling_statement data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_statement), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_statement*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_statement);

    return;
}

// scope header
typedef struct COMPILER__accountling_scope_header {
    COMPILER__namespace name;
    COMPILER__accountling_variable_argument argument;
    ANVIL__offset starting_offset;
    ANVIL__offset ending_offset;
} COMPILER__accountling_scope_header;

// append accountling scope header
void COMPILER__append__accountling_scope_header(ANVIL__list* list, COMPILER__accountling_scope_header data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_scope_header), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_scope_header*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_scope_header);

    return;
}

// close accountling scope headers
void COMPILER__close__accountling_scope_headers(ANVIL__counted_list headers, COMPILER__error* error) {
    // search for dragon.master_scope
    // setup name
    COMPILER__namespace temp_name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__define__master_namespace ".master_scope", COMPILER__lt__name, COMPILER__create_null__character_location()), error);

    // search
    for (COMPILER__scope_index scope_index = 0; scope_index < headers.count; scope_index++) {
        // check for name
        if (COMPILER__check__identical_namespaces(((COMPILER__accountling_scope_header*)headers.list.buffer.start)[scope_index].name, temp_name)) {
            // free name
            COMPILER__close__parsling_namespace(((COMPILER__accountling_scope_header*)headers.list.buffer.start)[scope_index].name);

            // exit loop
            break;
        }
    }

    // close temporary name
    COMPILER__close__parsling_namespace(temp_name);

    // close list
    ANVIL__close__counted_list(headers);

    return;
}

// variable member range
typedef struct COMPILER__accountling_variable_range {
    COMPILER__variable_member_index start;
    COMPILER__variable_member_index end;
} COMPILER__accountling_variable_range;

// create index
COMPILER__accountling_variable_range COMPILER__create__accountling_variable_range(COMPILER__variable_member_index start, COMPILER__variable_member_index end) {
    COMPILER__accountling_variable_range output;

    // setup output
    output.start = start;
    output.end = end;

    return output;
}

// variable
typedef struct COMPILER__accountling_variable {
    COMPILER__lexling name;
    COMPILER__variable_type_index type;
    COMPILER__accountling_variable_range members;
    COMPILER__cell_range cells;
} COMPILER__accountling_variable;

// create an accountling variable
COMPILER__accountling_variable COMPILER__create__accountling_variable(COMPILER__lexling name, COMPILER__variable_type_index type, COMPILER__accountling_variable_range members, COMPILER__cell_range cells) {
    COMPILER__accountling_variable output;

    // setup output
    output.name = name;
    output.type = type;
    output.members = members;
    output.cells = cells;

    return output;
}

// create null accountling variable
COMPILER__accountling_variable COMPILER__create_null__accountling_variable() {
    return COMPILER__create__accountling_variable(COMPILER__create_null__lexling(), 0, COMPILER__create__accountling_variable_range(0, 0), COMPILER__create__cell_range(0, 0));
}

// append accountling variable
void COMPILER__append__accountling_variable(ANVIL__list* list, COMPILER__accountling_variable data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_variable), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_variable*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_variable);

    return;
}

// all variable data for one function
typedef struct COMPILER__accountling_variables {
    ANVIL__counted_list lists[COMPILER__avat__COUNT]; // COMPILER__accountling_variable
} COMPILER__accountling_variables;

// open variables
COMPILER__accountling_variables COMPILER__open__variables(COMPILER__error* error) {
    COMPILER__accountling_variables output;

    // open function level variable lists
    output.lists[COMPILER__avat__master] = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_variable) * 256, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    output.lists[COMPILER__avat__member] = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_variable) * 1024, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    return output;
}

// close variables
void COMPILER__close__accountling_variables(COMPILER__accountling_variables variables) {
    // close data
    for (COMPILER__avat index = COMPILER__avat__master; index < COMPILER__avat__COUNT; index++) {
        ANVIL__close__counted_list(variables.lists[index]);
    }

    return;
}

// one function
typedef struct COMPILER__accountling_function {
    // program data
    ANVIL__counted_list offsets; // COMPILER__namespace
    COMPILER__accountling_variables variables;
    ANVIL__counted_list scope_headers; // COMPILER__accountling_scope_header
    ANVIL__counted_list strings; // ANVIL__buffer
    COMPILER__accountling_scope scope;
    ANVIL__counted_list function_inputs; // COMPILER__accountling_variable_argument
    ANVIL__counted_list function_outputs; // COMPILER__accountling_variable_argument

    // statistics
    ANVIL__cell_count next_available_workspace_cell;
} COMPILER__accountling_function;

// append accountling function
void COMPILER__append__accountling_function(ANVIL__list* list, COMPILER__accountling_function data, COMPILER__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMPILER__accountling_function), &(*error).memory_error_occured);

    // append data
    (*(COMPILER__accountling_function*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMPILER__accountling_function);

    return;
}

// close accountling scope
void COMPILER__close__accountling_scope(COMPILER__accountling_scope scope) {
    // close statements
    for (COMPILER__statement_index index = 0; index < scope.statements.count; index++) {
        // get statement
        COMPILER__accountling_statement statement = ((COMPILER__accountling_statement*)scope.statements.list.buffer.start)[index];

        // check statement type
        if (statement.statement_type == COMPILER__ast__scope) {
            // close scope
            COMPILER__close__accountling_scope(statement.scope_data);
        } else if (statement.statement_type == COMPILER__ast__predefined__pack__anything) {
            // close variables list
            ANVIL__close__counted_list(statement.pack__inputs);
        } else if (statement.statement_type == COMPILER__ast__user_defined_function_call) {
            // close io
            ANVIL__close__counted_list(statement.function_call__inputs);
            ANVIL__close__counted_list(statement.function_call__outputs);
        }
    }

    // close list
    ANVIL__close__counted_list(scope.statements);
    
    return;
}

// close strings
void COMPILER__close__accountling_strings(ANVIL__counted_list strings) {
    // close each individual string
    for (COMPILER__string_index index = 0; index < strings.count; index++) {
        // close string
        ANVIL__close__buffer(((ANVIL__buffer*)strings.list.buffer.start)[index]);
    }
    
    // close list
    ANVIL__close__counted_list(strings);

    return;
}

// close function
void COMPILER__close__accountling_function(COMPILER__accountling_function function, COMPILER__error* error) {
    // close offsets
    ANVIL__close__counted_list(function.offsets);

    // close variables
    COMPILER__close__accountling_variables(function.variables);

    // close scope headers
    COMPILER__close__accountling_scope_headers(function.scope_headers, error);

    // close strings
    COMPILER__close__accountling_strings(function.strings);

    // close scope
    COMPILER__close__accountling_scope(function.scope);

    // close function io
    ANVIL__close__counted_list(function.function_inputs);
    ANVIL__close__counted_list(function.function_outputs);

    return;
}

// all functions
typedef struct COMPILER__accountling_functions {
    COMPILER__accountling_function_headers headers;
    ANVIL__counted_list bodies; // COMPILER__accountling_function
} COMPILER__accountling_functions;

// close accountling functions
void COMPILER__close__accountling_functions(COMPILER__accountling_functions functions, COMPILER__error* error) {
    // close headers
    COMPILER__close__accountling_function_headers(functions.headers);

    // close bodies
    for (COMPILER__function_index index = 0; index < functions.bodies.count; index++) {
        // close one body
        COMPILER__close__accountling_function(((COMPILER__accountling_function*)functions.bodies.list.buffer.start)[index], error);
    }
    
    // close list
    ANVIL__close__counted_list(functions.bodies);

    return;
}

// one program
typedef struct COMPILER__accountling_program {
    COMPILER__accountling_structures structures;
    COMPILER__accountling_functions functions;
    COMPILER__function_index entry_point;
} COMPILER__accountling_program;

// close a program
void COMPILER__close__accountling_program(COMPILER__accountling_program program, COMPILER__error* error) {
    // close structures
    COMPILER__close__accountling_structures(program.structures);

    // close functions
    COMPILER__close__accountling_functions(program.functions, error);

    return;
}

// check to see if a type name exists
COMPILER__structure_index COMPILER__find__accountling_structure_name_index(ANVIL__counted_list structure_names, COMPILER__namespace searching_for) {
    COMPILER__structure_index output = 0;

    // check each structure entry or name
    ANVIL__current current_structure = ANVIL__calculate__current_from_list_filled_index(&structure_names.list);

    // check each entry
    while (ANVIL__check__current_within_range(current_structure)) {
        // get structure
        COMPILER__namespace name = *(COMPILER__namespace*)current_structure.start;

        // check name
        if (COMPILER__check__identical_namespaces(searching_for, name) == ANVIL__bt__true) {
            // match!
            return output;
        }

        // next structure
        current_structure.start += sizeof(COMPILER__namespace);
        output++;
    }

    // not found
    return output;
}

// check to see if a type member exists
COMPILER__structure_member_index COMPILER__find__accountling_structure_member_name_index(COMPILER__accountling_structure structure, COMPILER__namespace searching_for) {
    COMPILER__structure_member_index output = 0;

    // check each structure entry or name
    ANVIL__current current_member = ANVIL__calculate__current_from_list_filled_index(&structure.members.list);

    // check each entry
    while (ANVIL__check__current_within_range(current_member)) {
        // get member
        COMPILER__accountling_structure_member member = *(COMPILER__accountling_structure_member*)current_member.start;

        // check name
        if (COMPILER__check__identical_namespaces(searching_for, member.name) == ANVIL__bt__true) {
            // match!
            return output;
        }

        // next structure
        current_member.start += sizeof(COMPILER__accountling_structure_member);
        output++;
    }

    // not found
    return output;
}

// generate predefined type names
ANVIL__counted_list COMPILER__account__structures__generate_predefined_type_names(ANVIL__counted_list names, COMPILER__error* error) {
    // loop through blueprint and generate structures
    // setup current
    ANVIL__current current_blueprintling = ANVIL__create__buffer((ANVIL__address)COMPILER__global__predefined_types, (ANVIL__address)(COMPILER__global__predefined_types + sizeof(COMPILER__global__predefined_types) - 1));

    // loop until end of blueprint
    while (ANVIL__check__current_within_range(current_blueprintling) && COMPILER__read__blueprintling(&current_blueprintling) == COMPILER__abt__define_type) {
        // skip past define type marker
        COMPILER__next__blueprintling(&current_blueprintling);

        // read predefined type definition
        COMPILER__ptt type = COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // generate and append structure name
        // get the name of the function
        COMPILER__namespace name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_type_names[type], COMPILER__lt__name, COMPILER__create_null__character_location()), error);

        // next name index
        names.count++;

        // append the name
        COMPILER__append__namespace(&names.list, name, error);
        if (COMPILER__check__error_occured(error)) {
            return names;
        }

        // generate and append structure members
        // get member count
        COMPILER__structure_member_count member_count = COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // skip past members
        for (COMPILER__structure_member_index member_index = 0; member_index < member_count; member_index++) {
            // get the member ID
            COMPILER__read_and_next__blueprintling(&current_blueprintling);

            // get the member name
            COMPILER__read_and_next__blueprintling(&current_blueprintling);
        }
    }

    return names;
}

// generate predefined type data
ANVIL__counted_list COMPILER__account__structures__generate_predefined_type_data(ANVIL__counted_list structure_data, COMPILER__error* error) {
    // loop through blueprint and generate structures
    // setup current
    ANVIL__current current_blueprintling = ANVIL__create__buffer((ANVIL__address)COMPILER__global__predefined_types, (ANVIL__address)(COMPILER__global__predefined_types + sizeof(COMPILER__global__predefined_types) - 1));

    // loop until end of blueprint
    while (ANVIL__check__current_within_range(current_blueprintling) && COMPILER__read__blueprintling(&current_blueprintling) == COMPILER__abt__define_type) {
        // skip past define type marker
        COMPILER__next__blueprintling(&current_blueprintling);

        // read predefined type definition
        COMPILER__ptt definition_type = COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // setup accountling structure structure
        COMPILER__accountling_structure accountling_structure;
        accountling_structure.predefined = ANVIL__bt__true;
        accountling_structure.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_type_names[definition_type], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return structure_data;
        }
        accountling_structure.members = ANVIL__create__counted_list(COMPILER__open__list_with_error(sizeof(COMPILER__accountling_structure_member) * 16, error), 0);
        if (COMPILER__check__error_occured(error)) {
            return structure_data;
        }

        // generate and append structure members
        // get member count
        COMPILER__structure_member_count member_count = COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // record members
        for (COMPILER__structure_member_index member_index = 0; member_index < member_count; member_index++) {
            // get the member ID
            COMPILER__ptt member_type = COMPILER__read_and_next__blueprintling(&current_blueprintling);
            COMPILER__ptmnt member_name_ID = COMPILER__read_and_next__blueprintling(&current_blueprintling);

            // setup member
            COMPILER__accountling_structure_member member_temp;
            member_temp.structure_ID = member_type;
            member_temp.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_type_member_names[member_name_ID], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
            member_temp.predefined = ANVIL__bt__true;
            if (COMPILER__check__error_occured(error)) {
                return structure_data;
            }

            // record type
            COMPILER__append__accountling_structure_member(&accountling_structure.members.list, member_temp, error);
            if (COMPILER__check__error_occured(error)) {
                return structure_data;
            }
            accountling_structure.members.count++;
        }

        // append structure
        COMPILER__append__accountling_structure(&structure_data.list, accountling_structure, error);
        if (COMPILER__check__error_occured(error)) {
            return structure_data;
        }
        structure_data.count++;
    }

    return structure_data;
}

// perform actual recurse
void COMPILER__account__check__recursive_structures__do_search(ANVIL__counted_list structures, COMPILER__stack_index current_stack_index, ANVIL__buffer type_trace, COMPILER__error* error) {
    // get structure ID
    COMPILER__structure_index searching_for = ((COMPILER__structure_index*)type_trace.start)[current_stack_index];

    // search previous structs
    for (COMPILER__structure_index checking_against = 0; checking_against < current_stack_index; checking_against++) {
        // check struct
        if (searching_for == ((COMPILER__structure_index*)type_trace.start)[checking_against]) {
            // match found, compiler error
            *error = COMPILER__open__error("Accounting Error: Recursive structure definition detected.", COMPILER__get__namespace_lexling_location(((COMPILER__accountling_structure*)structures.list.buffer.start)[searching_for].name));

            return;
        }
    }

    // search down tree for each member
    for (COMPILER__structure_member_index member_index = 0; member_index < ((COMPILER__accountling_structure*)structures.list.buffer.start)[searching_for].members.count; member_index++) {
        // get member
        COMPILER__structure_index recursing_for = ((COMPILER__accountling_structure_member*)((COMPILER__accountling_structure*)structures.list.buffer.start)[searching_for].members.list.buffer.start)[member_index].structure_ID;

        // if is a dead end
        if (recursing_for > structures.count) {
            // skip pass
            continue;
        }

        // append structure ID
        ((COMPILER__structure_index*)type_trace.start)[current_stack_index + 1] = recursing_for;

        // otherwise, search for structure
        COMPILER__account__check__recursive_structures__do_search(structures, current_stack_index + 1, type_trace, error);
        if (COMPILER__check__error_occured(error)) {
            return;
        }
    }

    return;
}

// check for recursive types
void COMPILER__account__check__recursive_structures(ANVIL__counted_list structures, COMPILER__error* error) {
    // setup tracker
    ANVIL__buffer type_trace = ANVIL__open__buffer(sizeof(COMPILER__structure_index) * structures.count);

    // check each structure
    for (COMPILER__structure_index top_index = 0; top_index < structures.count; top_index++) {
        // setup starting index
        COMPILER__stack_index starting_index = 0;

        // fresh init trace data
        for (COMPILER__structure_index trace_index = 0; trace_index < structures.count; trace_index++) {
            // init as not found
            ((COMPILER__structure_index*)type_trace.start)[trace_index] = starting_index;
        }

        // push first type onto stack
        ((COMPILER__structure_index*)type_trace.start)[starting_index] = top_index;

        // recurse
        COMPILER__account__check__recursive_structures__do_search(structures, starting_index, type_trace, error);
        if (COMPILER__check__error_occured(error)) {
            goto quit;
        }
    }

    // quit offset
    quit:

    // destroy trace
    ANVIL__close__buffer(type_trace);

    return;
}

// search one structure for its cell count
void COMPILER__account__structures__calculate_structure_cell_counts__search(ANVIL__cell_count* cell_count, COMPILER__accountling_structures structures, COMPILER__structure_index searching, ANVIL__tab_count tabs) {
    // get structure
    COMPILER__accountling_structure structure = ((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[searching];

    // tally based on sub structures cell count
    for (COMPILER__structure_member_index index = 0; index < structure.members.count; index++) {
        // get structure index
        COMPILER__structure_index current_structure_index = ((COMPILER__accountling_structure_member*)structure.members.list.buffer.start)[index].structure_ID;

        // determine if should search based on structure index
        if (current_structure_index < structures.data_table.count) {
            // search sub-structures
            COMPILER__account__structures__calculate_structure_cell_counts__search(cell_count, structures, current_structure_index, tabs + 1);
        } else {
            // cell found
            (*cell_count)++;
        }
    }

    return;
}

// get the cell counts for all structures
void COMPILER__account__structures__calculate_structure_cell_counts(COMPILER__accountling_structures structures) {
    // for each structure
    for (COMPILER__structure_index index = 0; index < structures.data_table.count; index++) {
        // update structure
        // setup count
        ANVIL__cell_count cell_count = 0;

        // calculate count
        COMPILER__account__structures__calculate_structure_cell_counts__search(&cell_count, structures, index, 1);

        // apply count
        ((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[index].cell_count = cell_count;
    }

    return;
}

// generate and get all types
COMPILER__accountling_structures COMPILER__account__structures(COMPILER__parsling_program parsling_program, COMPILER__error* error) {
    COMPILER__accountling_structures output;

    // open tables
    output.name_table = ANVIL__create__counted_list(COMPILER__open__list_with_error(sizeof(COMPILER__namespace) * 32, error), 0);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    output.data_table = ANVIL__create__counted_list(COMPILER__open__list_with_error(sizeof(COMPILER__accountling_structure) * 32, error), 0);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // setup predefined structure names
    output.name_table = COMPILER__account__structures__generate_predefined_type_names(output.name_table, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // get all names
    // for each structure
    ANVIL__current current_structure = ANVIL__calculate__current_from_list_filled_index(&parsling_program.structures.list);

    // for each structure
    while (ANVIL__check__current_within_range(current_structure)) {
        // get structure
        COMPILER__parsling_structure structure = *(COMPILER__parsling_structure*)current_structure.start;

        // for each structure name
        ANVIL__current current_structure_name = ANVIL__calculate__current_from_list_filled_index(&structure.type_names.list);

        // for each structure name
        while (ANVIL__check__current_within_range(current_structure_name)) {
            // get name
            COMPILER__parsling_argument name = *(COMPILER__parsling_argument*)current_structure_name.start;

            // check if structure name is taken
            if (COMPILER__find__accountling_structure_name_index(output.name_table, name.name) < output.name_table.count) {
                // setup error
                *error = COMPILER__open__error("Accounting Error: Structure name is defined more than once.", COMPILER__get__namespace_lexling_location(name.name));

                return output;
            }

            // append name
            COMPILER__append__namespace(&output.name_table.list, name.name, error);
            output.name_table.count++;
            if (COMPILER__check__error_occured(error)) {
                return output;
            }

            // next structure name
            current_structure_name.start += sizeof(COMPILER__parsling_argument);
        }

        // next structure
        current_structure.start += sizeof(COMPILER__parsling_structure);
    }

    // setup predefined structures
    output.data_table = COMPILER__account__structures__generate_predefined_type_data(output.data_table, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // get all structure data
    // for each structure
    current_structure = ANVIL__calculate__current_from_list_filled_index(&parsling_program.structures.list);

    // for each structure
    while (ANVIL__check__current_within_range(current_structure)) {
        // get structure
        COMPILER__parsling_structure parsling_structure = *(COMPILER__parsling_structure*)current_structure.start;

        // for each structure name
        ANVIL__current current_structure_name = ANVIL__calculate__current_from_list_filled_index(&parsling_structure.type_names.list);

        // for each structure name
        while (ANVIL__check__current_within_range(current_structure_name)) {
            // get name
            COMPILER__parsling_argument structure_name = *(COMPILER__parsling_argument*)current_structure_name.start;

            // setup accountling structure structure
            COMPILER__accountling_structure accountling_structure;
            accountling_structure.predefined = ANVIL__bt__false;
            accountling_structure.name = structure_name.name;
            accountling_structure.members = ANVIL__create__counted_list(COMPILER__open__list_with_error(sizeof(COMPILER__accountling_structure_member) * 16, error), 0);
            if (COMPILER__check__error_occured(error)) {
                return output;
            }

            // for each structure member
            ANVIL__current current_structure_member = ANVIL__calculate__current_from_list_filled_index(&parsling_structure.arguments.list);

            // check for no arguments, this is an error
            if (parsling_structure.arguments.count < 1) {
                // set error
                *error = COMPILER__open__error("Accounting Error: A structure cannot have zero members.", COMPILER__get__namespace_lexling_location(structure_name.name));

                // close members
                ANVIL__close__counted_list(accountling_structure.members);

                return output;
            }

            // for each structure member
            while (ANVIL__check__current_within_range(current_structure_member)) {
                // get member
                COMPILER__parsling_argument parsling_member = *(COMPILER__parsling_argument*)current_structure_member.start;

                // if unknown type
                if (COMPILER__find__accountling_structure_name_index(output.name_table, parsling_member.type) >= output.name_table.count) {
                    // set error
                    *error = COMPILER__open__error("Accounting Error: A structure member is of unknown type.", COMPILER__get__namespace_lexling_location(parsling_member.type));

                    // close members
                    ANVIL__close__counted_list(accountling_structure.members);

                    return output;
                }

                // if type is defined in itself (not including longer recursions, just first level ones)
                if (COMPILER__check__identical_namespaces(accountling_structure.name, parsling_member.type)) {
                    // set error
                    *error = COMPILER__open__error("Accounting Error: A structure has itself as a member.", COMPILER__get__namespace_lexling_location(parsling_member.type));

                    // close members
                    ANVIL__close__counted_list(accountling_structure.members);

                    return output;
                }

                // create member
                COMPILER__accountling_structure_member accountling_member;
                accountling_member.structure_ID = COMPILER__find__accountling_structure_name_index(output.name_table, parsling_member.type);
                accountling_member.name = parsling_member.name;
                accountling_member.predefined = ANVIL__bt__false;

                // check for duplicate member
                if (COMPILER__find__accountling_structure_member_name_index(accountling_structure, accountling_member.name) < accountling_structure.members.count) {
                    // set error
                    *error = COMPILER__open__error("Accounting Error: A structure has two or more arguments with the same name.", COMPILER__get__namespace_lexling_location(parsling_member.name));

                    // free unused member
                    ANVIL__close__counted_list(accountling_structure.members);

                    return output;
                }

                // append member
                COMPILER__append__accountling_structure_member(&accountling_structure.members.list, accountling_member, error);
                if (COMPILER__check__error_occured(error)) {
                    return output;
                }
                accountling_structure.members.count++;

                // next member
                current_structure_member.start += sizeof(COMPILER__parsling_argument);
            }

            // append structure to list
            COMPILER__append__accountling_structure(&output.data_table.list, accountling_structure, error);
            if (COMPILER__check__error_occured(error)) {
                return output;
            }
            output.data_table.count++;

            // next structure name
            current_structure_name.start += sizeof(COMPILER__parsling_argument);
        }

        // next structure
        current_structure.start += sizeof(COMPILER__parsling_structure);
    }

    // check every type for recursive type definitions
    COMPILER__account__check__recursive_structures(output.data_table, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // apply a cell count for each structure
    COMPILER__account__structures__calculate_structure_cell_counts(output);

    return output;
}

// check a specific function header
ANVIL__bt COMPILER__account__search_for_function_header__check_headers_equal(COMPILER__accountling_function_header a, COMPILER__accountling_function_header b) {
    // check namespace
    if (COMPILER__check__identical_namespaces(a.name, b.name)) {
        // check inputs
        if (a.input_types.count != b.input_types.count) {
            // not a match
            goto not_equal;
        }
        for (COMPILER__structure_index input_ID = 0; input_ID < a.input_types.count; input_ID++) {
            // check type
            if (((COMPILER__structure_index*)a.input_types.list.buffer.start)[input_ID] != ((COMPILER__structure_index*)b.input_types.list.buffer.start)[input_ID]) {
                // not a match
                goto not_equal;
            }
        }

        // check outputs
        if (a.output_types.count != b.output_types.count) {
            // not a match
            goto not_equal;
        }
        for (COMPILER__structure_index output_ID = 0; output_ID < a.output_types.count; output_ID++) {
            // check type
            if (((COMPILER__structure_index*)a.output_types.list.buffer.start)[output_ID] != ((COMPILER__structure_index*)b.output_types.list.buffer.start)[output_ID]) {
                // not a match
                goto not_equal;
            }
        }

        // everything is equal, match!
        return ANVIL__bt__true;
    }

    not_equal:

    // not equal
    return ANVIL__bt__false;
}

// search for a function header in one list
COMPILER__function_header_index COMPILER__account__search_for_function_header__search_list(ANVIL__counted_list headers_list, COMPILER__accountling_function_header searching_for) {
    // setup index
    COMPILER__function_header_index function_index = 0;

    // search each function header
    // setup current
    ANVIL__current current_header = ANVIL__calculate__current_from_list_filled_index(&headers_list.list);

    // for each header (unless found which quits early)
    while (ANVIL__check__current_within_range(current_header)) {
        // get header
        COMPILER__accountling_function_header header = *(COMPILER__accountling_function_header*)current_header.start;

        // check for match
        if (COMPILER__account__search_for_function_header__check_headers_equal(header, searching_for) == ANVIL__bt__true) {
            // match!
            return function_index;
        }

        // next header
        current_header.start += sizeof(COMPILER__accountling_function_header);
        function_index++;
    }

    // not found
    return function_index;
}

// search for a header across all lists
COMPILER__accountling_function_header_location COMPILER__account__search_for_function_header__search_all_lists(COMPILER__accountling_function_headers headers, COMPILER__accountling_function_header searching_for) {
    COMPILER__accountling_function_header_location output;

    // init output
    output = COMPILER__create__accountling_function_header_location(COMPILER__afht__predefined, 0);

    // search each list until found
    for (; output.category_index < COMPILER__afht__COUNT; output.category_index++) {
        // search one list
        output.header_index = COMPILER__account__search_for_function_header__search_list(headers.category[output.category_index], searching_for);

        // check search status
        if (output.header_index < headers.category[output.category_index].count) {
            // match!
            return output;
        }
    }

    // not found
    return output;
}

// generate all predefined function headers
COMPILER__accountling_function_headers COMPILER__account__functions__generate_predefined_function_headers(COMPILER__accountling_structures structures, COMPILER__accountling_function_headers headers, COMPILER__error* error) {
    // generate individual functions
    // setup current blueprintling
    ANVIL__current current_blueprintling = ANVIL__create__buffer((ANVIL__address)COMPILER__global__predefined_one_off_function_calls, (ANVIL__address)COMPILER__global__predefined_one_off_function_calls + sizeof(COMPILER__global__predefined_one_off_function_calls) - 1);

    // for each definition
    while (ANVIL__check__current_within_range(current_blueprintling) && COMPILER__read__blueprintling(&current_blueprintling) == COMPILER__abt__define_function_call) {
        // skip past marker
        COMPILER__next__blueprintling(&current_blueprintling);

        // setup variable
        COMPILER__accountling_function_header header;

        // get predefined function call type
        COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // get name ID
        COMPILER__pfcnt name_ID = COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[name_ID], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // count inputs
        COMPILER__input_count input_count = COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // allocate inputs
        header.input_types = ANVIL__create__counted_list(COMPILER__open__list_with_error(sizeof(COMPILER__structure_index) * 8, error), 0);
        header.input_types.count = input_count;

        // get inputs
        for (COMPILER__input_count input_ID = 0; input_ID < input_count; input_ID++) {
            // get type
            COMPILER__append(&header.input_types.list, COMPILER__structure_index, (COMPILER__structure_index)COMPILER__read_and_next__blueprintling(&current_blueprintling), error);
            if (COMPILER__check__error_occured(error)) {
                return headers;
            }
        }

        // count outputs
        COMPILER__output_count output_count = COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // allocate outputs
        header.output_types = ANVIL__create__counted_list(COMPILER__open__list_with_error(sizeof(COMPILER__structure_index) * 8, error), 0);
        header.output_types.count = output_count;

        // get outputs
        for (COMPILER__output_count output_ID = 0; output_ID < output_count; output_ID++) {
            // get type
            COMPILER__append(&header.output_types.list, COMPILER__structure_index, (COMPILER__structure_index)COMPILER__read_and_next__blueprintling(&current_blueprintling), error);
            if (COMPILER__check__error_occured(error)) {
                return headers;
            }
        }

        // append function header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // generate copy headers
    for (COMPILER__structure_index index = 0; index < structures.name_table.count; index++) {
        // setup variable
        COMPILER__accountling_function_header header;

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__copy], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // allocate io
        header.input_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // append io data
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.input_types.count = 1;
        COMPILER__append(&header.output_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types.count = 1;

        // append header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // generate packer headers (skip dragon.cell)
    for (COMPILER__structure_index structure_index = 1; structure_index < structures.data_table.count; structure_index++) {
        // setup variable
        COMPILER__accountling_function_header header;

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__pack], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // setup inputs
        // allocate inputs
        header.input_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * ((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[structure_index].members.count, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // append each input
        for (COMPILER__structure_member_index member_index = 0; member_index < ((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[structure_index].members.count; member_index++) {
            // append input
            COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + ((COMPILER__accountling_structure_member*)((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[structure_index].members.list.buffer.start)[member_index].structure_ID, error);
            if (COMPILER__check__error_occured(error)) {
                return headers;
            }
            header.input_types.count++;
        }

        // setup outputs
        // allocate outputs
        header.output_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        
        // append destination
        COMPILER__append(&header.output_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + structure_index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types.count = 1;

        // append header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // generate null packer headers
    for (COMPILER__structure_index structure_index = 0; structure_index < structures.data_table.count; structure_index++) {
        // setup variable
        COMPILER__accountling_function_header header;

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__pack_null], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // setup inputs
        // allocate inputs (no inputs so nothing to append)
        header.input_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.input_types.count = 0;

        // setup outputs
        // allocate outputs
        header.output_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        
        // append destination
        COMPILER__append(&header.output_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + structure_index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types.count = 1;

        // append header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // generate increment packer headers
    for (COMPILER__structure_index structure_index = 0; structure_index < structures.data_table.count; structure_index++) {
        // setup variable
        COMPILER__accountling_function_header header;

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__pack_increment], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // setup inputs
        // allocate inputs
        header.input_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append packer starting point value
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__variable, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append packer increment by value
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__variable, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.input_types.count = 2;

        // setup outputs
        // allocate outputs
        header.output_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        
        // append destination
        COMPILER__append(&header.output_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + structure_index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types.count = 1;

        // append header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // generate structure sizes
    for (COMPILER__structure_index structure_index = 0; structure_index < structures.data_table.count; structure_index++) {
        // setup variable
        COMPILER__accountling_function_header header;

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__structure__structure_byte_size], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // setup inputs
        // allocate inputs
        header.input_types = COMPILER__open__counted_list_with_error(sizeof(COMPILER__structure_index) * 1, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append structure
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + structure_index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.input_types.count = 1;

        // setup outputs
        // allocate outputs
        header.output_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append structure
        COMPILER__append(&header.output_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types.count = 1;

        // append header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // generate structure to buffer
    for (COMPILER__structure_index structure_index = 0; structure_index < structures.data_table.count; structure_index++) {
        // setup variable
        COMPILER__accountling_function_header header;

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__structure__structure_to_buffer], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // setup inputs
        // allocate inputs
        header.input_types = COMPILER__open__counted_list_with_error(sizeof(COMPILER__structure_index) * 2, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append structure
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + structure_index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append structure
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.input_types.count = 2;

        // setup outputs
        // allocate outputs
        header.output_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append structure
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types.count = 1;

        // append header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // generate buffer to structure
    for (COMPILER__structure_index structure_index = 0; structure_index < structures.data_table.count; structure_index++) {
        // setup variable
        COMPILER__accountling_function_header header;

        // setup name
        header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__structure__buffer_to_structure], COMPILER__lt__name, COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }

        // setup inputs
        // allocate inputs
        header.input_types = COMPILER__open__counted_list_with_error(sizeof(COMPILER__structure_index) * 2, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append structure
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        // append structure
        COMPILER__append(&header.input_types.list, COMPILER__structure_index, COMPILER__aat__COUNT + structure_index, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.input_types.count = 2;

        // setup outputs
        // allocate outputs
        header.output_types = ANVIL__open__counted_list(sizeof(COMPILER__structure_index) * 1, &(*error).memory_error_occured);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        header.output_types.count = 0;

        // append header
        COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__predefined].list, header, error);
        if (COMPILER__check__error_occured(error)) {
            return headers;
        }
        headers.category[COMPILER__afht__predefined].count++;
    }

    // done
    return headers;
}

// account all user defined function header arguments
ANVIL__counted_list COMPILER__account__functions__user_defined_function_header_arguments(ANVIL__counted_list structure_names, ANVIL__counted_list parsling_header_arguments, COMPILER__error* error) {
    // allocate structure index list
    ANVIL__counted_list output = COMPILER__open__counted_list_with_error(sizeof(COMPILER__structure_index) * 8, error);

    // setup current
    ANVIL__current current_parsling_argument = ANVIL__calculate__current_from_list_filled_index(&parsling_header_arguments.list);

    // for each argument
    while (ANVIL__check__current_within_range(current_parsling_argument)) {
        // get argument
        COMPILER__parsling_argument parsling_argument = *(COMPILER__parsling_argument*)current_parsling_argument.start;

        // validate and check the argument for a valid argument type
        // get index
        COMPILER__structure_index structure_index = COMPILER__find__accountling_structure_name_index(structure_names, parsling_argument.type);

        // if index deems invalid
        if (structure_index >= structure_names.count) {
            // set error
            *error = COMPILER__open__error("Accounting Error: A function header argument has an unknown type.", COMPILER__get__namespace_lexling_location(parsling_argument.type));

            return output;
        }

        // record structure index
        COMPILER__append(&output.list, COMPILER__structure_index, structure_index + COMPILER__aat__COUNT, error);
        if (COMPILER__check__error_occured(error)) {
            return output;
        }

        // next argument
        current_parsling_argument.start += sizeof(COMPILER__parsling_argument);
        output.count++;
    }

    return output;
}

// account all user defined function headers
COMPILER__accountling_function_headers COMPILER__account__functions__user_defined_function_headers(COMPILER__accountling_function_headers headers, COMPILER__parsling_program parsling_program, ANVIL__counted_list structure_names, COMPILER__error* error) {
    // for each function
    // setup current
    ANVIL__current current_function = ANVIL__calculate__current_from_list_filled_index(&parsling_program.functions.list);

    // for each function
    while (ANVIL__check__current_within_range(current_function)) {
        // get parsling function
        COMPILER__parsling_function parsling_function = *(COMPILER__parsling_function*)current_function.start;

        // account header
        {
            // setup variable
            COMPILER__accountling_function_header header;

            // get name
            header.name = parsling_function.header.name.name;

            // get inputs
            header.input_types = COMPILER__account__functions__user_defined_function_header_arguments(structure_names, parsling_function.header.inputs, error);
            if (COMPILER__check__error_occured(error)) {
                return headers;
            }

            // get outputs
            header.output_types = COMPILER__account__functions__user_defined_function_header_arguments(structure_names, parsling_function.header.outputs, error);
            if (COMPILER__check__error_occured(error)) {
                return headers;
            }

            // check if function header is already used
            // check namespace
            if (COMPILER__account__search_for_function_header__search_all_lists(headers, header).category_index < COMPILER__afht__COUNT) {
                // already defined, error
                *error = COMPILER__open__error("Accounting Error: Two or more functions have the same header (name & io).", COMPILER__get__namespace_lexling_location(header.name));

                return headers;
            }

            // append header
            COMPILER__append__accountling_function_header(&headers.category[COMPILER__afht__user_defined].list, header, error);
            if (COMPILER__check__error_occured((error))) {
                return headers;
            }
            headers.category[COMPILER__afht__user_defined].count++;
        }

        // next function
        current_function.start += sizeof(COMPILER__parsling_function);
    }

    return headers;
}

// check scope headers for name
COMPILER__scope_index COMPILER__account__functions__get_scope_index(COMPILER__accountling_function* accountling_function, COMPILER__namespace scope_name) {
    COMPILER__scope_index output = 0;

    // search each scope header for name
    while (output < (*accountling_function).scope_headers.count) {
        // check name
        if (COMPILER__check__identical_namespaces(((COMPILER__accountling_scope_header*)(*accountling_function).scope_headers.list.buffer.start)[output].name, scope_name) == ANVIL__bt__true) {
            // match!
            return output;
        }

        // not a match, next one
        output++;
    }

    // no match found
    return output;
}

// check offsets for name
COMPILER__offset_index COMPILER__account__functions__get_offset_index(COMPILER__accountling_function* accountling_function, COMPILER__namespace offset_name) {
    COMPILER__offset_index output = 0;

    // search each offset for name
    while (output < (*accountling_function).offsets.count) {
        // check name
        if (COMPILER__check__identical_namespaces(((COMPILER__namespace*)(*accountling_function).offsets.list.buffer.start)[output], offset_name) == ANVIL__bt__true) {
            // match!
            return output;
        }

        // not a match, next one
        output++;
    }

    // no match found
    return output;
}

// get all scope level data
void COMPILER__account__functions__get_scope_level_data(COMPILER__accountling_function* accountling_function, COMPILER__parsling_scope parsling_scope, COMPILER__error* error) {
    // search through each statement
    // setup current
    ANVIL__current current_statement = ANVIL__calculate__current_from_list_filled_index(&parsling_scope.statements.list);

    // for each statement
    while (ANVIL__check__current_within_range(current_statement)) {
        // get statement
        COMPILER__parsling_statement parsling_statement = *(COMPILER__parsling_statement*)current_statement.start;

        // check type
        if (parsling_statement.type == COMPILER__stt__subscope) {
            // check for scope name
            if (COMPILER__account__functions__get_scope_index(accountling_function, parsling_statement.name.name) < (*accountling_function).scope_headers.count) {
                // error, scope already exists
                *error = COMPILER__open__error("Accounting Error: A function has two scopes with the same name.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

                return;
            }

            // check for offset name
            if (COMPILER__account__functions__get_offset_index(accountling_function, parsling_statement.name.name) < (*accountling_function).offsets.count) {
                // error, offset already exists
                *error = COMPILER__open__error("Accounting Error: A scope has the name of an offset.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

                return;
            }

            // create new scope header
            COMPILER__accountling_scope_header scope_header;
            scope_header.name = parsling_statement.name.name;
            scope_header.starting_offset = ANVIL__invalid_offset;
            scope_header.ending_offset = ANVIL__invalid_offset;

            // if scope name does not exist, append
            COMPILER__append__accountling_scope_header(&(*accountling_function).scope_headers.list, scope_header, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }
            (*accountling_function).scope_headers.count++;

            // search for more headers in sub-scope
            COMPILER__account__functions__get_scope_level_data(accountling_function, parsling_statement.subscope, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }
        } else if (parsling_statement.type == COMPILER__stt__offset) {
            // check for scope name
            if (COMPILER__account__functions__get_scope_index(accountling_function, parsling_statement.name.name) < (*accountling_function).scope_headers.count) {
                // error, scope already exists
                *error = COMPILER__open__error("Accounting Error: A function has a scope with the same name as an offset.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

                return;
            }

            // check for offset name
            if (COMPILER__account__functions__get_offset_index(accountling_function, parsling_statement.name.name) < (*accountling_function).offsets.count) {
                // error, offset already exists
                *error = COMPILER__open__error("Accounting Error: An offset is declared twice.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

                return;
            }

            // append offset
            COMPILER__append__namespace(&(*accountling_function).offsets.list, parsling_statement.name.name, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }
            (*accountling_function).offsets.count++;
        }

        // next statement
        current_statement.start += sizeof(COMPILER__parsling_statement);
    }

    return;
}

// get all data that is function wide (except variables)
void COMPILER__account__functions__get_function_level_data(COMPILER__accountling_function* accountling_function, COMPILER__parsling_scope parsling_scope, COMPILER__error* error) {
    // create scope header for master scope
    COMPILER__accountling_scope_header scope_header;
    scope_header.name = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__define__master_namespace ".master_scope", COMPILER__lt__name, COMPILER__create_null__character_location()), error);
    scope_header.starting_offset = ANVIL__invalid_offset;
    scope_header.ending_offset = ANVIL__invalid_offset;

    // append master scope header
    COMPILER__append__accountling_scope_header(&(*accountling_function).scope_headers.list, scope_header, error);
    if (COMPILER__check__error_occured(error)) {
        return;
    }
    (*accountling_function).scope_headers.count++;

    // get subscopes
    COMPILER__account__functions__get_scope_level_data(accountling_function, parsling_scope, error);

    return;
}

// convert string to converted string
ANVIL__buffer COMPILER__translate__string_literal_to_translated_buffer(COMPILER__lexling string_literal, COMPILER__error* error) {
    ANVIL__buffer output = ANVIL__create_null__buffer();
    ANVIL__length character_count = 0;

    // count characters
    for (ANVIL__character_index i = 1; i < ANVIL__calculate__buffer_length(string_literal.value) - 1; i++) {
        // check for escape sequence
        if (((ANVIL__character*)string_literal.value.start)[i] == '%') {
            // check for characters available
            if (i + 3 <= ANVIL__calculate__buffer_length(string_literal.value) - 1) {
                // validate characters
                ANVIL__bt invalid_hexadecimal_character_1;
                ANVIL__bt invalid_hexadecimal_character_2;
                ANVIL__bt semi_colon_missing;
                COMPILER__translate__character_to_hexadecimal(((ANVIL__character*)string_literal.value.start)[i + 1], &invalid_hexadecimal_character_1);
                COMPILER__translate__character_to_hexadecimal(((ANVIL__character*)string_literal.value.start)[i + 2], &invalid_hexadecimal_character_2);
                semi_colon_missing = (ANVIL__bt)(((ANVIL__character*)string_literal.value.start)[i + 3] != ';');

                // determine validity
                if (invalid_hexadecimal_character_1 != ANVIL__bt__false || invalid_hexadecimal_character_2 != ANVIL__bt__false || semi_colon_missing != ANVIL__bt__false) {
                    // invalid escape sequence
                    *error = COMPILER__open__error("Accounting Error: String literal has invalid escape sequences.", string_literal.location);
                    
                    return output;
                }

                // skip past characters
                i += 3;
            // error
            } else {
                *error = COMPILER__open__error("Accounting Error: String literal has invalid escape sequences.", string_literal.location);

                return output;
            }
        }

        // next character
        character_count++;
    }

    // check for empty string
    if (character_count == 0) {
        // return empty string
        return output;
    }

    // allocate string
    output = ANVIL__open__buffer(character_count);
    if (ANVIL__check__empty_buffer(output)) {
        *error = COMPILER__open__internal_memory_error();

        return output;
    }

    // translate string
    character_count = 0;
    for (ANVIL__character_index i = 1; i < ANVIL__calculate__buffer_length(string_literal.value) - 1; i++) {
        // check for escape sequence
        if (((ANVIL__character*)string_literal.value.start)[i] == '%') {
            // validate characters
            ANVIL__bt invalid_hexadecimal_character_1;
            ANVIL__bt invalid_hexadecimal_character_2;
            ANVIL__character a = COMPILER__translate__character_to_hexadecimal(((ANVIL__character*)string_literal.value.start)[i + 1], &invalid_hexadecimal_character_1);
            ANVIL__character b = COMPILER__translate__character_to_hexadecimal(((ANVIL__character*)string_literal.value.start)[i + 2], &invalid_hexadecimal_character_2);

            // write character
            ((ANVIL__character*)output.start)[character_count] = b + (a << 4);
            
            // skip past characters
            i += 3;
        } else {
            // write character
            ((ANVIL__character*)output.start)[character_count] = ((ANVIL__character*)string_literal.value.start)[i];
        }

        // next character
        character_count++;
    }

    return output;
}

// get a variable by index
COMPILER__accountling_variable COMPILER__account__functions__get_variable_by_variable_argument(COMPILER__accountling_variables variables, COMPILER__accountling_variable_argument index) {
    return ((COMPILER__accountling_variable*)variables.lists[index.type].list.buffer.start)[index.index];
}

// check variable argument is valid
ANVIL__bt COMPILER__check__accountling_variable_argument_is_declared(COMPILER__accountling_variables variables, COMPILER__accountling_variable_argument index) {
    if (index.type == COMPILER__avat__master) {
        return (index.index < variables.lists[COMPILER__avat__master].count);
    } else if (index.type == COMPILER__avat__member) {
        return (index.index < variables.lists[COMPILER__avat__member].count);
    } else {
        return ANVIL__bt__false;
    }
}

// search a variable for a match
ANVIL__bt COMPILER__account__functions__get_variable_by_variable_argument__recursively(COMPILER__accountling_variables variables, COMPILER__accountling_variable_argument variable_argument, COMPILER__namespace name, COMPILER__lexling_index sub_name_index, COMPILER__accountling_variable_argument* output) {
    // setup variable temp
    COMPILER__accountling_variable variable = COMPILER__account__functions__get_variable_by_variable_argument(variables, variable_argument);
    
    // check for empty members
    if (variable.members.start >= variables.lists[COMPILER__avat__member].count) {
        return ANVIL__bt__false;
    }

    // search each member
    for (COMPILER__variable_member_index member_index = variable.members.start; member_index <= variable.members.end; member_index++) {
        // get member variable
        COMPILER__accountling_variable member = COMPILER__account__functions__get_variable_by_variable_argument(variables, COMPILER__create__accountling_variable_argument(COMPILER__avat__member, member_index));
        
        // if names match
        if (ANVIL__calculate__buffer_contents_equal(member.name.value, COMPILER__get__lexling_by_index(name.lexlings, sub_name_index).value) == ANVIL__bt__true) {
            // name matches
            // if there are more names
            if (sub_name_index < name.lexlings.count - 1) {
                // search for next name
                return COMPILER__account__functions__get_variable_by_variable_argument__recursively(variables, COMPILER__create__accountling_variable_argument(COMPILER__avat__member, member_index), name, sub_name_index + 1, output);
            // full namespace matches!
            } else {
                // setup output
                *output = COMPILER__create__accountling_variable_argument(COMPILER__avat__member, member_index);

                return ANVIL__bt__true;
            }
        }
    }

    // no matches
    return ANVIL__bt__false;
}

// get a variable index by name
COMPILER__accountling_variable_argument COMPILER__account__functions__get_variable_argument_by_name(COMPILER__accountling_variables variables, COMPILER__namespace name) {
    COMPILER__accountling_variable_argument output = COMPILER__create_null__accountling_variable_argument();

    // search for variable
    for (COMPILER__variable_index master_index = 0; master_index < variables.lists[COMPILER__avat__master].count; master_index++) {
        // get member
        COMPILER__accountling_variable variable = ((COMPILER__accountling_variable*)variables.lists[COMPILER__avat__master].list.buffer.start)[master_index];

        // check master names
        if (ANVIL__calculate__buffer_contents_equal(variable.name.value, COMPILER__get__lexling_by_index(name.lexlings, 0).value)) {
            // check for master variable only
            if (name.lexlings.count < 2) {
                // match!
                return COMPILER__create__accountling_variable_argument(COMPILER__avat__master, master_index);
            // variable name is longer than just master
            } else {
                // get variable
                if (COMPILER__account__functions__get_variable_by_variable_argument__recursively(variables, COMPILER__create__accountling_variable_argument(COMPILER__avat__master, master_index), name, 1, &output) == ANVIL__bt__true) {
                    return output;
                }
            }
        }
    }

    // not a match
    return output;
}

// calculate cell range
COMPILER__cell_range COMPILER__calculate_and_advance__cells(COMPILER__accountling_structures structures, COMPILER__accountling_function* function, COMPILER__variable_type_index type) {
    // get length
    ANVIL__cell_count length = ((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[type].cell_count;

    // calculate cell range
    COMPILER__cell_range output = COMPILER__create__cell_range((*function).next_available_workspace_cell, (*function).next_available_workspace_cell + length - 1);

    // adjust cell range
    (*function).next_available_workspace_cell += length;

    return output;
}

// generate variables from source variable
COMPILER__accountling_variable_range COMPILER__account__functions__mark_variable__generate_structure_variable_members(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, ANVIL__cell_ID* current_sub_cell, COMPILER__variable_type_index type, COMPILER__error* error) {
    COMPILER__accountling_variable_range output;

    // get structure
    COMPILER__accountling_structure structure = ((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[type];

    // setup output
    output = COMPILER__create__accountling_variable_range((*accountling_function).variables.lists[COMPILER__avat__member].count, (*accountling_function).variables.lists[COMPILER__avat__member].count + structure.members.count - 1);

    // reserve members
    for (COMPILER__variable_member_index index = output.start; index <= output.end; index++) {
        // reserve member
        COMPILER__append__accountling_variable(&(*accountling_function).variables.lists[COMPILER__avat__member].list, COMPILER__create_null__accountling_variable(), error);
        if (COMPILER__check__error_occured(error)) {
            return output;
        }
        (*accountling_function).variables.lists[COMPILER__avat__member].count++;
    }

    // for each member
    for (COMPILER__structure_member_index index = output.start; index <= output.end; index++) {
        // get member
        COMPILER__accountling_structure_member member = ((COMPILER__accountling_structure_member*)structure.members.list.buffer.start)[index - output.start];

        // declare new variable
        if (member.structure_ID >= structures.data_table.count) {
            // declare variable
            ((COMPILER__accountling_variable*)(*accountling_function).variables.lists[COMPILER__avat__member].list.buffer.start)[index] = COMPILER__create__accountling_variable(COMPILER__get__lexling_by_index(member.name.lexlings, 0), member.structure_ID, COMPILER__create__accountling_variable_range(-1, -1), COMPILER__create__cell_range(*current_sub_cell, *current_sub_cell));
        // recurse
        } else {
            // setup range start
            COMPILER__cell_range cell_range;
            cell_range.start = *current_sub_cell;

            COMPILER__accountling_variable_range member_range;

            // recurse
            if (member.structure_ID > COMPILER__ptt__dragon_cell) {
                member_range = COMPILER__account__functions__mark_variable__generate_structure_variable_members(structures, accountling_function, current_sub_cell, member.structure_ID, error);

                // set range end
                cell_range.end = *current_sub_cell - 1;
            } else {
                member_range = COMPILER__create__accountling_variable_range(-1, -1);

                // set range end
                cell_range.end = *current_sub_cell;

                // next cell
                (*current_sub_cell)++;
            }

            // append variable
            ((COMPILER__accountling_variable*)(*accountling_function).variables.lists[COMPILER__avat__member].list.buffer.start)[index] = COMPILER__create__accountling_variable(COMPILER__get__lexling_by_index(member.name.lexlings, 0), member.structure_ID, member_range, cell_range);
        }
    }

    return output;
}

// generate variables from source variable
void COMPILER__account__functions__mark_variable__generate_master_variable_and_sub_variables(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_argument name, COMPILER__variable_type_index master_type, COMPILER__error* error) {
    // reserve cells
    COMPILER__cell_range cell_range = COMPILER__calculate_and_advance__cells(structures, accountling_function, master_type);

    // declare sub-variables
    ANVIL__cell_ID current_sub_cell = cell_range.start;

    // ensure type is valid
    COMPILER__accountling_variable_range member_range;
    if (master_type < structures.data_table.count && master_type > COMPILER__ptt__dragon_cell) {
        member_range = COMPILER__account__functions__mark_variable__generate_structure_variable_members(structures, accountling_function, &current_sub_cell, master_type, error);
    } else {
        member_range = COMPILER__create__accountling_variable_range(-1, -1);
    }

    // declare master variable
    COMPILER__append__accountling_variable(&(*accountling_function).variables.lists[COMPILER__avat__master].list, COMPILER__create__accountling_variable(COMPILER__get__lexling_by_index(name.name.lexlings, 0), master_type, member_range, cell_range), error);
    if (COMPILER__check__error_occured(error)) {
        return;
    }
    (*accountling_function).variables.lists[COMPILER__avat__master].count++;

    return;
}

// mark variable
COMPILER__accountling_variable_argument COMPILER__account__functions__mark_variable(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_argument name, COMPILER__variable_type_index expected_type, COMPILER__asvt argument_location, ANVIL__bt declare_if_nonexistant, ANVIL__bt* is_valid_argument, COMPILER__error* error) {
    // get argument
    COMPILER__accountling_variable_argument argument = COMPILER__account__functions__get_variable_argument_by_name((*accountling_function).variables, name.name);

    // check for manual type setting & check for conflicting types
    if (COMPILER__check__empty_namespace(name.type) == ANVIL__bt__false && COMPILER__find__accountling_structure_name_index(structures.name_table, name.type) != expected_type) {
        // not a match
        goto failure;
    }

    // search for master variable
    // if variable is a master namespace only
    if (name.name.lexlings.count < 2) {
        // check for variable declaration
        // is already declared
        if (COMPILER__check__accountling_variable_argument_is_declared((*accountling_function).variables, argument) == ANVIL__bt__true) {
            // if types match, found correct function
            if (COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, argument).type == expected_type) {
                goto success;
            // if types don't match, maybe another function matches
            } else {
                goto failure;
            }
        // is not declared
        } else {
            // if argument is in an output slot
            if (argument_location == COMPILER__asvt__output) {
                // if declaration is enabled
                if (declare_if_nonexistant == ANVIL__bt__true) {
                    // declare variable and all its sub-variables
                    COMPILER__account__functions__mark_variable__generate_master_variable_and_sub_variables(structures, accountling_function, name, expected_type, error);
                }

                // check error
                if (COMPILER__check__error_occured(error)) {
                    goto failure;
                }
            // variables cannot be declared in statement inputs
            } else {
                *error = COMPILER__open__error("Accounting Error: Variables cannot be declared in statement inputs.", COMPILER__get__namespace_lexling_location(name.name));

                goto failure;
            }

            // check to see if cell range is out of bounds
            if ((*accountling_function).next_available_workspace_cell >= ANVIL__srt__start__function_io) {
                // error, too many cells were used
                *error = COMPILER__open__error("Accounting Error: A function has too many variables.", COMPILER__get__namespace_lexling_location(name.name));

                goto failure;
            }

            // update argument
            argument = COMPILER__account__functions__get_variable_argument_by_name((*accountling_function).variables, name.name);

            // return argument
            goto success;
        }
    // otherwise, the variable MUST already be declared to access its members
    } else {
        // if already declared
        if (COMPILER__check__accountling_variable_argument_is_declared((*accountling_function).variables, argument) == ANVIL__bt__true) {
            // if types match, found correct function
            if (COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, argument).type == expected_type) {
                goto success;
            // if types don't match, maybe another function matches
            } else {
                goto failure;
            }
        // error, variable sub-structure being used on non-exsistant master namespace || variable is not found
        } else {
            // open error
            *error = COMPILER__open__error("Accounting Error: An argument is trying to access a variable that doesn't exist.", COMPILER__get__namespace_lexling_location(name.name));

            goto failure;
        }
    }

    success:
    *is_valid_argument = ANVIL__bt__true;
    return argument;

    failure:
    *is_valid_argument = ANVIL__bt__false;
    return COMPILER__create_null__accountling_variable_argument();
}

// check for set
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__sets(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // check for name & argument counts
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__set], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 1) {
        // check for literal type
        // check for string literal
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__string_literal && COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name) {
            // check output variable type
            // get index
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }

            // convert string
            ANVIL__buffer string_data = COMPILER__translate__string_literal_to_translated_buffer(COMPILER__get__lexling_by_index(COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).name.lexlings, 0), error);
            if (COMPILER__check__error_occured(error) && ANVIL__check__empty_buffer(string_data) == ANVIL__bt__false) {
                // close string
                ANVIL__close__buffer(string_data);
                goto failure;
            }

            // add string to library
            //COMPILER__append__buffer_with_error(&(*accountling_function).strings.list, string_data, error);
            COMPILER__append(&(*accountling_function).strings.list, ANVIL__buffer, string_data, error);
            (*accountling_function).strings.count++;
            if (COMPILER__check__error_occured(error)) {
                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__set__string;
            (*accountling_statement).set_string__variable_argument = variable_argument;
            (*accountling_statement).set_string__string_value_index = (*accountling_function).strings.count - 1;

            // variable declared / reused
            goto match;
        // check for integer literal
        } else if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name) {
            // temporaries
            ANVIL__cell_integer_value integer_value = 0;
            ANVIL__buffer integer_string = COMPILER__get__lexling_by_index(COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).name.lexlings, 0).value;

            // attempt translation
            if (COMPILER__translate__string_to_boolean(integer_string, &integer_value) || COMPILER__translate__string_to_binary(integer_string, &integer_value) || COMPILER__translate__string_to_integer(integer_string, &integer_value) || COMPILER__translate__string_to_hexadecimal(integer_string, &integer_value)) {
                // get index
                ANVIL__bt is_valid_argument;
                COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
                if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                    goto failure;
                }
                
                // create statement
                (*accountling_statement).statement_type = COMPILER__ast__predefined__set__cell;
                (*accountling_statement).set_cell__raw_value = integer_value;
                (*accountling_statement).set_cell__variable_argument = variable_argument;

                // valid variable declaration
                goto match;
            }

            // no valid literal was found, error
            *error = COMPILER__open__error("Accounting Error: A set function call did not have a valid literal.", COMPILER__get__namespace_lexling_location(COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).name));

            goto failure;
        }

        // statement is not a setter, but not yet known to be invalid
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for printing
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__prints(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a character print
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__print__character], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 0) {
        // if inputs are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name) {
            // check input variable type
            // get index
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__print__character;
            (*accountling_statement).print__variable_argument = variable_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for packers
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__copy(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a copy
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__copy], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name && parsling_statement.outputs.count == 1 && COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name) {
        // get argument for type
        COMPILER__accountling_variable_argument argument_type = COMPILER__account__functions__get_variable_argument_by_name((*accountling_function).variables, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).name);
        if (argument_type.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // get variable arguments
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument input_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, argument_type).type, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || input_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, argument_type).type, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // check if both types are the same
        if (COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, input_argument).type != COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, output_argument).type) {
            // not a match
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__copy__anything;
        (*accountling_statement).copy__input = input_argument;
        (*accountling_statement).copy__output = output_argument;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for packers
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__packers(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a packer
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__pack], parsling_statement.name.name) && parsling_statement.outputs.count == 1) {
        // get type
        COMPILER__structure_index packing_type = COMPILER__find__accountling_structure_name_index(structures.name_table, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).type);

        // if inputs are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name && packing_type < structures.name_table.count) {
            // check input variable type
            // get index
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), packing_type, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }

            // get structure
            COMPILER__accountling_structure packing_structure = ((COMPILER__accountling_structure*)structures.data_table.list.buffer.start)[packing_type];

            // validate packer inputs
            if (packing_structure.members.count == parsling_statement.inputs.count) {
                // check for matches
                for (COMPILER__structure_member_index index = 0; index < packing_structure.members.count; index++) {
                    // get statement and structure data
                    COMPILER__accountling_structure_member accountling_member = ((COMPILER__accountling_structure_member*)packing_structure.members.list.buffer.start)[index];
                    COMPILER__accountling_variable_argument argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, index), accountling_member.structure_ID, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
                    if (COMPILER__check__error_occured(error)) {
                        goto failure;
                    }

                    // check for discrepencies
                    if (argument.type >= COMPILER__avat__COUNT) {
                        goto failure;
                    }
                }
            // not a match
            } else {
                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__pack__anything;
            (*accountling_statement).pack__output = variable_argument;

            // setup packing variables
            (*accountling_statement).pack__inputs = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_variable_argument) * packing_structure.members.count, error);
            if (COMPILER__check__error_occured(error)) {
                goto failure;
            }
            
            // setup variables
            for (COMPILER__structure_member_index index = 0; index < packing_structure.members.count; index++) {
                // get argument
                COMPILER__accountling_structure_member accountling_member = ((COMPILER__accountling_structure_member*)packing_structure.members.list.buffer.start)[index];
                COMPILER__accountling_variable_argument argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, index), accountling_member.structure_ID, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);

                // append argument
                COMPILER__append__accountling_variable_argument(&(*accountling_statement).pack__inputs.list, argument, error);
                if (COMPILER__check__error_occured(error)) {
                    goto failure;
                }
                (*accountling_statement).pack__inputs.count++;
            }

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for null packers
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__null_packers(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a packer
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__pack_null], parsling_statement.name.name) && parsling_statement.inputs.count == 0 && parsling_statement.outputs.count == 1) {
        // get type
        COMPILER__structure_index packing_type = COMPILER__find__accountling_structure_name_index(structures.name_table, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).type);

        // if inputs are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name && packing_type < structures.name_table.count) {
            // mark output variable
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), packing_type, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__pack_null__anything;
            (*accountling_statement).pack__output = variable_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for increment packers
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__increment_packers(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a packer
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__pack_increment], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1) {
        // get type
        COMPILER__structure_index packing_type = COMPILER__find__accountling_structure_name_index(structures.name_table, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).type);

        // if outputs are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name && packing_type < structures.name_table.count) {
            // mark variables
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument increment_start_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || increment_start_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            COMPILER__accountling_variable_argument increment_by_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || increment_by_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), packing_type, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__pack_increment__anything;
            (*accountling_statement).pack_increment_start = increment_start_argument;
            (*accountling_statement).pack_increment_by = increment_by_argument;
            (*accountling_statement).pack__output = output_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for scope jumping
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__jumping(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a top jump
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__jump__top], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // if inputs are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).category == COMPILER__pat__offset) {
            // check input variable type
            // get index
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            
            // get offset ID by name
            (*accountling_statement).scope_index = COMPILER__account__functions__get_scope_index(accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).name);
            if ((*accountling_statement).scope_index >= (*accountling_function).scope_headers.count) {
                *error = COMPILER__open__error("Accounting Error: A scope offset is used but never declared.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

                goto failure;
            }


            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__jump__top;
            (*accountling_statement).jump__variable_argument = variable_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    // if is a bottom jump
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__jump__bottom], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // if inputs are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).category == COMPILER__pat__offset) {
            // check input variable type
            // get index
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            
            // get offset ID by name
            (*accountling_statement).scope_index = COMPILER__account__functions__get_scope_index(accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).name);
            if ((*accountling_statement).scope_index >= (*accountling_function).scope_headers.count) {
                *error = COMPILER__open__error("Accounting Error: A scope offset is used but never declared.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__jump__bottom;
            (*accountling_statement).jump__variable_argument = variable_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    // if is a normal jump
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__jump], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // if inputs are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).category == COMPILER__pat__offset) {
            // check input variable type
            // get index
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument variable_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || variable_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            
            // get offset ID by name
            (*accountling_statement).offset_index = COMPILER__account__functions__get_offset_index(accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).name);
            if ((*accountling_statement).offset_index >= (*accountling_function).offsets.count) {
                *error = COMPILER__open__error("Accounting Error: An offset is used but never declared.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__jump__offset;
            (*accountling_statement).jump__variable_argument = variable_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for checking
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__data_checking(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is an integer within range
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__within_range], parsling_statement.name.name) && parsling_statement.inputs.count == 3 && parsling_statement.outputs.count == 2) {
        // if io names are correct parsing type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 2).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name && COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 1).category == COMPILER__pat__name) {
            // check input variable types
            // get index
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument lower_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || lower_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            COMPILER__accountling_variable_argument checking_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || checking_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            COMPILER__accountling_variable_argument higher_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 2), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || higher_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            COMPILER__accountling_variable_argument result_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || result_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            COMPILER__accountling_variable_argument inverted_result_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || inverted_result_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }

            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__check__integer_within_range;
            (*accountling_statement).within_range__lower_bound = lower_argument;
            (*accountling_statement).within_range__checking = checking_argument;
            (*accountling_statement).within_range__higher_bound = higher_argument;
            (*accountling_statement).within_range__normal_result = result_argument;
            (*accountling_statement).within_range__inverted_result = inverted_result_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for integer operations
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__integer_operations(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if has valid argument counts
    if (parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1 && (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__add], parsling_statement.name.name) || COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__subtract], parsling_statement.name.name) || COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__multiply], parsling_statement.name.name) || COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__divide], parsling_statement.name.name) || COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__modulous], parsling_statement.name.name))) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // addition
        if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__add], parsling_statement.name.name)) {
            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__integer__addition;
            (*accountling_statement).integer_operation__first_argument = first_argument;
            (*accountling_statement).integer_operation__second_argument = second_argument;
            (*accountling_statement).integer_operation__output_argument = output_argument;

            // match
            goto match;
        // subtraction
        } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__subtract], parsling_statement.name.name)) {
            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__integer__subtraction;
            (*accountling_statement).integer_operation__first_argument = first_argument;
            (*accountling_statement).integer_operation__second_argument = second_argument;
            (*accountling_statement).integer_operation__output_argument = output_argument;

            // match
            goto match;
        // multiplication
        } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__multiply], parsling_statement.name.name)) {
            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__integer__multiplication;
            (*accountling_statement).integer_operation__first_argument = first_argument;
            (*accountling_statement).integer_operation__second_argument = second_argument;
            (*accountling_statement).integer_operation__output_argument = output_argument;

            // match
            goto match;
        // division
        } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__divide], parsling_statement.name.name)) {
            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__integer__division;
            (*accountling_statement).integer_operation__first_argument = first_argument;
            (*accountling_statement).integer_operation__second_argument = second_argument;
            (*accountling_statement).integer_operation__output_argument = output_argument;

            // match
            goto match;
        // modulous
        } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__integer__modulous], parsling_statement.name.name)) {
            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__integer__modulous;
            (*accountling_statement).integer_operation__first_argument = first_argument;
            (*accountling_statement).integer_operation__second_argument = second_argument;
            (*accountling_statement).integer_operation__output_argument = output_argument;

            // match
            goto match;
        // not the right argument type
        } else {
            goto failure;
        }
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for bit operations
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__bit_operations(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    ANVIL__bt is_valid_argument;

    // if is an or
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__bits__or], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1) {
        // get variables
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__bits_or;
        (*accountling_statement).bit_operation__first_argument = first_argument;
        (*accountling_statement).bit_operation__second_argument = second_argument;
        (*accountling_statement).bit_operation__output_argument = output_argument;

        // match
        goto match;
    // if is an invert
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__bits__invert], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 1) {
        // get variables
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__bits_invert;
        (*accountling_statement).bit_operation__first_argument = first_argument;
        (*accountling_statement).bit_operation__output_argument = output_argument;

        // match
        goto match;
    // if is an and
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__bits__and], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1) {
        // get variables
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__bits_and;
        (*accountling_statement).bit_operation__first_argument = first_argument;
        (*accountling_statement).bit_operation__second_argument = second_argument;
        (*accountling_statement).bit_operation__output_argument = output_argument;

        // match
        goto match;
    // if is a xor
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__bits__xor], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1) {
        // get variables
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__bits_xor;
        (*accountling_statement).bit_operation__first_argument = first_argument;
        (*accountling_statement).bit_operation__second_argument = second_argument;
        (*accountling_statement).bit_operation__output_argument = output_argument;

        // match
        goto match;
    // if is shift higher
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__bits__shift_higher], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1) {
        // get variables
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__bits_shift_higher;
        (*accountling_statement).bit_operation__first_argument = first_argument;
        (*accountling_statement).bit_operation__second_argument = second_argument;
        (*accountling_statement).bit_operation__output_argument = output_argument;

        // match
        goto match;
    // if is a shift lower
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__bits__shift_lower], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1) {
        // get variables
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__bits_shift_lower;
        (*accountling_statement).bit_operation__first_argument = first_argument;
        (*accountling_statement).bit_operation__second_argument = second_argument;
        (*accountling_statement).bit_operation__output_argument = output_argument;

        // match
        goto match;
    // if is an overwrite
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__bits__overwrite], parsling_statement.name.name) && parsling_statement.inputs.count == 3 && parsling_statement.outputs.count == 1) {
        // get variables
        COMPILER__accountling_variable_argument first_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument third_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 2), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || third_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__bits_overwrite;
        (*accountling_statement).bit_operation__first_argument = first_argument;
        (*accountling_statement).bit_operation__second_argument = second_argument;
        (*accountling_statement).bit_operation__third_argument = third_argument;
        (*accountling_statement).bit_operation__output_argument = output_argument;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for buffer requesting and returning
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__buffer_acquisition(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is buffer request
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__request], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 1) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument input_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || input_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__buffer__request;
        (*accountling_statement).buffer_acquisition__buffer_length = input_argument;
        (*accountling_statement).buffer_acquisition__buffer_result = output_argument;

        // match
        goto match;
    // if is buffer return
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__return], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 0) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument input_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || input_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__buffer__return;
        (*accountling_statement).buffer_acquisition__buffer_return = input_argument;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for cell address movers
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__cell_address_movers(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is address to cell
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__address_to_cell], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 2) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument address_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || address_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument byte_count_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || byte_count_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument cell_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || cell_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument advancement_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || advancement_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__mover__address_to_cell;
        (*accountling_statement).cell_address_mover__address = address_argument;
        (*accountling_statement).cell_address_mover__byte_count = byte_count_argument;
        (*accountling_statement).cell_address_mover__cell = cell_argument;
        (*accountling_statement).cell_address_mover__advancement = advancement_argument;

        // match
        goto match;
    // if is cell to address
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__cell_to_address], parsling_statement.name.name) && parsling_statement.inputs.count == 3 && parsling_statement.outputs.count == 1) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument cell_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || cell_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument byte_count_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || byte_count_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument address_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 2), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || address_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument advancement_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || advancement_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__mover__cell_to_address;
        (*accountling_statement).cell_address_mover__cell = cell_argument;
        (*accountling_statement).cell_address_mover__byte_count = byte_count_argument;
        (*accountling_statement).cell_address_mover__address = address_argument;
        (*accountling_statement).cell_address_mover__advancement = advancement_argument;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for file buffer movers
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__file_buffer_movers(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is file to buffer
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__file_to_buffer], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 1) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument file_path_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || file_path_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__mover__file_to_buffer;
        (*accountling_statement).file_buffer_mover__file_path = file_path_argument;
        (*accountling_statement).file_buffer_mover__buffer_0 = buffer_argument;

        // match
        goto match;
    // if is buffer to file
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__buffer_to_file], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument file_path_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || file_path_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__mover__buffer_to_file;
        (*accountling_statement).file_buffer_mover__file_path = file_path_argument;
        (*accountling_statement).file_buffer_mover__buffer_0 = buffer_argument;

        // match
        goto match;
    // if is delete file
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__delete_file], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 0) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument file_path_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || file_path_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__delete_file;
        (*accountling_statement).file_buffer_mover__file_path = file_path_argument;

        // match
        goto match;
    // if is buffer copy low to high
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__copy__low_to_high], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument first_buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__mover__buffer_copy__low_to_high;
        (*accountling_statement).file_buffer_mover__buffer_0 = first_buffer_argument;
        (*accountling_statement).file_buffer_mover__buffer_1 = second_buffer_argument;

        // match
        goto match;
    // if is buffer copy high to low
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__copy__high_to_low], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument first_buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || first_buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument second_buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || second_buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__mover__buffer_copy__high_to_low;
        (*accountling_statement).file_buffer_mover__buffer_0 = first_buffer_argument;
        (*accountling_statement).file_buffer_mover__buffer_1 = second_buffer_argument;

        // match
        goto match;
    // if is not a match
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for structure buffer movers
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__structure_buffer_movers(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is structure size
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__structure__structure_byte_size], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 1) {
        // get argument for structure type
        COMPILER__accountling_variable_argument structure_type = COMPILER__account__functions__get_variable_argument_by_name((*accountling_function).variables, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).name);
        if (structure_type.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        
        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument structure_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, structure_type).type, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || structure_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument size_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || size_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__structure__size;
        (*accountling_statement).structure_buffer_mover__structure = structure_argument;
        (*accountling_statement).structure_buffer_mover__size = size_argument;

        // match
        goto match;
    // if is structure to buffer
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__structure__structure_to_buffer], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 1) {
        // get argument for structure type
        COMPILER__accountling_variable_argument structure_type = COMPILER__account__functions__get_variable_argument_by_name((*accountling_function).variables, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0).name);
        if (structure_type.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // get variables
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument structure_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__account__functions__get_variable_by_variable_argument((*accountling_function).variables, structure_type).type, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || structure_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument advancement_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || advancement_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // match
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__structure__structure_to_buffer;
        (*accountling_statement).structure_buffer_mover__structure = structure_argument;
        (*accountling_statement).structure_buffer_mover__buffer = buffer_argument;
        (*accountling_statement).structure_buffer_mover__advancement = advancement_argument;
        
        // match
        goto match;
    // if is buffer to structure
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__structure__buffer_to_structure], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 1) {
        // get argument for structure type
        COMPILER__structure_index structure_type = COMPILER__find__accountling_structure_name_index(structures.name_table, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).type);

        // validate type
        if (COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0).category == COMPILER__pat__name && structure_type < structures.name_table.count) {
            // get variables
            ANVIL__bt is_valid_argument;
            COMPILER__accountling_variable_argument buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || buffer_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }
            COMPILER__accountling_variable_argument structure_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), structure_type, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
            if (COMPILER__check__error_occured(error) || structure_argument.type >= COMPILER__avat__COUNT) {
                goto failure;
            }

            // match
            // setup output statement
            (*accountling_statement).statement_type = COMPILER__ast__predefined__structure__buffer_to_structure;
            (*accountling_statement).structure_buffer_mover__structure = structure_argument;
            (*accountling_statement).structure_buffer_mover__buffer = buffer_argument;
        
            // match
            goto match;
        }
    // if is not a match
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for compiler functions
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__compiler_assorted(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a compiler call
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__compiler__compile], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 3) {
        // check input variable type
        // get index
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument code_buffers_buffer_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || code_buffers_buffer_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument settings_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_compiler__settings, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || settings_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument anvil_code_output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || anvil_code_output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument debug_information_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 1), COMPILER__ptt__dragon_compiler__debug_information, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || debug_information_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument error_information_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 2), COMPILER__ptt__dragon_compiler__error, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || error_information_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__compilation__compile;
        (*accountling_statement).compile__user_code_buffers = code_buffers_buffer_argument;
        (*accountling_statement).compile__settings = settings_argument;
        (*accountling_statement).compile__anvil_code = anvil_code_output_argument;
        (*accountling_statement).compile__debug_information = debug_information_argument;
        (*accountling_statement).compile__error_information = error_information_argument;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for context functions
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__context_assorted(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a context install program call
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__context__install_program], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // check input variable type
        // get index
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument context_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || context_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument program_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || program_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__context__install_program;
        (*accountling_statement).context__context_buffer = context_argument;
        (*accountling_statement).context__program_buffer = program_argument;

        // match
        goto match;
    // if is a context run call
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__context__run], parsling_statement.name.name) && parsling_statement.inputs.count == 2 && parsling_statement.outputs.count == 0) {
        // check input variable type
        // get index
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument context_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || context_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument instruction_count_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 1), COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || instruction_count_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__context__run;
        (*accountling_statement).context__context_buffer = context_argument;
        (*accountling_statement).context__instruction_count = instruction_count_argument;

        // match
        goto match;
    // if is an execution stop
    } else if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__execution__stop], parsling_statement.name.name) && parsling_statement.inputs.count == 0 && parsling_statement.outputs.count == 0) {
        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__execution__stop;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for time
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__time_functions(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a time function
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__time__now], parsling_statement.name.name) && parsling_statement.inputs.count == 0 && parsling_statement.outputs.count == 1) {
        // check input variable type
        // get index
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument output_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_time, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || output_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__time__get_current_time;
        (*accountling_statement).time__get_time_data = output_argument;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}

// check for allocation related functions
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__allocation_related_functions(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    // if is a find allocation function
    if (COMPILER__check__namespace_against_c_string(COMPILER__global__predefined_function_call_names[COMPILER__pfcnt__buffer__find_allocation], parsling_statement.name.name) && parsling_statement.inputs.count == 1 && parsling_statement.outputs.count == 2) {
        // get parameters
        // get index
        ANVIL__bt is_valid_argument;
        COMPILER__accountling_variable_argument source_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, 0), COMPILER__ptt__dragon_buffer, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || source_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument was_found_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 0), COMPILER__ptt__dragon_cell, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || was_found_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }
        COMPILER__accountling_variable_argument found_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, 1), COMPILER__ptt__dragon_buffer, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error) || found_argument.type >= COMPILER__avat__COUNT) {
            goto failure;
        }

        // setup output statement
        (*accountling_statement).statement_type = COMPILER__ast__predefined__allocation__find;
        (*accountling_statement).allocation__source_buffer = source_argument;
        (*accountling_statement).allocation__was_found = was_found_argument;
        (*accountling_statement).allocation__found_buffer = found_argument;

        // match
        goto match;
    // not the right argument type
    } else {
        goto failure;
    }

    // not a match
    failure:
    return ANVIL__bt__false;

    // match!
    match:
    return ANVIL__bt__true;
}


// check for user defined function calls
ANVIL__bt COMPILER__account__functions__check_and_get_statement_translation__user_defined_function_calls(COMPILER__accountling_structures structures, COMPILER__accountling_function_headers function_headers, COMPILER__accountling_function* accountling_function, COMPILER__parsling_statement parsling_statement, COMPILER__accountling_statement* accountling_statement, COMPILER__error* error) {
    COMPILER__function_header_index match_count = 0;
    COMPILER__function_header_index match_index = -1;

    // get matches
    for (COMPILER__function_header_index header_index = 0; header_index < function_headers.category[COMPILER__afht__user_defined].count; header_index++) {
        // get header
        COMPILER__accountling_function_header header = ((COMPILER__accountling_function_header*)function_headers.category[COMPILER__afht__user_defined].list.buffer.start)[header_index];
        
        // check for match
        if (COMPILER__check__identical_namespaces(parsling_statement.name.name, header.name) && parsling_statement.inputs.count == header.input_types.count && parsling_statement.outputs.count == header.output_types.count) {
            // check inputs
            for (COMPILER__argument_index io_index = 0; io_index < parsling_statement.inputs.count; io_index++) {
                // check input
                ANVIL__bt is_valid_argument;
                COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, io_index), ((COMPILER__structure_index*)header.input_types.list.buffer.start)[io_index] - COMPILER__aat__COUNT, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
                if (COMPILER__check__error_occured(error)) {
                    goto failure;
                }
                if (is_valid_argument == ANVIL__bt__false) {
                    // not a match
                    // go to next header
                    goto next_header;
                }
            }
            // check outputs
            for (COMPILER__argument_index io_index = 0; io_index < parsling_statement.outputs.count; io_index++) {
                // check output
                ANVIL__bt is_valid_argument;
                COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, io_index), ((COMPILER__structure_index*)header.output_types.list.buffer.start)[io_index] - COMPILER__aat__COUNT, COMPILER__asvt__output, ANVIL__bt__false, &is_valid_argument, error);
                if (COMPILER__check__error_occured(error)) {
                    goto failure;
                }
                if (is_valid_argument == ANVIL__bt__false) {
                    // not a match
                    // go to next header
                    goto next_header;
                }
            }

            // match!
            match_count++;
            match_index = header_index;
        }

        next_header:
        if (match_count > 1) {
            break;
        }
    }

    // if there was no matches
    if (match_count == 0) {
        // another function maybe
        goto failure;
    }

    // if there was more than one match
    if (match_count > 1) {
        // error
        *error = COMPILER__open__error("Accounting Error: A user defined function call has more than one possible function to call.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

        goto failure;
    }

    // only one match, success!
    // setup statement
    (*accountling_statement).statement_type = COMPILER__ast__user_defined_function_call;
    (*accountling_statement).function_call__calling_function_header_index = match_index;
    (*accountling_statement).function_call__inputs = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_variable_argument) * parsling_statement.inputs.count, error);
    (*accountling_statement).function_call__outputs = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_variable_argument) * parsling_statement.outputs.count, error);
    for (COMPILER__argument_index io_index = 0; io_index < parsling_statement.inputs.count; io_index++) {
        // append variable
        ANVIL__bt is_valid_argument;
        COMPILER__append__accountling_variable_argument(&(*accountling_statement).function_call__inputs.list, COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.inputs, io_index), ((COMPILER__structure_index*)((COMPILER__accountling_function_header*)function_headers.category[COMPILER__afht__user_defined].list.buffer.start)[match_index].input_types.list.buffer.start)[io_index] - COMPILER__aat__COUNT, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error), error);
        if (COMPILER__check__error_occured(error)) {
            goto failure;
        }
        (*accountling_statement).function_call__inputs.count++;
    }
    for (COMPILER__argument_index io_index = 0; io_index < parsling_statement.outputs.count; io_index++) {
        // append variable
        ANVIL__bt is_valid_argument;
        COMPILER__append__accountling_variable_argument(&(*accountling_statement).function_call__outputs.list, COMPILER__account__functions__mark_variable(structures, accountling_function, COMPILER__get__parsling_argument_by_index(parsling_statement.outputs, io_index), ((COMPILER__structure_index*)((COMPILER__accountling_function_header*)function_headers.category[COMPILER__afht__user_defined].list.buffer.start)[match_index].output_types.list.buffer.start)[io_index] - COMPILER__aat__COUNT, COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error), error);
        if (COMPILER__check__error_occured(error)) {
            goto failure;
        }
        (*accountling_statement).function_call__outputs.count++;
    }

    // goto success
    goto success;

    failure:
    return ANVIL__bt__false;

    success:
    return ANVIL__bt__true;
}

// get all statements & variables
void COMPILER__account__functions__function_sequential_information__one_scope(COMPILER__accountling_structures structures, COMPILER__accountling_function_headers function_headers, COMPILER__accountling_function* accountling_function, COMPILER__accountling_scope* accountling_scope, COMPILER__parsling_statement parsling_source_statement, COMPILER__error* error) {
    // translate all statements
    // setup current statement
    ANVIL__current current_statement = ANVIL__calculate__current_from_list_filled_index(&parsling_source_statement.subscope.statements.list);

    // setup statements
    (*accountling_scope).statements = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_statement) * 32, error);
    if (COMPILER__check__error_occured(error)) {
        return;
    }

    // get condition
    // if scope is not master scope
    if (COMPILER__check__empty_namespace(parsling_source_statement.name.name) == ANVIL__bt__false) {
        ANVIL__bt is_valid_argument;
        (*accountling_scope).condition = COMPILER__account__functions__mark_variable(structures, accountling_function, parsling_source_statement.subscope_flag_name, COMPILER__ptt__dragon_cell, COMPILER__asvt__input, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__check__error_occured(error)) {
            return;
        }
        if (is_valid_argument == ANVIL__bt__false) {
            // set error
            *error = COMPILER__open__error("Accounting Error: A scope condition variable does not exist or is of the wrong type.", COMPILER__get__namespace_lexling_location(parsling_source_statement.subscope_flag_name.name));

            return;
        }

        // copy condition
        ((COMPILER__accountling_scope_header*)(*accountling_function).scope_headers.list.buffer.start)[COMPILER__account__functions__get_scope_index(accountling_function, parsling_source_statement.name.name)].argument = (*accountling_scope).condition;
    // otherwise, a master scope requires a manual condition
    } else {
        // setup name
        COMPILER__namespace temp_search_name = COMPILER__open__namespace_from_single_lexling(COMPILER__create__lexling(COMPILER__lt__name, ANVIL__open__buffer_from_string((u8*)(COMPILER__define__master_namespace ".always"), ANVIL__bt__false, ANVIL__bt__false), COMPILER__create_null__character_location()), error);
        if (COMPILER__check__error_occured(error)) {
            return;
        }

        // setup condition
        (*accountling_scope).condition = COMPILER__account__functions__get_variable_argument_by_name((*accountling_function).variables, temp_search_name);
        COMPILER__scope_index scope_index_temp = COMPILER__account__functions__get_scope_index(accountling_function, parsling_source_statement.name.name);
        ((COMPILER__accountling_scope_header*)(*accountling_function).scope_headers.list.buffer.start)[scope_index_temp].argument = (*accountling_scope).condition;

        // free temp name
        COMPILER__close__parsling_namespace(temp_search_name);
    }

    // for each statement
    while (ANVIL__check__current_within_range(current_statement)) {
        // get parsling statement
        COMPILER__parsling_statement parsling_statement = *(COMPILER__parsling_statement*)current_statement.start;

        // setup accountling statement
        COMPILER__accountling_statement accountling_statement = COMPILER__create_null__accountling_statement();

        // determine statement type
        if (parsling_statement.type == COMPILER__stt__function_call) {
            // find definition
            // find sets
            if (COMPILER__account__functions__check_and_get_statement_translation__sets(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find prints
            if (COMPILER__account__functions__check_and_get_statement_translation__prints(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find copiers
            if (COMPILER__account__functions__check_and_get_statement_translation__copy(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find packers
            if (COMPILER__account__functions__check_and_get_statement_translation__packers(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find null packers
            if (COMPILER__account__functions__check_and_get_statement_translation__null_packers(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find increment packers
            if (COMPILER__account__functions__check_and_get_statement_translation__increment_packers(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find jumps
            if (COMPILER__account__functions__check_and_get_statement_translation__jumping(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find checks
            if (COMPILER__account__functions__check_and_get_statement_translation__data_checking(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find bit operations
            if (COMPILER__account__functions__check_and_get_statement_translation__bit_operations(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find integer operations
            if (COMPILER__account__functions__check_and_get_statement_translation__integer_operations(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find buffer acquisitions
            if (COMPILER__account__functions__check_and_get_statement_translation__buffer_acquisition(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find cell address movers
            if (COMPILER__account__functions__check_and_get_statement_translation__cell_address_movers(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find file buffer movers
            if (COMPILER__account__functions__check_and_get_statement_translation__file_buffer_movers(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find structure buffer movers
            if (COMPILER__account__functions__check_and_get_statement_translation__structure_buffer_movers(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find compile functions
            if (COMPILER__account__functions__check_and_get_statement_translation__compiler_assorted(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find compile functions
            if (COMPILER__account__functions__check_and_get_statement_translation__context_assorted(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find time functions
            if (COMPILER__account__functions__check_and_get_statement_translation__time_functions(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find allocation related functions
            if (COMPILER__account__functions__check_and_get_statement_translation__allocation_related_functions(structures, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // find function call
            if (COMPILER__account__functions__check_and_get_statement_translation__user_defined_function_calls(structures, function_headers, accountling_function, parsling_statement, &accountling_statement, error)) {
                // check for error
                if (COMPILER__check__error_occured(error)) {
                    return;
                }
                goto next_statement;
            }

            // error, no statement was matched
            *error = COMPILER__open__error("Accounting Error: A function call was not valid.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

            return;
        } else if (parsling_statement.type == COMPILER__stt__offset) {
            // search for offset
            COMPILER__offset_index offset_index = COMPILER__account__functions__get_offset_index(accountling_function, parsling_statement.name.name);

            // setup offset data
            accountling_statement.offset_index = offset_index;
            accountling_statement.statement_type = COMPILER__ast__offset;
            
            /*// append statement
            COMPILER__append__accountling_statement(&(*accountling_scope).statements.list, accountling_statement, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }*/
        } else if (parsling_statement.type == COMPILER__stt__subscope) {
            // get scope data
            COMPILER__account__functions__function_sequential_information__one_scope(structures, function_headers, accountling_function, &accountling_statement.scope_data, parsling_statement, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }
            accountling_statement.scope_index = COMPILER__account__functions__get_scope_index(accountling_function, parsling_statement.name.name);
            accountling_statement.statement_type = COMPILER__ast__scope;

            /*// append statement
            COMPILER__append__accountling_statement(&(*accountling_scope).statements.list, accountling_statement, error);
            if (COMPILER__check__error_occured(error)) {
                return;
            }*/
        // unrecognized statement type
        } else {
            *error = COMPILER__open__error("Internal Error: Unrecognized statement type in accounter, oops.", COMPILER__get__namespace_lexling_location(parsling_statement.name.name));

            return;
        }

        // next statement offset
        next_statement:

        // append statement
        COMPILER__append__accountling_statement(&(*accountling_scope).statements.list, accountling_statement, error);

        // check for error
        if (COMPILER__check__error_occured(error)) {
            return;
        }

        // increment count
        (*accountling_scope).statements.count++;

        // advance current
        current_statement.start += sizeof(COMPILER__parsling_statement);
    }

    return;
}

// search for a variable in a function variable list
COMPILER__variable_index COMPILER__account__functions__function_io_variables__get_argument_index(ANVIL__counted_list variable_arguments, COMPILER__accountling_variable_argument argument) {
    // search each argument
    for (COMPILER__variable_index index = 0; index < variable_arguments.count; index++) {
        // check for match
        if (((COMPILER__accountling_variable_argument*)variable_arguments.list.buffer.start)[index].index == argument.index && ((COMPILER__accountling_variable_argument*)variable_arguments.list.buffer.start)[index].type == argument.type) {
            // match!
            return index;
        }
    }

    // not found
    return variable_arguments.count;
}

// get function arguments
void COMPILER__account__functions__function_io_variables(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__parsling_function parsling_function, COMPILER__error* error) {
    ANVIL__bt is_valid_argument;

    // setup variable lists
    (*accountling_function).function_inputs = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_variable_argument) * parsling_function.header.inputs.count, error);
    if (COMPILER__check__error_occured(error)) {
        return;
    }
    (*accountling_function).function_outputs = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_variable_argument) * parsling_function.header.outputs.count, error);
    if (COMPILER__check__error_occured(error)) {
        return;
    }

    // declare input variables
    for (COMPILER__argument_index io_index = 0; io_index < parsling_function.header.inputs.count; io_index++) {
        // if variable is not declared already
        // get argument
        COMPILER__parsling_argument parsling_argument = COMPILER__get__parsling_argument_by_index(parsling_function.header.inputs, io_index);

        // check if variable exists
        COMPILER__accountling_variable_argument accountling_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, parsling_argument, COMPILER__find__accountling_structure_name_index(structures.name_table, parsling_argument.type), COMPILER__asvt__output, ANVIL__bt__false, &is_valid_argument, error);
        if (accountling_argument.index >= COMPILER__avat__COUNT && is_valid_argument == ANVIL__bt__true) {
            // declare variable
            accountling_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, parsling_argument, COMPILER__find__accountling_structure_name_index(structures.name_table, parsling_argument.type), COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);

            // append variable
            COMPILER__append__accountling_variable_argument(&(*accountling_function).function_inputs.list, accountling_argument, error);
            (*accountling_function).function_inputs.count++;
        // input is declared twice, error
        } else {
            *error = COMPILER__open__error("Accounting Error: An input argument is declared more than once.", COMPILER__get__namespace_lexling_location(parsling_argument.name));

            return;
        }
        if (COMPILER__check__error_occured(error)) {
            return;
        }
    }

    // declare outputs
    for (COMPILER__argument_index io_index = 0; io_index < parsling_function.header.outputs.count; io_index++) {
        // if variable is not declared already
        // get argument
        COMPILER__parsling_argument parsling_argument = COMPILER__get__parsling_argument_by_index(parsling_function.header.outputs, io_index);

        // check if variable exists
        COMPILER__accountling_variable_argument accountling_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, parsling_argument, COMPILER__find__accountling_structure_name_index(structures.name_table, parsling_argument.type), COMPILER__asvt__output, ANVIL__bt__false, &is_valid_argument, error);
        if (COMPILER__account__functions__function_io_variables__get_argument_index((*accountling_function).function_outputs, accountling_argument) >= (*accountling_function).function_outputs.count && is_valid_argument == ANVIL__bt__true) {
            // declare variable
            accountling_argument = COMPILER__account__functions__mark_variable(structures, accountling_function, parsling_argument, COMPILER__find__accountling_structure_name_index(structures.name_table, parsling_argument.type), COMPILER__asvt__output, ANVIL__bt__true, &is_valid_argument, error);

            // append variable
            COMPILER__append__accountling_variable_argument(&(*accountling_function).function_outputs.list, accountling_argument, error);
            (*accountling_function).function_outputs.count++;
        // input is declared twice, error
        } else {
            *error = COMPILER__open__error("Accounting Error: An output argument is declared more than once.", COMPILER__get__namespace_lexling_location(parsling_argument.name));

            return;
        }
        if (COMPILER__check__error_occured(error)) {
            return;
        }
    }

    return;
}

// generate predefined variables
void COMPILER__account__functions__predefined_variables(COMPILER__accountling_structures structures, COMPILER__accountling_function* accountling_function, COMPILER__error* error) {
    COMPILER__accountling_variable variable;

    // setup current
    ANVIL__current current_blueprintling = ANVIL__create__buffer((ANVIL__address)COMPILER__global__predefined_variables, (ANVIL__address)(COMPILER__global__predefined_variables + sizeof(COMPILER__global__predefined_variables) - 1));

    // loop until end of blueprint
    while (ANVIL__check__current_within_range(current_blueprintling) && COMPILER__read__blueprintling(&current_blueprintling) == COMPILER__abt__define_type) {
        // skip past define type marker
        COMPILER__next__blueprintling(&current_blueprintling);

        // read predefined variable name
        variable.name = COMPILER__create__lexling(COMPILER__lt__name, ANVIL__open__buffer_from_string((u8*)COMPILER__global__predefined_variable_names[(COMPILER__index)COMPILER__read_and_next__blueprintling(&current_blueprintling)], ANVIL__bt__false, ANVIL__bt__false), COMPILER__create_null__character_location());

        // read type
        variable.type = (COMPILER__variable_type_index)COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // get cell range
        variable.cells.start = (ANVIL__cell_ID)COMPILER__read_and_next__blueprintling(&current_blueprintling);
        variable.cells.end = (ANVIL__cell_ID)COMPILER__read_and_next__blueprintling(&current_blueprintling);

        // setup variable range (since predefined there is no variable range, make invalid)
        variable.members = COMPILER__create__accountling_variable_range(-1, -1);

        // append variable
        COMPILER__append__accountling_variable(&(*accountling_function).variables.lists[COMPILER__avat__master].list, variable, error);
        if (COMPILER__check__error_occured(error)) {
            return;
        }

        // increment count
        (*accountling_function).variables.lists[COMPILER__avat__master].count++;
    }

    return;
}

// account all functions
COMPILER__accountling_functions COMPILER__account__functions__user_defined_function_bodies(COMPILER__accountling_functions functions, COMPILER__accountling_structures structures, COMPILER__parsling_program parsling_program, COMPILER__error* error) {
    // open function body list
    functions.bodies = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_function) * functions.headers.category[COMPILER__afht__user_defined].count, error);

    // account each function
    ANVIL__current current_parsling_function = ANVIL__calculate__current_from_list_filled_index(&parsling_program.functions.list);

    // for each function
    while (ANVIL__check__current_within_range(current_parsling_function)) {
        // get function
        COMPILER__parsling_function parsling_function = *(COMPILER__parsling_function*)current_parsling_function.start;

        // account function
        {
            // setup function variable
            COMPILER__accountling_function accountling_function;

            // intialize statistics
            accountling_function.next_available_workspace_cell = ANVIL__srt__start__workspace;

            // allocate scope headers list
            accountling_function.scope_headers = COMPILER__open__counted_list_with_error(sizeof(COMPILER__accountling_scope_header) * 16, error);
            if (COMPILER__check__error_occured(error)) {
                return functions;
            }

            // allocate offsets list
            accountling_function.offsets = COMPILER__open__counted_list_with_error(sizeof(COMPILER__namespace) * 16, error);
            if (COMPILER__check__error_occured(error)) {
                // close data
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);

                return functions;
            }

            // get scope headers & offsets
            COMPILER__account__functions__get_function_level_data(&accountling_function, parsling_function.scope.subscope, error);
            if (COMPILER__check__error_occured(error)) {
                // close data
                ANVIL__close__counted_list(accountling_function.offsets);
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);
                
                return functions;
            }

            // allocate variables list
            accountling_function.variables = COMPILER__open__variables(error);
            if (COMPILER__check__error_occured(error)) {
                // close prior data
                ANVIL__close__counted_list(accountling_function.offsets);
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);
                COMPILER__close__accountling_variables(accountling_function.variables);

                return functions;
            }

            // open strings
            accountling_function.strings = COMPILER__open__counted_list_with_error(sizeof(ANVIL__buffer) * 16, error);
            if (COMPILER__check__error_occured(error)) {
                // close prior data
                ANVIL__close__counted_list(accountling_function.offsets);
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);
                COMPILER__close__accountling_variables(accountling_function.variables);

                return functions;
            }

            // get function IO
            COMPILER__account__functions__function_io_variables(structures, &accountling_function, parsling_function, error);
            if (COMPILER__check__error_occured(error)) {
                // close prior data
                ANVIL__close__counted_list(accountling_function.offsets);
                COMPILER__close__accountling_strings(accountling_function.strings);
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);
                COMPILER__close__accountling_variables(accountling_function.variables);
                ANVIL__close__counted_list(accountling_function.function_inputs);
                ANVIL__close__counted_list(accountling_function.function_outputs);

                return functions;
            }

            // generate predefined variables
            COMPILER__account__functions__predefined_variables(structures, &accountling_function, error);
            if (COMPILER__check__error_occured(error)) {
                // close prior data
                ANVIL__close__counted_list(accountling_function.offsets);
                COMPILER__close__accountling_strings(accountling_function.strings);
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);
                COMPILER__close__accountling_variables(accountling_function.variables);
                ANVIL__close__counted_list(accountling_function.function_inputs);
                ANVIL__close__counted_list(accountling_function.function_outputs);

                return functions;
            }

            // get statements
            COMPILER__account__functions__function_sequential_information__one_scope(structures, functions.headers, &accountling_function, &accountling_function.scope, parsling_function.scope, error);
            if (COMPILER__check__error_occured(error)) {
                // close prior data
                ANVIL__close__counted_list(accountling_function.offsets);
                COMPILER__close__accountling_strings(accountling_function.strings);
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);
                COMPILER__close__accountling_scope(accountling_function.scope);
                COMPILER__close__accountling_variables(accountling_function.variables);
                ANVIL__close__counted_list(accountling_function.function_inputs);
                ANVIL__close__counted_list(accountling_function.function_outputs);

                return functions;
            }

            // append function
            COMPILER__append__accountling_function(&functions.bodies.list, accountling_function, error);
            if (COMPILER__check__error_occured(error)) {
                // close prior data
                ANVIL__close__counted_list(accountling_function.offsets);
                COMPILER__close__accountling_strings(accountling_function.strings);
                COMPILER__close__accountling_scope_headers(accountling_function.scope_headers, error);
                COMPILER__close__accountling_scope(accountling_function.scope);
                COMPILER__close__accountling_variables(accountling_function.variables);
                ANVIL__close__counted_list(accountling_function.function_inputs);
                ANVIL__close__counted_list(accountling_function.function_outputs);

                return functions;
            }
            functions.bodies.count++;
        }

        // next function
        current_parsling_function.start += sizeof(COMPILER__parsling_function);
    }

    return functions;
}

// account all functions & headers
COMPILER__accountling_functions COMPILER__account__functions(COMPILER__parsling_program parsling_program, COMPILER__accountling_structures structures, COMPILER__error* error) {
    COMPILER__accountling_functions output;

    // open output
    output.headers = COMPILER__open__accountling_function_headers(error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // generate predefined headers
    output.headers = COMPILER__account__functions__generate_predefined_function_headers(structures, output.headers, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // account user defined function headers
    output.headers = COMPILER__account__functions__user_defined_function_headers(output.headers, parsling_program, structures.name_table, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // account user defined function bodies
    output = COMPILER__account__functions__user_defined_function_bodies(output, structures, parsling_program, error);

    return output;
}

// search for main
COMPILER__function_index COMPILER__find__entry_point(COMPILER__accountling_functions functions, COMPILER__error* error) {
    // setup main name
    COMPILER__namespace searching_for = COMPILER__open__namespace_from_single_lexling(COMPILER__open__lexling_from_string(COMPILER__define__master_namespace ".main", COMPILER__lt__name, COMPILER__create_null__character_location()), error);
    if (COMPILER__check__error_occured(error)) {
        return -1;
    }

    // search for main
    for (COMPILER__function_index index = 0; index < functions.headers.category[COMPILER__afht__user_defined].count; index++) {
        // get header
        COMPILER__accountling_function_header header = ((COMPILER__accountling_function_header*)functions.headers.category[COMPILER__afht__user_defined].list.buffer.start)[index];

        // check header
        if (COMPILER__check__identical_namespaces(header.name, searching_for) == ANVIL__bt__true && header.input_types.count == 0 && header.output_types.count == 0) {
            // match!
            COMPILER__close__parsling_namespace(searching_for);
            return index;
        }
    }

    // not found
    *error = COMPILER__open__error("Accounting Error: The entry point '" COMPILER__define__master_namespace ".main()()' could not be found.", COMPILER__create_null__character_location());
    
    // return
    COMPILER__close__parsling_namespace(searching_for);

    return -1;
}

// account all files into one accountling program
COMPILER__accountling_program COMPILER__account__program(COMPILER__parsling_program parsling_program, COMPILER__error* error) {
    COMPILER__accountling_program output;

    // get all structures
    output.structures = COMPILER__account__structures(parsling_program, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // get all functions & function headers
    output.functions = COMPILER__account__functions(parsling_program, output.structures, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // search for main
    output.entry_point = COMPILER__find__entry_point(output.functions, error);

    return output;
}

// print accountling structure
void COMPILER__print__accountling_structure(COMPILER__accountling_structures accountling_structures, COMPILER__accountling_structure accountling_structure, COMPILER__structure_index index, ANVIL__tab_count tab_depth) {
    // print header
    ANVIL__print__tabs(tab_depth);
    printf("[ ID: %lu, Cell Count: %lu ] Structure '!", index, accountling_structure.cell_count);
    COMPILER__print__namespace(accountling_structure.name);
    printf("'\n");

    // print members
    // setup current
    ANVIL__current current_member = ANVIL__calculate__current_from_list_filled_index(&accountling_structure.members.list);

    // for each member
    while (ANVIL__check__current_within_range(current_member)) {
        // get member
        COMPILER__accountling_structure_member member = *(COMPILER__accountling_structure_member*)current_member.start;

        // print header
        ANVIL__print__tabs(tab_depth + 1);

        // print index, member name and member type
        COMPILER__print__namespace(member.name);
        printf(" !");
        if (member.structure_ID >= accountling_structures.name_table.count) {
            printf("(internal||error)");
        } else {
            COMPILER__print__namespace(((COMPILER__namespace*)accountling_structures.name_table.list.buffer.start)[member.structure_ID]);
        }
        printf("[ %lu ]\n", member.structure_ID);

        // next member
        current_member.start += sizeof(COMPILER__accountling_structure_member);
    }

    return;
}

// print accountling structures
void COMPILER__print__accountling_structures(COMPILER__accountling_structures structures, ANVIL__tab_count tab_depth) {
    // print header
    ANVIL__print__tabs(tab_depth);
    printf("Structure Name Table (%lu):\n", structures.name_table.count);

    // print name table
    // setup current
    ANVIL__current current_name = ANVIL__calculate__current_from_list_filled_index(&structures.name_table.list);

    // for each name
    while (ANVIL__check__current_within_range(current_name)) {
        // get name
        COMPILER__namespace name = *(COMPILER__namespace*)current_name.start;

        // print name
        ANVIL__print__tabs(tab_depth + 1);
        printf("!");
        COMPILER__print__namespace(name);
        printf("\n");

        // next name
        current_name.start += sizeof(COMPILER__namespace);
    }

    // print header
    ANVIL__print__tabs(tab_depth);
    printf("Structures (%lu):\n", structures.data_table.count);

    // print all structures
    // setup current
    ANVIL__current current_structure = ANVIL__calculate__current_from_list_filled_index(&structures.data_table.list);
    COMPILER__structure_index structure_ID = 0;

    // for each structure
    while (ANVIL__check__current_within_range(current_structure)) {
        // get structure
        COMPILER__accountling_structure accountling_structure = *(COMPILER__accountling_structure*)current_structure.start;

        // print structure
        COMPILER__print__accountling_structure(structures, accountling_structure, structure_ID, tab_depth + 1);

        // next structure
        current_structure.start += sizeof(COMPILER__accountling_structure);
        structure_ID++;
    }

    return;
}

// print accountling function header arguments
void COMPILER__print__accountling_function_header_arguments(ANVIL__counted_list arguments) {
    // print start of arguments
    printf("(");

    // setup current
    ANVIL__current current_structure_index = ANVIL__calculate__current_from_list_filled_index(&arguments.list);

    // for each structure index
    while (ANVIL__check__current_within_range(current_structure_index)) {
        // get structure index
        COMPILER__structure_index structure_index = *(COMPILER__structure_index*)current_structure_index.start;

        // print start of argument
        printf("[ raw: %lu, ", structure_index);

        // check argument for embedded type
        if (structure_index < COMPILER__aat__COUNT) {
            // print aat
            printf("aat: %lu", structure_index);
        } else {
            // print structure index
            printf("structure_index: %lu", structure_index - COMPILER__aat__COUNT);
        }

        // print end of argument
        printf(" ]");

        // next argument
        current_structure_index.start += sizeof(COMPILER__structure_index);
    }

    // print end of arguments
    printf(")");

    return;
}

// print an accountling function header
void COMPILER__print__accountling_function_header(COMPILER__accountling_function_header header) {
    // print name
    COMPILER__print__namespace(header.name);

    // print io
    COMPILER__print__accountling_function_header_arguments(header.input_types);
    COMPILER__print__accountling_function_header_arguments(header.output_types);

    return;
}

// print an accountling variable argument
void COMPILER__print__accountling_variable_argument(COMPILER__accountling_variable_argument argument) {
    // print data
    printf("[ list_ID: %lu, index: %lu ]", (ANVIL__u64)argument.type, (ANVIL__u64)argument.index);

    return;
}

// print an accounting variable
void COMPILER__print__accountling_variable(COMPILER__accountling_function function, COMPILER__accountling_structures structures, COMPILER__accountling_variable variable, COMPILER__variable_member_index* member_index, ANVIL__bt is_in_member_list, COMPILER__variable_index master_variable_index, ANVIL__tab_count tab_depth) {
    // print variable data
    ANVIL__print__tabs(tab_depth);
    if (is_in_member_list == ANVIL__bt__true) {
        printf("[ member_index: %lu ] ", (*member_index));
        (*member_index)++;
    }
    printf("[ master_index: %lu, type: %lu, name: '", master_variable_index, variable.type);
    ANVIL__print__buffer(variable.name.value);
    printf("', cells: [ %lu->%lu ], member_range: [ %lu->%lu ]]\n", (ANVIL__u64)variable.cells.start, (ANVIL__u64)variable.cells.end, variable.members.start, variable.members.end);

    // print all members
    if (variable.members.start != (COMPILER__variable_member_index)-1) {
        for (COMPILER__variable_member_index index = variable.members.start; index <= variable.members.end; index++) {
            // get member variable
            COMPILER__accountling_variable member_variable = ((COMPILER__accountling_variable*)function.variables.lists[COMPILER__avat__member].list.buffer.start)[index];

            // only print if variable is not end node
            if (member_variable.type < structures.data_table.count) {
                // print member
                COMPILER__print__accountling_variable(function, structures, member_variable, member_index, ANVIL__bt__true, master_variable_index, tab_depth + 1);
            }
        }
    }

    return;
}

// print an accountling scope
void COMPILER__print__accountling_scope(COMPILER__accountling_scope statements, ANVIL__tab_count tab_depth) {
    // print each statement
    for (COMPILER__statement_index statement_index = 0; statement_index < statements.statements.count; statement_index++) {
        // get statement
        COMPILER__accountling_statement statement = ((COMPILER__accountling_statement*)statements.statements.list.buffer.start)[statement_index];

        // print statement start
        ANVIL__print__tabs(tab_depth + 1);

        // print statement data based on type
        if (statement.statement_type == COMPILER__ast__predefined__set__cell) {
            printf("COMPILER__ast__predefined__set__cell(raw_value: %lu, variable_argument: ", statement.set_cell__raw_value);
            COMPILER__print__accountling_variable_argument(statement.set_cell__variable_argument);
            printf(");\n");
        } else if (statement.statement_type == COMPILER__ast__predefined__set__string) {
            printf("COMPILER__ast__predefined__set__string(string_index: %lu, variable_argument: ", statement.set_string__string_value_index);
            COMPILER__print__accountling_variable_argument(statement.set_string__variable_argument);
            printf(");\n");
        } else if (statement.statement_type == COMPILER__ast__predefined__pack__anything) {
            printf("COMPILER__ast__predefined__pack__anything(input_arguments: ");
            for (COMPILER__variable_index index = 0; index < statement.pack__inputs.count; index++) {
                COMPILER__print__accountling_variable_argument(((COMPILER__accountling_variable_argument*)statement.pack__inputs.list.buffer.start)[index]);
            }
            printf(")(output_argument: ");
            COMPILER__print__accountling_variable_argument(statement.pack__output);
            printf(");\n");
        } else if (statement.statement_type == COMPILER__ast__predefined__jump__top) {
            printf("COMPILER__ast__predefined__jump__top(scope_index: %lu, condition: ", statement.scope_index);
            COMPILER__print__accountling_variable_argument(statement.jump__variable_argument);
            printf(");\n");
        } else if (statement.statement_type == COMPILER__ast__predefined__jump__bottom) {
            printf("COMPILER__ast__predefined__jump__bottom(scope_index: %lu, condition: ", statement.scope_index);
            COMPILER__print__accountling_variable_argument(statement.jump__variable_argument);
            printf(");\n");
        } else if (statement.statement_type == COMPILER__ast__predefined__jump__offset) {
            printf("COMPILER__ast__predefined__jump__offset(offset_index: %lu, condition: ", statement.offset_index);
            COMPILER__print__accountling_variable_argument(statement.jump__variable_argument);
            printf(");\n");
        } else if (statement.statement_type == COMPILER__ast__user_defined_function_call) {
            printf("COMPILER__ast__user_defined_function_call(input_arguments: ");
            for (COMPILER__variable_index index = 0; index < statement.function_call__inputs.count; index++) {
                COMPILER__print__accountling_variable_argument(((COMPILER__accountling_variable_argument*)statement.function_call__inputs.list.buffer.start)[index]);
            }
            printf(")(output_arguments: ");
            for (COMPILER__variable_index index = 0; index < statement.function_call__outputs.count; index++) {
                COMPILER__print__accountling_variable_argument(((COMPILER__accountling_variable_argument*)statement.function_call__outputs.list.buffer.start)[index]);
            }
            printf(");\n");
        } else if (statement.statement_type == COMPILER__ast__offset) {
            printf("COMPILER__ast__offset(offset_index: %lu)", statement.offset_index);
            printf("\n");
        } else if (statement.statement_type == COMPILER__ast__scope) {
            printf("COMPILER__ast__scope(scope_index: %lu)\n", statement.scope_index);
            if (statement.scope_data.statements.count > 0) {
                COMPILER__print__accountling_scope(statement.scope_data, tab_depth + 1);
            }
        } else {
            printf("[ Internal Issue: Debug info not implemented for statement type '%lu' ]\n", (ANVIL__u64)statement.statement_type);
        }
    }

    return;
}

// print accountling functions
void COMPILER__print__accountling_functions(COMPILER__accountling_structures structures, COMPILER__accountling_functions functions, ANVIL__tab_count tab_depth) {
    // print section header
    ANVIL__print__tabs(tab_depth);
    printf("Function Header Table:\n");

    // print each header table
    for (COMPILER__afht category_index = 0; category_index < COMPILER__afht__COUNT; category_index++) {
        // print section header
        ANVIL__print__tabs(tab_depth + 1);
        printf("Section (%i):\n", category_index);

        // print header table
        // setup current
        ANVIL__current current_header = ANVIL__calculate__current_from_list_filled_index(&functions.headers.category[category_index].list);
        COMPILER__function_header_index header_index = 0;

        // for each header
        while (ANVIL__check__current_within_range(current_header)) {
            // get header
            COMPILER__accountling_function_header header = *(COMPILER__accountling_function_header*)current_header.start;

            // print header
            ANVIL__print__tabs(tab_depth + 2);
            printf("[ %lu ] ", header_index);
            COMPILER__print__accountling_function_header(header);

            // next header
            printf("\n");
            current_header.start += sizeof(COMPILER__accountling_function_header);
            header_index++;
        }
    }

    // if there function bodies, print them
    if (functions.bodies.count > 0) {
        // print section header
        ANVIL__print__tabs(tab_depth);
        printf("Function Bodies Table [ function_body_count: %lu ]:\n", functions.bodies.count);

        // print each function
        for (COMPILER__function_index function_index = 0; function_index < functions.bodies.count; function_index++) {
            // get function
            COMPILER__accountling_function function = ((COMPILER__accountling_function*)functions.bodies.list.buffer.start)[function_index];

            // print data
            {
                // print name
                ANVIL__print__tabs(tab_depth + 1);
                printf("Function '");
                COMPILER__print__accountling_function_header(((COMPILER__accountling_function_header*)functions.headers.category[COMPILER__afht__user_defined].list.buffer.start)[function_index]);
                printf("':\n");

                // print scope headers
                if (function.scope_headers.count > 0) {
                    // print scope headers header
                    ANVIL__print__tabs(tab_depth + 2);
                    printf("Scopes:\n");
                    
                    // print each scope header
                    for (COMPILER__scope_index scope_index = 0; scope_index < function.scope_headers.count; scope_index++) {
                        // get scope
                        COMPILER__accountling_scope_header scope_header = ((COMPILER__accountling_scope_header*)function.scope_headers.list.buffer.start)[scope_index];

                        // print scope data
                        ANVIL__print__tabs(tab_depth + 3);
                        COMPILER__print__namespace(scope_header.name);
                        printf(": ");
                        COMPILER__print__accountling_variable_argument(scope_header.argument);
                        printf("\n");
                    }
                }

                // print offsets
                if (function.offsets.count > 0) {
                    // print offsets header
                    ANVIL__print__tabs(tab_depth + 2);
                    printf("Offsets:\n");
                    
                    // print each offset
                    for (COMPILER__offset_index offset_index = 0; offset_index < function.offsets.count; offset_index++) {
                        // get offset
                        COMPILER__namespace offset = ((COMPILER__namespace*)function.offsets.list.buffer.start)[offset_index];

                        // print scope data
                        ANVIL__print__tabs(tab_depth + 3);
                        COMPILER__print__namespace(offset);
                        printf("\n");
                    }
                }

                // print variable declarations
                if (function.variables.lists[COMPILER__avat__master].count > 0) {
                    // setup member index
                    COMPILER__variable_member_index member_index = 0;

                    // print header
                    ANVIL__print__tabs(tab_depth + 2);
                    printf("Variables [ cells_used: %lu/%lu (%lu->%lu) ]:\n", function.next_available_workspace_cell - (ANVIL__u64)ANVIL__srt__start__workspace, (ANVIL__u64)ANVIL__srt__workspace__COUNT, (ANVIL__u64)ANVIL__srt__start__workspace, (ANVIL__u64)function.next_available_workspace_cell - 1);

                    // print each master & member variables
                    for (COMPILER__variable_index index = 0; index < function.variables.lists[COMPILER__avat__master].count; index++) {
                        // get variable
                        COMPILER__accountling_variable master_variable = ((COMPILER__accountling_variable*)function.variables.lists[COMPILER__avat__master].list.buffer.start)[index];

                        // print master & member variables
                        COMPILER__print__accountling_variable(function, structures, master_variable, &member_index, ANVIL__bt__false, index, tab_depth + 3);
                    }
                }

                // print inputs
                if (function.function_inputs.count > 0) {
                    // print header
                    ANVIL__print__tabs(tab_depth + 2);
                    printf("Inputs:\n");

                    // print inputs
                    for (COMPILER__argument_index io_index = 0; io_index < function.function_inputs.count; io_index++) {
                        // print input
                        ANVIL__print__tabs(tab_depth + 3);
                        printf("[ variable_index: %lu ]\n", ((COMPILER__accountling_variable_argument*)function.function_inputs.list.buffer.start)[io_index].index);
                    }
                }

                // print outputs
                if (function.function_outputs.count > 0) {
                    // print header
                    ANVIL__print__tabs(tab_depth + 2);
                    printf("Outputs:\n");

                    // print outputs
                    for (COMPILER__argument_index io_index = 0; io_index < function.function_outputs.count; io_index++) {
                        // print output
                        ANVIL__print__tabs(tab_depth + 3);
                        printf("[ variable_index: %lu ]\n", ((COMPILER__accountling_variable_argument*)function.function_outputs.list.buffer.start)[io_index].index);
                    }
                }

                // print statements
                if (function.scope.statements.count > 0) {
                    // print statements header
                    ANVIL__print__tabs(tab_depth + 2);
                    printf("Statements:\n");
                    
                    // print statements
                    COMPILER__print__accountling_scope(function.scope, tab_depth + 2);
                }

                // print strings
                if (function.strings.count > 0) {
                    // print strings header
                    ANVIL__print__tabs(tab_depth + 2);
                    printf("Strings:\n");
                    
                    // print each string
                    for (COMPILER__string_index string_index = 0; string_index < function.strings.count; string_index++) {
                        // get string
                        ANVIL__buffer string = ((ANVIL__buffer*)function.strings.list.buffer.start)[string_index];

                        // print string data
                        ANVIL__print__tabs(tab_depth + 3);
                        printf("[ %lu ] \"", string_index);
                        ANVIL__print__buffer(string);
                        printf("\"\n");
                    }
                }
            }
        }
    }

    return;
}

// print accountling program
void COMPILER__print__accountling_program(COMPILER__accountling_program program) {
    // print header
    printf("Accounted Program:\n");

    // print structures
    COMPILER__print__accountling_structures(program.structures, 1);

    // print functions
    COMPILER__print__accountling_functions(program.structures, program.functions, 1);

    return;
}

#endif
