#include <iostream>
#include <deque>
#include <stack>
#include <map>

enum ExprType {
    FIXED, // 0
    STR_START, // 1
    STR_END, // 2
    WILDCARD, // 3
    WORD, // 4
    DIGIT, // 5
    WHITESPACE, // 6
    CHARSET_OPEN, // 7
    CHARSET_CLOSE, // 8
    CHARSET // 9
};

struct Expr {
    ExprType type;
    std::string content = "";
    std::deque<Expr> children = {};
};

#define PUSH_FIXED if (content.length()) toks.push_back(Expr{ExprType::FIXED, content}); content.clear();
std::deque<Expr> compile_repattern(std::string s) {
    std::string content;
    std::deque<Expr> toks;
    std::stack<char> bracket_stack; 
    for (unsigned int i=0; i < s.length(); i++) {
        switch (s[i]) {
            case '^':
                PUSH_FIXED
                toks.push_back(Expr{ExprType::STR_START});
                break;
            case '$':
                PUSH_FIXED
                toks.push_back(Expr{ExprType::STR_END});
                break;
            case '.':
                PUSH_FIXED
                toks.push_back(Expr{ExprType::WILDCARD});
                break;
            case '[':
                PUSH_FIXED
                toks.push_back(Expr{ExprType::CHARSET_OPEN});
                bracket_stack.push(ExprType::CHARSET_OPEN);
                break;
            case ']':
            {
                if (bracket_stack.size() == 0 || bracket_stack.top() != ExprType::CHARSET_OPEN) {
                    content += ']';
                    break;
                }
                bracket_stack.pop();
                PUSH_FIXED
                Expr cs = Expr{ExprType::CHARSET};
                while (toks.back().type != ExprType::CHARSET_OPEN) {
                    cs.children.push_back(toks.back());
                    toks.pop_back();
                }
                toks.pop_back();             
                toks.push_back(cs);
                break;
            }
            case '\\':
                if (i == s.length() - 1) {
                    content += '\\';
                    PUSH_FIXED
                    break;
                }
                switch (s[i+1]) {
                    case 'n':
                        content += '\n';
                        break;
                    case 't':
                        content += '\t';
                        break;
                    case 'r':
                        content += '\r';
                        break;
                    case 'w':
                        PUSH_FIXED
                        toks.push_back(Expr{ExprType::WORD});
                        break;
                    case 'd':
                        PUSH_FIXED
                        toks.push_back(Expr{ExprType::DIGIT});
                        break;
                    case 's':
                        PUSH_FIXED
                        toks.push_back(Expr{ExprType::WHITESPACE});
                        break;
                    default:
                        content += s[i+1];
                }
                i++;
                break;
            default:
                content += s[i];
        }
    }
    PUSH_FIXED
    return toks;
}

int match_repattern(std::deque<Expr> pattern, std::string text, unsigned int* match_size_ret) {
    int match_start_idx = 0;
    unsigned int match_size = 0;
    bool found = false;
    bool early_exit = false;
    while (!found && !early_exit && match_start_idx < (int) text.length()) {
        unsigned int matching_exprs = 0;
        for (Expr expr: pattern) {
            if (expr.type == ExprType::FIXED) {
                std::string s = text.substr(match_start_idx + match_size, expr.content.length());
                if (expr.content == s)
                    match_size += s.length();
                else
                    break;
            } else if (expr.type == ExprType::STR_START) {
                if (match_start_idx > 0) {
                    early_exit = true;
                    break;
                }
            } else if (expr.type == ExprType::STR_END) {
                if (match_start_idx + match_size - 1 < text.length() - 1) {
                    early_exit = true;
                    break;
                }
            } else if (expr.type == ExprType::WILDCARD) {
                int idx = match_start_idx + match_size - 1;
                if (idx == (int) text.length() - 1 || text[idx] == '\n') 
                    break;
                match_size++;
            } else if (expr.type == ExprType::WORD) {
                int idx = match_start_idx + match_size - 1;
                std::cout << "WORD IDX " << idx << " " << match_size << text[idx+1] << std::endl;
                if (idx == (int) text.length() - 1 || (!isalnum(text[idx+1]) && text[idx+1] != '_'))
                    break;
                match_size++;
            } else if (expr.type == ExprType::DIGIT) {
                int idx = match_start_idx + match_size - 1;
                if (idx == (int) text.length() - 1 || !isdigit(text[idx+1]))
                    break;
                match_size++;                
            } else if (expr.type == ExprType::WHITESPACE) {
                int idx = match_start_idx + match_size - 1;
                if (idx == (int) text.length() - 1 || !isspace(text[idx+1])) 
                    break;
                match_size++;
            }
            
            matching_exprs++;
            if (matching_exprs == pattern.size())
                found = true;
        }
        
        if (!found) {
            match_start_idx++;
            match_size = 0;
        }
    }
    
    *match_size_ret = match_size;
    if (!found || early_exit)
        return -1;
    return match_start_idx;
};

void print_children_recurs(std::deque<Expr> childs) {
    for (Expr e: childs) {
        std::cout << e.type << " " << e.content << std::endl;
        if (e.children.size())
            print_children_recurs(e.children);
    }
}

int main() {
    std::deque<Expr> compiled = compile_repattern(R"([asd])");

    print_children_recurs(compiled);

    unsigned int size;
    int idx = match_repattern(compiled, "asd", &size);
    std::cout << "\n- Parse - \n";
    std::cout << "Index: " << idx << "\n";
    std::cout << "Size: " << size << std::endl;    
    return 0;
}
