// App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define ZLIB_WINAPI
#include <iostream>

#include "argparse.hpp"

#include "zlib.h"

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("Back to the future");

    argparse::ArgumentParser packCommand("pack");
    packCommand.add_description("Packs given folder");
    packCommand.add_argument("input")
        .help("Path to folder that should be packed");
    packCommand.add_argument("output")
        .help("Path to folder were file will be placed");

    argparse::ArgumentParser unpackCommand("unpack");
    unpackCommand.add_description("Unpacks given file");
    unpackCommand.add_argument("input")
        .help("Path to file");
    unpackCommand.add_argument("output")
        .help("Path to folder were unpacked content will be placed");


    program.add_subparser(packCommand);
    program.add_subparser(unpackCommand);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    z_stream c_stream; /* compression stream */
    auto xd = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);


    return 0;
}