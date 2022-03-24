#ifndef HUFFMAN_CODEC_HPP
#define HUFFMAN_CODEC_HPP

#include "huffman_table.hpp"
#include "util.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <math.h>
#include <string.h>

class huffman_codec {
public:
    huffman_codec(huffman_table * h) { _table = h; };
    ~huffman_codec() { };

    std::vector<bool> * convert(const std::string &s) {
        std::vector<bool> * bits = new std::vector<bool>();
        for (int i = 0; i < s.length(); i++) {
            char x = s[i];
            // std::cout << i << std::endl;
            bits->insert( bits->end(), _table->_ascii_to_bits->at(x).begin(), _table->_ascii_to_bits->at(x).end() );
        }
        return bits;
    }

    std::string convert(std::vector<bool> * bits) {
        return huffman_tree_decoder(_table->_huffman_tree, bits);
    }

    // Encode every string and write it into a file separated by delimeters.
    // As they are written in, keep track of the bit index for each one
    // using an unordered_map<encoded_string, std::pair<bit_index, frequency>>
    // Then, when we look things up we'll get std::pair<encoded, <bit_index, freq>>
    std::unordered_map<std::vector<bool>, std::pair<int, int>> * write_encoded_column_file(std::map<std::string, int> & column, std::string path) {
        std::vector<bool> total_column;
        std::vector<bool> * tmp;
        std::unordered_map<std::vector<bool>, std::pair<int, int>> * index_info = new std::unordered_map<std::vector<bool>, std::pair<int, int>>();
        int bit_index = 0;
        for (auto mapentry : column) {
            tmp = convert(mapentry.first);
            total_column.insert(total_column.end(), tmp->begin(), tmp->end());
            auto it = index_info->find(*tmp);
            if (it != index_info->end()) {
                it->second.second = it->second.second + 1;
            }
            else {
                std::pair<int, int> entry;
                std::pair<std::vector<bool>, std::pair<int, int>> kvpair;
                entry.first = bit_index;
                entry.second = 1;
                kvpair.first = *tmp;
                kvpair.second = entry;
                index_info->insert(kvpair);
            }
            bit_index += tmp->size();
            delete tmp;
        }

        std::vector<char> stream;
        unsigned long streamsz = total_column.size();
        size_t bufsz = 1024;
        unsigned char * iobuf = (unsigned char *) malloc(bufsz);
        std::ofstream out(path);

        out.write((char *) &streamsz, sizeof(unsigned long));

        unsigned char pack;
        unsigned int bufctr = 0;
        for (int i = 0; i < floor((double) total_column.size() / 8); i++ ) {

            pack = 0;

            if (bufctr == bufsz) {
                out.write((const char *) iobuf, bufsz);
                bufctr = 0;
            }
            
            for (int j = 0; j < 8; j++) {
                // std::cout << total_column.at(i*8 + j);
                pack = pack | (total_column.at(i*8 + j) << j);
            }
            bufctr += 1;
            // std::cout << std::endl;
            stream.push_back(pack);
            // std::cout << pack << std::hex << std::endl;
            
        }

        if (bufctr != 0) {
            out.write((const char *) iobuf, bufsz);
        }

        return index_info;
    }

    std::string get_string_by_bit_index(std::string path, unsigned int bit_index) {
        std::ifstream encoded_col(path);
        unsigned int byte_index = floor(bit_index / 8);
        unsigned int shift_n = bit_index - (byte_index * 8);

        // Add sizeof unsigned long because the first sizeof ul bytes are bit size
        byte_index += sizeof(unsigned long);

        unsigned char * buf = (unsigned char *) malloc(20);
        memset(buf, 0x00, 20);

        encoded_col.seekg(byte_index, std::ios::beg);
        encoded_col.read((char *)buf, 20);

        std::vector<bool> s;

        char bitmask = 0x01;
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 8; j++) {
                s.push_back((buf[i] & (0x01 << j)) == 0);
            }
        }

        std::cout << print_bitstream(&s) << std::endl;

        // Trim the front excess
        s.erase(s.begin(), s.begin()+shift_n);

        // Find the newline character and cut everything after it
        std::vector<bool> newlinecode = _table->_ascii_to_bits->at('\n');

        // Crappy search algorithm using strings
        std::string newlinestr = print_bitstream(&newlinecode);
        std::string bitstream = print_bitstream(&s);

        size_t end = bitstream.find(newlinestr);

        // Trim the back excess
        s.erase(s.begin()+end, s.end());

        return convert(&s);
    }

    huffman_table * _table;
};
#endif // HUFFMAN_CODEC_HPP