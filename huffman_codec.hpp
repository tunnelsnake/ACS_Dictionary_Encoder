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
#include <unordered_map>

class huffman_codec
{
public:
    huffman_codec() {}
    huffman_codec(huffman_table *h) { _table = h; }
    ~huffman_codec(){};

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

    // Format:
    // unsigned long : tells you the index of where the encoded column data is, (marks end of next segment).
    // Ascii pairs of map and frequencies (rebuild huffman with this info)
    // unsigned long : tells you how many bits of encoded data there are in the encoded segment.

    void write_enc_col(std::string ipath, std::string opath, std::unordered_map<std::string, int> &entry_freq)
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

        std::string line;
        std::vector<bool> *converted;
        std::vector<bool> encoded_data;
        while (column_file >> line)
        {
            converted = convert(line);
            encoded_data.insert(encoded_data.end(), converted->begin(), converted->end());
            delete converted;
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
                print_aid.clear();
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
        encoded_file.close();
    }

    std::unordered_map<std::string, int> *load_encoded(std::string path)
    {
        std::ifstream encoded_file(path);

        unsigned long dict_size;
        encoded_file.read((char *)&dict_size, sizeof(unsigned long));

        char *dictbuf = (char *)malloc(dict_size);
        encoded_file.read(dictbuf, dict_size);

        // Rebuild the frequency dictionary
        // string + \0x00 + frequency + \x00

        // Read a line from the buffer (read from beginning to newline)
        unsigned long last = 0;
        unsigned long cursor = 0;
        char *line;

        std::unordered_map<std::string, int> *m = new std::unordered_map<std::string, int>;
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