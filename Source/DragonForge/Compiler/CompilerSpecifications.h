#ifndef DRAGON__compiler__specifications
#define DRAGON__compiler__specifications

/* Include */
// anvil instructions
#include "../../AnvilCoder.h"

/* Define */
// general compiler types
typedef BASIC__u64 COMPILER__index;
typedef COMPILER__index COMPILER__argument_index;
typedef COMPILER__index COMPILER__io_count;
typedef COMPILER__io_count COMPILER__input_count;
typedef COMPILER__io_count COMPILER__output_count;
typedef COMPILER__index COMPILER__accountling_index;
typedef COMPILER__accountling_index COMPILER__variable_index;
typedef COMPILER__accountling_index COMPILER__variable_type_index;
typedef COMPILER__accountling_index COMPILER__variable_member_index;
typedef BASIC__u64 COMPILER__blueprintling;
typedef COMPILER__index COMPILER__function_index;
typedef COMPILER__index COMPILER__function_header_index;
typedef COMPILER__index COMPILER__structure_index;
typedef COMPILER__index COMPILER__structure_member_index;
typedef COMPILER__structure_member_index COMPILER__structure_member_count;
typedef BASIC__buffer COMPILER__structure_handle;
typedef BASIC__buffer COMPILER__function_handle;
typedef COMPILER__index COMPILER__stack_index;
typedef COMPILER__index COMPILER__offset_index;
typedef COMPILER__index COMPILER__scope_index;
typedef COMPILER__index COMPILER__string_index;
typedef COMPILER__index COMPILER__statement_index;

// definitions
#define COMPILER__define__variable_is_internal_type -1
#define COMPILER__define__variable_name_index_from_name_list 0 // the first index is the variable name, all following names are structure accesses

// master namespace
#define COMPILER__define__master_namespace "df"

// accountling statement variable type (used for distinguishing statement inputs and outputs)
typedef enum COMPILER__asvt {
    COMPILER__asvt__input,
    COMPILER__asvt__output,
    COMPILER__asvt__COUNT,
} COMPILER__asvt;

// accountling variable argument type
typedef enum COMPILER__avat {
    COMPILER__avat__master,
    COMPILER__avat__member,
    COMPILER__avat__COUNT,
} COMPILER__avat;

// accounting blueprintling types
typedef enum COMPILER__abt {
    COMPILER__abt__end_blueprint,
    COMPILER__abt__define_type,
    COMPILER__abt__define_function_call,
} COMPILER__abt;

// accountling function header type (a category, NOT the individual built in instructions)
typedef enum COMPILER__afht {
    COMPILER__afht__predefined,
    COMPILER__afht__user_defined,
    COMPILER__afht__COUNT,
} COMPILER__afht;

// parsling argument type
typedef enum COMPILER__pat {
    // start
    COMPILER__pat__START,

    // invalid
    COMPILER__pat__invalid = COMPILER__pat__START,

    // anything that uses name lexlings and possibly colons
    COMPILER__pat__name,

    // offsets
    COMPILER__pat__offset,

    // literals
    COMPILER__pat__string_literal,

    // type
    COMPILER__pat__type,

    // end
    COMPILER__pat__END,

    // count
    COMPILER__pat__COUNT = COMPILER__pat__END - COMPILER__pat__START,
} COMPILER__pat;
char* COMPILER__global__general_argument_type_names[] = {
    "invalid",
    "name",
    "offset",
    "string",
    "type",
};

// parsling argument location type
typedef enum COMPILER__palt {
    // locations
    COMPILER__palt__structure_declaration,
    COMPILER__palt__function_declaration,
    COMPILER__palt__function_call,

    // count
    COMPILER__palt__COUNT,
} COMPILER__palt;

