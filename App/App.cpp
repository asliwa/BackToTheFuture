// App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>

#include "argparse.hpp"

#include "zlib.h"

#include "Packer.h"

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

    if (program.is_subcommand_used("pack"))
    {
        auto input = packCommand.get("input");
        auto output = packCommand.get("output");

        std::cout << input << std::endl;
        std::cout << output << std::endl;

        Packer{ input, output };
    }
    
    if (program.is_subcommand_used("unpack"))
    {

    }


    /*std::cout << xd << std::endl;
    std::cout << xdd << std::endl;*/
    return 0;
}