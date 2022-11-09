# cegex-pp

RegEx library in c++, for educational purposes. Use at your own risk.

Implemented operators: `^$.*+?[]()\\`

## Build
```make```
or
```make build && make test```

## Usage
```
// Pattern that finds one or more consecutive lowercase characters
re_t compiled = compile_repattern(R"[a-z]+") // R"()" -> raw string
std::string text = "Regex";
ReMatch m = match_repattern(compiled, text); // Get first encountered match
// m.start_idx -- 1
// m.size --  4
// m.captures -- Empty vector<std::string>
```
> Refer to `src/test.cpp` for more