// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

//////////////////////////////////////////
#include <zedwallet++/CommandDispatcher.h>
//////////////////////////////////////////

#include <zedwallet++/AddressBook.h>
#include <zedwallet++/ColouredMsg.h>
#include <zedwallet++/CommandImplementations.h>
#include <zedwallet++/Open.h>
#include <zedwallet++/Transfer.h>
#include <zedwallet++/Fusion.h>

bool handleCommand(
    const std::string command,
    const std::shared_ptr<WalletBackend> walletBackend)
{
    /* Basic commands */
    if (command == "advanced")
    {
        advanced(walletBackend);
    }
    else if (command == "address")
    {
        std::cout << SuccessMsg(walletBackend->getPrimaryAddress()) << std::endl;
    }
    else if (command == "balance")
    {
        balance(walletBackend);
    }
    else if (command == "backup")
    {
        backup(walletBackend);
    }
    else if (command == "exit")
    {
        return false;
    }
    else if (command == "help")
    {
        help(walletBackend);
    }
    else if (command == "transfer")
    {
        transfer(walletBackend);
    }
    /* Advanced commands */
    else if (command == "ab_add")
    {
        addToAddressBook();
    }
    else if (command == "ab_delete")
    {
        deleteFromAddressBook();
    }
    else if (command == "ab_list")
    {
        listAddressBook();
    }
    else if (command == "ab_send")
    {
        sendFromAddressBook(walletBackend);
    }
    else if (command == "change_password")
    {
        changePassword(walletBackend);
    }
    else if (command == "make_integrated_address")
    {
        createIntegratedAddress();
    }
    else if (command == "incoming_transfers")
    {
        const bool printIncoming = true;
        const bool printOutgoing = false;

        listTransfers(printIncoming, printOutgoing, walletBackend);
    }
    else if (command == "list_transfers")
    {
        const bool printIncoming = true;
        const bool printOutgoing = true;

        listTransfers(printIncoming, printOutgoing, walletBackend);
    }
    else if (command == "optimize")
    {
        optimize(walletBackend);
    }
    else if (command == "outgoing_transfers")
    {
        const bool printIncoming = false;
        const bool printOutgoing = true;

        listTransfers(printIncoming, printOutgoing, walletBackend);
    }
    else if (command == "reset")
    {
        reset(walletBackend);
    }
    else if (command == "save")
    {
        save(walletBackend);
    }
    else if (command == "save_csv")
    {
        saveCSV(walletBackend);
    }
    else if (command == "send_all")
    {
        sendAll(walletBackend);
    }
    else if (command == "status")
    {
        status(walletBackend);
    }
    /* This should never happen */
    else
    {
        throw std::runtime_error("Command was defined but not hooked up!");
    }

    return true;
}

std::shared_ptr<WalletBackend> handleLaunchCommand(
    const std::string launchCommand,
    const Config &config)
{
    if (launchCommand == "create")
    {
        return createWallet(config);
    }
    else if (launchCommand == "open")
    {
        return openWallet(config);
    }
    else if (launchCommand == "seed_restore")
    {
        return importWalletFromSeed(config);
    }
    else if (launchCommand == "key_restore")
    {
        return importWalletFromKeys(config);
    }
    else if (launchCommand == "view_wallet")
    {
        return importViewWallet(config);
    }
    /* This should never happen */
    else
    {
        throw std::runtime_error("Command was defined but not hooked up!");
    }
}
