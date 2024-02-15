#include <iostream>
#include <fstream>
#include <unordered_map>

int main() {
    std::ifstream in("trace.txt");
    std::string s;
    std::unordered_map<std::string, size_t> instrs;
    size_t instrs_count = 0;
    while (in >> s) {
        instrs[s]++;
        instrs_count++;
    }
    for (const auto& [instr, count] : instrs) {
        std::cout << instr << " " << 100 * (double)count / instrs_count << "%" << std::endl;
    }
}