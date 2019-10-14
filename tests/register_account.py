import sys
import DCore as D

name = sys.argv[1]

w = D.Wallet.testnet()
w.set_password('test')
w.unlock('test')

brainkey = D.generate_brain_key()
privkey = D.derive_private_key(brainkey)
pubkey = D.PublicKey(privkey.get_public_key())

a = w.get_account('public-account-1')
owner = D.Authority()
owner.weight_threshold = 1
owner.account_auths = { a.get_id(): 1 }
active = D.Authority()
active.key_auths = { pubkey: 1 }
active.weight_threshold = 1

try:
    trx = w.create_account(brainkey, name, a.name)
    print(trx)
    trx = w.register_account(name, D.PublicKey('DCT7SgGZgKt6KWMJMiHmaMxw99mkfqMGLAywRfbPNKB4GV7tSr3BK'), pubkey, pubkey, a.name)
    print(trx)
    trx = w.register_multisig_account(name, owner, active, pubkey, a.name)
    print(trx)
except D.Exception as e:
    print(e)
