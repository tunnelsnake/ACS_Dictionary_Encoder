#include "huffman_codec.hpp"
#include "util.hpp"
#include <vector>
#include <string>

int main(int argc, char **argv)
{

    if (argc != 4)
    {
        std::cerr << "usage: " << argv[0] << " <encode | query> <filepath> <query only: string> <encode only: output-path>" << std::endl;
        exit(-1);
    }

    if (strcmp(argv[1], "encode") == 0)
    {
        std::cout << "Encoding " << argv[2] << std::endl;
        std::unordered_map<std::string, int> entries;
        read_column_file_new(argv[2], entries);
        huffman_table t(entries);
        huffman_codec c(&t);
        c.write_enc_col(argv[2], argv[3], entries);
    }
    else if (strcmp(argv[1], "query") == 0)
    {
        if (argc != 4)
        {
            std::cerr << "Not enough arguments" << std::endl;
            exit(-1);
        }

        std::cout << "Querying " << argv[2] << " for string " << argv[3] << std::endl;
        std::unordered_map<std::string, int> *frequencies;
        huffman_codec c;
        frequencies = c.load_encoded(argv[2]);

        auto it = frequencies->find(std::string(argv[3]));
        if (it != frequencies->end())
        {
            std::cout << "Found " << argv[3] << ".  Frequency " << it->second << std::endl;
        }
        else
        {
            std::cout << "Did not find " << argv[3] << "." << std::endl;
        }
        delete frequencies;
    }
    else
    {
        std::cerr << "Invalid mode. Try encode or query." << std::endl;
        exit(-1);
    }
}