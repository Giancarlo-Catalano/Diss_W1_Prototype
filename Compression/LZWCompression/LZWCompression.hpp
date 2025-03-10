//
// Created by gian on 15/12/22.
//

#ifndef EVOCOM_LZWCOMPRESSION_HPP
#define EVOCOM_LZWCOMPRESSION_HPP

#include "../../Dependencies/LZW/LZW.hpp"
#include "../Compression.hpp"

namespace GC {

    class LZWCompression : public Compression {
    public:

        using CodeType = uint16_t;
///
/// @brief Compresses the contents of `is` and writes the result to `os`.
/// @param [in] is      input stream
/// @param [out] os     output stream
///
        void compressToWriter(const Block &input, GC::AbstractBitWriter& writer) const  {
            JP::EncoderDictionary ed;
            CodeType i{JP::globals::dms}; // Index
            char c;


            size_t inputIndex = 0;
            auto getCharFromInput = [&]() {
                return input[inputIndex++];
            };

            Block result;
            auto pushCodeToOutput = [&](const CodeType code) {
                writer.writeSmallAmount(code);
            };

            while (inputIndex < input.size()) {
                c = getCharFromInput();
                const CodeType temp{i};

                if ((i = ed.search_and_insert(temp, c)) == JP::globals::dms) {
                    pushCodeToOutput(temp);
                    i = ed.search_initials(c);
                }
            }

            if (i != JP::globals::dms)
                pushCodeToOutput(i);
        }

///
/// @brief Decompresses the contents of `is` and writes the result to `os`.
/// @param [in] is      input stream
/// @param [out] os     output stream
///
        Block decompressFromReader(const size_t sizeOfResult, GC::AbstractBitReader& reader) const {

            std::vector<std::pair<CodeType, char>> dictionary;

            // "named" lambda function, used to reset the dictionary to its initial contents
            const auto reset_dictionary = [&dictionary] {
                dictionary.clear();
                dictionary.reserve(JP::globals::dms);

                const long int minc = std::numeric_limits<char>::min();
                const long int maxc = std::numeric_limits<char>::max();

                for (long int c = minc; c <= maxc; ++c)
                    dictionary.push_back({JP::globals::dms, static_cast<char> (c)});
            };

            const auto rebuild_string = [&dictionary](CodeType k) -> const std::vector<char> * {
                static std::vector<char> s; // String

                s.clear();

                // the length of a string cannot exceed the dictionary's number of entries
                s.reserve(JP::globals::dms);

                while (k != JP::globals::dms) {
                    s.push_back(dictionary[k].second);
                    k = dictionary[k].first;
                }

                std::reverse(s.begin(), s.end());
                return &s;
            };

            reset_dictionary();

            CodeType i{JP::globals::dms}; // Index
            CodeType k; // Key

            auto readCodeFromInput = [&]() -> CodeType {
                return reader.readSmallAmount();
            };

            Block output(sizeOfResult);
            size_t indexInOutput = 0;
            auto writeCharToOutput = [&](const char toWrite) {
                output[indexInOutput++]=toWrite;
            };
            auto writeStringToOutput = [&](const std::vector<char> *str) {
                for (size_t i = 0; i < str->size(); i++) {
                    writeCharToOutput(str->at(i));
                }

            };

            while (indexInOutput<sizeOfResult) {
                k = readCodeFromInput();
                // dictionary's maximum size was reached
                if (dictionary.size() == JP::globals::dms)
                    reset_dictionary();

                if (k > dictionary.size())
                    throw std::runtime_error("invalid compressed code");

                const std::vector<char> *s; // String

                if (k == dictionary.size()) {
                    dictionary.push_back({i, rebuild_string(i)->front()});
                    s = rebuild_string(k);
                } else {
                    s = rebuild_string(k);

                    if (i != JP::globals::dms)
                        dictionary.push_back({i, s->front()});
                }

                writeStringToOutput(s);
                i = k;
            }
            return output;
        }

        std::string to_string() const {return "{LZWCompression}";}

        void compress(const Block& block, AbstractBitWriter& writer) const {
            writer.writeSmallAmount(block.size()); //the decompressor needs to know when to stop
            compressToWriter(block, writer);
        }

        Block decompress(AbstractBitReader& reader) const {
            const size_t resultSize = reader.readSmallAmount();
            //LOG("the decompressed size of the block is", resultSize);
            return decompressFromReader(resultSize, reader);
        }

    };

} // GC

#endif //EVOCOM_LZWCOMPRESSION_HPP
