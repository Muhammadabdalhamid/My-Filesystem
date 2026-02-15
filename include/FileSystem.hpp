#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

class FileSystem {
public:
    FileSystem();
    ~FileSystem();                              // needed with PImpl + unique_ptr

    FileSystem(const FileSystem&) = delete;      // keep it simple for now
    FileSystem& operator=(const FileSystem&) = delete;

    FileSystem(FileSystem&&) noexcept;           // allow move
    FileSystem& operator=(FileSystem&&) noexcept;

    std::vector<std::string> ls(std::string_view path) const;
    void mkdir(std::string_view path);
    void addContentToFile(std::string_view filePath, std::string_view content);
    std::string readContentFromFile(std::string_view filePath) const;

private:
    struct Impl;                     // forward declaration (in .cpp)
    std::unique_ptr<Impl> pimpl_;    // owns the real implementation
};
