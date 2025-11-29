#pragma once
#include <string>
#include <vector>

namespace vm {

class CodeOptimizer {
public:
    CodeOptimizer() = default;

    // High-level entry similar to Python generate_target_code
    // Returns final assembly string
    std::string generateTargetCode(const std::string& tac);

private:
    // very light preprocessing to keep parity
    std::string preOptimizations(const std::string& tac);

    // split into basic blocks (very simplified)
    std::vector<std::string> buildBlocks(const std::string& tac);
};

} // namespace vm
