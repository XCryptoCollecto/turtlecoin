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

    try
    {
        auto [quit, walletBackend] = selectionScreen(config);

        if (!quit)
        {
            mainLoop(walletBackend);
        }
    }
    catch (const std::exception &e)
    {
        std::cout << WarningMsg("Unexpected error: " + std::string(e.what()))
                  << std::endl
                  << "Please report this error, and what you were doing to "
                  << "cause it." << std::endl;

        std::cout << "Hit any key to exit: ";

        getchar();
    }
}
