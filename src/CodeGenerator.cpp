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

std::pair<std::string,int> CodeGenerator::getIntRegFor(const std::string& var){
    // already in a register
    for (auto &kv: regDesc){ if (kv.second==var) return {kv.first,0}; }
    // free reg
    for (auto &r: availRegs){ if (regDesc.find(r)==regDesc.end() || regDesc[r].empty()){ regDesc[r]=var; return {r,1}; } }
    // spill round-robin
    if (availRegs.empty()) return {"x5",1};
    const std::string victim = availRegs[rrIdx]; rrIdx=(rrIdx+1)%availRegs.size();
    // spill victim to stack
    std::string v = regDesc[victim];
    if (!v.empty()){
        if (addrDesc.find(v)==addrDesc.end()) { spOffset+=4; addrDesc[v] = {spOffset,victim}; }
        std::ostringstream os; os<<"    sw "<<victim<<",-"<<addrDesc[v].offset<<"(x8)"; emit(os.str());
    }
    regDesc[victim]=var; return {victim,1};
}

void CodeGenerator::spillAll(){
    emit("# ---- start of spill ----");
    for (auto &kv: addrDesc){
        const std::string &var = kv.first; auto &ad = kv.second; (void)var;
        // find reg that currently holds var
        for (auto &rkv: regDesc){ if (rkv.second==var){ std::ostringstream os; os<<"    sw "<<rkv.first<<",-"<<ad.offset<<"(x8)"; emit(os.str()); rkv.second.clear(); }}
    }
    emit("# ---- end of spill ----");
}

// Helper: filter out unwanted lines before emitting
namespace {
static bool should_emit_line(const std::string& line,bool& global_main_emitted) {
    if (line.empty()) return false;
    // Remove - INT ... and similar declarations
    {
        std::istringstream iss(line); std::string dash,type,name;
        if (iss >> dash >> type >> name) {
            if (dash == "-" && (type == "INT" || type == "FLOAT" || type == "BOOL" || type == "CHAR" || type == "STR")) {
                return false;
            }
        }
    }
    // Remove print ... lines
    if (line.rfind("print ",0) == 0) return false;
    // Remove repeated .global main
    if (line == ".global main") {
        if (global_main_emitted) return false;
        global_main_emitted = true;
    }
    return true;
}

// Overload emit to filter lines
}

void CodeGenerator::emit(const std::string& line) {
    static bool global_main_emitted = false;
    if (should_emit_line(line,global_main_emitted)) {
        text_segment.push_back(line);
    }
}