// predefined variable names
typedef enum COMPILER__pvn {
    // generic
    COMPILER__pvn__always,
    COMPILER__pvn__never,
    COMPILER__pvn__true,
    COMPILER__pvn__false,
    COMPILER__pvn__constant_0,
    COMPILER__pvn__constant_1,
    COMPILER__pvn__constant_2,
    COMPILER__pvn__constant_3,
    COMPILER__pvn__constant_4,
    COMPILER__pvn__constant_5,
    COMPILER__pvn__constant_6,
    COMPILER__pvn__constant_7,
    COMPILER__pvn__constant_8,
    COMPILER__pvn__constant_16,
    COMPILER__pvn__constant_32,
    COMPILER__pvn__constant_64,
    COMPILER__pvn__error,
    COMPILER__pvn__catch_addresses,
    COMPILER__pvn__cell_byte_size,
    COMPILER__pvn__context_byte_size,
    COMPILER__pvn__ascii_character_byte_size,
    COMPILER__pvn__context__input,
    COMPILER__pvn__context__output__result,
    COMPILER__pvn__context__output__error,
    COMPILER__pvn__call_stack,

    // anvil scraplets
    COMPILER__pvn__constant__anvil__instruction_ID__byte_size,
    COMPILER__pvn__constant__anvil__flag_ID__byte_size,
    COMPILER__pvn__constant__anvil__operation_ID__byte_size,
    COMPILER__pvn__constant__anvil__cell_ID__byte_size,
    COMPILER__pvn__constant__anvil__cell__byte_size,
    COMPILER__pvn__constant__anvil__operation_ID__cell_to_cell,
    COMPILER__pvn__constant__anvil__operation_ID__bits_or,
    COMPILER__pvn__constant__anvil__operation_ID__bits_invert,
    COMPILER__pvn__constant__anvil__operation_ID__bits_and,
    COMPILER__pvn__constant__anvil__operation_ID__bits_xor,
    COMPILER__pvn__constant__anvil__operation_ID__bits_shift_higher,
    COMPILER__pvn__constant__anvil__operation_ID__bits_shift_lower,
    COMPILER__pvn__constant__anvil__operation_ID__bits_overwrite,
    COMPILER__pvn__constant__anvil__operation_ID__integer_add,
    COMPILER__pvn__constant__anvil__operation_ID__integer_subtract,
    COMPILER__pvn__constant__anvil__operation_ID__integer_multiply,
    COMPILER__pvn__constant__anvil__operation_ID__integer_division,
    COMPILER__pvn__constant__anvil__operation_ID__integer_modulous,
    COMPILER__pvn__constant__anvil__operation_ID__integer_within_range,
    COMPILER__pvn__constant__anvil__operation_ID__flag_or,
    COMPILER__pvn__constant__anvil__operation_ID__flag_invert,
    COMPILER__pvn__constant__anvil__operation_ID__flag_and,
    COMPILER__pvn__constant__anvil__operation_ID__flag_xor,
    COMPILER__pvn__constant__anvil__operation_ID__flag_get,
    COMPILER__pvn__constant__anvil__operation_ID__flag_set,
} COMPILER__pvn;
// all predefined variable names
char* COMPILER__global__predefined_variable_names[] = {
    // generic
    COMPILER__define__master_namespace ".always",
    COMPILER__define__master_namespace ".never",
    COMPILER__define__master_namespace ".true",
    COMPILER__define__master_namespace ".false",
    COMPILER__define__master_namespace ".constant.0",
    COMPILER__define__master_namespace ".constant.1",
    COMPILER__define__master_namespace ".constant.2",
    COMPILER__define__master_namespace ".constant.3",
    COMPILER__define__master_namespace ".constant.4",
    COMPILER__define__master_namespace ".constant.5",
    COMPILER__define__master_namespace ".constant.6",
    COMPILER__define__master_namespace ".constant.7",
    COMPILER__define__master_namespace ".constant.8",
    COMPILER__define__master_namespace ".constant.16",
    COMPILER__define__master_namespace ".constant.32",
    COMPILER__define__master_namespace ".constant.64",
    COMPILER__define__master_namespace ".error",
    COMPILER__define__master_namespace ".catch_addresses",
    COMPILER__define__master_namespace ".cell.byte_size",
    COMPILER__define__master_namespace ".context.byte_size",
    COMPILER__define__master_namespace ".ascii_character.byte_size",
    COMPILER__define__master_namespace ".context.input",
    COMPILER__define__master_namespace ".context.output.result",
    COMPILER__define__master_namespace ".context.output.error",
    COMPILER__define__master_namespace ".call_stack",
};

// statement type type
typedef enum COMPILER__stt {
    // invalid
    COMPILER__stt__invalid,

    // statements
    COMPILER__stt__function_call,
    COMPILER__stt__offset,
    COMPILER__stt__subscope,

    // headers
    COMPILER__stt__function_header,
    COMPILER__stt__structure_header,

    // count
    COMPILER__stt__COUNT,
} COMPILER__stt;

// accountling statement type
typedef enum COMPILER__ast {
    // built in calls
    COMPILER__ast__predefined__execution__stop,

    // data insertion
    COMPILER__ast__predefined__set__cell,
    COMPILER__ast__predefined__set__string,

    // print
    COMPILER__ast__predefined__print__character,

    // filling structures
    COMPILER__ast__predefined__copy__anything,
    COMPILER__ast__predefined__pack__anything,
    COMPILER__ast__predefined__pack_null__anything,
    COMPILER__ast__predefined__pack_increment__anything,

    // jumps
    COMPILER__ast__predefined__jump__top,
    COMPILER__ast__predefined__jump__bottom,
    COMPILER__ast__predefined__jump__offset,

    // bits
    COMPILER__ast__predefined__bits_or,
    COMPILER__ast__predefined__bits_invert,
    COMPILER__ast__predefined__bits_and,
    COMPILER__ast__predefined__bits_xor,
    COMPILER__ast__predefined__bits_shift_higher,
    COMPILER__ast__predefined__bits_shift_lower,
    COMPILER__ast__predefined__bits_overwrite,

    // integers
    COMPILER__ast__predefined__check__integer_within_range,
    COMPILER__ast__predefined__integer__addition,
    COMPILER__ast__predefined__integer__subtraction,
    COMPILER__ast__predefined__integer__multiplication,
    COMPILER__ast__predefined__integer__division,
    COMPILER__ast__predefined__integer__modulous,

    // buffers
    COMPILER__ast__predefined__buffer__request,
    COMPILER__ast__predefined__buffer__return,

    // structures
    COMPILER__ast__predefined__structure__size,
    COMPILER__ast__predefined__structure__structure_to_buffer,
    COMPILER__ast__predefined__structure__buffer_to_structure,

    // movers
    COMPILER__ast__predefined__mover__address_to_cell,
    COMPILER__ast__predefined__mover__cell_to_address,
    COMPILER__ast__predefined__mover__file_to_buffer,
    COMPILER__ast__predefined__mover__buffer_to_file,
    COMPILER__ast__predefined__mover__buffer_copy__low_to_high,
    COMPILER__ast__predefined__mover__buffer_copy__high_to_low,

    // file deleters
    COMPILER__ast__predefined__delete_file,

    // allocations
    COMPILER__ast__predefined__allocation__find,

    // compilation
    COMPILER__ast__predefined__compilation__compile,

    // context
    COMPILER__ast__predefined__context__install_program,
    COMPILER__ast__predefined__context__run,

    // time
    COMPILER__ast__predefined__time__get_current_time,

    // user defined call
    COMPILER__ast__user_defined_function_call,

    // offsets
    COMPILER__ast__offset,

    // scopes
    COMPILER__ast__scope,
    
    // invalid
    COMPILER__ast__invalid,

    // COUNT
    COMPILER__ast__COUNT,
} COMPILER__ast;

