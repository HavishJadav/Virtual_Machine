#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <optional>

namespace vm {

enum class Datatype { INT, FLOAT, BOOL, CHAR, STR, UNKNOWN };

struct AddrDesc {
    int offset = 0;
    std::string reg; // empty if not in register
};

struct FunctionContext {
    std::string name;
    int nextArg = 0;         // how many - arg processed
    int frameSize = 32;      // simple fixed frame size (aligned); can grow
    std::unordered_map<std::string,int> localOffset; // var -> positive bytes; used as -offset(s0)
};

class CodeGenerator {
public:
    CodeGenerator();

    // Public API similar to Python main(blocks)
    std::string generate(const std::vector<std::string>& blocks, const std::string& outLabelHint = "main");

private:
    // helpers
    Datatype toDatatype(const std::string& s) const;
    bool isBinaryArithmetic(const std::string& line) const;
    bool isDeclaration(const std::string& line) const;
    bool isAssignment(const std::string& line) const;
    bool isReturn(const std::string& line) const;
    bool isGoto(const std::string& line) const;
    bool isIf(const std::string& line) const;

    std::pair<std::string,int> getIntRegFor(const std::string& var);
    void spillAll();

    // function/call lowering
    std::optional<std::vector<std::string>> lowerFunctionMeta(const std::string& line);
    std::optional<std::vector<std::string>> lowerParamOrCall(const std::string& line);

    // local management and operand materialization
    int allocateLocal(const std::string& var);
    std::string materialize(const std::string& src, std::vector<std::string>& out);
    std::string ensureRegFor(const std::string& var, std::vector<std::string>& out);
    void spillIfNeeded(const std::string& var, const std::string& reg, std::vector<std::string>& out);

    // emit helpers
    void emit(const std::string& s);

private:
    // descriptors and state
    std::vector<std::string> availRegs;
    std::unordered_map<std::string,std::string> regDesc; // reg -> var
    std::unordered_map<std::string,AddrDesc> addrDesc;   // var -> addr
    int rrIdx = 0;

    int spOffset = 0; // bytes allocated below sp (positive growing)

    std::optional<FunctionContext> currentFunc; // active function context
    int pendingArgIdx = 0; // a0..a7 index for next call

    // sections
    std::vector<std::string> text_segment;
    std::vector<std::string> data_segment;
};

} // namespace vm