std::string CodeGenerator::generate(const std::vector<std::string>& blocks,const std::string& outLabelHint){
    text_segment.clear(); data_segment.clear(); regDesc.clear(); addrDesc.clear(); rrIdx=0; spOffset=0;

    // Reset static for each generate call
    {
        static bool global_main_emitted = false;
        global_main_emitted = false;
    }


    // Flatten all lines from all blocks into a single vector
    std::vector<std::string> lines_flat;
    for (const auto& block : blocks) {
        std::istringstream iss(block);
        std::string l;
        while (std::getline(iss,l)) {
            if (!l.empty()) lines_flat.push_back(l);
        }
    }

    for (size_t i = 0; i < lines_flat.size(); ++i) {
        const std::string& line = lines_flat[i];
        // Check for string assignment: x = "..." STR
        std::smatch m_str_assign;
        std::regex re_str_assign(R"(^([\w@]+)\s*=\s*\"(.*)\"\s+STR$)",std::regex::icase);
        if (std::regex_match(line,m_str_assign,re_str_assign)) {
            std::string dst = m_str_assign[1];
            std::string strval = m_str_assign[2];
            // Create a unique label for the string
            std::string label = dst;
            data_segment.push_back(label + ": \n.asciz \"" + strval + "\"");

            // Hardcoded base address for .data section (example: 0x10010000)
            std::string base_hi = "0x10010"; // upper 20 bits
            int offset = 0;
            // Calculate offset for each label
            for (const auto& l : data_segment) {
                if (l.find(label + ":") == 0) break;
                // Estimate offset by counting string lengths (+1 for null terminator)
                size_t pos1 = l.find(": .asciz \"");
                size_t pos2 = l.rfind("\"");
                if (pos1 != std::string::npos && pos2 != std::string::npos && pos2 > pos1+10) {
                    offset += (int)(pos2 - (pos1+10)) + 1;
                }
            }

            auto [r,_n] = getIntRegFor(dst);
            emit("    lui " + r + "," + base_hi);
            emit("    addi " + r + "," + r + ",0x0");
            if (offset > 0) {
                std::ostringstream oss;
                oss << std::hex << offset;
                std::string hex_offset = oss.str();
                if (hex_offset.length() == 1) hex_offset = "0" + hex_offset;
                emit("    addi " + r + "," + r + ",0x" + hex_offset);
            }
            // Optionally,store address in stack
            if (addrDesc.find(dst) == addrDesc.end()) { spOffset += 4; addrDesc[dst] = {spOffset,r}; }
            std::ostringstream os; os << "    sw " << r << ",-" << addrDesc[dst].offset << "(x8)"; emit(os.str());
            continue;
        }

        // Check for string printing: print x STR
        std::smatch m_str_print;
        std::regex re_str_print(R"(^print\s+([\w@]+)\s+STR$)",std::regex::icase);
        if (std::regex_match(line,m_str_print,re_str_print)) {
            std::string var = m_str_print[1];
            // Load address from stack to a register
            auto [r,_n] = getIntRegFor(var);
            if (addrDesc.find(var) != addrDesc.end()) {
                std::ostringstream os; os << "    lw " << r << ",-" << addrDesc[var].offset << "(x8)"; emit(os.str());
            }
            emit("    addi a0," + r + ",0"); // mv a0,r
            emit("    addi a7,x0,4"); // li a7,4
            emit("    ecall");
            continue;
        }

        // ...existing code for each line...
        std::smatch m_cmp,m_if;
        std::regex re_cmp(R"(^([\w@]+)\s*=\s*([\w@]+)\s*([<>=!]+)\s*([\w@\d]+)\s+INT$)",std::regex::icase);
        std::regex re_if(R"(^if\s+([\w@]+)\s+GOTO\s+([#\w]+)\s+else\s+GOTO\s+([#\w]+))",std::regex::icase);
        if (i+1 < lines_flat.size() && std::regex_match(line,m_cmp,re_cmp) && std::regex_match(lines_flat[i+1],m_if,re_if)) {
            std::string cmp_var = m_cmp[1];
            std::string lhs = m_cmp[2];
            std::string op = m_cmp[3];
            std::string rhs = m_cmp[4];
            std::string if_var = m_if[1];
            std::string label_true = m_if[2];
            std::string label_false = m_if[3];
            if (!label_true.empty() && label_true[0] == '#') label_true = label_true.substr(1);
            if (!label_false.empty() && label_false[0] == '#') label_false = label_false.substr(1);
            if (cmp_var == if_var) {
                // Generate direct branch for the comparison
                auto [r1,_n1] = getIntRegFor(lhs);
                auto [r2,_n2] = getIntRegFor(rhs);
                if (isNumber(lhs)) emit("    li "+r1+","+lhs);
                if (isNumber(rhs)) emit("    li "+r2+","+rhs);
                if (op=="<")
                    emit("    blt "+r1+","+r2+","+label_true);
                else if (op==">")
                    emit("    bgt "+r1+","+r2+","+label_true);
                else if (op=="<=")
                    emit("    ble "+r1+","+r2+","+label_true);
                else if (op==">=")
                    emit("    bge "+r1+","+r2+","+label_true);
                else if (op=="==")
                    emit("    beq "+r1+","+r2+","+label_true);
                else if (op=="!=")
                    emit("    bne "+r1+","+r2+","+label_true);
                else
                    emit("    # TODO: unsupported op in if: "+op);
                emit("    beq x0,x0,"+label_false);
                ++i; // skip the if line
                continue;
            }
        }
        if (line.back()==':'){
            std::string label_line = line;
            if (!label_line.empty() && label_line[0] == '#') label_line = label_line.substr(1);
            emit(label_line);
            continue;
        }
        if (isGoto(line)){
            spillAll();
            std::string label = line.substr(line.find(' ')+1);
            if (!label.empty() && label[0] == '#') label = label.substr(1);
            emit("    beq x0,x0,"+label);
            continue;
        }
    if (isReturn(line)){ std::istringstream iss(line); std::string kw,val,typ; iss>>kw>>val>>typ; auto [r,_new] = getIntRegFor(val); if (isNumber(val)) emit("    addi "+r+",x0,"+val); emit("    addi a0,"+r+",0"); emit("    addi x5,x0,0"); emit("    sw x5,-60(x8)"); continue; }
    if (isAssignment(line)){ std::istringstream iss(line); std::string dst,eq,src,typ; iss>>dst>>eq>>src>>typ; auto [r,_n] = getIntRegFor(dst); if (isNumber(src)) emit("    addi "+r+",x0,"+src); else { if (addrDesc.find(src)==addrDesc.end()) { spOffset+=4; addrDesc[src] = {spOffset,""}; } std::ostringstream os; os<<"    lw "<<r<<",-"<<addrDesc[src].offset<<"(x8)"; emit(os.str()); } if (addrDesc.find(dst)==addrDesc.end()) { spOffset+=4; addrDesc[dst] = {spOffset,r}; } std::ostringstream os; os<<"    sw "<<r<<",-"<<addrDesc[dst].offset<<"(x8)"; emit(os.str()); continue; }
    if (isBinaryArithmetic(line)){ std::istringstream iss(line); std::string dst,eq,a,op,b,typ; iss>>dst>>eq>>a>>op>>b>>typ; auto [rd,_n0] = getIntRegFor(dst); auto [ra,_n1] = getIntRegFor(a); auto [rb,_n2] = getIntRegFor(b); if (isNumber(a)) emit("    addi "+ra+",x0,"+a); else { if (addrDesc.find(a)==addrDesc.end()) { spOffset+=4; addrDesc[a] = {spOffset,""}; } std::ostringstream os; os<<"    lw "<<ra<<",-"<<addrDesc[a].offset<<"(x8)"; emit(os.str()); } if (isNumber(b)) emit("    addi "+rb+",x0,"+b); else { if (addrDesc.find(b)==addrDesc.end()) { spOffset+=4; addrDesc[b] = {spOffset,""}; } std::ostringstream os; os<<"    lw "<<rb<<",-"<<addrDesc[b].offset<<"(x8)"; emit(os.str()); } std::string rvop; if (op=="+") rvop="add"; else if (op=="-") rvop="sub"; else if (op=="*") rvop="mul"; else if (op=="/") rvop="div"; else if (op=="%") rvop="rem"; else if (op=="&") rvop="and"; else if (op=="|") rvop="or"; else if (op=="^") rvop="xor"; else if (op=="<<") rvop="sll"; else if (op==">>") rvop="srl"; else rvop="add"; emit("    "+rvop+" "+rd+","+ra+","+rb); if (addrDesc.find(dst)==addrDesc.end()) { spOffset+=4; addrDesc[dst] = {spOffset,rd}; } std::ostringstream os; os<<"    sw "<<rd<<",-"<<addrDesc[dst].offset<<"(x8)"; emit(os.str()); continue; }
    if (isIf(line)){
        std::smatch m;
        std::regex re_cmp(R"(^if\s+([\w@]+)\s*([<>=!]+)\s*([\w@\d]+)\s+GOTO\s+([#\w]+)\s+else\s+GOTO\s+([#\w]+))",std::regex::icase);
        if (std::regex_match(line,m,re_cmp)) {
            std::string lhs = m[1];
            std::string op = m[2];
            std::string rhs = m[3];
            std::string label_true = m[4];
            std::string label_false = m[5];
            if (!label_true.empty() && label_true[0] == '#') label_true = label_true.substr(1);
            if (!label_false.empty() && label_false[0] == '#') label_false = label_false.substr(1);
            auto [r1,_n1] = getIntRegFor(lhs);
            auto [r2,_n2] = getIntRegFor(rhs);
            if (isNumber(lhs)) emit("    li "+r1+","+lhs);
            if (isNumber(rhs)) emit("    li "+r2+","+rhs);
            if (op=="<") emit("    blt "+r1+","+r2+","+label_true);
            else if (op==">") emit("    bgt "+r1+","+r2+","+label_true);
            else if (op=="<=") emit("    ble "+r1+","+r2+","+label_true);
            else if (op==">=") emit("    bge "+r1+","+r2+","+label_true);
            else if (op=="==") emit("    beq "+r1+","+r2+","+label_true);
            else if (op=="!=") emit("    bne "+r1+","+r2+","+label_true);
            else emit("    # TODO: unsupported op in if: "+op);
            emit("    beq x0,x0,"+label_false);
            continue;
        } else {
            std::regex re_simple(R"(^if\s+([\w@]+)\s+GOTO\s+([#\w]+)\s+else\s+GOTO\s+([#\w]+))",std::regex::icase);
            if (std::regex_match(line,m,re_simple)) {
                std::string cond = m[1];
                std::string label_true = m[2];
                std::string label_false = m[3];
                if (!label_true.empty() && label_true[0] == '#') label_true = label_true.substr(1);
                if (!label_false.empty() && label_false[0] == '#') label_false = label_false.substr(1);
                auto [r,_n] = getIntRegFor(cond);
                emit("    beq "+r+",x0,"+label_false);
            } else {
                emit("    # TODO if-handling: "+line);
            }
            continue;
        }
    }
        emit(line);
        // ...end existing code for each line...
    }

    std::ostringstream out;
    if (!data_segment.empty()){
        out << ".section\n .data\n";
        for (auto &l : data_segment) out << l << "\n";
    }
    out << ".section\n.text\n";
    // Remove .global {function_name} from output
    for (auto &l : text_segment) {
        if (l != (".global " + outLabelHint)) out << l << "\n";
    }
    return out.str();
}

} // namespace vm
