// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

////////////////////////////////////
#include <zedwallet++/AddressBook.h>
////////////////////////////////////

#include <config/WalletConfig.h>

#include <fstream>

#include <WalletBackend/ValidateParameters.h>

#include <zedwallet++/ColouredMsg.h>
#include <zedwallet++/Utilities.h>

const std::string getAddressBookName(const std::vector<AddressBookEntry> addressBook)
{
    while (true)
    {
        std::string friendlyName;

        std::cout << InformationMsg("What friendly name do you want to ")
                  << InformationMsg("give this address book entry?: ");

        std::getline(std::cin, friendlyName);

        ZedUtilities::trim(friendlyName);

        const auto it = std::find(addressBook.begin(), addressBook.end(),
                                  AddressBookEntry(friendlyName));

        if (it != addressBook.end())
        {
            std::cout << WarningMsg("An address book entry with this ")
                      << WarningMsg("name already exists!")
                      << std::endl << std::endl;

            continue;
        }

        return friendlyName;
    }
}

const std::string getAddressBookPaymentID()
{
    while (true)
    {
        std::string paymentID;

        std::cout << InformationMsg("\nDoes this address book entry have a "
                                    "payment ID associated with it?\n")
                  << WarningMsg("Warning: If you were given a payment ID,\n")
                  << WarningMsg("you MUST use it, or your funds may be lost!\n")
                  << "Hit enter for the default of no payment ID: ";

        std::getline(std::cin, paymentID);

        ZedUtilities::trim(paymentID);

        if (paymentID == "")
        {
            return std::string();
        }

        if (paymentID == "cancel")
        {
            return "cancel";
        }

        /* Validate the payment ID */
        if (WalletError error = validatePaymentID(paymentID); error != SUCCESS)
        {
            std::cout << WarningMsg("Invalid payment ID: ")
                      << WarningMsg(error) << std::endl;
        }
        else
        {
            return paymentID;
        }
    }
}

const std::string getAddressBookAddress()
{
    while (true)
    {
        std::cout << InformationMsg("\nWhat address does this user have?: ");

        std::string address;

        std::getline(std::cin, address);

        ZedUtilities::trim(address);

        const bool integratedAddressesAllowed = true;

        if (address == "cancel")
        {
            return "cancel";
        }

        if (WalletError error = validateAddresses({address}, integratedAddressesAllowed); error != SUCCESS)
        {
            std::cout << WarningMsg("Invalid address: ")
                      << WarningMsg(error) << std::endl;
        }
        else
        {
            return address;
        }
    }
}

void addToAddressBook()
{
    std::cout << InformationMsg("Note: You can type cancel at any time to "
                                "cancel adding someone to your address book.")
              << std::endl << std::endl;

    auto addressBook = getAddressBook();

    const std::string friendlyName = getAddressBookName(addressBook);

    if (friendlyName == "cancel")
    {
        std::cout << WarningMsg("Cancelling addition to address book.")
                  << std::endl;
        return;
    }

    const std::string address = getAddressBookAddress();

    if (address == "cancel")
    {
        std::cout << WarningMsg("Cancelling addition to address book.")
                  << std::endl;
        return;
    }

    std::string paymentID;

    const bool isIntegratedAddress = address.length() == WalletConfig::integratedAddressLength;

    /* Don't prompt for a payment ID if we have an integrated address */
    if (!isIntegratedAddress)
    {
        paymentID = getAddressBookPaymentID();

        if (paymentID == "cancel")
        {
            std::cout << WarningMsg("Cancelling addition to address book.")
                      << std::endl;

            return;
        }
    }

    addressBook.emplace_back(friendlyName, address, paymentID);

    if (saveAddressBook(addressBook))
    {
        std::cout << SuccessMsg("\nA new entry has been added to your address "
                                "book!\n");
    }
}

const std::tuple<bool, AddressBookEntry> getAddressBookEntry(
    const std::vector<AddressBookEntry> addressBook)
{
    while (true)
    {
        std::string friendlyName;

        std::cout << InformationMsg("Who do you want to send to from your ")
                  << InformationMsg("address book?: ");

        std::getline(std::cin, friendlyName);

        ZedUtilities::trim(friendlyName);

        if (friendlyName == "cancel")
        {
            return {true, AddressBookEntry()};
        }

        /* TODO: number based indexing */
        const auto it = std::find(addressBook.begin(), addressBook.end(),
                                  AddressBookEntry(friendlyName));

        if (it != addressBook.end())
        {
            return {false, *it};
        }

        std::cout << std::endl
                  << WarningMsg("Could not find a user with the name of ")
                  << InformationMsg(friendlyName)
                  << WarningMsg(" in your address book!")
                  << std::endl << std::endl;

        const bool list = ZedUtilities::confirm(
            "Would you like to list everyone in your address book?"
        );

        std::cout << "\n";

        if (list)
        {
            listAddressBook();
        }
    }
}

