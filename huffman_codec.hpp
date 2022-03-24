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

class huffman_codec
{
public:
    huffman_codec() {}
    huffman_codec(huffman_table *h) { _table = h; }

    ~huffman_codec(){};

    // std::vector<bool> * convert(const std::string &s) {
    //     std::vector<bool> * bits = new std::vector<bool>();
    //     for (int i = 0; i < s.length(); i++) {
    //         char x = s[i];
    //         // std::cout << i << std::endl;
    //         bits->insert( bits->end(), _table->_ascii_to_bits->at(x).begin(), _table->_ascii_to_bits->at(x).end() );
    //     }
    //     return bits;
    // }

    std::vector<bool> *convert(const std::string &s)
    {
        std::vector<bool> *bits = new std::vector<bool>();
        bits->insert(bits->end(), _table->_ascii_to_bits->at(s).begin(), _table->_ascii_to_bits->at(s).end());
        return bits;
    }

    std::string convert(std::vector<bool> *bits)
    {
        return huffman_tree_decoder(_table->_huffman_tree, bits);
    }

    // Encode every string and write it into a file separated by delimeters.
    // As they are written in, keep track of the bit index for each one
    // using an unordered_map<encoded_string, std::pair<bit_index, frequency>>
    // Then, when we look things up we'll get std::pair<encoded, <bit_index, freq>>
    // std::unordered_map<std::vector<bool>, std::pair<int, int>> * write_encoded_column_file(std::map<std::string, int> & column, std::string path) {
    //     std::vector<bool> total_column;
    //     std::vector<bool> * tmp;
    //     std::unordered_map<std::vector<bool>, std::pair<int, int>> * index_info = new std::unordered_map<std::vector<bool>, std::pair<int, int>>();
    //     int bit_index = 0;
    //     for (auto mapentry : column) {
    //         tmp = convert(mapentry.first);
    //         total_column.insert(total_column.end(), tmp->begin(), tmp->end());
    //         auto it = index_info->find(*tmp);
    //         if (it != index_info->end()) {
    //             it->second.second = it->second.second + 1;
    //         }
    //         else {
    //             std::pair<int, int> entry;
    //             std::pair<std::vector<bool>, std::pair<int, int>> kvpair;
    //             entry.first = bit_index;
    //             entry.second = 1;
    //             kvpair.first = *tmp;
    //             kvpair.second = entry;
    //             index_info->insert(kvpair);
    //         }
    //         bit_index += tmp->size();
    //         delete tmp;
    //     }

    //     std::vector<char> stream;
    //     unsigned long streamsz = total_column.size();
    //     size_t bufsz = 1024;
    //     unsigned char * iobuf = (unsigned char *) malloc(bufsz);
    //     std::ofstream out(path);

    //     out.write((char *) &streamsz, sizeof(unsigned long));

    //     unsigned char pack;
    //     unsigned int bufctr = 0;
    //     for (int i = 0; i < floor((double) total_column.size() / 8); i++ ) {

    //         pack = 0;

    //         if (bufctr == bufsz) {
    //             out.write((const char *) iobuf, bufsz);
    //             bufctr = 0;
    //         }

    //         for (int j = 0; j < 8; j++) {
    //             // std::cout << total_column.at(i*8 + j);
    //             pack = pack | (total_column.at(i*8 + j) << j);
    //         }
    //         bufctr += 1;
    //         // std::cout << std::endl;
    //         stream.push_back(pack);
    //         // std::cout << pack << std::hex << std::endl;

    //     }

    //     if (bufctr != 0) {
    //         out.write((const char *) iobuf, bufsz);
    //     }

    //     return index_info;
    // }

    // std::string get_string_by_bit_index(std::string path, unsigned int bit_index) {
    //     std::ifstream encoded_col(path);
    //     unsigned int byte_index = floor(bit_index / 8);
    //     unsigned int shift_n = bit_index - (byte_index * 8);

    //     // Add sizeof unsigned long because the first sizeof ul bytes are bit size
    //     byte_index += sizeof(unsigned long);

    //     unsigned char * buf = (unsigned char *) malloc(20);
    //     memset(buf, 0x00, 20);

    //     encoded_col.seekg(byte_index, std::ios::beg);
    //     encoded_col.read((char *)buf, 20);

    //     std::vector<bool> s;

    //     char bitmask = 0x01;
    //     for (int i = 0; i < 20; i++) {
    //         for (int j = 0; j < 8; j++) {
    //             s.push_back((buf[i] & (0x01 << j)) == 0);
    //         }
    //     }

    //     std::cout << print_bitstream(&s) << std::endl;

