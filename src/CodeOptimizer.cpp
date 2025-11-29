#include "../include/CodeOptimizer.hpp"
#include "../include/CodeGenerator.hpp"
#include <sstream>
#include <fstream>
#include <filesystem>

namespace vm {

static inline std::string trim(const std::string& s){
    size_t a= s.find_first_not_of(" \t\r\n"); if (a==std::string::npos) return ""; size_t b=s.find_last_not_of(" \t\r\n"); return s.substr(a,b-a+1);
}

std::string CodeOptimizer::preOptimizations(const std::string& tac){
    // very light: remove duplicate consecutive returns and empty lines
    std::istringstream in(tac); std::string line, prev; std::ostringstream out;
    while(std::getline(in,line)){
        std::string t=trim(line);
        if (t.empty()) continue;
        if (t.rfind("return ",0)==0 && prev.rfind("return ",0)==0) continue;
        out<<line<<"\n"; prev=t;
    }
    return out.str();
}

std::vector<std::string> CodeOptimizer::buildBlocks(const std::string& tac){
    std::vector<std::string> blocks; std::ostringstream cur;
    std::istringstream in(tac); std::string line;
    while(std::getline(in,line)){
        std::string t=trim(line);
        if (t.empty()) continue;
        if (t.back()==':'){
            if (cur.tellp()>0){ blocks.push_back(cur.str()); cur.str(""); cur.clear(); }
            cur<<t<<"\n";
        } else if (t.rfind("goto ",0)==0 || t.rfind("GOTO ",0)==0 || t.rfind("return ",0)==0){
            cur<<t<<"\n"; blocks.push_back(cur.str()); cur.str(""); cur.clear();
        } else {
            cur<<t<<"\n";
        }
    }
    if (cur.tellp()>0) blocks.push_back(cur.str());
    return blocks;
}

std::string CodeOptimizer::generateTargetCode(const std::string& tac){
    auto pre = preOptimizations(tac);
    auto blocks = buildBlocks(pre);

    CodeGenerator gen;
    std::string asmCode = gen.generate(blocks, "main");

    return asmCode;
}

} // namespace vm
