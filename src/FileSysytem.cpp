#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "FileSystem.hpp"

namespace {
struct Node {
    explicit Node(std::string name = {}) : name_(std::move(name)) {}
    virtual ~Node() = default;

    const std::string& name() const { return name_; }

    virtual bool isFile() const = 0;

    // LeetCode ls semantics when this node is the target:
    // - File: returns { filename }
    // - Directory: returns sorted children names
    virtual std::vector<std::string> ls() const = 0;

private:
    std::string name_;
};

struct File final : Node {
    explicit File(std::string name = {}, std::string content = {})
        : Node(std::move(name)), content_(std::move(content)) {}

    bool isFile() const override { return true; }

    std::vector<std::string> ls() const override {
        return { name() };
    }

    void appendContent(std::string_view newContent) {
        content_ += newContent;
    }

    const std::string& readContent() const {
        return content_;
    }

private:
    std::string content_;
};

struct Directory final : Node {
    explicit Directory(std::string name = {}) : Node(std::move(name)) {}

    bool isFile() const override { return false; }

    std::vector<std::string> ls() const override {
        std::vector<std::string> result;
        result.reserve(children_.size());
        for (const auto& [childName, _] : children_) {
            result.push_back(childName);
        }
        // std::map keeps keys sorted
        return result;
    }

    // Returns pointer to child node if it exists, otherwise nullptr.
    Node* getChild(const std::string& childName) {
        auto it = children_.find(childName);
        return (it == children_.end()) ? nullptr : it->second.get();
    }

    const Node* getChild(const std::string& childName) const {
        auto it = children_.find(childName);
        return (it == children_.end()) ? nullptr : it->second.get();
    }

    // Ensures a directory child exists. If missing, creates it.
    // Throws if a file exists with the same name.
    Directory* ensureDir(const std::string& dirName) {
        auto it = children_.find(dirName);
        if (it == children_.end()) {
            auto newDir = std::make_unique<Directory>(dirName);
            Directory* ptr = newDir.get();
            children_.emplace(dirName, std::move(newDir));
            return ptr;
        }

        Node* child = it->second.get();
        if (child->isFile()) {
            throw std::runtime_error("Expected directory but found file: " + dirName);
        }
        return static_cast<Directory*>(child);
    }

    // Ensures a file child exists. If missing, creates it.
    // Throws if a directory exists with the same name.
    File* ensureFile(const std::string& fileName) {
        auto it = children_.find(fileName);
        if (it == children_.end()) {
            auto newFile = std::make_unique<File>(fileName);
            File* ptr = newFile.get();
            children_.emplace(fileName, std::move(newFile));
            return ptr;
        }

        Node* child = it->second.get();
        if (!child->isFile()) {
            throw std::runtime_error("Expected file but found directory: " + fileName);
        }
        return static_cast<File*>(child);
    }

private:
    std::map<std::string, std::unique_ptr<Node>> children_;
};

} // namespace

struct FileSystem::Impl{
    std::unique_ptr<Directory> root;
    Impl() : root(std::make_unique<Directory>("")) {}
    ~Impl() = default;
    std::vector<std::string> splitPath(std::string_view path) const{
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

    template <typename DirPtr, typename NodePtr>
    NodePtr resolveNodeImpl(DirPtr rootDir, std::string_view path) const {
        if (path.empty() || path.front() != '/') {
            throw std::runtime_error("Path must be absolute and start with '/'");
        }

        auto parts = splitPath(path);

        if (parts.empty()) {
            return static_cast<NodePtr>(rootDir);
        }

        DirPtr current = rootDir;
        for (std::size_t i = 0; i + 1 < parts.size(); ++i) {
            const auto& part = parts[i];

            NodePtr child = current->getChild(part);
            if (!child) {
                throw std::runtime_error("Path component does not exist: " + std::string(part));
            }

            if (child->isFile()) {
                throw std::runtime_error("Expected directory but found file: " + std::string(part));
            }

            current = static_cast<DirPtr>(child);
        }

        const auto& leaf = parts.back();
        NodePtr leafNode = current->getChild(leaf);
        if (!leafNode) {
            throw std::runtime_error("Path does not exist: " + std::string(leaf));
        }

        return leafNode;
    }

    const Node* resolveNode(std::string_view path) const {
        return resolveNodeImpl<const Directory*, const Node*>(root.get(), path);
    }

    Node* resolveNode(std::string_view path) {
        return resolveNodeImpl<Directory*, Node*>(root.get(), path);
    }

    Directory* ensureDirPath(std::string_view path) {
        if (path.empty() || path.front() != '/') {
            throw std::runtime_error("Path must be absolute and start with '/'");
        }

        auto parts = splitPath(path);

        Directory* current = root.get();
        for (const auto& part : parts) {
            current = current->ensureDir(part);
        }
        return current;
    }

    File* ensureFilePath(std::string_view path) {
        if (path.empty() || path.front() != '/') {
            throw std::runtime_error("Path must be absolute and start with '/'");
        }

        auto parts = splitPath(path);
        if (parts.empty()) {
            throw std::runtime_error("File path cannot be root directory");
        }

        Directory* current = root.get();
        for (std::size_t i = 0; i + 1 < parts.size(); ++i) {
            current = current->ensureDir(parts[i]);
        }
        return current->ensureFile(parts.back());
    }
};