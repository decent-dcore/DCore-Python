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

msg = D.Operation.Custom.MessagePayload()
msg.sender = D.AccountId(D.ObjectId(1,2,19))
msg.key = D.PublicKey('DCT7SgGZgKt6KWMJMiHmaMxw99mkfqMGLAywRfbPNKB4GV7tSr3BK')
msg.receivers = [D.Operation.Custom.MessagePayload.Data('hello python', ppk, pub, a.get_id())]

tr = D.Operation.Custom()
tr.payer = D.AccountId(D.ObjectId(1,2,19))
tr.id = D.Operation.Custom.SUBTYPE_MESSAGING
tr.message_payload = msg
#print(tr.message_payload.receivers[0].get_message(ppk, pub))

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
