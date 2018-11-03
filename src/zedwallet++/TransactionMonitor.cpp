// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

///////////////////////////////////////////
#include <zedwallet++/TransactionMonitor.h>
///////////////////////////////////////////

#include <zedwallet++/ColouredMsg.h>
#include <zedwallet++/CommandImplementations.h>

void TransactionMonitor::start()
{
    /* Grab new transactions and push them into a queue for processing */
    m_walletBackend->m_eventHandler->onTransaction.subscribe([this](const auto tx)
    {
        m_queuedTransactions.push(tx);
    });

    while (!m_shouldStop)
    {
        const auto tx = m_queuedTransactions.pop();

        /* Make sure we're not printing a garbage tx */
        if (m_shouldStop)
        {
            break;
        }

        /* Don't print out fusion transactions */
        if (!tx.isFusionTransaction())
        {
            /* Aquire the lock, so we're not interleaving our output when a
               command is being handled, for example, transferring */
            std::scoped_lock lock(*m_mutex);

            std::cout << InformationMsg("\nNew transaction found!\n\n");

            if (tx.totalAmount() < 0)
            {
                printOutgoingTransfer(tx);
            }
            else
            {
                printIncomingTransfer(tx);
            }
        }
    }

    m_walletBackend->m_eventHandler->onTransaction.unsubscribe();
}

void TransactionMonitor::stop()
{
    m_shouldStop = true;
    m_queuedTransactions.stop();
}

std::shared_ptr<std::mutex> TransactionMonitor::getMutex() const
{
    return m_mutex;
}
