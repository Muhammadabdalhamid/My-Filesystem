#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

/*
 * In-memory hierarchical file system (LeetCode-style).
 *
 * Supported operations:
 *   - ls(path)
 *   - mkdir(path)
 *   - addContentToFile(filePath, content)
 *   - readContentFromFile(filePath)
 *
 * Paths are absolute and use '/' as separator.
 * The root directory is "/".
 *
 * Notes:
 *   • mkdir behaves like `mkdir -p` (creates intermediate directories).
 *   • addContentToFile creates the file if it does not exist.
 *   • ls returns sorted names for directories.
 *   • ls on a file returns a single-element list with the file name.
 *   • Invalid operations may throw std::runtime_error (implementation-defined).
 */
class FileSystem {
public:
    FileSystem();
    ~FileSystem();

    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;

    FileSystem(FileSystem&&) noexcept;
    FileSystem& operator=(FileSystem&&) noexcept;

    /*
     * List directory contents or file name.
     *
     * Behavior:
     *   - If `path` refers to a directory:
     *         returns the names of all children (sorted lexicographically).
     *   - If `path` refers to a file:
     *         returns a single-element vector containing the file name.
     *
     * Examples:
     *   ls("/")            -> ["a", "b"]
     *   ls("/a")           -> ["file.txt"]
     *   ls("/a/file.txt")  -> ["file.txt"]
     */
    std::vector<std::string> ls(std::string_view path) const;

    /*
     * Create directories along the given path.
     *
     * Behavior:
     *   - Creates all missing intermediate directories.
     *   - Similar to Unix `mkdir -p`.
     *   - If directories already exist, nothing happens.
     *
     * Example:
     *   mkdir("/a/b/c") creates directories a, b, and c if missing.
     */
    void mkdir(std::string_view path);

    /*
     * Append content to a file.
     *
     * Behavior:
     *   - If the file does not exist, it is created.
     *   - Content is appended to existing file content.
     *
     * Example:
     *   addContentToFile("/a/x.txt", "Hello");
     *   addContentToFile("/a/x.txt", " World");
     *   readContentFromFile("/a/x.txt") -> "Hello World"
     */
    void addContentToFile(std::string_view filePath, std::string_view content);

    /*
     * Read file content.
     *
     * Behavior:
     *   - Returns the full content of the file.
     *   - Throws an error if the path refers to a directory
     *     or does not exist (implementation-defined).
     */
    std::string readContentFromFile(std::string_view filePath) const;

private:
    // PImpl: hides internal Node/Directory/File tree implementation
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};