// program stage type
typedef enum COMPILER__pst {
    // stages
    COMPILER__pst__invalid, // invalid
    COMPILER__pst__lexing, // lexing files
    COMPILER__pst__parsing, // parsing files
    COMPILER__pst__accounting, // semantics for all files
    COMPILER__pst__generating, // building program
    COMPILER__pst__running, // running program

    // count
    COMPILER__pst__COUNT,
} COMPILER__pst;

// accountling argument type
typedef enum COMPILER__aat {
    COMPILER__aat__invalid,
    COMPILER__aat__cell_value, // raw value
    COMPILER__aat__string_index, // string
    COMPILER__aat__offset, // offset
    COMPILER__aat__scope, // scope
    COMPILER__aat__variable, // variable / structure member from variable
    COMPILER__aat__COUNT,
} COMPILER__aat;

// predefined types
// names
char* COMPILER__global__predefined_type_names[] = {
    COMPILER__define__master_namespace ".cell",
    COMPILER__define__master_namespace ".buffer",
    COMPILER__define__master_namespace ".current",
    COMPILER__define__master_namespace ".time",
    COMPILER__define__master_namespace ".compiler.settings",
    COMPILER__define__master_namespace ".compiler.debug_information",
    COMPILER__define__master_namespace ".compiler.error",
    COMPILER__define__master_namespace ".text.location",
};
// predefined type's member names
const char* COMPILER__global__predefined_type_member_names[] = {
    "value",
    "start",
    "end",
    "buffer",
    "progress",
    "seconds",
    "nanoseconds",
    "include_standard",
    "debug_enabled",
    "generate_kickstarter",
    "data",
    "occured",
    "message",
    "location",
    "file_index",
    "line_number",
    "character_index",
    "INTERNAL",
};
// predefined type member name type
typedef enum COMPILER__ptmnt {
    COMPILER__ptmnt__value,
    COMPILER__ptmnt__start,
    COMPILER__ptmnt__end,
    COMPILER__ptmnt__buffer,
    COMPILER__ptmnt__progress,
    COMPILER__ptmnt__seconds,
    COMPILER__ptmnt__nanoseconds,
    COMPILER__ptmnt__include_standard,
    COMPILER__ptmnt__debug_enabled,
    COMPILER__ptmnt__generate_kickstarter,
    COMPILER__ptmnt__data,
    COMPILER__ptmnt__occured,
    COMPILER__ptmnt__message,
    COMPILER__ptmnt__location_information,
    COMPILER__ptmnt__file_index,
    COMPILER__ptmnt__line_number,
    COMPILER__ptmnt__character_index,
    COMPILER__ptmnt__COUNT,
} COMPILER__ptmnt;
// predefined type type
typedef enum COMPILER__ptt {
    // start
    COMPILER__ptt__START = 0,

    // type definitions
    COMPILER__ptt__dragon_cell = COMPILER__ptt__START,
    COMPILER__ptt__dragon_buffer,
    COMPILER__ptt__dragon_current,
    COMPILER__ptt__dragon_time,
    COMPILER__ptt__dragon_compiler__settings,
    COMPILER__ptt__dragon_compiler__debug_information,
    COMPILER__ptt__dragon_compiler__error,
    COMPILER__ptt__dragon_text_location,

    // user defined start
    COMPILER__ptt__USER_DEFINED_START,

    // end
    COMPILER__ptt__END = COMPILER__ptt__USER_DEFINED_START,

    // count
    COMPILER__ptt__COUNT = COMPILER__ptt__END - COMPILER__ptt__START,
} COMPILER__ptt;
// blueprint
COMPILER__blueprintling COMPILER__global__predefined_types[] = {
    // cell
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_cell,
        1,
        COMPILER__define__variable_is_internal_type,
            COMPILER__ptmnt__value,
    // buffer
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_buffer,
        2,
        COMPILER__ptt__dragon_cell, // start
            COMPILER__ptmnt__start,
        COMPILER__ptt__dragon_cell, // end
            COMPILER__ptmnt__end,
    // current
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_current,
        2,
        COMPILER__ptt__dragon_buffer,
            COMPILER__ptmnt__buffer,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__progress,
    // time
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_time,
        2,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__seconds, // seconds
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__nanoseconds, // nanoseconds
    // compiler settings
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_compiler__settings,
        3,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__include_standard,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__debug_enabled,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__generate_kickstarter,
    // compiler debug information
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_compiler__debug_information,
        1,
        COMPILER__ptt__dragon_buffer,
            COMPILER__ptmnt__data,
    // error information
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_compiler__error,
        3,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__occured,
        COMPILER__ptt__dragon_buffer,
            COMPILER__ptmnt__message,
        COMPILER__ptt__dragon_text_location,
            COMPILER__ptmnt__location_information,
    // text location
    COMPILER__abt__define_type,
        COMPILER__ptt__dragon_text_location,
        3,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__file_index,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__line_number,
        COMPILER__ptt__dragon_cell,
            COMPILER__ptmnt__character_index,
    COMPILER__abt__end_blueprint,
};

