# -*- coding: utf-8 -*-
from dcore import *

MAINNET_ENDPOINT = 'wss://api.decent.ch'
TESTNET_ENDPOINT = 'wss://testnet-socket.dcore.io'
WALLET_FILE = Path.instance().home + '/wallet.json'

CORE_ASSET_ID = AssetId(ObjectId(1,3,0))
CORE_UNIT_PRICE = Price.unit_price(CORE_ASSET_ID)

def _wallet(wallet_file, endpoint):
    w = Wallet()
    w.connect(wallet_file, endpoint)
    return w

Wallet.mainnet = staticmethod(lambda wallet_file = WALLET_FILE: _wallet(wallet_file, MAINNET_ENDPOINT))
Wallet.testnet = staticmethod(lambda wallet_file = WALLET_FILE: _wallet(wallet_file, TESTNET_ENDPOINT))

def dump_accounts_balances(wallet):
    for name, id in wallet.lookup_accounts('', wallet.get_account_count()).items():
        print(f'{name}:', ', '.join([b.pretty_amount for b in wallet.list_account_balances(str(id))]))

def calculate_signature(trx, key, chain_id):
    """Calculate transaction signature.

       trx - transaction to be signed
       key - signing private key
       chain_id - chain identification
    """
    return key.sign_compact(trx.signature_digest(chain_id), True)
