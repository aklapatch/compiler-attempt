#pragma once
#include<cstdint>
#include <vector>
#include "tokenizer.hpp"

enum builtin_types {
    i8_e = 0,
    u8_e,
    i16_e,
    u16_e,
    i32_e,
    u32_e,
    i64_e,
    u64_e,
    i128_e,
    u128_e,
    isize_e,
    usize_e,
    f16_e,
    f32_e,
    f64_e,
    f128_e,
    bool_e,
    void_e,
    type_e,
};

// TODO
// parse enum and structs and functions

class LexUnit {
  public:
    virtual void print(std::string ref) {}
    virtual ~LexUnit() {}
};
 
class Uint : public LexUnit {
  public:
    uint64_t val;
    Uint(uint64_t v): val(v) {}
    void print(std::string ref) {
      std::cout << "Uint: "; 
      std::cout << val;
      std::cout << "\n";
    }
};

class Sint : public LexUnit {
  public:
    int64_t val;
    Sint(int64_t v): val(v) {}
    void print(std::string ref) {
      std::cout << "Sint: "; 
      std::cout << val << "\n";
    }
};

class Float : public LexUnit {
  public:
    double val;
    Float(double v): val(v) {}
    void print(std::string ref) {
      std::cout << "Float: "; 
      std::cout << val << "\n";
    }
};

// Identifiers are struct members and functions inside packages too
// like: struc.member or package:>func
class Identifier : public LexUnit {
  public:
    std::vector<token> val;
    Identifier(std::vector<token> v) : val(v) {}
    void print(std::string ref) {
      std::cout << "Identifier: "; 
        for (auto tok: val){
          std::cout << tok.get_str(ref);
        }
      std::cout << "\n";
    }
};

class PushTop : public LexUnit {
  public:
    Identifier arg;
    PushTop(Identifier a) : arg(a) {}
    void print(std::string ref) {
      std::cout << "PusTop: "; 
      arg.print(ref);
      std::cout << "\n";
    }
};

class Bool : public LexUnit {
  public:
    bool value;
    Bool(bool v) : value(v) {}
    void print(std::string ref) {
      std::cout << "Bool: " << value << "\n"; 
    }

};

class Assign : public LexUnit{
  public:
    Identifier arg;
    Assign(Identifier a) : arg(a) {}
    void print(std::string ref) {
      std::cout << "Assign: "; 
      arg.print(ref);
      std::cout << "\n";
    }
};

struct typed_name {
  token name;
  std::vector<token> type;

  void print(std::string ref_str){
    std::cout << "typed name: name=" << name.get_str(ref_str) << " type=";
    for (auto tok: type){
      std::cout << tok.get_str(ref_str);
      std::cout << " ";
    }
    std::cout << "\n";
  }
      
};

class VarDecl : public LexUnit {
  public:
    token name;
    std::vector<token> type;
    LexUnit value;

    VarDecl(token n,std::vector<token> t,LexUnit v) : name(n), type(t), value(v) {}
    
    void print(std::string ref) {
      std::cout << "VarDecl: "; 
      std::cout << "name="<< name.get_str(ref) << "\n";
      std::cout << "type=";
      for (auto tok: type){
        std::cout << tok.get_str(ref) ;
        std::cout  << " ";
      }
      std::cout << "\n";
    }
};

class Quote : public LexUnit {
  public:
    std::vector<LexUnit> contents;

    Quote(std::vector<LexUnit> c) : contents(c) {}
    void print(std::string ref) {
      std::cout << "Quote: "; 
      for (auto unit: contents){
        unit.print(ref);
          std::cout << " ";
      }
      std::cout << "\n";
    }
};

class EnumDecl : public LexUnit {
  public:
    typed_name name;

    std::vector<token> members;
    std::vector<LexUnit> values;

    EnumDecl(typed_name n, std::vector<token> m, std::vector<LexUnit> v) : name(n), members(m), values(v) {}

    void print(std::string ref){
      std::cout << "EnumDecl: name=";
      name.print(ref);
      std::cout << " members=";
      for (auto member : members){
        std::cout << member.get_str(ref);
      }
      std::cout << " values=";
      for (auto value : values){
        value.print(ref);
      }
    }
};

// We can't let structs have a typed name otherwise we can't use anonymous structs
class StructDecl : public LexUnit {
  public:
    token name;

    std::vector<typed_name> members;

    StructDecl(token n, std::vector<typed_name> m) : name(n), members(m) {}

    void print(std::string ref){
      std::cout << "StructDecl: name=" << name.get_str(ref);
      std::cout << " members=";
      for (auto member : members){
        member.print(ref);
      }
    }
};

class FnDecl : public LexUnit {
  public:
    token name;
    std::vector<token> type;

    std::vector<typed_name> in_args;
    std::vector<typed_name> out_args;

    std::vector<LexUnit> body;

    FnDecl(token n,std::vector<token> t,std::vector<typed_name> ia,std::vector<typed_name> oa,std::vector<LexUnit>  b) : name(n), type(t),in_args(ia), out_args(oa), body(b) {}