void sendFromAddressBook(const std::shared_ptr<WalletBackend> walletBackend)
{
    auto addressBook = getAddressBook();

    if (isAddressBookEmpty(addressBook))
    {
        return;
    }

    std::cout << InformationMsg("Note: You can type cancel at any time to ")
              << InformationMsg("cancel the transaction\n\n");

    const auto [cancel, addressBookEntry] = getAddressBookEntry(addressBook);

    if (cancel)
    {
        std::cout << WarningMsg("Cancelling transaction.") << std::endl;
        return;
    }

    /* TODO */
}

bool isAddressBookEmpty(const std::vector<AddressBookEntry> addressBook)
{
    if (addressBook.empty())
    {
        std::cout << WarningMsg("Your address book is empty! Add some people ")
                  << WarningMsg("to it first.")
                  << std::endl;

        return true;
    }

    return false;
}

void deleteFromAddressBook()
{
    auto addressBook = getAddressBook();

    if (isAddressBookEmpty(addressBook))
    {
        return;
    }

    while (true)
    {
        std::cout << InformationMsg("Note: You can type cancel at any time ")
                  << InformationMsg("to cancel the deletion.\n\n");

        std::string friendlyName;

        std::cout << InformationMsg("What address book entry do you want to ")
                  << InformationMsg("delete?: ");

        std::getline(std::cin, friendlyName);

        ZedUtilities::trim(friendlyName);

        if (friendlyName == "cancel")
        {
            std::cout << WarningMsg("Cancelling deletion.\n");
            return;
        }

        const auto it = std::remove(addressBook.begin(), addressBook.end(),
                                    AddressBookEntry(friendlyName));

        if (it != addressBook.end())
        {
            addressBook.erase(it);

            if (saveAddressBook(addressBook))
            {
                std::cout << std::endl
                          << SuccessMsg("This entry has been deleted from ")
                          << SuccessMsg("your address book!")
                          << std::endl;
            }

            return;
        }

        std::cout << WarningMsg("\nCould not find a user with the name of ")
                  << InformationMsg(friendlyName)
                  << WarningMsg(" in your address book!\n\n");

        const bool list = ZedUtilities::confirm(
            "Would you like to list everyone in your address book?"
        );

        std::cout << "\n";

        if (list)
        {
            listAddressBook();
        }
    }
}

void listAddressBook()
{
    auto addressBook = getAddressBook();

    if (isAddressBookEmpty(addressBook))
    {
        return;
    }

    int index = 1;

    for (const auto &i : addressBook)
    {
        std::cout << InformationMsg("Address Book Entry #")
                  << InformationMsg(index)
                  << InformationMsg(":\n\nFriendly Name:\n")
                  << SuccessMsg(i.friendlyName)
                  << InformationMsg("\n\nAddress:\n")
                  << SuccessMsg(i.address) << "\n\n";

        if (i.paymentID != "")
        {
            std::cout << InformationMsg("Payment ID:\n")
                      << SuccessMsg(i.paymentID) << "\n\n\n";
        }
        else
        {
            std::cout << std::endl;
        }

        index++;
    }
}

std::vector<AddressBookEntry> getAddressBook()
{
    std::vector<AddressBookEntry> addressBook;

    std::ifstream input(WalletConfig::addressBookFilename);

    /* If file exists, read current values */
    if (input)
    {
        json j;
        input >> j;

        addressBook = j.get<std::vector<AddressBookEntry>>();
    }

    return addressBook;
}

void to_json(json &j, const AddressBookEntry &a)
{
    j = {
        {"friendlyName", a.friendlyName},
        {"address", a.address},
        {"paymentID", a.paymentID},
    };
}

void from_json(const json &j, AddressBookEntry &a)
{
    a.friendlyName = j.at("friendlyName").get<std::string>();
    a.address = j.at("address").get<std::string>();
    a.paymentID = j.at("paymentID").get<std::string>();
}

bool saveAddressBook(const std::vector<AddressBookEntry> addressBook)
{
    json addressBookJson = addressBook;

    std::ofstream output(WalletConfig::addressBookFilename);

    if (output)
    {
        output << std::setw(4) << addressBookJson << std::endl;
    }
    else
    {
        std::cout << WarningMsg("Failed to save address book to disk!")
                  << std::endl
                  << WarningMsg("Check you are able to write files to your ")
                  << WarningMsg("current directory.") << std::endl;

        return false;
    }

    output.close();

    return true;
}
