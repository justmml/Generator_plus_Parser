// main.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "Parser.h"
#include "Generator.h"

int main(int argc, char* argv[])
{
#if defined(_PARSER)
    return Parser::Main(argc, argv);
#elif defined(_GENERATOR)
    return Generator::Main(argc, argv);
#else
#error Application type (_PARSER or _GENERATOR) required.
#endif
}