    void print(std::string ref){
      std::cout << "FnDecl: name=" << name.get_str(ref) << " type=";
      for (auto tok : type){
        std::cout << tok.get_str(ref) << " ";
      }
      std::cout << " inputs=";
      for (auto arg: in_args){
        arg.print(ref);
      }
      std::cout << " outputs= ";
      for (auto arg: out_args){
        arg.print(ref);
      }

      std::cout << " body=";
      for (auto part: body){
        part.print(ref);
      }
    }
};

int getType(std::string input_str, std::vector<token> tokens, uint32_t &i, std::vector<token> &out){
  out.clear();

  // keep going and get all the types. if you get 2 identifiers in a row it's time to stop.
  do {
    // assume you start at the ::
    if (tokens[i].type != tok_type_dec){
      ERROR("Expected ::, got something else!");
      return 1;
    }
    ++i;
    if (tokens[i].type != tok_identifier){
      ERROR("Expected an identifier, got something else!");
      return 1;
    }

    out.push_back(tokens[i]);

    ++i;

    // if you get anything that's not a :: stop without errors.
    if (tokens[i].type != tok_type_dec){
      return 0;
    }
    ++i;
  } while (true);
}

int parseBool(std::string input_str,
    token input,
    uint32_t &i,
    LexUnit &out){

  if (input.type != tok_bool){
    ERROR("Expected bool, got something else!");
    return 1;
  }
  std::string bool_words = input.get_str(input_str);

  if (bool_words == "true"){
    out = Bool(true);
    return 0;
  }else if (bool_words == "false"){
    out = Bool(false);
    return 0;
  }else {
    ERROR("Bool words did not match true or false");
    return 1;
  }
}

LexUnit parseFloat(std::string input_str, token input){
    double value = std::stod(input.get_str(input_str));
    return Float(value);
}

LexUnit parseInt(std::string input_str, token input){
  if (input.get_str(input_str)[0] == '-'){
    int64_t val = std::stoi(input.get_str(input_str));
    return Sint(val);
  }

  uint64_t val = std::stoi(input.get_str(input_str));
  return Uint(val);
}

int parseNumber(std::string input_str, token input, LexUnit &out){
  if (input.type == tok_float){
    out = parseFloat(input_str, input);
    return 0;
  }
  else if (input.type == tok_int){
    out = parseInt(input_str, input);
    return 0;
  }
  else {
    ERROR("Input token type does not match!");
    return 1;
  }
}

int parseVarDecl(
    std::string input_str, 
    std::vector<token> tokens, 
    uint32_t &i, 
    VarDecl &out){

  // assume you are after (def
  if (tokens[i].type != tok_identifier){
    ERROR("Expected an identifier, found something else!");
    return 1;
  }

  // TODO: after this, parse the value and store it in the VarDecl
  // get the type of the variable if there is one
  out.name = tokens[i];
  out.type.clear();
  ++i;
  
  if (tokens[i].type == tok_type_dec){
    int ret = getType(
        input_str,
        tokens,
        i,
        out.type);
    if (ret!= 0){
      ERROR("Getting a type returned an error!");
      return ret;
    }
  }

  int ret = parseNumber(
      input_str,
      tokens[i],
      out.value);
  if (ret != 0){
    ERROR("Parsing a variable number failed!");
    return ret;
  }

  return 0;
}

int parseAll(std::string input_str, std::vector<token> tokens, std::vector<LexUnit> out){
  out.clear();


    return 0;
}

int parseFnDecl(std::string input_str, std::vector<token> tokens, uint32_t &i, FnDecl &out){

  // Assume that you start after the (fn part, and the next token is the identifier.
  if (tokens[i].type != tok_identifier){
    ERROR("Incorrect token type for function declaration");
    return 1;
  }
  token fn_name = tokens[i];

  ++i;
  if (tokens[i].type != tok_l_paren){
    ERROR("Expected a (, got something else!");
    return 1;
  }
  ++i;

  std::vector<typed_name> in_args;

  while( tokens[i].type != tok_arg_sep || tokens[i].type == tok_identifier){
    if(tokens[i].type != tok_identifier){
      ERROR("Expected a identifier, got something else!");
      return 1;
    }

    typed_name tmp;
    tmp.name = tokens[i];

    ++i;
    int rc = getType(
        input_str,
        tokens,
        i,
        tmp.type);
    if (rc != 0){
      ERROR("Failed to parse a type!");
      return rc;
    }

    ++i;

    in_args.push_back(tmp);
  }

  std::vector<typed_name> out_args;

  while( tokens[i].type != tok_r_paren){
    if(tokens[i].type != tok_identifier){
      ERROR("Expected an identifier, got something else!");
      return 1;
    }

    typed_name tmp;
    tmp.name = tokens[i];

    ++i;
    int rc = getType(
        input_str,
        tokens,
        i,
        tmp.type);
    if (rc != 0){
      ERROR("Failed to parse a type!");
      return rc;
    }

    ++i;

    out_args.push_back(tmp);
  }

  //Increment to get past the right paren 
  ++i;

  std::vector<token> type;
  std::vector<LexUnit> body;
  int rc = parseAll(
      input_str,
      tokens,
      body);
  if (rc != 0){
    ERROR("Failed to parse function body!");
    return rc;
  }


  out = FnDecl(
      fn_name, 
      type, 
      in_args,
      out_args,
      body);

  return 0;
}


