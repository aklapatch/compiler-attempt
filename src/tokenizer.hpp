#pragma once
#include <string>
#include <cstdint>
#include <vector>

#define ERROR(x) (std::cout << "\n " << __func__ << " " << __LINE__ << " " << (x) << "\n")

#define VARNAME_TO_STR(name) (#name) 

enum token_type {
    tok_none = 0, // Error type, couldn't find another type for it.

    tok_comment,
    tok_doc_comment,
    
    // declarations
    tok_enum,
    tok_struct,
    tok_union,
    tok_def,
    tok_import,
    tok_package,
    tok_fn,

    tok_using,

    // characters and quote blocks
    tok_quote,
    tok_char,

    // assigning variables/putting vals on the stack
    tok_assign,
    tok_push_top,

    // type declarations
    tok_d_type,

    // items in a package or other data_types
    tok_inside_package,
    // the . character
    tok_inside_type,
    // the :: sequence, a single : is invalid
    tok_type_dec,

    // the character that separates arguments
    tok_arg_sep,

    // parens, brackets and braces
    tok_r_paren,
    tok_l_paren,
    tok_r_brack, // ]
    tok_l_brack, // [
    tok_r_brace, // }
    tok_l_brace, // {

    tok_identifier,
    tok_float,
    tok_int,
    tok_bool,
    tok_num_tok_types
};

std::string tokToStr(token_type tok){
    std::string tok_names[] = {
        "tok_none",
        "tok_comment",
        "tok_doc_comment",
        "tok_enum",
        "tok_struct",
        "tok_union",
        "tok_def",
        "tok_import",
        "tok_package",
        "tok_fn",
        "tok_using",
        "tok_quote",
        "tok_char",
        "tok_assign",
        "tok_push_top",
        "tok_d_type",
        "tok_inside_package",
        "tok_inside_type",
        "tok_type_dec",
        "tok_arg_sep",
        "tok_r_paren",
        "tok_l_paren",
        "tok_r_brack",
        "tok_l_brack",
        "tok_r_brace",
        "tok_l_brace",
        "tok_identifier",
        "tok_float",
        "tok_int",
        "tok_bool",
        "tok_num_tok_types"};

    return tok_names[tok];
}


typedef struct token {
    token_type type;
    ssize_t start, len;

    std::string get_str(std::string ref_string){
        return std::string(ref_string, start, len);
    }
    token(token_type t, ssize_t s, ssize_t l) : type(t), start(s), len(l) {}
    token(): type(tok_none), start(0), len(0) {}

} token;

bool isReservedChar(char input){
    std::vector<char> reserved_chars = { '(', ')', '{', '}', '[', ']', '#' , ':', '|','.' };
    for( auto c : reserved_chars){
        if (input == c){
            return true;
        }
    }
    return false;
}