// blueprint
COMPILER__blueprintling COMPILER__global__predefined_variables[] = {
    COMPILER__abt__define_type,
        COMPILER__pvn__true,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__true,
        ANVIL__srt__constant__true,

    COMPILER__abt__define_type,
        COMPILER__pvn__false,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__false,
        ANVIL__srt__constant__false,

    COMPILER__abt__define_type,
        COMPILER__pvn__always,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__always_run__value,
        ANVIL__srt__constant__always_run__value,

    COMPILER__abt__define_type,
        COMPILER__pvn__never,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__never_run__value,
        ANVIL__srt__constant__never_run__value,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_0,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__0,
        ANVIL__srt__constant__0,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_1,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__1,
        ANVIL__srt__constant__1,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_2,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__2,
        ANVIL__srt__constant__2,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_3,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__3,
        ANVIL__srt__constant__3,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_4,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__4,
        ANVIL__srt__constant__4,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_5,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__5,
        ANVIL__srt__constant__5,
        
    COMPILER__abt__define_type,
        COMPILER__pvn__constant_6,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__6,
        ANVIL__srt__constant__6,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_7,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__7,
        ANVIL__srt__constant__7,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_8,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__8,
        ANVIL__srt__constant__8,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_16,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__16,
        ANVIL__srt__constant__16,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_32,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__32,
        ANVIL__srt__constant__32,

    COMPILER__abt__define_type,
        COMPILER__pvn__constant_64,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__64,
        ANVIL__srt__constant__64,

    COMPILER__abt__define_type,
        COMPILER__pvn__error,
        COMPILER__ptt__dragon_cell,
        ANVIL__rt__error_code,
        ANVIL__rt__error_code,

    COMPILER__abt__define_type,
        COMPILER__pvn__catch_addresses,
        COMPILER__ptt__dragon_cell,
        ANVIL__rt__address_catch_toggle,
        ANVIL__rt__address_catch_toggle,

    COMPILER__abt__define_type,
        COMPILER__pvn__cell_byte_size,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__cell_byte_size,
        ANVIL__srt__constant__cell_byte_size,

    COMPILER__abt__define_type,
        COMPILER__pvn__context_byte_size,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__context_byte_size,
        ANVIL__srt__constant__context_byte_size,

    COMPILER__abt__define_type,
        COMPILER__pvn__ascii_character_byte_size,
        COMPILER__ptt__dragon_cell,
        ANVIL__srt__constant__ascii_character_byte_size,
        ANVIL__srt__constant__ascii_character_byte_size,

    COMPILER__abt__define_type,
        COMPILER__pvn__context__input,
        COMPILER__ptt__dragon_buffer,
        ANVIL__srt__input_buffer_start,
        ANVIL__srt__input_buffer_end,

    COMPILER__abt__define_type,
        COMPILER__pvn__context__output__result,
        COMPILER__ptt__dragon_buffer,
        ANVIL__srt__output_result_buffer_start,
        ANVIL__srt__output_result_buffer_end,

    COMPILER__abt__define_type,
        COMPILER__pvn__context__output__error,
        COMPILER__ptt__dragon_buffer,
        ANVIL__srt__output_error_buffer_start,
        ANVIL__srt__output_error_buffer_end,
    
    COMPILER__abt__define_type,
        COMPILER__pvn__call_stack,
        COMPILER__ptt__dragon_current,
        ANVIL__srt__stack__start_address,
        ANVIL__srt__stack__current_address,
        
    COMPILER__abt__end_blueprint,
};

