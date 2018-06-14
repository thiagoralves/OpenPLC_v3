


  MATIEC - IEC 61131-3 compiler


  The following compiler has been based on the
  FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)


  Copyright (C) 2003-2012  Mario de Sousa (msousa@fe.up.pt)


****************************************************************
****************************************************************
****************************************************************
*********                                              *********
*********                                              *********
*********         O V E R A L L    G O A L S           *********
*********                                              *********
*********                                              *********
****************************************************************
****************************************************************
****************************************************************



 This project has the goal of producing an open source compiler for the programming languages defined
in the IEC 61131-3 standard. These programming languages are mostly used in the industrial automation 
domain, to program PLCs (Programmable Logic Controllers).

 This standard defines 5 programming languages:
     - IL : Instructtion List
            A textual programming language, somewhat similar to assembly.
     - ST : Structured Text
            A textual programming language, somewhat similar to Pascal.
     - FBD: Function Block Diagram
            A graphical programming language, somewhat similar to an electrical circuit diagram based on small 
             scale integration ICs (Integrated Circuits) (counters, AND/OR/XOR/... logic gates, timers, ...).
     - LD : Ladder Diagram
            A graphical programming language, somewhat similar to an electrical circuit diagram based on 
             relays (used for basic cabled logic controllers).
     - SFC: Sequential Function Chart
            A graphical programming language, that defines a state machine, based largely on Grafcet.
            (may also be expressed in textual format).

 Of the above 5 languages, the standard defines textual representations for IL, ST and SFC. 
It is these 3 languages that we target, and we currently support all three, as long as they are
expressed in the textual format as defined in the standard.

 Currently the matiec project generates two compilers (more correctly, code translaters, but we like
to call them compilers :-O ): iec2c, and iec2iec

 Both compilers accept the same input: a text file with ST, IL and/or SFC code.

 The iec2c compiler generates ANSI C code which is equivalent to the IEC 61131-3 code expressed in the input file.

 The iec2iec compiler generates IEC61131-3 code which is equivalent to the IEC 61131-3 code expressed in the input file.
This last compiler should generate and output file which should be almost identical to the input file (some formating 
may change, as well as the case of letters, etc.). This 'compiler' is mostly used by the matiec project contributors
to help debug the lexical and syntax portions of the compilers.



 To compile/build these compilers, just
$./configure; make





****************************************************************
****************************************************************
****************************************************************
*********                                              *********
*********                                              *********
*********   O V E R A L L    A R C H I T E C T U R E   *********
*********                                              *********
*********                                              *********
****************************************************************
****************************************************************
****************************************************************

 The compiler works in 4(+1) stages:
 ==================================
 Stage 1    - Lexical analyser       - implemented with flex (stage1_2/iec_flex.ll)
 Stage 2    - Syntax parser          - implemented with bison (stage1_2/iec_bison.yy)
 Stage pre3 - Populate symbol tables - Symbol tables that will ease searching for symbols in the abstract symbol tree.
 Stage 3    - Semantics analyser     - currently does type checking only
 Stage 4    - Code generator         - generates ANSI C code

 Stage 5    - Binary code generator - gcc, javac, etc... (Not integrated into matiec compiler. Must be called explicitly by the user.)



 Data structures passed between stages, in global variables:
 ==========================================================
   1->2   : tokens (int), and token values (char *) (defined in stage1_2/stage1_2_priv.hh)
   2->1   : symbol tables (implemented in util/symtable.[hh|cc], and defined in stage1_2/stage1_2_priv.hh)
   2->3   : abstract syntax tree (tree of C++ objects, whose classes are defined in absyntax/absyntax.hh)
pre3->3,4 : global symbol tables (defined in util/[d]symtable.[hh|cc] and declared in absyntax_utils/absyntax_utils.hh)
   3->4   : abstract syntax tree (same as 2->3), but now annotated (i.e. some extra data inserted into the absyntax tree)

   4->5   : file with program in c, java, etc...




 The compiler works in several passes:
 ====================================

Stage 1 and Stage 2
-------------------
  Executed in one single pass. This pass will:
  - Do lexical analysis
  - Do syntax analysis
  - Execute the absyntax_utils/add_en_eno_param_decl_c visitor class
    This class will add the EN and ENO parameter declarations to all
    functions that do not have them already explicitly declared by the user.
    This will let us handle these parameters in the remaining compiler just as if
    they were standard input/output parameters.


