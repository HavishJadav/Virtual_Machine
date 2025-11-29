#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "include/CodeOptimizer.hpp"

int main(int argc, char** argv){
    if (argc < 2){
        std::cerr<<"Usage: tac_vm_translator <path-to-tac-file>\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in){ std::cerr<<"Failed to open input: "<<argv[1]<<"\n"; return 2; }
    std::ostringstream buf; buf<<in.rdbuf();

    vm::CodeOptimizer opt;
    std::string asmCode = opt.generateTargetCode(buf.str());

    // write to outputs/<basename>.asm
    std::filesystem::create_directories("outputs");
    std::filesystem::path inpath(argv[1]);
    std::filesystem::path outpath = std::filesystem::path("outputs") / inpath.stem();
    outpath += ".asm";
    std::ofstream out(outpath);
    out<<asmCode;

    std::cout<<"Wrote "<<outpath.string()<<"\n";
    return 0;
}
