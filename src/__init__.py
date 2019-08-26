from dcore import *

def testnet(self, wallet_file):
    self.connect(wallet_file, 'wss://testnet-socket.dcore.io')

def mainnet(self, wallet_file):
    self.connect(wallet_file, 'wss://api.decent.ch')

Wallet.testnet = testnet
Wallet.mainnet = mainnet
