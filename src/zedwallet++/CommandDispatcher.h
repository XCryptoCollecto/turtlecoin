// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <WalletBackend/WalletBackend.h>

#include <zedwallet++/ParseArguments.h>

bool handleCommand(
    const std::string command,
    const std::shared_ptr<WalletBackend> walletBackend);

std::shared_ptr<WalletBackend> handleLaunchCommand(
    const std::string launchCommand,
    const Config &config);
