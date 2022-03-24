#ifndef HUFFMAN_TABLE_HPP
#define HUFFMAN_TABLE_HPP

#include <map>
#include <vector>
#include <queue>
#include <iostream>

typedef struct huffman_node {
    std::string * ascii;
    int  freq;
    bool leaf;
    huffman_node * left = NULL;
    huffman_node * right = NULL;
} huffman_node;

void huffman_tree_builder(huffman_node * h, std::vector<bool> &prefix, std::map<std::string, std::vector<bool>> * _a_to_bin) {
	std::vector<bool> lprefix = prefix;
	std::vector<bool> rprefix = prefix;
	lprefix.push_back(false);
	rprefix.push_back(true);
    std::string pstr;
    for (int i = 0; i < prefix.size(); i++) {pstr.append(prefix[i] ? "1" : "0");}
	if (h->leaf){ 
        // std::cout << std::string(h->ascii) << std::endl; 
        (*_a_to_bin)[*h->ascii] = prefix; 
    }
	if (h->left != NULL) {
		huffman_tree_builder(h->left, lprefix, _a_to_bin);
	}
	if (h->right != NULL) {
		huffman_tree_builder(h->right, rprefix, _a_to_bin);
	}
}

std::string huffman_tree_decoder(huffman_node * root, std::vector<bool> * str) {
    std::string plaintext;
    huffman_node * cur = root;
    // std::cout << root << std::endl;
    // std::cout << root->freq << std::endl;
    // std::cout << root->leaf << std::endl;
    // std::cout << root->left << std::endl;
    // std::cout << root->right << std::endl;

    // std::cout << "decoded ";
    for (int i = 0; i < str->size(); i++) {
        if (str->at(i) != true) {
            cur = cur->left;
        }
        else {
            cur = cur->right;
        }
        // std::cout << str->at(i) ? "1 " : "0 ";


        if (cur->leaf) {
            plaintext = *cur->ascii;
            // std::cout << " as " << cur->ascii << std::endl;
            // std::cout << "decoded ";
            std::string current_code;
            cur = root;
        }
    }
    // std::cout << std::endl;
    return plaintext;
}

class huffman_table {
public:
    huffman_table() { 
        _ascii_to_bits = new std::map<std::string, std::vector<bool>>();
        _bits_to_ascii = new std::map<std::vector<bool>, std::string>();
    }

    huffman_table(const std::map<std::string, int> & entries) {
        huffman_node ** tmp = (huffman_node **) malloc(sizeof(huffman_node *));
        huffman_node * root;
        _ascii_to_bits = new std::map<std::string, std::vector<bool>>();
        _bits_to_ascii = new std::map<std::vector<bool>, std::string>();

        auto cmp = [](huffman_node* left, huffman_node* right) { return left->freq > right->freq; };
        std::priority_queue<huffman_node*, std::vector<huffman_node*>, decltype(cmp)> q(cmp);

        // Create standalone huffman nodes for each
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            // std::cout << it->first;
            *tmp = (huffman_node *) malloc(sizeof(huffman_node));
            (*tmp)->ascii = &(it->first);
            (*tmp)->freq = it->second;
            (*tmp)->left = NULL;
            (*tmp)->right = NULL;
            (*tmp)->leaf = true;
            q.emplace(*tmp);
        }

        huffman_node * left_child;
        huffman_node * right_child;
        while (q.size() > 1) {

            left_child = q.top();
            q.pop();

            right_child = q.top();
            q.pop();

            *tmp = (huffman_node *) malloc(sizeof(huffman_node));

            (*tmp)->freq = left_child->freq + right_child->freq;
            (*tmp)->leaf = false;
            (*tmp)->left = left_child;
            (*tmp)->right = right_child;
            q.emplace(*tmp);
        }

        // The last remaining node must be the root of the huffman tree
        if (q.size() == 1) {
            root = q.top();
            q.pop();
            // std::cout << root->ascii << " " << root->freq << std::endl;

            std::vector<bool> prefix = {  };
            huffman_tree_builder(root, prefix, _ascii_to_bits);
        }

        // Create the reverse map
        for (auto it = (*_ascii_to_bits).begin(); it != (*_ascii_to_bits).end(); ++it) {
            (*_bits_to_ascii)[it->second] = it->first;
        }

        _huffman_tree = root;
    }

    ~huffman_table() {
        delete _ascii_to_bits;
        delete _bits_to_ascii;
      }

    void print() {
        for (auto it = _ascii_to_bits->begin(); it != _ascii_to_bits->end(); ++it) {
            std::string code;
            for (auto c = it->second.begin(); c != it->second.end(); ++c) {
                code += *c ? '1' : '0';
            }
            // std::cout << it->first << " : " << code << std::endl;
            code.clear();
        }
    }

    huffman_node * _huffman_tree;
    std::map<std::string, std::vector<bool>> * _ascii_to_bits;
    std::map<std::vector<bool>, std::string> * _bits_to_ascii;
};
#endif // HUFFMAN_TABLE_HPP