// output a vector of tokens.
int tokenize(std::string input, std::vector<token> &output){
    output.clear();

    ssize_t i = 0, end = input.size();
    while (i < end){

        if(isspace(input[i])){
            ++i;
            continue;
        }

        // see if what kind of comment this is.
        if(input[i] == '#'){
            token tok;
            tok.start = i;
            tok.type = tok_comment;

            ++i;
            if(i >= end){
                tok.len = 1;
                output.push_back(tok);
                continue;
            }
            if(input[i] == '#'){
                tok.type = tok_doc_comment;
                ++i;
            }
            
            // treat this as a block comment;
            if(input[i] == '*'){
                do { 
                    ++i;
                    if(i+1 >= end){
                        ERROR("Reached end of string");
                        return 1;
                    }
                    // go to the next end comment
                } while(input[i] != '*' && input[i+1] != '#');
                i += 2; // increment for the ending # as well
            }
            else {
                // regular comment 
                // go to the nearest newline
                while(input[i] != '\n' && i < end){
                    ++i;
                }
            }
            tok.len = i - tok.start;
            output.push_back(tok);
            ++i;
            continue;
        }

        // handle numbers
        if (isdigit(input[i]) || (input[i] == '-' && i+1 < end && isdigit(input[i+1]))){
            token tok;
            tok.start = i;

            bool have_dot = false;
            ++i;
            while ( (isdigit(input[i]) || input[i] == '.') && i < end){
                if (input[i] == '.'){
                    if (have_dot){
                        ERROR("too many periods in a number!");
                    }
                    have_dot = true;
                }
                ++i;
            }

            // check for errors
            if (isalpha(input[i])){
                ERROR("Expected space, got something else");
                return 1;
            }

            tok.len = i - tok.start;
            tok.type = have_dot ? tok_float : tok_int;
            output.push_back(tok);
            
            // i is already incremented
            continue;
        }
        
        // typing declarations
        if (input[i] == ':'){
            token tok;
            tok.start = i;
            tok.type = tok_type_dec;

            ++i;
            if (i >= end){
                ERROR("Reached end of string with incomplete expression!");
                return 1;
            }
            if (input[i] == ':'){
                tok.len = 2;
                output.push_back(tok);
            }
            else if (input[i] == '>'){ 
                tok.type = tok_inside_package;
                tok.len = 2;
                output.push_back(tok);
            }
            else {
                ERROR("Colon sequence incomplete!");
                return 1;
            }
            ++i;
            continue;
        }

        if(input[i] == '|'){
            output.push_back({ tok_arg_sep, i, 1 });
            ++i;
            continue;
        }

        if (input[i] == '('){
            output.push_back({ tok_l_paren, i, 1});
            ++i;
            continue;
        }
        if (input[i] == ')'){
            output.push_back({ tok_r_paren, i, 1});
            ++i;
            continue;
        }
        if (input[i] == '['){
            output.push_back({ tok_l_brack, i, 1});
            ++i;
            continue;
        }
        if (input[i] == ']'){
            output.push_back({ tok_r_brack, i, 1});
            ++i;
            continue;
        }
        if (input[i] == '{'){
            output.push_back({ tok_l_brace, i, 1});
            ++i;
            continue;
        }
        if (input[i] == '}'){
            output.push_back({ tok_r_brace, i, 1});
            ++i;
            continue;
        }

        if (input[i] == '.'){
            output.push_back({tok_inside_type , i, 1});
            ++i;
            continue;
        }

        if (input[i] == '<'){
            token tok;
            tok.start = i;
            ++i;
            if (input[i] == '<'){
                ++i;

                tok.len = 2;
                tok.type = tok_push_top;
                output.push_back(tok);
                continue;
            }
            else { 
                // reset i since we don't want to increment it 
                // for the next go round.
                i = tok.start;
            }
        }

        if (input[i] == '>'){
            token tok;
            tok.start = i;
            ++i;
            if (input[i] == '>'){
                ++i;
                tok.len = 2;
                tok.type = tok_assign;
                output.push_back(tok);
                continue;
            }
            else { 
                // reset i since we don't want to increment it 
                // for the next go round.
                i = tok.start;
            }
        }

        // handle strings and quoted phrases.
        if (input[i] == '"'){
            // go to the end and get everything in between.
            token tok;
            tok.type = tok_quote;
            tok.start = i;
            ++i;
            while (input[i] != '"' && i < end){
                ++i;
                // ignore escape sequences
                if (input[i] == '\\'){
                    ++i;
                }
            }
            if (i >= end){
                ERROR("End of string without closing \'");
                return 1;
            }
            ++i; // include the final "
            tok.len = i - tok.start;

            output.push_back(tok);
            continue;
        }
        
        // handle strings and quoted phrases.
        if (input[i] == '\''){
            // go to the end and get everything in between.
            token tok;
            tok.type = tok_char;
            tok.start = i;
            ++i;
            while (input[i] != '\'' && i < end){
                ++i;
                // ignore escape sequences
                if (input[i] == '\\'){
                    ++i;
                }
            }
            if (i >= end){
                ERROR("End of string without closing \'");
                return 1;
            }
            ++i;
            tok.len = i - tok.start;

            output.push_back(tok);
            continue;
        }
        
        //handle letters and identifiers
        // just go until you hit a space or a reserved char.
            token tok;
            tok.type = tok_identifier;
            tok.start = i;
            while (!isReservedChar(input[i]) && !isspace(input[i]) && i < end){
                ++i;
            }

            tok.len = i - tok.start;

            // if you get a def or something, then set the token_type
            std::string out_str = tok.get_str(input);
            if (out_str == "def"){
                tok.type = tok_def;
            }
            else if (out_str == "enum"){
                tok.type = tok_enum;
            }
            else if (out_str == "struct"){
                tok.type = tok_struct;
            }
            else if (out_str == "import"){
                tok.type = tok_import;
            }
            else if (out_str == "fn"){
                tok.type = tok_fn;
            }
            else if (out_str == "package"){
                tok.type = tok_package;
            }
            else if (out_str == "union"){
                tok.type = tok_union;
            }
            else if (out_str == "true" || out_str == "false"){
                tok.type = tok_bool;
            }

            output.push_back(tok);
    }
    return 0;
}
