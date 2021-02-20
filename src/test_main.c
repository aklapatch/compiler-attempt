#include "file-to-buf.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"

int main(int argc, char** argv){

    std::string file_str;
    int rc = file_to_string("lang-sample.txt", file_str);
    if (rc != 0){
        return rc;
    }

    std::vector<token> tokens;
    rc = tokenize(file_str, tokens);
    if (rc != 0){
        return rc;
    }
    for (auto tok: tokens){
        std::cout << tok.get_str(file_str) <<  " TYPE=" << tokToStr(tok.type) << "\n";
    }

    // go through every token and try out parsing VarDecls
    for (uint32_t i = 0; i < tokens.size(); ++i){
        

    }

    return rc;
}