// all predefined function call names
char* COMPILER__global__predefined_function_call_names[] = {
    // stop
    COMPILER__define__master_namespace ".execution.stop",

    // set
    COMPILER__define__master_namespace ".set",

    // prints
    COMPILER__define__master_namespace ".print.character",

    // moving cells
    COMPILER__define__master_namespace ".copy",
    COMPILER__define__master_namespace ".pack",
    COMPILER__define__master_namespace ".pack.null",
    COMPILER__define__master_namespace ".pack.increment",

    // jumps
    COMPILER__define__master_namespace ".jump.top",
    COMPILER__define__master_namespace ".jump.bottom",
    COMPILER__define__master_namespace ".jump",

    // bit operations
    COMPILER__define__master_namespace ".bits.or",
    COMPILER__define__master_namespace ".bits.invert",
    COMPILER__define__master_namespace ".bits.and",
    COMPILER__define__master_namespace ".bits.xor",
    COMPILER__define__master_namespace ".bits.shift_higher",
    COMPILER__define__master_namespace ".bits.shift_lower",
    COMPILER__define__master_namespace ".bits.overwrite",

    // integer operations
    COMPILER__define__master_namespace ".integer.within_range",
    COMPILER__define__master_namespace ".integer.add",
    COMPILER__define__master_namespace ".integer.subtract",
    COMPILER__define__master_namespace ".integer.multiply",
    COMPILER__define__master_namespace ".integer.divide",
    COMPILER__define__master_namespace ".integer.modulous",

    // buffer operations
    COMPILER__define__master_namespace ".buffer.request",
    COMPILER__define__master_namespace ".buffer.return",
    COMPILER__define__master_namespace ".address_to_cell",
    COMPILER__define__master_namespace ".cell_to_address",
    COMPILER__define__master_namespace ".file_to_buffer",
    COMPILER__define__master_namespace ".buffer_to_file",
    COMPILER__define__master_namespace ".buffer.copy.low_to_high",
    COMPILER__define__master_namespace ".buffer.copy.high_to_low",
    COMPILER__define__master_namespace ".delete_file",
    COMPILER__define__master_namespace ".find_allocation",

    // structure operations
    COMPILER__define__master_namespace ".structure_to_buffer",
    COMPILER__define__master_namespace ".buffer_to_structure",
    COMPILER__define__master_namespace ".structure.byte_size",

    // compilation operations
    COMPILER__define__master_namespace ".compiler.compile",

    // context operations
    COMPILER__define__master_namespace ".context.install_program",
    COMPILER__define__master_namespace ".context.run",

    // time operations
    COMPILER__define__master_namespace ".time.now",
};
// predefined function call name type
typedef enum COMPILER__pfcnt {
    // execution
    COMPILER__pfcnt__execution__stop,
    
    // sets
    COMPILER__pfcnt__set,

    // prints
    COMPILER__pfcnt__print__character,

    // moving cells
    COMPILER__pfcnt__copy,
    COMPILER__pfcnt__pack,
    COMPILER__pfcnt__pack_null,
    COMPILER__pfcnt__pack_increment,

    // jumps
    COMPILER__pfcnt__jump__top,
    COMPILER__pfcnt__jump__bottom,
    COMPILER__pfcnt__jump,

    // bit operations
    COMPILER__pfcnt__bits__or,
    COMPILER__pfcnt__bits__invert,
    COMPILER__pfcnt__bits__and,
    COMPILER__pfcnt__bits__xor,
    COMPILER__pfcnt__bits__shift_higher,
    COMPILER__pfcnt__bits__shift_lower,
    COMPILER__pfcnt__bits__overwrite,

    // integer operations
    COMPILER__pfcnt__integer__within_range,
    COMPILER__pfcnt__integer__add,
    COMPILER__pfcnt__integer__subtract,
    COMPILER__pfcnt__integer__multiply,
    COMPILER__pfcnt__integer__divide,
    COMPILER__pfcnt__integer__modulous,

    // buffer operations
    COMPILER__pfcnt__buffer__request,
    COMPILER__pfcnt__buffer__return,
    COMPILER__pfcnt__buffer__address_to_cell,
    COMPILER__pfcnt__buffer__cell_to_address,
    COMPILER__pfcnt__buffer__file_to_buffer,
    COMPILER__pfcnt__buffer__buffer_to_file,
    COMPILER__pfcnt__buffer__copy__low_to_high,
    COMPILER__pfcnt__buffer__copy__high_to_low,
    COMPILER__pfcnt__buffer__delete_file,
    COMPILER__pfcnt__buffer__find_allocation,

    // structure operations
    COMPILER__pfcnt__structure__structure_to_buffer,
    COMPILER__pfcnt__structure__buffer_to_structure,
    COMPILER__pfcnt__structure__structure_byte_size,

    // compiler operations
    COMPILER__pfcnt__compiler__compile,

    // context options
    COMPILER__pfcnt__context__install_program,
    COMPILER__pfcnt__context__run,

    // time
    COMPILER__pfcnt__time__now,

    // COUNT
    COMPILER__pfcnt__COUNT,
} COMPILER__pfcnt;
// header blueprint for one off built in functions
COMPILER__blueprintling COMPILER__global__predefined_one_off_function_calls[] = {
    // stop program execution
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__execution__stop,
        COMPILER__pfcnt__execution__stop,
        0,
        0,
    
    // set cell
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__set__cell,
        COMPILER__pfcnt__set,
        1,
        COMPILER__aat__cell_value,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        
    // set string
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__set__string,
        COMPILER__pfcnt__set,
        1,
        COMPILER__aat__string_index,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,

    // jumps
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__jump__top,
        COMPILER__pfcnt__jump__top,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__offset,
        0,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__jump__bottom,
        COMPILER__pfcnt__jump__bottom,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__offset,
        0,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__jump__offset,
        COMPILER__pfcnt__jump,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__offset,
        0,
    
    // bits
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__bits_or,
        COMPILER__pfcnt__bits__or,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__bits_invert,
        COMPILER__pfcnt__bits__invert,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__bits_and,
        COMPILER__pfcnt__bits__and,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__bits_xor,
        COMPILER__pfcnt__bits__xor,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__bits_shift_higher,
        COMPILER__pfcnt__bits__shift_higher,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__bits_shift_lower,
        COMPILER__pfcnt__bits__shift_lower,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__bits_overwrite,
        COMPILER__pfcnt__bits__overwrite,
        3,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    
    // integers
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__check__integer_within_range,
        COMPILER__pfcnt__integer__within_range,
        3,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__integer__addition,
        COMPILER__pfcnt__integer__add,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__integer__subtraction,
        COMPILER__pfcnt__integer__subtract,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__integer__multiplication,
        COMPILER__pfcnt__integer__multiply,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__integer__division,
        COMPILER__pfcnt__integer__divide,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__integer__modulous,
        COMPILER__pfcnt__integer__modulous,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    
    // buffers
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__buffer__request,
        COMPILER__pfcnt__buffer__request,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__buffer__return,
        COMPILER__pfcnt__buffer__return,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        0,

    // movers
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__mover__address_to_cell,
        COMPILER__pfcnt__buffer__address_to_cell,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__mover__cell_to_address,
        COMPILER__pfcnt__buffer__cell_to_address,
        3,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        0,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__mover__file_to_buffer,
        COMPILER__pfcnt__buffer__file_to_buffer,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__mover__buffer_to_file,
        COMPILER__pfcnt__buffer__buffer_to_file,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        0,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__mover__buffer_copy__low_to_high,
        COMPILER__pfcnt__buffer__copy__low_to_high,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        0,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__mover__buffer_copy__high_to_low,
        COMPILER__pfcnt__buffer__copy__high_to_low,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        0,
    
    // file deleters
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__delete_file,
        COMPILER__pfcnt__buffer__delete_file,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        0,

    // allocations
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__allocation__find,
        COMPILER__pfcnt__buffer__find_allocation,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
    
    // compiler assorted calls
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__compilation__compile,
        COMPILER__pfcnt__compiler__compile,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_compiler__settings,
        3,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_compiler__debug_information,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_compiler__error,
        
    // context assorted calls
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__context__install_program,
        COMPILER__pfcnt__context__install_program,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        0,
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__context__run,
        COMPILER__pfcnt__context__run,
        2,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_buffer,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_cell,
        0,
    
    // time calls
    COMPILER__abt__define_function_call,
        COMPILER__ast__predefined__time__get_current_time,
        COMPILER__pfcnt__time__now,
        0,
        1,
        COMPILER__aat__COUNT + COMPILER__ptt__dragon_time,
    
    // end of blueprint
    COMPILER__abt__end_blueprint,
};