Stage Pre3
----------
  Executed in one single pass. This pass will populate the following symbol tables:
   - function_symtable;            /* A symbol table with all globally declared functions POUs. */
   - function_block_type_symtable; /* A symbol table with all globally declared functions block POUs. */
   - program_type_symtable;        /* A symbol table with all globally declared program POUs. */
   - type_symtable;                /* A symbol table with all user declared (non elementary) datat type definitions. */
   - enumerated_value_symtable;    /* A symbol table with all identifiers (values) declared for enumerated types. */


Stage 3
-------
  Executes two algorithms (flow control analysis, and data type analysis) in several passes.

  Flow control:
     Pass 1: Does flow control analysis (for now only of IL code) 
             Implemented in -> stage3/flow_control_analysis_c
             This will anotate the abstract syntax tree 
             (Every object of the class il_instruction_c that is in the abstract syntax tree will have the variable 'prev_il_instruction' correctly filled in.)

  Data Type Analysis
     Pass 1: Analyses the possible data types each expression/literal/IL instruction/etc. may take 
             Implemented in -> stage3/fill_candidate_datatypes_c
             This will anotate the abstract syntax tree 
             (Every object of in the abstract syntax tree that may have a data type, will have the variable 'candidate_datatypes' correctly filled in.
             Additionally, objects in the abstract syntax tree that represen function invocations will have the variable 
              'candidate_functions' correctly filled in.)
Pass 2: Narrows all the possible data types each expression/literal/IL instruction/etc. may take down to a single data type 
             Implemented in -> stage3/narrow_candidate_datatypes_c
             This will anotate the abstract syntax tree 
             (Every object of in the abstract syntax tree that may have a data type, will have the variable 'datatype' correctly filled in.
              Additionally, objects in the abstract syntax tree that represen function invocations will have the variables 
              'called_function_declaration' and 'extensible_param_count' correctly filled in.
              Additionally, objects in the abstract syntax tree that represen function block (FB) invocations will have the variable
              'called_fb_declaration' correctly filled in.)
     Pass 2: Prints error messages in the event of the IEC 61131-3 source code being analysed contains semantic data type incompatibility errors.
             Implemented in -> stage3/print_datatype_errors_c

  
Stage 4
-------
  Has 2 possible implementations.
  
  iec2c  :  Generates C source code in a single pass (stage4/generate_c).
  iec2iec:  Generates IEC61131 source code in a single pass (stage4/generate_iec).






****************************************************************
****************************************************************
****************************************************************
*********                                              *********
*********                                              *********
*********               N O T E S                      *********
*********                                              *********
*********                                              *********
****************************************************************
****************************************************************
****************************************************************






 NOTE 1
 ======
 Note that stage 2 passes data back to stage 1. This is only
possible because both stages are executed in the same pass.



 NOTE 2
 ======
 It would be nice to get this parser integrated into the gcc
group of compilers. We would then be able to compile our st/il
programs directly into executable binaries, for all the processor
architectures gcc currently supports.
 The gcc compilers are divided into a frontend and backend. The
data structure between these two stages is called the syntax
tree. In essence, we would need to create a new frontend that
would parse the st/il program and build the syntax tree.
Unfortunately the gcc syntax tree is not very well documented,
and doing semantic checking on this tree would probably be a
nightmare.
 We therefore chose to follow the same route as the gnat (ada 95)
and cobol compilers, i.e. generate our own abstract syntax tree,
do semantic checking on our tree, do whatever optimisation
we can at this level on our own tree, and only then build
the gcc syntax tree from our abstract syntax tree.
 All this may still be integrated with the gcc backend to generate
a new gnu compiler for the st and il programming languages.
Since generating the gcc syntax tree will probably envolve some
trial and error effort due to the sparseness of documentation,
we chose to start off by coding a C++ code generator for
our stage 4. We may later implement a gcc syntax tree generator
as an alternative stage 4 process, and then integrate it with
the gcc toplevel.c file (command line parsing, etc...).



****************************************************************
****************************************************************
****************************************************************
*********                                              *********
*********                                              *********
*********               S T A G E      1               *********
*********                                              *********
*********                                              *********
****************************************************************
****************************************************************
****************************************************************



Issue 1
=======

 The syntax defines the common_character_representation as:
<any printable character except '$', '"' or "'"> | <escape sequences>

 Flex includes the function print_char() that defines