    //     // Trim the front excess
    //     s.erase(s.begin(), s.begin()+shift_n);

    //     // Find the newline character and cut everything after it
    //     std::vector<bool> newlinecode = _table->_ascii_to_bits->at('\n');

    //     // Crappy search algorithm using strings
    //     std::string newlinestr = print_bitstream(&newlinecode);
    //     std::string bitstream = print_bitstream(&s);

    //     size_t end = bitstream.find(newlinestr);

    //     // Trim the back excess
    //     s.erase(s.begin()+end, s.end());

    //     return convert(&s);
    // }

    // Format:
    // unsigned long : tells you the index of where the encoded column data is, (marks end of next segment).
    // Ascii pairs of map and frequencies (rebuild huffman with this info)
    // unsigned long : tells you how many bits of encoded data there are in the encoded segment.

    void write_enc_col(std::string ipath, std::string opath, std::map<std::string, int> &entry_freq)
    {
        std::ifstream column_file(ipath);
        std::ofstream encoded_file(opath);

        std::string dict = export_word_frequencies(entry_freq);
        unsigned long dict_sz = dict.size();

        _enc_col_path = opath;
        _payload_offset = dict_sz;

        encoded_file.write((char *)&dict_sz, sizeof(unsigned long));
        encoded_file << dict;

        std::vector<char> stream;

        // int bufsz = 1024;
        // int bufloc = 0;
        // char * iobuf = (char *) malloc(1024);

        std::string line;
        std::vector<bool> *converted;
        std::vector<bool> encoded_data;
        while (column_file >> line)
        {
            converted = convert(line);
            encoded_data.insert(encoded_data.end(), converted->begin(), converted->end());
            // delete converted;
        }

        int ctr = 0;
        char x;
        std::string print_aid = "";

        unsigned long bitstream_size = encoded_data.size();
        encoded_file.write((char *)&bitstream_size, sizeof(unsigned long));

        // Per Bit
        for (int i = 0; i < encoded_data.size(); i++)
        {
            // OR x with itself.  If the bit is 1, shift it in, otherwise or with 0.
            x = x | (encoded_data[i] == true ? (0x01 << ctr) : 0x00);
            print_aid += encoded_data[i] == true ? '1' : '0';
            ctr++;
            // When counter hits 8, flush it to a vector.
            if (ctr == 8)
            {
                stream.push_back(x);
                x = 0;
                ctr = 0;
                // std::cout << print_aid << std::endl;
                print_aid.clear();
                // std::cout << std::hex << x << std::endl;
            }
        }

        // Write the last byte in. It is not completely full.
        if (ctr != 0)
        {
            stream.push_back(x);
        }

        for (int i = 0; i < stream.size(); i++)
        {
            encoded_file.write(stream.data() + i, 1);
        }

        // free(iobuf);
        encoded_file.close();

        // std::cout << encoded_data.size() / 8 << std::endl;
    }

    std::map<std::string, int> *load_encoded(std::string path)
    {
        // std::cout << path << std::endl;
        std::ifstream encoded_file(path);

        unsigned long dict_size;
        encoded_file.read((char *)&dict_size, sizeof(unsigned long));
        // std::cout << std::hex << dict_size << std::endl;

        // std::cout << encoded_file.tellg() << std::endl;
        char *dictbuf = (char *)malloc(dict_size);
        encoded_file.read(dictbuf, dict_size);

        // std::cout << "guuucii" << std::endl;

        // Rebuild the frequency dictionary
        // string + \0x00 + frequency + \x00

        // Read a line from the buffer (read from beginning to newline)
        unsigned long last = 0;
        unsigned long cursor = 0;
        char *line;

        std::map<std::string, int> *m = new std::map<std::string, int>;
        size_t linesz;

        while (cursor < dict_size)
        {
            linesz = strlen(&dictbuf[cursor]);

            // find index of comma
            char *comma = strchr(&(dictbuf[cursor]), ',');

            // convert the string int to int
            int freq = atoi(comma + 1);

            *comma = 0;
            // get the string into std::string
            char *tmp = &(dictbuf[cursor]);
            std::string s(tmp);


            (*m)[s] = freq;
            // insert into map
            cursor += linesz + 1;
        }

        // for (auto it : *m)
        // {
        //     std::cout << it.first << " " << it.second << std::endl;
        // }

        _table = new huffman_table(*m);
        _enc_col_path = path;
        _payload_offset = dict_size;

        return m;
    }

    huffman_table *_table;
    std::string _enc_col_path = "";
    int _payload_offset = -1;
};
#endif // HUFFMAN_CODEC_HPP