// cell ranges
typedef struct COMPILER__cell_range {
    ANVIL__cell_ID start;
    ANVIL__cell_ID end;
} COMPILER__cell_range;

// create custom cell range
COMPILER__cell_range COMPILER__create__cell_range(ANVIL__cell_ID start, ANVIL__cell_ID end) {
    COMPILER__cell_range output;

    // setup output
    output.start = start;
    output.end = end;

    return output;
}

// create null cell range
COMPILER__cell_range COMPILER__create_null__cell_range() {
    return COMPILER__create__cell_range(0, 0);
}

/* Blueprints */
// advance blueprintling by one
void COMPILER__next__blueprintling(BASIC__current* current) {
    // next blueprintling
    (*current).start += sizeof(COMPILER__blueprintling);

    return;
}

// read one blueprintling but do not advance current
COMPILER__blueprintling COMPILER__read__blueprintling(BASIC__current* current) {
    return *(COMPILER__blueprintling*)(*current).start;
}

// read one blueprintling and advance current to next one
COMPILER__blueprintling COMPILER__read_and_next__blueprintling(BASIC__current* current) {
    COMPILER__blueprintling output;

    // read output
    output = COMPILER__read__blueprintling(current);

    // advance current
    COMPILER__next__blueprintling(current);

    return output;
}

/* Translation */
// calculate the amount of digits in a decimal number
BASIC__length COMPILER__calculate__decimals_digit_count_in_number(BASIC__u64 number) {
    BASIC__length output = 0;

    // check for zero
    if (number == 0) {
        return 1;
    }

    // while there are digits left
    while (number > 0) {
        // divide by ten
        number /= 10;

        // increment digit count
        output++;
    }

    return output;
}

// create buffer string from number
BASIC__buffer COMPILER__translate__integer_value_to_string(BASIC__u64 number) {
    BASIC__buffer output;
    BASIC__length digit_count;

    // count digits
    digit_count = COMPILER__calculate__decimals_digit_count_in_number(number);

    // allocate output
    output = BASIC__open__buffer(sizeof(BASIC__character) * digit_count);

    // if allocation succeded
    if (BASIC__check__empty_buffer(output) == BASIC__bt__false) {
        // calculate characters
        for (BASIC__u64 i = digit_count; i > 0; i--) {
            // create character
            ((BASIC__character*)output.start)[i - 1] = '0' + (number % 10);

            // next character
            number /= 10;
        }
    }

    return output;
}

/* Character Locations */
// parsing character location
typedef struct COMPILER__character_location {
    BASIC__file_index file_index;
    BASIC__line_number line_number;
    BASIC__character_index character_index;
} COMPILER__character_location;

// create custom character marker location
COMPILER__character_location COMPILER__create__character_location(BASIC__file_index file_index, BASIC__line_number line_number, BASIC__character_index character_index) {
    COMPILER__character_location output;

    output.file_index = file_index;
    output.line_number = line_number;
    output.character_index = character_index;

    return output;
}

// create null character location
COMPILER__character_location COMPILER__create_null__character_location() {
    return COMPILER__create__character_location(-1, -1, -1);
}

/* Errors */
// error information
typedef struct COMPILER__error {
    BASIC__bt occured;
    BASIC__buffer message;
    COMPILER__character_location location;
    BASIC__bt memory_error_occured;
} COMPILER__error;

// create custom error
COMPILER__error COMPILER__create__error(BASIC__bt occured, BASIC__buffer message, COMPILER__character_location location, BASIC__bt memory_error_occured) {
    COMPILER__error output;

    // setup output
    output.occured = occured;
    output.message = message;
    output.location = location;
    output.memory_error_occured = memory_error_occured;

    return output;
}

// create null error
COMPILER__error COMPILER__create_null__error() {
    // return empty
    return COMPILER__create__error(BASIC__bt__false, BASIC__create_null__buffer(), COMPILER__create_null__character_location(), BASIC__bt__false);
}

// open a specific error
COMPILER__error COMPILER__open__error(const char* message, COMPILER__character_location location) {
    return COMPILER__create__error(BASIC__bt__true, BASIC__open__buffer_from_string((u8*)message, BASIC__bt__true, BASIC__bt__false), location, BASIC__bt__false);
}

// open a generic memory allocation error
COMPILER__error COMPILER__open__internal_memory_error() {
    return COMPILER__create__error(BASIC__bt__true, BASIC__open__buffer_from_string((u8*)"Internal memory error.", BASIC__bt__true, BASIC__bt__false), COMPILER__create_null__character_location(), BASIC__bt__true);
}

