#include "huffman_codec.hpp"
#include "util.hpp"

#include <vector>
#include <string>

int main(int argc, char ** argv) {

    if (argc != 3 && argc != 4) {
        std::cerr << "usage: " << argv[0] << " <encode | query> <filepath> <string>" << std::endl; 
        exit(-1);
    }

    if (strcmp(argv[1], "encode") == 0) {
        std::cout << "Encoding " << argv[2] << std::endl;
        std::map<std::string, int> entries;
        read_column_file_new(argv[2], entries);
        huffman_table t(entries);
        huffman_codec c(&t);
        c.write_enc_col(argv[2], "test_bitstream.txt", entries);
    }
    else if (strcmp(argv[1],"query") == 0) {
        if (argc != 4) {
            std::cerr << "Not enough arguments" << std::endl;
            exit(-1);
        }

        std::cout << "Querying " << argv[2] << " for string " << argv[3] << std::endl;
        std::map<std::string, int> * frequencies;
        huffman_codec c;
        frequencies = c.load_encoded(argv[2]);

        auto it = frequencies->find(std::string(argv[3]));
        if ( it != frequencies->end()) {
            std::cout << "Found " << argv[3] << ".  Frequency " << it->second << std::endl;
        }
        else {
            std::cout << "Did not find " << argv[3] << "." << std::endl;
        }
        delete frequencies;
    }
    else {
        std::cerr << "Invalid mode. Try encode or query." << std::endl;
        exit(-1);
    }


    // std::vector<std::string> * column = read_column_file(argv[1]);
    // std::map<char, int> * f = ascii_frequencies(column);

    // std::map<char, int> ascii_frequencies;
    
    // print_word_frequencies(entries);



    // t.print();
    // 0101100100 = first string in there.
    // std::vector<bool> test = {false, true, false, true, true, false, false, true, false, false};
    // std::cout << c.convert(&test) << std::endl;
    // auto index_info = c.write_encoded_column_file(entries, "test_bitstream.txt");

    // Write the encoded column file.  All we need now is the huffman tree to decode the whole thing

    // huffman_codec c2("test_bitstream.txt");



    // Now find a string from the file
    // auto encoded_string_ptr = c.convert(entries.begin()->first);
    // std::cout << entries.begin()->first << std::endl;
    // auto q = index_info->find(*encoded_string_ptr);
    // auto p = c.get_string_by_bit_index("test_bitstream.txt", q->second.first);
    // std::cout << p << std::endl;
}