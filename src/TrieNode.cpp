#include "../includes/webserv.hpp"

void split(const std::string& str, std::vector<std::string>& components) {
    std::string item;
    std::stringstream ss(str);
    while (std::getline(ss, item, '/')) {
        components.push_back(item);
    }
}

void insert(TrieNode* root, const std::string& path, unsigned char permissions) {
    std::vector<std::string> components;
    split(path, components);

    TrieNode* current = root;
    for (std::vector<std::string>::iterator it = components.begin();
    it != components.end(); ++it)
    {
        if (current->children.find(*it) == current->children.end())
            current->children[*it] = new TrieNode();
        current = current->children[*it];
    }
    current->permissions = permissions;
}

TrieNode* findBestMatch(TrieNode* root, const std::string& filepath) {
    std::vector<std::string> filepathComponents;
    split(filepath, filepathComponents);
    TrieNode* current = root;
    TrieNode* bestMatchNode = NULL;

    for (std::vector<std::string>::iterator it = filepathComponents.begin();
    it != filepathComponents.end(); ++it)
    {
        if (current->children.find(*it) != current->children.end())
        {
            current = current->children[*it];
            bestMatchNode = current;
        } else {
            break;
        }
    }
    return bestMatchNode;
}

void deleteTrie(TrieNode* root) {
    delete root;
}

