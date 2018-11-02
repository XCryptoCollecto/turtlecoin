// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

#include <iostream>

#include <config/CliHeader.h>

#include <zedwallet++/ColouredMsg.h>
#include <zedwallet++/Menu.h>
#include <zedwallet++/ParseArguments.h>

int main(int argc, char **argv)
{
    Config config = parseArguments(argc, argv);

    std::cout << InformationMsg(CryptoNote::getProjectCLIHeader()) << std::endl;

    auto [quit, walletBackend] = selectionScreen(config);

    if (!quit)
    {
        mainLoop(walletBackend);
    }
}
