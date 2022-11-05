#include "cegex-pp.hpp"

void print_children_recurs(re_t &childs) {
    for (Expr e: childs) {
        std::cout << e.type << " " << e.content << std::endl;
        if (e.children.size()) {
            std::cout << "CHILDREN START" << std::endl;
            print_children_recurs(e.children);
            std::cout << "CHILDREN END" << std::endl;
        }
    }
}

int main() {
    re_t compiled = compile_repattern(R"(^(www\.)?[a-z0-9]+\.[a-z]+(/[a-zA-Z0-9]*)*$)");
    // print_children_recurs(compiled);
    std::string text = "www.google.com/asd/";
    ReMatch m = match_repattern(compiled, text);

    std::cout << "\n- Parse - \n";
    std::cout << "Index: " << m.start_idx << "\n";
    std::cout << "Size: " << m.size << std::endl; 
    std::cout << "Matches: ";
    for (auto cap: m.captures) {
      std::cout << cap << " ";  
    }
    return 0;
}
