#include <iostream>



std::vector<std::string> TestSplitPath(std::string_view path){
    std::vector<std::string> parts;
    size_t start = 0;
    while (start < path.size()) {
        size_t end = path.find('/', start + 1);
        if(end == std::string_view::npos) {
            end = path.size();
        }
        std::string part(path.substr(start + 1, end - start - 1));
        if (!part.empty()) {
            parts.push_back(std::move(part));
        }
        start = end;
    }
    return parts;
}

int main() {
    std::cout << "Hello FileSystem!\n";
    auto parts = TestSplitPath("/afs/ba/cert/123");
    for (const auto& part : parts) {
        std::cout << part << "\n";
    }
    return 0;
}