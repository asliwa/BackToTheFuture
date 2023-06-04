// App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <filesystem>

#include "argparse.hpp"

#include "zlib.h"

#include "Pack.h"
#include "Unpack.h"

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


        auto folderName = std::filesystem::path(input).filename();


        Pack{ input, output }.execute();
    }
    
    if (program.is_subcommand_used("unpack"))
    {
        auto input = unpackCommand.get("input");
        auto output = unpackCommand.get("output");

        std::cout << input << std::endl;
        std::cout << output << std::endl;

        // validate arguments

        //std::filesystem::path(input);

        Unpack{ input, output }.execute();
    }

    return 0;
}