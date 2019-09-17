# -*- coding: utf-8 -*-
import sys
import DCore
import PySide2.QtCore as QtCore
import PySide2.QtWidgets as QtWidgets

class TokenSummaryModel(QtCore.QAbstractTableModel):
    def __init__(self, parent = None):
        QtCore.QAbstractTableModel.__init__(self, parent)
        self.tokens = []

    def rowCount(self, parent = QtCore.QModelIndex()):
        return len(self.tokens)

    def columnCount(self, parent = QtCore.QModelIndex()):
        return 2

    def headerData(self, section, orientation, role):
        if orientation == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:
            if section == 0:
                return 'Symbol'
            elif section == 1:
                return 'Count'
        return None

    def data(self, index, role):
        if role != QtCore.Qt.DisplayRole:
            return None

        token, count = self.tokens[index.row()]
        if index.column() == 0:
            return token.symbol
        elif index.column() == 1:
            return count
        return None

    def clear_tokens(self):
        self.beginRemoveRows(QtCore.QModelIndex(), 0, self.rowCount())
        self.tokens = []
        self.endRemoveRows()

    def add_token(self, token, count):
        rows = self.rowCount()
        self.beginInsertRows(QtCore.QModelIndex(), rows, rows + 1)
        self.tokens.append((token, count))
        self.endInsertRows()

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, wallet):
        QtWidgets.QMainWindow.__init__(self)
        self.wallet = wallet

        self.accounts = QtWidgets.QComboBox()
        self.accounts.setEditable(True)
        self.accounts.setInsertPolicy(QtWidgets.QComboBox.NoInsert)
        for account in self.wallet.list_my_accounts():
            self.accounts.addItem(account.name)
            self.accounts.setItemData(self.accounts.count() - 1, account.get_id())
        self.accounts.activated[int].connect(self.load_summary)
        self.accounts.lineEdit().editingFinished.connect(lambda: self.load_summary(self.accounts.currentIndex()))

        t = self.addToolBar('Accounts')
        t.addWidget(QtWidgets.QLabel('Account:'))
        t.addWidget(self.accounts)

        self.token_summary_model = TokenSummaryModel(self)
        self.token_summary = QtWidgets.QTableView(self)
        self.token_summary.setModel(self.token_summary_model)
        self.token_summary.setFocus()
        self.setCentralWidget(self.token_summary)

        self.load_summary(self.accounts.currentIndex())

    def load_summary(self, index):
        if index == -1:
            return

        account_id = self.accounts.currentData()
        self.token_summary_model.clear_tokens()

        try:
            for id, count in self.wallet.get_non_fungible_token_summary(account_id).items():
                tokens = self.wallet.get_non_fungible_tokens([id])
                self.token_summary_model.add_token(tokens[0], count)
        except DCore.Exception as e:
            QtWidgets.QMessageBox.critical(self, 'Load Summary', str(e))

#d = DCore.NonFungibleTokenDataValue('abc', 123)
#print(d)

#print(wallet.get_non_fungible_token('NOTAPPLE'))
#for nft_data in wallet.list_non_fungible_token_data('NOTAPPLE'):
#    for v in nft_data.data:
#        print(type(v), v)

app = QtWidgets.QApplication(sys.argv)

if len(sys.argv) < 2:
    wallet_file, filter = QtWidgets.QFileDialog.getOpenFileName(None, 'Wallet File')
else:
    wallet_file = sys.argv[1]

wallet = DCore.Wallet()
wallet.connect(wallet_file)

if len(sys.argv) < 3:
    wallet_passwd, ok = QtWidgets.QInputDialog.getText(None, 'Unlock Wallet', 'Password:', QtWidgets.QLineEdit.EchoMode.PasswordEchoOnEdit)
else:
    wallet_passwd, ok = sys.argv[2], True

if ok:
    wallet.unlock(wallet_passwd)

window = MainWindow(wallet)
window.show()

sys.exit(app.exec_())
