import sys, time
import DCore as D

name = sys.argv[1]

w = D.Wallet.testnet()
w.unlock('test')

chp = w.get_chain_properties()
gp = w.get_global_properties()
dgp = w.get_dynamic_global_properties()

a = w.get_account(name)
ppk = D.PrivateKey.from_string('5KfatbpE1zVdnHgFydT7Cg9hJmUVLN7vQXJkBbzGrNSND3uFmAa')
pub = a.options.memo_key
b = D.Balance()
b.amount = 1

tr = D.Operation.Transfer()
tr.sender = D.AccountId(D.ObjectId(1,2,19))
tr.receiver = a.object_id
tr.amount = b
tr.memo = D.Memo('python test', ppk, pub)
#print(tr.memo.get_message(ppk, pub))

op = D.Operation(tr)
gp.parameters.current_fees.set_fee(op, D.CORE_UNIT_PRICE)
op.validate()
#print(op)

trx = D.SignedTransaction()
trx.set_reference_block(dgp.head_block_id)
trx.expiration = D.TimePointSec(round(time.time()) + 30)
trx.operations = [op]
trx.sign(ppk, chp.chain_id)
trx.validate()
print(trx)

w.broadcast_transaction(trx)
