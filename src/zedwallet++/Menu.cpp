// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

/////////////////////////////
#include <zedwallet++/Menu.h>
/////////////////////////////

#include <config/WalletConfig.h>

#include <zedwallet++/CommandDispatcher.h>
#include <zedwallet++/Commands.h>
#include <zedwallet++/GetInput.h>
#include <zedwallet++/Sync.h>

std::tuple<bool, std::shared_ptr<WalletBackend>> selectionScreen(const Config &config)
{
    while (true)
    {
        /* Get the users action */
        std::string launchCommand = getAction(config);

        /* User wants to exit */
        if (launchCommand == "exit")
        {
            return {true, nullptr};
        }

        /* Handle the user input */
        std::shared_ptr<WalletBackend> walletBackend = handleLaunchCommand(
            launchCommand, config
        );

        /* Action failed, for example wallet file is corrupted. */
        if (walletBackend == nullptr)
        {
            std::cout << InformationMsg("Returning to selection screen...")
                      << std::endl;

            continue;
        }

        /* Node is down, user wants to exit */
        if (!checkNodeStatus(walletBackend))
        {
            return {true, nullptr};
        }
    
        /* If we're creating a wallet, don't print the lengthy sync process */
        if (launchCommand == "create")
        {
            std::stringstream str;

            str << std::endl
                << "Your wallet is syncing with the network in the background."
                << std::endl
                << "Until this is completed new transactions might not show "
                << "up." << std::endl
                << "Use the status command to check the progress."
                << std::endl;

            std::cout << InformationMsg(str.str());
        }
        else
        {
            syncWallet(walletBackend);
        }

        /* Return the wallet info */
        return {false, walletBackend};
    }
}

bool checkNodeStatus(const std::shared_ptr<WalletBackend> walletBackend)
{
    while (true)
    {
        const auto [walletSyncProgress, localDaemonBlockCount, networkBlockCount]
            = walletBackend->getSyncStatus();

        /* Daemon is online */
        if (networkBlockCount != 0 || localDaemonBlockCount != 0)
        {
            break;
        }

        std::stringstream msg;

        msg << "It looks like " << WalletConfig::daemonName << " isn't open!"
            << std::endl << std::endl
            << "Ensure " << WalletConfig::daemonName
            << " is open and has finished syncing." << std::endl
            << "If it's still not working, try restarting "
            << WalletConfig::daemonName << "."
            << "The daemon sometimes gets stuck."
            << std::endl << "Alternatively, perhaps "
            << WalletConfig::daemonName << " can't communicate with any peers."
            << std::endl << std::endl
            << "The wallet can't function fully until it can communicate with "
            << "the network.";

        std::cout << WarningMsg(msg.str()) << std::endl;

        /* Print the commands */
        printCommands(nodeDownCommands());

        /* See what the user wants to do */
        std::string command = parseCommand(
            nodeDownCommands(), nodeDownCommands(),
            "What would you like to do?: "
        );

        /* If they want to try again, check the node height again */
        if (command == "try_again")
        {
            continue;
        }
        /* If they want to exit, exit */
        else if (command == "exit")
        {
            return false;
        }
        /* If they want to continue, proceed to the menu screen */
        else if (command == "continue")
        {
            return true;
        }
    }

    return true;
}

std::string getAction(const Config &config)
{
    if (config.walletGiven || config.passGiven)
    {
        return "open";
    }

    printCommands(startupCommands());

    return parseCommand(
        startupCommands(), startupCommands(), "What would you like to do?: "
    );
}

void mainLoop(const std::shared_ptr<WalletBackend> walletBackend)
{
    if (walletBackend->isViewWallet())
    {
        printCommands(basicViewWalletCommands());
    }
    else
    {
        printCommands(basicCommands());
    }
    
    while (true)
    {
        std::string command;

        if (walletBackend->isViewWallet())
        {
            command = parseCommand(
                basicViewWalletCommands(), allViewWalletCommands(),
                getPrompt(walletBackend)
            );
        }
        else
        {
            command = parseCommand(
                basicCommands(), allCommands(), getPrompt(walletBackend)
            );
        }

        /* User exited */
        if (!handleCommand(command, walletBackend))
        {
            return;
        }
    }
}
