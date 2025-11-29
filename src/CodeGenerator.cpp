#include "../include/CodeGenerator.hpp"
#include <sstream>
#include <regex>
#include <algorithm>

namespace vm {

static bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    size_t i=0; if (s[0]=='-') i=1; for (; i<s.size(); ++i) if (!isdigit((unsigned char)s[i])) return false; return true;
}

CodeGenerator::CodeGenerator(){
    availRegs = {"x5","x6","x7","x28","x29","x30","x31"};
}

Datatype CodeGenerator::toDatatype(const std::string& s) const{
    std::string t=s; std::transform(t.begin(),t.end(),t.begin(),::tolower);
    if (t=="int") return Datatype::INT;
    if (t=="float") return Datatype::FLOAT;
    if (t=="bool") return Datatype::BOOL;
    if (t=="char") return Datatype::CHAR;
    if (t=="str") return Datatype::STR;
    return Datatype::UNKNOWN;
}

bool CodeGenerator::isBinaryArithmetic(const std::string& line) const{
    // format: dst = a OP b TYPE
    std::istringstream iss(line);
    std::vector<std::string> p; std::string tok; while(iss>>tok) p.push_back(tok);
    if (p.size()!=6) return false; // dst = a op b TYPE
    static const std::set<std::string> ops={"+","-","*","/","%","&","|","^","<<",">>","&&","||",">","<",">=","<=","!=","=="};
    return ops.count(p[3])>0 && p[1]=="=";
}

bool CodeGenerator::isDeclaration(const std::string& line) const{
    std::istringstream iss(line); std::vector<std::string> p; std::string tok; while(iss>>tok) p.push_back(tok);
    return p.size()==3 && p[0]=="-"; // - TYPE name
}

bool CodeGenerator::isAssignment(const std::string& line) const{
    std::istringstream iss(line); std::vector<std::string> p; std::string tok; while(iss>>tok) p.push_back(tok);
    return p.size()==4 && p[1]=="="; // x = y TYPE
}

bool CodeGenerator::isReturn(const std::string& line) const{ return line.rfind("return ",0)==0; }
bool CodeGenerator::isGoto(const std::string& line) const{ return line.rfind("GOTO ",0)==0 || line.rfind("goto ",0)==0; }
bool CodeGenerator::isIf(const std::string& line) const{ return line.rfind("if ",0)==0 || line.rfind("IF ",0)==0; }