// create an error report in json
BASIC__buffer COMPILER__serialize__error_json(COMPILER__error error, BASIC__bt* error_occured) {
    BASIC__buffer output;
    BASIC__list json;
    BASIC__buffer temp_buffer;

    // initialize json string builder
    json = BASIC__open__list(sizeof(BASIC__character) * 2048, error_occured);

    // generate json
    BASIC__list__append__buffer_data(&json, BASIC__open__buffer_from_string((u8*)"\"error\": {\n\t\"message\": \"", BASIC__bt__false, BASIC__bt__false), error_occured);
    BASIC__list__append__buffer_data(&json, error.message, error_occured);
    BASIC__list__append__buffer_data(&json, BASIC__open__buffer_from_string((u8*)"\",\n\t\"file_index\": \"", BASIC__bt__false, BASIC__bt__false), error_occured);
    temp_buffer = COMPILER__translate__integer_value_to_string(error.location.file_index);
    BASIC__list__append__buffer_data(&json, temp_buffer, error_occured);
    BASIC__close__buffer(temp_buffer);
    BASIC__list__append__buffer_data(&json, BASIC__open__buffer_from_string((u8*)"\",\n\t\"line_number\": \"", BASIC__bt__false, BASIC__bt__false), error_occured);
    temp_buffer = COMPILER__translate__integer_value_to_string(error.location.line_number);
    BASIC__list__append__buffer_data(&json, temp_buffer, error_occured);
    BASIC__close__buffer(temp_buffer);
    BASIC__list__append__buffer_data(&json, BASIC__open__buffer_from_string((u8*)"\",\n\t\"character_index\": \"", BASIC__bt__false, BASIC__bt__false), error_occured);
    temp_buffer = COMPILER__translate__integer_value_to_string(error.location.character_index);
    BASIC__list__append__buffer_data(&json, temp_buffer, error_occured);
    BASIC__close__buffer(temp_buffer);
    BASIC__list__append__buffer_data(&json, BASIC__open__buffer_from_string((u8*)"\"\n}\n", BASIC__bt__false, BASIC__bt__false), error_occured);

    // create buffer from list
    output = BASIC__list__open_buffer_from_list(&json, error_occured);

    // clean up list
    BASIC__close__list(json);

    return output;
}

// check to see if an error occured
BASIC__bt COMPILER__check__error_occured(COMPILER__error* error) {
    // check for memory error
    if ((*error).memory_error_occured == BASIC__bt__true) {
        // set error
        *error = COMPILER__open__internal_memory_error();
    }

    return (*error).occured;
}

// close an error
void COMPILER__close__error(COMPILER__error error) {
    // clean up buffers
    BASIC__close__buffer(error.message);

    return;
}

/* List Functions With Errors */
// compiler general list appending macro
#define COMPILER__append(list, type, data, error) BASIC__list__append(list, type, data, &((*error).memory_error_occured))

// open a list but the error is a compiler error
BASIC__list COMPILER__open__list_with_error(BASIC__list_increase list_increase, COMPILER__error* error) {
    // open with error
    return BASIC__open__list(list_increase, &((*error).memory_error_occured));
}

/* Counted List Functions With Errors */
BASIC__counted_list COMPILER__open__counted_list_with_error(BASIC__list_increase increase, COMPILER__error* error) {
    return BASIC__create__counted_list(COMPILER__open__list_with_error(increase, error), 0);
}

/* Conversion */
// translate string to boolean
BASIC__bt COMPILER__translate__string_to_boolean(BASIC__buffer string, ANVIL__cell_integer_value* value) {
    // check possible values
    if (BASIC__calculate__buffer_contents_equal(string, BASIC__open__buffer_from_string((u8*)COMPILER__define__master_namespace ".boolean.false", BASIC__bt__false, BASIC__bt__false)) == BASIC__bt__true) {
        *value = (ANVIL__cell_integer_value)(BASIC__bt__false);

        return BASIC__bt__true;
    }
    if (BASIC__calculate__buffer_contents_equal(string, BASIC__open__buffer_from_string((u8*)COMPILER__define__master_namespace ".boolean.true", BASIC__bt__false, BASIC__bt__false)) == BASIC__bt__true) {
        *value = (ANVIL__cell_integer_value)(BASIC__bt__true);

        return BASIC__bt__true;
    }

    return BASIC__bt__false;
}

// translate string to binary
BASIC__bt COMPILER__translate__string_to_binary(BASIC__buffer string, ANVIL__cell_integer_value* value) {
    BASIC__buffer prefix = BASIC__open__buffer_from_string((u8*)COMPILER__define__master_namespace ".binary.", BASIC__bt__false, BASIC__bt__false);
    BASIC__buffer current;
    BASIC__u64 character_count_limit = sizeof(BASIC__u64) * BASIC__define__bits_in_byte;
    BASIC__u64 character_count = 0;

    // check for prefix
    if (BASIC__calculate__buffer_starts_with_buffer(string, prefix) == BASIC__bt__false) {
        // not a binary literal
        return BASIC__bt__false;
    }

    // setup current
    current = BASIC__create__buffer(string.start + BASIC__calculate__buffer_length(prefix), string.end);

    // pre check for all valid characters
    while (BASIC__check__current_within_range(current)) {
        // check character
        if ((BASIC__check__character_range_at_current(current, '0', '1') || BASIC__check__character_range_at_current(current, '_', '_')) == BASIC__bt__false) {
            // not a binary literal
            return BASIC__bt__false;
        }

        // count binary character
        if (BASIC__check__character_range_at_current(current, '0', '1') == BASIC__bt__true) {
            character_count++;
        }

        // advance current
        current.start += sizeof(BASIC__character);
    }

    // check for same character limit
    if (character_count > character_count_limit) {
        // binary literal to large, conversion failed
        return BASIC__bt__false;
    }

    // setup bit index
    BASIC__bit_count bit_index = 0;

    // reset current
    current = BASIC__create__buffer(string.start + BASIC__calculate__buffer_length(prefix), string.end);

    // convert binary string to binary number
    while (BASIC__check__current_within_range(current)) {
        if (BASIC__check__character_range_at_current(current, '_', '_') == BASIC__bt__false) {
            // append value
            *value += (((*(BASIC__character*)current.start) - '0') << bit_index);

            // next bit index
            bit_index++;
        }

        // next character
        current.start += sizeof(BASIC__character);
    }

    return BASIC__bt__true;
}

