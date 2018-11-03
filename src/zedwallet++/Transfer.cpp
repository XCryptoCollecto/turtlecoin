// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

/////////////////////////////////
#include <zedwallet++/Transfer.h>
/////////////////////////////////

std::tuple<WalletError, Crypto::Hash> transfer(const std::shared_ptr<WalletBackend> walletBackend)
{
    return {SUCCESS, Crypto::Hash()};
}

std::tuple<WalletError, Crypto::Hash> sendAll(const std::shared_ptr<WalletBackend> walletBackend)
{
    return {SUCCESS, Crypto::Hash()};
}
