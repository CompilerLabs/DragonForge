#ifndef DRAGON__compiler__parser
#define DRAGON__compiler__parser

/* Include */
#include "CompilerSpecifications.h"
#include "Lexer.h"

/* Parser */
// namespace struct
typedef struct COMPILER__namespace {
    ANVIL__counted_list lexlings; // COMPILER__lexling
} COMPILER__namespace;

// create name
COMPILER__namespace COMPILER__create__namespace(ANVIL__counted_list lexlings) {
    COMPILER__namespace output;

    output.lexlings = lexlings;

    return output;
}

// create null namespace
COMPILER__namespace COMPILER__create_null__namespace() {
    return COMPILER__create__namespace(ANVIL__create_null__counted_list());
}

// open namespace from string
COMPILER__namespace COMPILER__open__namespace_from_single_lexling(COMPILER__lexling lexling, COMPILER__error* error) {
    COMPILER__namespace output = COMPILER__create_null__namespace();

    // open lexlings list
    output.lexlings = COMPILER__open__counted_list_with_error(sizeof(COMPILER__lexling), error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // append lexling
    COMPILER__append(&output.lexlings.list, COMPILER__lexling, lexling, error);
    output.lexlings.count++;
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    return output;
}

// check if namespaces are the same
ANVIL__bt COMPILER__check__identical_namespaces(COMPILER__namespace a, COMPILER__namespace b) {
    ANVIL__current current_namespace_a = ANVIL__calculate__current_from_list_filled_index(&a.lexlings.list);
    ANVIL__current current_namespace_b = ANVIL__calculate__current_from_list_filled_index(&b.lexlings.list);

    // check each count
    if (a.lexlings.count != b.lexlings.count) {
        // not equivalent
        return ANVIL__bt__false;
    }

    // check for discrepencies
    while (ANVIL__check__current_within_range(current_namespace_a) && ANVIL__check__current_within_range(current_namespace_b)) {
        // check name equivalency
        if (ANVIL__calculate__buffer_contents_equal((*(COMPILER__lexling*)current_namespace_a.start).value, (*(COMPILER__lexling*)current_namespace_b.start).value) == ANVIL__bt__false) {
            // not equivalent
            return ANVIL__bt__false;
        }

        // next name
        current_namespace_a.start += sizeof(COMPILER__lexling);
        current_namespace_b.start += sizeof(COMPILER__lexling);
    }

    // no discrepencies, equivalent
    return ANVIL__bt__true;
}

// check if namespace is null
ANVIL__bt COMPILER__check__empty_namespace(COMPILER__namespace name) {
    return name.lexlings.count == 0;
}

// get lexling location from namespace
COMPILER__character_location COMPILER__get__namespace_lexling_location(COMPILER__namespace name) {
    return ((COMPILER__lexling*)name.lexlings.list.buffer.start)[0].location;
}

// check single lexling name for equivalence
ANVIL__bt COMPILER__check__namespace_against_c_string(const char* string, COMPILER__namespace checking) {
    COMPILER__namespace temp_name;
    COMPILER__lexling temp_lexling;

    // setup temps
    temp_lexling = COMPILER__open__lexling_from_string(string, COMPILER__lt__name, COMPILER__create_null__character_location());
    temp_name.lexlings.count = 1;
    temp_name.lexlings.list = ANVIL__create__list(ANVIL__create__buffer((ANVIL__address)&temp_lexling, (ANVIL__address)(&temp_lexling) + sizeof(temp_lexling) - 1), sizeof(temp_lexling) - 1, sizeof(temp_lexling));

    // return comparison
    return COMPILER__check__identical_namespaces(temp_name, checking);
}

// parsling argument
typedef struct COMPILER__parsling_argument {
    // definition
    COMPILER__pat category;
    COMPILER__namespace name;

    // optional
    COMPILER__namespace type;
    COMPILER__namespace tag;
} COMPILER__parsling_argument;

// create a custom argument
COMPILER__parsling_argument COMPILER__create__parsling_argument(COMPILER__pat category, COMPILER__namespace name, COMPILER__namespace type, COMPILER__namespace tag) {
    COMPILER__parsling_argument output;

    // setup output
    output.category = category;
    output.name = name;
    output.type = type;
    output.tag = tag;

    return output;
}

// get a parsling argument from a counted list by index
COMPILER__parsling_argument COMPILER__get__parsling_argument_by_index(ANVIL__counted_list arguments, COMPILER__argument_index index) {
    // return find
    return ((COMPILER__parsling_argument*)arguments.list.buffer.start)[index];
}

// setup null argument
COMPILER__parsling_argument COMPILER__create_null__parsling_argument() {
    return COMPILER__create__parsling_argument(COMPILER__pat__invalid, COMPILER__create_null__namespace(), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
}

// one scope
typedef struct COMPILER__parsling_scope {
    ANVIL__counted_list statements; // COMPILER__parsling_statement
} COMPILER__parsling_scope;

// create custom scope
COMPILER__parsling_scope COMPILER__create__parsling_scope(ANVIL__counted_list statements) {
    COMPILER__parsling_scope output;

    // setup output
    output.statements = statements;

    return output;
}

// create null scope
COMPILER__parsling_scope COMPILER__create_null__parsling_scope() {
    // return empty
    return COMPILER__create__parsling_scope(ANVIL__create_null__counted_list());
}

// one statement
typedef struct COMPILER__parsling_statement {
    // statement type
    COMPILER__stt type;

    // name
    COMPILER__parsling_argument name; // offset name, subscope name & function call name

    // function call data
    ANVIL__counted_list inputs; // COMPILER__parsling_argument
    ANVIL__counted_list outputs; // COMPILER__parsling_argument

    // subscope data
    COMPILER__parsling_argument subscope_flag_name;
    COMPILER__parsling_scope subscope;
} COMPILER__parsling_statement;

// create a custom statement
COMPILER__parsling_statement COMPILER__create__parsling_statement(COMPILER__stt type, COMPILER__parsling_argument name, ANVIL__counted_list inputs, ANVIL__counted_list outputs, COMPILER__parsling_argument subscope_flag_name, COMPILER__parsling_scope subscope) {
    COMPILER__parsling_statement output;

    // setup output
    output.type = type;
    output.name = name;
    output.inputs = inputs;
    output.outputs = outputs;
    output.subscope_flag_name = subscope_flag_name;
    output.subscope = subscope;

    return output;
}

// create a null statement
COMPILER__parsling_statement COMPILER__create_null__parsling_statement() {
    // return empty
    return COMPILER__create__parsling_statement(COMPILER__stt__invalid, COMPILER__create_null__parsling_argument(), ANVIL__create_null__counted_list(), ANVIL__create_null__counted_list(), COMPILER__create_null__parsling_argument(), COMPILER__create_null__parsling_scope());
}

// one structure
typedef struct COMPILER__parsling_structure {
    ANVIL__counted_list type_names; // COMPILER__parsling_argument
    ANVIL__counted_list arguments; // COMPILER__parsling_argument
} COMPILER__parsling_structure;

// create a custom structure
COMPILER__parsling_structure COMPILER__create__parsling_structure(ANVIL__counted_list type_names, ANVIL__counted_list arguments) {
    COMPILER__parsling_structure output;

    // setup output
    output.type_names = type_names;
    output.arguments = arguments;

    return output;
}

// create a null structure
COMPILER__parsling_structure COMPILER__create_null__parsling_structure() {
    // return empty
    return COMPILER__create__parsling_structure(ANVIL__create_null__counted_list(), ANVIL__create_null__counted_list());
}

// one alias
typedef struct COMPILER__parsling_alias {
    COMPILER__parsling_argument new_name;
    COMPILER__parsling_argument old_name;
} COMPILER__parsling_alias;

// create a custom alias
COMPILER__parsling_alias COMPILER__create__parsling_alias(COMPILER__parsling_argument new_name, COMPILER__parsling_argument old_name) {
    COMPILER__parsling_alias output;

    // setup output
    output.new_name = new_name;
    output.old_name = old_name;

    return output;
}

// create a null alias
COMPILER__parsling_alias COMPILER__create_null__parsling_alias() {
    // return empty
    return COMPILER__create__parsling_alias(COMPILER__create_null__parsling_argument(), COMPILER__create_null__parsling_argument());
}

// one function
typedef struct COMPILER__parsling_function {
    COMPILER__parsling_statement header;
    COMPILER__parsling_statement scope;
} COMPILER__parsling_function;

// create a custom function
COMPILER__parsling_function COMPILER__create__parsling_function(COMPILER__parsling_statement header, COMPILER__parsling_statement scope) {
    COMPILER__parsling_function output;

    // setup output
    output.header = header;
    output.scope = scope;

    return output;
}

// create a null function
COMPILER__parsling_function COMPILER__create_null__parsling_function() {
    // return empty
    return COMPILER__create__parsling_function(COMPILER__create_null__parsling_statement(), COMPILER__create_null__parsling_statement());
}

// one program
typedef struct COMPILER__parsling_program {
    ANVIL__counted_list functions; // COMPILER__parsling_function
    ANVIL__counted_list structures; // COMPILER__parsling_structure
    ANVIL__counted_list aliases; // COMPILER__parsling_alias
} COMPILER__parsling_program;

// create a custom program
COMPILER__parsling_program COMPILER__create__parsling_program(ANVIL__counted_list functions, ANVIL__counted_list structures, ANVIL__counted_list aliases) {
    COMPILER__parsling_program output;

    // setup output
    output.functions = functions;
    output.structures = structures;
    output.aliases = aliases;

    return output;
}

// create a null program
COMPILER__parsling_program COMPILER__create_null__parsling_program() {
    // return empty
    return COMPILER__create__parsling_program(ANVIL__create_null__counted_list(), ANVIL__create_null__counted_list(), ANVIL__create_null__counted_list());
}

// close namespace
void COMPILER__close__parsling_namespace(COMPILER__namespace name) {
    // close list
    ANVIL__close__counted_list(name.lexlings);

    return;
}

// close argument
void COMPILER__close__parsling_argument(COMPILER__parsling_argument argument) {
    // close namespaces
    COMPILER__close__parsling_namespace(argument.name);
    if (ANVIL__check__empty_list(argument.type.lexlings.list) == ANVIL__bt__false) {
        COMPILER__close__parsling_namespace(argument.type);
    }
    if (ANVIL__check__empty_list(argument.tag.lexlings.list) == ANVIL__bt__false) {
        COMPILER__close__parsling_namespace(argument.tag);
    }

    return;
}

// close arguments
void COMPILER__close__parsling_arguments(ANVIL__counted_list* arguments) {
    ANVIL__current current = ANVIL__calculate__current_from_list_filled_index(&(*arguments).list);

    // close arguments
    while (ANVIL__check__current_within_range(current)) {
        // close argument
        COMPILER__close__parsling_argument(*(COMPILER__parsling_argument*)current.start);

        // next argument
        current.start += sizeof(COMPILER__parsling_argument);
    }

    // close arguments buffer
    ANVIL__close__counted_list(*arguments);

    return;
}

// forward declaration
void COMPILER__close__parsling_statements(ANVIL__counted_list*);

// close scope
void COMPILER__close__parsling_scope(COMPILER__parsling_scope scope) {
    // close statements
    if (ANVIL__check__empty_list(scope.statements.list) == ANVIL__bt__false) {
        // close statements
        COMPILER__close__parsling_statements(&scope.statements);
    }

    return;
}

// close statement
void COMPILER__close__parsling_statement(COMPILER__parsling_statement statement) {
    // close name
    COMPILER__close__parsling_argument(statement.name);

    // close io
    if (ANVIL__check__empty_list(statement.inputs.list) == ANVIL__bt__false) {
        COMPILER__close__parsling_arguments(&statement.inputs);
    }
    if (ANVIL__check__empty_list(statement.outputs.list) == ANVIL__bt__false) {
        COMPILER__close__parsling_arguments(&statement.outputs);
    }

    // close scope
    COMPILER__close__parsling_scope(statement.subscope);
    COMPILER__close__parsling_argument(statement.subscope_flag_name);

    return;
}

// close statements
void COMPILER__close__parsling_statements(ANVIL__counted_list* list) {
    ANVIL__current current_statement = ANVIL__calculate__current_from_list_filled_index(&(*list).list);

    // clean up each statement
    while (ANVIL__check__current_within_range(current_statement)) {
        // clean up statement
        COMPILER__close__parsling_statement(*(COMPILER__parsling_statement*)current_statement.start);

        // next statement
        current_statement.start += sizeof(COMPILER__parsling_statement);
    }

    // clean up statements buffer
    ANVIL__close__counted_list(*list);

    return;
}

// close structure
void COMPILER__close__parsling_structure(COMPILER__parsling_structure structure) {
    // close lists
    COMPILER__close__parsling_arguments(&structure.type_names);
    COMPILER__close__parsling_arguments(&structure.arguments);

    return;
}

// close function
void COMPILER__close__parsling_function(COMPILER__parsling_function function) {
    // close header
    COMPILER__close__parsling_statement(function.header);
    
    // close scope
    COMPILER__close__parsling_statement(function.scope);

    return;
}

// close parsling program
void COMPILER__close__parsling_program(COMPILER__parsling_program program) {
    ANVIL__current current = ANVIL__calculate__current_from_list_filled_index(&program.functions.list);

    // free each function
    while (ANVIL__check__current_within_range(current)) {
        // free function
        COMPILER__close__parsling_function(*(COMPILER__parsling_function*)current.start);

        // next function
        current.start += sizeof(COMPILER__parsling_function);
    }
    
    // next current
    current = ANVIL__calculate__current_from_list_filled_index(&program.structures.list);

    // free each structure
    while (ANVIL__check__current_within_range(current)) {
        // free structure
        COMPILER__close__parsling_structure(*(COMPILER__parsling_structure*)current.start);

        // next structure
        current.start += sizeof(COMPILER__parsling_structure);
    }

    // free the lists
    ANVIL__close__counted_list(program.functions);
    ANVIL__close__counted_list(program.structures);

    return;
}

// advance the current buffer by N number of lexlings
void COMPILER__advance__lexling_current(ANVIL__current* current, COMPILER__lexling_index lexling_count) {
    // advance the pointer
    (*current).start += sizeof(COMPILER__lexling) * lexling_count;

    return;
}

// revert lexling current by N number of lexlings
void COMPILER__revert__lexling_current(ANVIL__current* current, COMPILER__lexling_index lexling_count) {
    (*current).start -= sizeof(COMPILER__lexling) * lexling_count;

    return;
}

// parse a namepspace with only one name
COMPILER__namespace COMPILER__parse__namespace__one_name_only(ANVIL__current* current, COMPILER__error* error) {
    COMPILER__namespace output;

    // open lexling list
    output.lexlings = COMPILER__open__counted_list_with_error(sizeof(COMPILER__lexling) * 8, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // check for name
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
        // append lexling
        COMPILER__append(&output.lexlings.list, COMPILER__lexling, COMPILER__read__lexling_from_current(*current), error);
        if (COMPILER__check__error_occured(error)) {
            return output;
        }

        // one lexling
        output.lexlings.count++;

        // next lexling
        COMPILER__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *error = COMPILER__open__error("Parsing Error: Expected a name.", COMPILER__read__lexling_from_current(*current).location);
    }

    return output;
}

// parse namespace
COMPILER__namespace COMPILER__parse__namespace(ANVIL__current* current, COMPILER__error* error) {
    COMPILER__namespace output;

    // open lexling list
    output.lexlings = COMPILER__open__counted_list_with_error(sizeof(COMPILER__lexling) * 8, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // check for initial name
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
        // append lexling
        COMPILER__append(&output.lexlings.list, COMPILER__lexling, COMPILER__read__lexling_from_current(*current), error);
        if (COMPILER__check__error_occured(error)) {
            return output;
        }

        // count lexling
        output.lexlings.count++;

        // next lexling
        COMPILER__advance__lexling_current(current, 1);

        // get other lexlings
        while (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__colon) {
            // skip past colon
            COMPILER__advance__lexling_current(current, 1);

            // get name
            if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
                // append name
                COMPILER__append(&output.lexlings.list, COMPILER__lexling, COMPILER__read__lexling_from_current(*current), error);
                if (COMPILER__check__error_occured(error)) {
                    return output;
                }

                // count lexling
                output.lexlings.count++;

                // next lexling
                COMPILER__advance__lexling_current(current, 1);
            // name missing, error
            } else {
                // set error
                *error = COMPILER__open__error("Parsing Error: Expected a name.", COMPILER__read__lexling_from_current(*current).location);

                return output;
            }
        }
    // if it is a string
    } else if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__string_literal) {
        // append string
        COMPILER__append(&output.lexlings.list, COMPILER__lexling, COMPILER__read__lexling_from_current(*current), error);
        if (COMPILER__check__error_occured(error)) {
            return output;
        }

        // next lexling
        COMPILER__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *error = COMPILER__open__error("Parsing Error: Expected a name.", COMPILER__read__lexling_from_current(*current).location);
    }

    return output;
}