all printable characters portably (i.e. whatever character
encoding is currently being used , ASCII, EBCDIC, etc...)
Unfortunately, we cannot generate the definition of
common_character_representation portably, since flex
does not allow definition of sets by subtracting
elements in one set from another set (Note how
common_character_representation could be defined by
subtracting '$' '"' and "'" from print_char() ).
This means we must build up the defintion of
common_character_representation using only set addition,
which leaves us with the only choice of defining the
characters non-portably...

 In short, the definition we use for common_character_representation
only works for ASCII character encoding!




Issue 2
=======

We extend the IEC 61131-3 standard syntax to allow inclusion of 
other files. The accepted syntax is:

   {#include "<filename>" }

We use a pragma directive for this (allowed by the standard itself),
 since it is an extension of the standard. In principle, this would 
be ignored by other standard complient compilers!



****************************************************************
****************************************************************
****************************************************************
*********                                              *********
*********                                              *********
*********               S T A G E      2               *********
*********                                              *********
*********                                              *********
****************************************************************
****************************************************************
****************************************************************

 Overall Comments
 ================

 
 Comment 1
 ---------
 We have augmented the syntax the specification defines to include
restrictions defined in the semantics of the languages.

 This is required because the syntax cannot be parsed by a LALR(1)
parser as it is presented in the specification. Many reduce/reduce
and shift/reduce conflicts arise. This is mainly because the parser
cannot discern how to reduce an identifier. Identifiers show up in
many places in the syntax, and it is not entirely possible to
figure out if the identifier is a variable_name, enumeration
value, function block name, etc... only from the context in
which it appears.

 A more detailed example of why we need symbol tables are
the type definitions...  In definition of new types
(section B 1.3.3) the parser needs to figure out the class of
the new type being defined (enumerated, structure, array, etc...).
This works well when the base classes are elementary types
(or structures, enumeration, arrays, etc. thereof). It becomes
confusing to the parser when the new_type is based on a previously
user defined type.

TYPE
  new_type_1 : INT := 99;
  new_type_2 : new_type_1 := 100;
END_TYPE

 When parsing new_type_1, the parser can figure out that the
identifier new_type_1 is a simple_type_name, because it is
based on a elementary type without structure, arrays, etc...
 While parsing new_type_2, it becomes confused how to reduce
the new_type_2 identifier, as it is based on the identifier
new_type_1, of which it does not know the class (remember, at this
stage new_type_1 is a simple identifier!).
 We therefore need to keep track of the class of the user
defined types as they are declared, so that the lexical analyser
can tell the syntax parser what class the type belongs to. We
cannot use the abstract syntax tree itself to search for the
declaration of new_type_1 as we only get a handle to the root
of the tree at the end of the parsing.

 We therefore maintain an independent and parallel table of symbols,
that is filled as we come across the type delcarations in the code.
Actually, we ended up also needing to store variable names in
the symbol table. Since variable names come and go out of scope
depending on what portion of code we are parsing, we sometimes
need to remove the variable names from the symbol table.
Since the ST and IL languages only have a single level of scope,
I (Mario) found it easier to simply use a second symbol table for
the variable names that is completely cleared when the parser
reaches the end of a function (function block or program).

What I mean when I say that these languages have a single level
of scope is that all variables used in a function (function block
or program) must be declared inside that function (function block
or program). Even global variables must be re-declared as EXTERN
before a function may access them! This means that it is easy
to simply load up the variable name symbol table when we start
parsing a function (function block or program), and to clear it
when we reach the end. Checking whether variables declared
as EXTERN really exist inside a RESOURCE or a CONFIGURATION
is left to stage 3 (semantic checking) where we can use the
abstract tree itself to search for the variables (NOTE: semantic
cheching at stage 3 has not yet been implemented, so we may yet
end up using a symbol table too at that stage!).

 Due to the use of the symbol tables, and special identifier
tokens depending on the type of identifier it had previously
been declared in the code being parsed, the syntax was slightly
changed regarding the definition of variable names, derived
function names, etc... FROM for e.g.:
variable_name: identifier;
TO
variable_name: variable_name_token;

 Flex first looks at the symbol tables when it finds an identifier,
and returns the correct token corresponding to the identifier
type in question. Only if the identifier is not currently stored
in any symbol table, does flex return a simple identifier_token.

 This means that the declarations of variables, functions etc...
were changed FROM:
function_declaration: FUNCTION derived_function_name ...
TO
function_declaration: FUNCTION identifier ...
since the initial definition of derived_function_name had been
changed FROM
derived_function_name: identifier;
TO
derived_function_name: derived_function_name_token;




 Comment 2
 ---------
 Since the ST and IL languages share a lot of common syntax,
I have decided to write a single parser to handle both languages
simultaneously. This approach has the advantage that the user
may mix the language used in the same file, as long as each function
is written in a single lanuage.

 This approach also assumes that all the IL language operators are
keywords, which means that it is not possible to define variables
using names such as "LD", "ST", etc...
Note that the spec does not consider these operators to be keywords,
so it means that they should be available for variable names! On the
other hand, all implementations of the ST and IL languages seems to
treat them as keywords, so there is not much harm in doing the same.

 If it ever becomes necessary to allow variables with names of IL 
operators, either the syntax will have to be augmented, or we can 
brake up the parser in two: one for ST and another for IL.



/********************************/
/* B 1.3.3 - Derived data types */
/********************************/

Issue 1
=======

According to the spec, the valid construct
TYPE new_str_type : STRING := "hello!"; END_TYPE
has two possible routes to type_declaration...

Route 1:
type_declaration: single_element_type_declaration
single_element_type_declaration: simple_type_declaration
simple_type_declaration: identifier ':' simple_spec_init
simple_spec_init: simple_specification ASSIGN constant
(shift:  identifier <- 'new_str_type')
simple_specification: elementary_type_name
elementary_type_name: STRING
(shift: elementary_type_name <- STRING)
(reduce: simple_specification <- elementary_type_name)
(shift: constant <- "hello!")
(reduce: simple_spec_init: simple_specification ASSIGN constant)
(reduce: ...)


Route 2:
type_declaration: string_type_declaration
string_type_declaration: identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init
(shift:  identifier <- 'new_str_type')
elementary_string_type_name: STRING
(shift: elementary_string_type_name <- STRING)
(shift: string_type_declaration_size <- /* empty */)
string_type_declaration_init: ASSIGN character_string
(shift: character_string <- "hello!")
(reduce: string_type_declaration_init <- ASSIGN character_string)
(reduce: string_type_declaration <- identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init )
(reduce: type_declaration <- string_type_declaration)


 At first glance it seems that removing route 1 would make
the most sense. Unfortunately the construct 'simple_spec_init'
shows up multiple times in other rules, so changing this construct
would mean changing all the rules in which it appears.
I (Mario) therefore chose to remove route 2 instead. This means
that the above declaration gets stored in a
simple_type_declaration_c, and not in a string_type_declaration_c
as would be expected!


/***********************/
/* B 1.5.1 - Functions */
/***********************/


Issue 1
=======

 Due to reduce/reduce conflicts between identifiers
being reduced to either a variable or an enumerator value,
we were forced to keep a symbol table of the names
of all declared variables. Variables are no longer
created from simple identifier_token, but from
variable_name_token.

 BUT, in functions the function name may be used as
a variable! In order to be able to parse this correctly,
the token parser (flex) must return a variable_name_token
when it comes across the function name, while parsing
the function itself.
We do this by inserting the function name into the variable
symbol table, and having flex return a variable_name_token
whenever it comes across it.
When we finish parsing the function the variable name
symbol table is cleared of all entries, and the function
name is inserted into the library element symbol table. This
means that from then onwards flex will return a
derived_function_name_token whenever it comes across the
function name.

In order to insert the function name into the variable_name
symbol table BEFORE the function body gets parsed, we
need the parser to reduce a construct that contains the
the function name. That is why we created the extra
construct 'function_name_declaration', i.e. to force
the parser to reduce it, before parsing the function body,
and therefore get an oportunity to insert the function name
into the variable name symbol table!


/********************************/
/* B 3.2.4 Iteration Statements */
/********************************/

Issue 1
=======

For the 'FOR' iteration loop

  FOR control_variable ASSIGN expression TO expression BY expression DO statement_list END_FOR

The spec declares the control variable in the syntax as

  control_variable: identifier;

but then defines the semantics of control_variable (Section 3.3.2.4)
as being of an integer type (e.g., SINT, INT, or DINT).
Obviously this presuposes that the control_variable must have been
declared in some 'VAR .. VAR_END' construct, so I (Mario) changed
the syntax to read

  control_variable: variable_name;




**************************************************************************

  Copyright (C) 2003-2012  Mario de Sousa (msousa@fe.up.pt)
