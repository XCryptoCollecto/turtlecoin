// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

/////////////////////////////////
#include <zedwallet++/GetInput.h>
/////////////////////////////////

#include <config/WalletConfig.h>

#include "linenoise.hpp"

#include <zedwallet++/Commands.h>
#include <zedwallet++/Utilities.h>

/* Note: this is not portable, it only works with terminals that support ANSI
   codes (e.g., not Windows) - however! due to the way linenoise-cpp works,
   it will actually convert these codes for us to the windows equivalent. <3 */
std::string yellowANSIMsg(const std::string msg)
{
    const std::string CYELLOW = "\033[1;33m";
    const std::string RESET = "\033[0m";

    return CYELLOW + msg + RESET;
}

std::string getPrompt(std::shared_ptr<WalletBackend> walletBackend)
{
    const int promptLength = 20;
    const std::string extension = ".wallet";

    const std::string walletFileName = walletBackend->getWalletLocation();

    std::string walletName = walletFileName;

    /* Filename ends in .wallet, remove extension */
    if (std::equal(extension.rbegin(), extension.rend(), walletFileName.rbegin()))
    {
        const size_t extPos = walletFileName.find_last_of('.');

        walletName = walletFileName.substr(0, extPos);
    }

    const std::string shortName = walletName.substr(0, promptLength);

    return "[" + WalletConfig::ticker + " " + shortName + "]: ";
}

template<typename T>
std::string getInput(
    const std::vector<T> &availableCommands,
    const std::string prompt)
{
    linenoise::SetCompletionCallback(
    [availableCommands](const char *input, std::vector<std::string> &completions)
    {
        /* Convert to std::string */
        std::string c = input;

        for (const auto &command : availableCommands)
        {
            /* Does command begin with input? */
            if (command.commandName.compare(0, c.length(), c) == 0)
            {
                completions.push_back(command.commandName);
            }
        }
    });

    const std::string promptMsg = yellowANSIMsg(prompt);

    /* 256 max commands in the wallet command history */
    linenoise::SetHistoryMaxLen(256);

    /* The inputted command */
    std::string command;

    bool quit = linenoise::Readline(promptMsg.c_str(), command);
    
    if (quit)
    {
        return "exit";
    }
	
    /* Remove any whitespace */
    ZedUtilities::trim(command);

    if (command != "")
    {
        linenoise::AddHistory(command.c_str());
    }

    return command;
}

/* Template instantations that we are going to use - this allows us to have
   the template implementation in the .cpp file. */
template
std::string getInput(const std::vector<Command> &availableCommands,
                     std::string prompt);

template
std::string getInput(const std::vector<AdvancedCommand> &availableCommands,
                     std::string prompt);