// parse one argument
COMPILER__parsling_argument COMPILER__parse__statement_argument(ANVIL__current* current, COMPILER__palt argument_location, COMPILER__error* error) {
    COMPILER__parsling_argument output = COMPILER__create_null__parsling_argument();

    // check type
    // is variable / literal
    if (COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
        // parse name
        if (argument_location == COMPILER__palt__function_call) {
            // parse namespace for call argument
            output = COMPILER__create__parsling_argument(COMPILER__pat__name, COMPILER__parse__namespace(current, error), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
        } else {
            // parse namespace for header
            output = COMPILER__create__parsling_argument(COMPILER__pat__name, COMPILER__parse__namespace__one_name_only(current, error), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
        }
        
        // check for type
        // is type definition
        if (COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__exclamation_point) {
            // next lexling
            COMPILER__advance__lexling_current(current, 1);

            // parse type
            output.type = COMPILER__parse__namespace__one_name_only(current, error);
        } else if (argument_location == COMPILER__palt__function_declaration) {
            // set error
            *error = COMPILER__open__error("Parsing Error: Missing type in function header argument.", COMPILER__read__lexling_from_current(*current).location);

            return output;
        } else {
            // setup blank type
            output.type = COMPILER__create_null__namespace();
        }

        // check for tag
        // check for hashtag
        if (COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__hashtag) {
            // advance current
            COMPILER__advance__lexling_current(current, 1);

            // check for name
            // found tag name
            if (COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
                // add tag
                output.tag = COMPILER__parse__namespace__one_name_only(current, error);
                if (COMPILER__check__error_occured(error)) {
                    return output;
                }
            // error
            } else {
                // set error
                *error = COMPILER__open__error("Parsing Error: Missing tag name after tag marker '#'.", COMPILER__read__lexling_from_current(*current).location);

                return output;
            }
        }
    // offset
    } else if (COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__at) {
        // advance current past at
        COMPILER__advance__lexling_current(current, 1);

        // if correct type
        if (COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
            // get name
            output = COMPILER__create__parsling_argument(COMPILER__pat__offset, COMPILER__parse__namespace(current, error), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
        // error
        } else {
            *error = COMPILER__open__error("Parse Error: Offset is missing name.", COMPILER__read__lexling_from_current(*current).location);

            return output;
        }
    // string literal
    } else if (COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__string_literal) {
        // get argument
        output = COMPILER__create__parsling_argument(COMPILER__pat__string_literal, COMPILER__parse__namespace(current, error), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
    // error
    } else {
        *error = COMPILER__open__error("Parse Error: Unrecognized argument type.", COMPILER__read__lexling_from_current(*current).location);

        return output;
    }

    // check argument for variable only
    if (argument_location == COMPILER__palt__function_declaration && output.category != COMPILER__pat__name) {
        // set error
        *error = COMPILER__open__error("Parse Error: A non-variable / non-type argument was detected in a header.", COMPILER__read__lexling_from_current(*current).location);

        return output;
    }

    return output;
}

// parse statement arguments (includes structure declarations, function calls and function declarations)
ANVIL__counted_list COMPILER__parse__statement_arguments(ANVIL__current* current, COMPILER__palt argument_location, COMPILER__error* error) {
    ANVIL__counted_list output;

    // open names list
    output = COMPILER__open__counted_list_with_error(sizeof(COMPILER__parsling_argument) * 8, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    
    // check for opening parenthesis
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__left_parenthesis) {
        // next lexling
        COMPILER__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *error = COMPILER__open__error("Parse Error: Arguments is missing opening parenthesis.", COMPILER__read__lexling_from_current(*current).location);

        return output;
    }

    // get arguments
    while (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type != COMPILER__lt__right_parenthesis) {
        // if should check for comma
        if (output.count > 0) {
            // if lexling is comma
            if (COMPILER__read__lexling_from_current(*current).type != COMPILER__lt__comma) {
                // set error
                *error = COMPILER__open__error("Parse Error: Arguments is missing comma separator.", COMPILER__read__lexling_from_current(*current).location);

                return output;
            } else {
                // skip past comma
                COMPILER__advance__lexling_current(current, 1);
            }
        }

        // parse argument
        COMPILER__parsling_argument argument = COMPILER__parse__statement_argument(current, argument_location, error);
        if (COMPILER__check__error_occured(error)) {
            return output;
        }

        // append argument
        COMPILER__append(&output.list, COMPILER__parsling_argument, argument, error);
    
        // increment count
        output.count++;
    }

    // check for closing parenthesis
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__right_parenthesis) {
        // next lexling
        COMPILER__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *error = COMPILER__open__error("Parse Error: Arguments is missing closing parenthesis.", COMPILER__read__lexling_from_current(*current).location);

        return output;
    }

    return output;
}

// predefine
COMPILER__parsling_scope COMPILER__parse__scope(ANVIL__current* current, COMPILER__error* error);

// parse one statement
COMPILER__parsling_statement COMPILER__parse__statement(ANVIL__current* current, COMPILER__palt argument_location, COMPILER__error* error) {
    COMPILER__parsling_statement output = COMPILER__create_null__parsling_statement();
    ANVIL__bt is_scoped_offset = ANVIL__bt__false;

    // check for offset
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__at) {
        // advance current past @ symbol
        COMPILER__advance__lexling_current(current, 1);

        // check for offset name
        if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
            // set name
            output.name = COMPILER__create__parsling_argument(COMPILER__pat__offset, COMPILER__parse__namespace(current, error), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
            if (COMPILER__check__error_occured(error)) {
                return output;
            }
        // invalid syntax
        } else {
            // set error
            *error = COMPILER__open__error("Parse Error: Offset statement name is an invalid lexling.", COMPILER__read__lexling_from_current(*current).location);

            return output;
        }

        // look ahead for a scope setter
        {
            // setup temps
            ANVIL__current look_ahead_current = *current;
            COMPILER__error look_ahead_error = COMPILER__create_null__error();

            // parse ahead one namespace
            COMPILER__namespace temp = COMPILER__parse__namespace(&look_ahead_current, &look_ahead_error);

            // check for error
            if (COMPILER__check__error_occured(&look_ahead_error)) {
                // clear error (already set to not a lookahead scope)
                COMPILER__close__error(look_ahead_error);
            }

            // close temporary namespace
            COMPILER__close__parsling_namespace(temp);

            // check for the equals sign
            if (ANVIL__check__current_within_range(look_ahead_current) && COMPILER__read__lexling_from_current(look_ahead_current).type == COMPILER__lt__equals) {
                // is a named scope
                is_scoped_offset = ANVIL__bt__true;
            }
        }

        // is a scope
        if (is_scoped_offset == ANVIL__bt__true) {
            // is a named scope
            output.type = COMPILER__stt__subscope;

            // get scope flag namespace
            output.subscope_flag_name = COMPILER__create__parsling_argument(COMPILER__pat__name, COMPILER__parse__namespace(current, error), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
            if (COMPILER__check__error_occured(error)) {
                return output;
            }

            // check for invalid namespace
            if (output.subscope_flag_name.category != COMPILER__pat__name) {
                // set error
                *error = COMPILER__open__error("Parse Error: A named scope doesn't have its flagging variable.", COMPILER__read__lexling_from_current(*current).location);

                return output;
            }

            // skip past equals, already checked for it in lookahead
            COMPILER__advance__lexling_current(current, 1);

            // parse scope
            output.subscope = COMPILER__parse__scope(current, error);
        // is a normal offset
        } else {
            // setup type
            output.type = COMPILER__stt__offset;
        }
    // is a function call
    } else if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__name) {
        // get name
        output.name = COMPILER__create__parsling_argument(COMPILER__pat__offset, COMPILER__parse__namespace(current, error), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
        if (COMPILER__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }

        // get inputs
        output.inputs = COMPILER__parse__statement_arguments(current, argument_location, error);
        if (COMPILER__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }

        // get outputs
        output.outputs = COMPILER__parse__statement_arguments(current, argument_location, error);
        if (COMPILER__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }

        // set type
        if (argument_location == COMPILER__palt__function_declaration) {
            output.type = COMPILER__stt__function_header;
        } else if (argument_location == COMPILER__palt__function_call) {
            output.type = COMPILER__stt__function_call;
        }
    // error
    } else {
        // set error
        if (argument_location == COMPILER__palt__function_declaration) {
            *error = COMPILER__open__error("Parse Error: Unrecognized header type.", COMPILER__read__lexling_from_current(*current).location);
        } else {
            *error = COMPILER__open__error("Parse Error: Unrecognized statement type.", COMPILER__read__lexling_from_current(*current).location);
        }
    }

    return output;
}

// parse a scope
COMPILER__parsling_scope COMPILER__parse__scope(ANVIL__current* current, COMPILER__error* error) {
    COMPILER__parsling_scope output;

    // check for scope opener
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__left_curly_bracket) {
        // advance current
        COMPILER__advance__lexling_current(current, 1);
    // scope opener not found, error
    } else {
        // set error
        *error = COMPILER__open__error("Parse Error: Scope is missing left curly bracket.", COMPILER__read__lexling_from_current(*current).location);

        return output;
    }

    // parse statements
    // open statements list
    output.statements = COMPILER__open__counted_list_with_error(sizeof(COMPILER__parsling_statement) * 16, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }
    
    // get statements
    while (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type != COMPILER__lt__right_curly_bracket) {
        // parse statement
        COMPILER__parsling_statement statement = COMPILER__parse__statement(current, COMPILER__palt__function_call, error);
        if (COMPILER__check__error_occured(error)) {
            // close statement
            COMPILER__close__parsling_statement(statement);
            
            return output;
        }

        // add statement
        COMPILER__append(&output.statements.list, COMPILER__parsling_statement, statement, error);
        output.statements.count++;
        if (COMPILER__check__error_occured(error)) {
            return output;
        }
    }

    // check for scope closer
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__right_curly_bracket) {
        // advance current
        COMPILER__advance__lexling_current(current, 1);
    // scope opener not found, error
    } else {
        // setup error
        *error = COMPILER__open__error("Parse Error: Scope is missing right curly bracket.", COMPILER__read__lexling_from_current(*current).location);

        return output;
    }

    return output;
}

// parse a structure
COMPILER__parsling_structure COMPILER__parse__structure(ANVIL__current* current, COMPILER__error* error) {
    COMPILER__parsling_structure output = COMPILER__create_null__parsling_structure();

    // open type names list
    output.type_names = COMPILER__open__counted_list_with_error(sizeof(COMPILER__parsling_argument) * 8, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // parse type names
    while (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__exclamation_point) {
        // skip past exclamation point
        COMPILER__advance__lexling_current(current, 1);
        
        // parse type name
        COMPILER__namespace name = COMPILER__parse__namespace__one_name_only(current, error);
        if (COMPILER__check__error_occured(error)) {
            return output;
        }

        // append argument
        COMPILER__append(&output.type_names.list, COMPILER__parsling_argument, COMPILER__create__parsling_argument(COMPILER__pat__type, name, COMPILER__create_null__namespace(), COMPILER__create_null__namespace()), error);
        output.type_names.count++;
        if (COMPILER__check__error_occured(error)) {
            return output;
        }
    }

    // check for equals sign
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__equals) {
        // no saving data necessary, next lexling
        COMPILER__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        *error = COMPILER__open__error("Parse Error: A structure definition has an equals sign missing.", COMPILER__read__lexling_from_current(*current).location);

        // setup blank output list
        output.arguments = COMPILER__open__counted_list_with_error(1, error);

        // quit
        return output;
    }

    // parse arguments
    output.arguments = COMPILER__parse__statement_arguments(current, COMPILER__palt__structure_declaration, error);

    return output;
}

// parse a function
COMPILER__parsling_function COMPILER__parse__function(ANVIL__current* current, COMPILER__error* error) {
    COMPILER__parsling_function output = COMPILER__create_null__parsling_function();

    // parse header
    output.header = COMPILER__parse__statement(current, COMPILER__palt__function_declaration, error);
    if (COMPILER__check__error_occured(error)) {
        return output;
    }

    // check for equals sign
    if (ANVIL__check__current_within_range(*current) && COMPILER__read__lexling_from_current(*current).type == COMPILER__lt__equals) {
        // no saving data necessary, next lexling
        COMPILER__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        *error = COMPILER__open__error("Parse Error: A function definition has an equals sign missing.", COMPILER__read__lexling_from_current(*current).location);

        // quit
        return output;
    }

    // parse scope
    output.scope.subscope = COMPILER__parse__scope(current, error);
    output.scope.inputs = ANVIL__create_null__counted_list();
    output.scope.outputs = ANVIL__create_null__counted_list();
    output.scope.type = COMPILER__stt__subscope;
    output.scope.subscope_flag_name = COMPILER__create__parsling_argument(COMPILER__pat__name, COMPILER__create_null__namespace(), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());
    output.scope.name = COMPILER__create__parsling_argument(COMPILER__pat__name, COMPILER__create_null__namespace(), COMPILER__create_null__namespace(), COMPILER__create_null__namespace());

    return output;
}

// parse a program (file)
COMPILER__parsling_program COMPILER__parse__program(COMPILER__lexlings lexlings, COMPILER__error* error) {
    COMPILER__parsling_program output;
    COMPILER__parsling_function temp_function;
    COMPILER__parsling_structure temp_structure;
    ANVIL__current current_lexling;

    // open the function list
    output.functions = COMPILER__open__counted_list_with_error(sizeof(COMPILER__parsling_function) * 64, error);
    if (COMPILER__check__error_occured(error)) {
        goto quit;
    }

    // open the structure list
    output.structures = COMPILER__open__counted_list_with_error(sizeof(COMPILER__parsling_structure) * 64, error);
    if (COMPILER__check__error_occured(error)) {
        goto quit;
    }

    // open the aliases list
    // TODO

    // setup current
    current_lexling = ANVIL__calculate__list_current_buffer(&lexlings.data.list);

    // parse functions & structures
    while (ANVIL__check__current_within_range(current_lexling)) {
        // if end of files
        if (COMPILER__read__lexling_from_current(current_lexling).type == COMPILER__lt__end_of_files) {
            // finished parsing all files
            goto quit;
        }
        // if end of file
        if (COMPILER__read__lexling_from_current(current_lexling).type == COMPILER__lt__end_of_file) {
            // finished parsing file
            current_lexling.start += sizeof(COMPILER__lexling);
        // file continues
        } else {
            // determine abstraction type
            if (COMPILER__read__lexling_from_current(current_lexling).type == COMPILER__lt__exclamation_point) {
                // parse structure
                temp_structure = COMPILER__parse__structure(&current_lexling, error);

                // append structure
                COMPILER__append(&output.structures.list, COMPILER__parsling_structure, temp_structure, error);
                output.structures.count++;
            } else {
                // parse function
                temp_function = COMPILER__parse__function(&current_lexling, error);

                // append function
                COMPILER__append(&output.functions.list, COMPILER__parsling_function, temp_function, error);
                output.functions.count++;
            }
        }

        // check for error
        if (COMPILER__check__error_occured(error) == ANVIL__bt__true) {
            goto quit;
        }
    }

    // quit
    quit:

    return output;
}

// print namespace
void COMPILER__print__namespace(COMPILER__namespace name) {
    ANVIL__current current_lexling;

    // setup current
    current_lexling = ANVIL__calculate__current_from_list_filled_index(&name.lexlings.list);

    // print starting name lexling
    ANVIL__print__buffer(COMPILER__read__lexling_from_current(current_lexling).value);
    current_lexling.start += sizeof(COMPILER__lexling);

    // for each remaining lexling
    while (ANVIL__check__current_within_range(current_lexling)) {
        // print colon
        printf(":");

        // print name
        ANVIL__print__buffer(COMPILER__read__lexling_from_current(current_lexling).value);
        current_lexling.start += sizeof(COMPILER__lexling);
    }

    return;
}

// print argument
void COMPILER__print__parsling_argument(COMPILER__parsling_argument argument) {
    // print type
    printf("[");
    ANVIL__print__buffer(COMPILER__convert__general_argument_type_to_string_buffer(argument.category));
    printf("]");

    // print data
    COMPILER__print__namespace(argument.name);

    // print type modifier if exists
    if (ANVIL__check__empty_list(argument.type.lexlings.list) == ANVIL__bt__false) {
        // print type modifier
        printf("[type]");
        COMPILER__print__namespace(argument.type);
    }

    // print tag modifier if exists
    if (ANVIL__check__empty_list(argument.tag.lexlings.list) == ANVIL__bt__false) {
        // print tag modifier
        printf("[tag]");
        COMPILER__print__namespace(argument.tag);
    }

    return;
}

// print arguments
void COMPILER__print__parsling_arguments(ANVIL__counted_list* arguments) {
    ANVIL__current current = ANVIL__calculate__current_from_list_filled_index(&(*arguments).list);

    // print opener
    printf("(");

    // for each name
    while (ANVIL__check__current_within_range(current)) {
        // print separator
        if (current.start != (*arguments).list.buffer.start) {
            // print
            printf(" ");
        }

        // print argument
        COMPILER__print__parsling_argument(*(COMPILER__parsling_argument*)current.start);

        // next buffer
        current.start += sizeof(COMPILER__parsling_argument);
    }

    // print closer
    printf(")");

    return;
}

// forward declaration
void COMPILER__print__parsed_scope(COMPILER__parsling_scope scope, ANVIL__tab_count tab_depth);

// print statement
void COMPILER__print__parsed_statement(COMPILER__parsling_statement statement, ANVIL__tab_count tab_depth) {
    // print tabs
    ANVIL__print__tabs(tab_depth);

    // print statement
    if (statement.type == COMPILER__stt__offset) {
        // print offset information
        printf("@[normal]");
        COMPILER__print__namespace(statement.name.name);

        // print new line
        printf("\n");
    } else if (statement.type == COMPILER__stt__function_call || statement.type == COMPILER__stt__function_header) {
        // print statement name
        COMPILER__print__namespace(statement.name.name);

        // print inputs
        COMPILER__print__parsling_arguments(&statement.inputs);

        // print outputs
        COMPILER__print__parsling_arguments(&statement.outputs);

        // print new line
        printf("\n");
    } else if (statement.type == COMPILER__stt__subscope) {
        // print scope
        printf("@[subscope]");
        if (COMPILER__check__empty_namespace(statement.name.name) == ANVIL__bt__false) {
            COMPILER__print__namespace(statement.name.name);
        }
        printf(" ");
        if (COMPILER__check__empty_namespace(statement.name.name) == ANVIL__bt__false) {
        COMPILER__print__namespace(statement.subscope_flag_name.name);
        }
        printf(":\n");
        COMPILER__print__parsed_scope(statement.subscope, tab_depth + 1);
    }

    return;
}

// print a scope
void COMPILER__print__parsed_scope(COMPILER__parsling_scope scope, ANVIL__tab_count tab_depth) {
    ANVIL__current current_statement = ANVIL__calculate__current_from_list_filled_index(&scope.statements.list);

    // print statements
    if (ANVIL__check__empty_list(scope.statements.list) == ANVIL__bt__false) {
        // print each statement
        while (ANVIL__check__current_within_range(current_statement)) {
            // print statement
            COMPILER__print__parsed_statement(*(COMPILER__parsling_statement*)current_statement.start, tab_depth);

            // advance current
            current_statement.start += sizeof(COMPILER__parsling_statement);
        }
    }

    return;
}

// print a function
void COMPILER__print__parsed_function(COMPILER__parsling_function function, ANVIL__tab_count tab_depth) {
    // print header
    ANVIL__print__tabs(tab_depth);
    printf("Function: ");
    if (function.header.type != COMPILER__stt__invalid) {
        COMPILER__print__parsed_statement(function.header, 0);

        // print scope(s)
        COMPILER__print__parsed_statement(function.scope, tab_depth + 1);
    }

    return;
}

// print a structure
void COMPILER__print__parsed_structure(COMPILER__parsling_structure structure, ANVIL__tab_count tab_depth) {
    ANVIL__current current_name = ANVIL__calculate__current_from_list_filled_index(&structure.type_names.list);

    // print header
    ANVIL__print__tabs(tab_depth);
    printf("Structure: ");

    // print names
    while (ANVIL__check__current_within_range(current_name)) {
        // print name
        COMPILER__print__parsling_argument((*(COMPILER__parsling_argument*)current_name.start));

        // next name
        current_name.start += sizeof(COMPILER__parsling_argument);
    }

    // print arguments
    COMPILER__print__parsling_arguments(&structure.arguments);

    // next line
    printf("\n");

    return;
}

// print a program
void COMPILER__print__parsed_program(COMPILER__parsling_program program) {
    ANVIL__current current_structure = ANVIL__calculate__current_from_list_filled_index(&program.structures.list);
    ANVIL__current current_function = ANVIL__calculate__current_from_list_filled_index(&program.functions.list);
    
    // print header
    printf("Parsed Program:\n");

    // print each structure
    while (ANVIL__check__current_within_range(current_structure)) {
        // print structure
        COMPILER__print__parsed_structure(*(COMPILER__parsling_structure*)current_structure.start, 1);

        // advance current
        current_structure.start += sizeof(COMPILER__parsling_structure);
    }

    // print each function
    while (ANVIL__check__current_within_range(current_function)) {
        // print function
        COMPILER__print__parsed_function(*(COMPILER__parsling_function*)current_function.start, 1);

        // advance current
        current_function.start += sizeof(COMPILER__parsling_function);
    }

    return;
}

#endif
