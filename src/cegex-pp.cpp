#include <iostream>
#include <vector>
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
    CAPTURE_OPEN, // 7
    CAPTURE_CLOSE, // 8
    CAPTURE, // 9
    REPEAT_NONE_OR_MORE, // 10
    REPEAT_ONCE_OR_MORE, // 11
    REPEAT_NONE_OR_ONCE, // 12
    
};


struct Expr {
    ExprType type;
    std::string content = "";
    std::deque<Expr> children = {};
};

using re_t = std::deque<Expr>;

struct ReMatch {
    int start_idx;
    unsigned int size;
    std::vector<std::string> captures;
};

#define PUSH_FIXED if (content.length()) toks.push_back(Expr{ExprType::FIXED, content}); content.clear();
re_t compile_repattern(std::string s) {
    std::string content;
    re_t toks;
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
            case '*':
            {
                PUSH_FIXED
                Expr preceding = toks.back();
                toks.push_back(Expr{ExprType::REPEAT_NONE_OR_MORE, "", {ExprType::STR_START, preceding}});
                break;
            }
            case '+':
            {
                PUSH_FIXED
                Expr preceding = toks.back();
                toks.push_back(Expr{ExprType::REPEAT_ONCE_OR_MORE, "", {ExprType::STR_START, preceding}});
                break;
            }
            case '?':
            {
                PUSH_FIXED
                Expr preceding = toks.back();
                toks.push_back(Expr{ExprType::REPEAT_NONE_OR_ONCE, "", {ExprType::STR_START, preceding}});
            }
            case '(':
                PUSH_FIXED
                toks.push_back(Expr{ExprType::CAPTURE_OPEN});
                bracket_stack.push(ExprType::CAPTURE_OPEN);
                break;
            case ')':
            {
                if (bracket_stack.size() == 0 || bracket_stack.top() != ExprType::CAPTURE_OPEN) {
                    content += ')';
                    break;
                }
                bracket_stack.pop();
                PUSH_FIXED
                Expr cs = Expr{ExprType::CAPTURE};
                cs.children.push_back(Expr{ExprType::STR_START});
                while (toks.back().type != ExprType::CAPTURE_OPEN) {
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

ReMatch match_repattern(re_t &pattern, std::string &text) {
    int match_start_idx = 0;
    unsigned int match_size = 0;
    std::vector<std::string> captures;
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
            } else if (expr.type == ExprType::CAPTURE) {
                std::string remain = text.substr(match_start_idx + (int) match_size);
                ReMatch m = match_repattern(expr.children, remain);
                if (m.start_idx == -1)
                    break;
                captures.push_back(remain.substr(0, m.size));
                match_size += m.size;
            } else if (expr.type == ExprType::REPEAT_NONE_OR_MORE || expr.type == ExprType::REPEAT_ONCE_OR_MORE || expr.type == ExprType::REPEAT_NONE_OR_ONCE) {
                std::string remain = text.substr(match_start_idx + (int) match_size);
                ReMatch m = match_repattern(expr.children, remain);
                if (expr.type == ExprType::REPEAT_ONCE_OR_MORE && m.start_idx == -1)
                    break;
                match_size += m.size;
                if (expr.type != ExprType::REPEAT_NONE_OR_ONCE) {
                    while (m.start_idx != -1 && (unsigned int) match_start_idx + match_size < text.length()) {
                        remain = text.substr(match_start_idx + (int) match_size);
                        m = match_repattern(expr.children, remain);
                        match_size += m.size;
                    }
                }
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
    
    if (!found || early_exit) match_start_idx = -1;
        
    return ReMatch {match_start_idx, match_size, captures};
};

void print_children_recurs(re_t &childs) {
    for (Expr e: childs) {
        std::cout << e.type << " " << e.content << std::endl;
        if (e.children.size())
            print_children_recurs(e.children);
    }
}

int main() {
    re_t compiled = compile_repattern(R"(a?)");
    print_children_recurs(compiled);
    
    std::string text = "aaad";
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