// translate string to integer
BASIC__bt COMPILER__translate__string_to_integer(BASIC__buffer string, ANVIL__cell_integer_value* value) {
    BASIC__buffer prefix = BASIC__open__buffer_from_string((u8*)COMPILER__define__master_namespace ".integer.", BASIC__bt__false, BASIC__bt__false);
    BASIC__buffer suffix;
    BASIC__u64 digit = 0;

    // check for prefix
    if (BASIC__calculate__buffer_starts_with_buffer(string, prefix) == BASIC__bt__false) {
        // not an integer literal
        return BASIC__bt__false;
    }

    // create suffix
    suffix = BASIC__create__buffer(string.start + BASIC__calculate__buffer_length(prefix), string.end);

    // translate number
    // if number is negative
    if (*(BASIC__character*)suffix.start == (BASIC__character)'n') {
        // for each character
        for (BASIC__character_index i = BASIC__calculate__buffer_length(suffix); i > 1; i--) {
            // check for valid character
            if (((((BASIC__character*)suffix.start)[i - 1] >= '0' && ((BASIC__character*)suffix.start)[i - 1] <= '9') || ((BASIC__character*)suffix.start)[i - 1] == '_') == BASIC__bt__false) {
                // invalid character
                return BASIC__bt__false;
            }

            // if calculable character
            if (((BASIC__character*)suffix.start)[i - 1] != '_') {
                // add value
                *value += BASIC__calculate__exponent(10, digit) * (((BASIC__character*)suffix.start)[i - 1] - '0');

                // next digit power
                digit++;
            }
        }

        // turn number negative using twos compliment
        *value = ~(*value);
        *value = (*value) + 1;
    // if number is positive
    } else {
        // for each character
        for (BASIC__character_index i = BASIC__calculate__buffer_length(suffix); i > 0; i--) {
            // check for valid character
            if (((((BASIC__character*)suffix.start)[i - 1] >= '0' && ((BASIC__character*)suffix.start)[i - 1] <= '9') || ((BASIC__character*)suffix.start)[i - 1] == '_') == BASIC__bt__false) {
                // invalid character
                return BASIC__bt__false;
            }

            // if calculable character
            if (((BASIC__character*)suffix.start)[i - 1] != '_') {
                // add value
                *value += BASIC__calculate__exponent(10, digit) * (((BASIC__character*)suffix.start)[i - 1] - '0');

                // next digit power
                digit++;
            }
        }
    }

    return BASIC__bt__true;
}

// translate character to hexadecimal
ANVIL__cell_integer_value COMPILER__translate__character_to_hexadecimal(BASIC__character character, BASIC__bt* invalid_character) {
    // set character as valid
    *invalid_character = BASIC__bt__false;

    // translate character
    if (character >= '0' && character <= '9') {
        return character - '0';
    } else if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    } else if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    } else {
        // invalid character
        *invalid_character = BASIC__bt__true;
    }

    return BASIC__define__null_address;
}

// translate string to hexedecimal
BASIC__bt COMPILER__translate__string_to_hexadecimal(BASIC__buffer string, ANVIL__cell_integer_value* value) {
    BASIC__buffer prefix = BASIC__open__buffer_from_string((u8*)COMPILER__define__master_namespace ".hexadecimal.", BASIC__bt__false, BASIC__bt__false);
    BASIC__buffer suffix;
    BASIC__buffer current;
    BASIC__bt invalid_character;
    ANVIL__cell_integer_value hex_digit;

    // check for prefix
    if (BASIC__calculate__buffer_starts_with_buffer(string, prefix) == BASIC__bt__false) {
        // not a hexadecimal literal
        return BASIC__bt__false;
    }

    // create suffix
    suffix = BASIC__create__buffer(string.start + BASIC__calculate__buffer_length(prefix), string.end);

    // create current
    current = suffix;

    // validate number
    while (BASIC__check__current_within_range(current)) {
        // check separator
        if (*(BASIC__character*)current.start == '_') {
            // skip
            current.start += sizeof(BASIC__character);

            continue;
        }

        // check digit
        hex_digit = COMPILER__translate__character_to_hexadecimal(*(BASIC__character*)current.start, &invalid_character);
        if (invalid_character == BASIC__bt__true) {
            // invalid digit, invalid hex string
            return BASIC__bt__false;
        }

        // next character
        current.start += sizeof(BASIC__character);
    }

    // create current
    current = suffix;

    // translate number
    while (BASIC__check__current_within_range(current)) {
        // check separator
        if (*(BASIC__character*)current.start == '_') {
            // skip
            current.start += sizeof(BASIC__character);

            continue;
        }

        // check digit
        hex_digit = COMPILER__translate__character_to_hexadecimal(*(BASIC__character*)current.start, &invalid_character);

        // append digit
        *value = (*value) << 4;
        *value = (*value) & (~15);
        *value = (*value) + hex_digit;

        // next character
        current.start += sizeof(BASIC__character);
    }

    return BASIC__bt__true;
}

BASIC__buffer COMPILER__convert__general_argument_type_to_string_buffer(COMPILER__pat argument_type) {
    return BASIC__open__buffer_from_string((u8*)(COMPILER__global__general_argument_type_names[argument_type]), BASIC__bt__false, BASIC__bt__false);
}

#endif
