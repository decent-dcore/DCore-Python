# -*- coding: utf-8 -*-
from dcore import *

MAINNET_ENDPOINT = 'wss://api.decent.ch'
TESTNET_ENDPOINT = 'wss://testnet-socket.dcore.io'
WALLET_FILE = Path.instance().home + '/wallet.json'

def _wallet(wallet_file, endpoint):
    w = Wallet()
    w.connect(wallet_file, endpoint)
    return w

Wallet.mainnet = staticmethod(lambda wallet_file = WALLET_FILE: _wallet(wallet_file, MAINNET_ENDPOINT))
Wallet.testnet = staticmethod(lambda wallet_file = WALLET_FILE: _wallet(wallet_file, TESTNET_ENDPOINT))

def dump_accounts_balances(wallet):
    for name, id in wallet.list_accounts('', wallet.get_account_count()).items():
        print(f'{name}:', ', '.join([b.pretty_amount for b in wallet.list_account_balances(str(id))]))
