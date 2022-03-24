#include "huffman_codec.hpp"
#include "util.hpp"

#include <vector>
#include <string>

int main(int argc, char ** argv) {
    // std::vector<std::string> * column = read_column_file(argv[1]);
    // std::map<char, int> * f = ascii_frequencies(column);

    std::map<std::string, int> entries;
    std::map<char, int> ascii_frequencies;
    
    read_column_file_new(argv[1], entries, ascii_frequencies);
    print_frequencies(ascii_frequencies);

    huffman_table t(&ascii_frequencies);
    huffman_codec c(&t);

    t.print();
    auto index_info = c.write_encoded_column_file(entries, "test_bitstream.txt");

    // Now find a string from the file
    auto encoded_string_ptr = c.convert(entries.begin()->first);
    std::cout << entries.begin()->first << std::endl;
    auto q = index_info->find(*encoded_string_ptr);
    auto p = c.get_string_by_bit_index("test_bitstream.txt", q->second.first);
    std::cout << p << std::endl;

}