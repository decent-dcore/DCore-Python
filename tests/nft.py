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
                return 'Balance'
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

    def get_token(self, row):
        return self.tokens[row][0] if row < self.rowCount() else None

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
        self.accounts.lineEdit().editingFinished.connect(self.load_summary)

        t = self.addToolBar('Accounts')
        t.addWidget(QtWidgets.QLabel('Account:'))
        t.addWidget(self.accounts)

        self.token_summary_model = TokenSummaryModel(self)
        self.token_summary = QtWidgets.QTableView()
        self.token_summary.setModel(self.token_summary_model)
        self.token_summary.setSelectionBehavior(QtWidgets.QAbstractItemView.SelectionBehavior.SelectRows)
        self.token_summary.selectionModel().currentRowChanged.connect(lambda current: self.load_info(current.row()))

        self.token_info = QtWidgets.QLabel()

        self.central_widget = QtWidgets.QSplitter(self)
        self.central_widget.addWidget(self.token_summary)
        self.central_widget.addWidget(self.token_info)
        self.setCentralWidget(self.central_widget)
        self.statusBar().show()

        index = self.accounts.currentIndex()
        if index != -1:
            self.load_summary(index)
            self.token_summary.selectRow(0)

    def get_account_id(self, name):
        account = self.wallet.get_account(name)
        if account is None:
            self.statusBar().showMessage('Account {0} does not exist.'.format(name), 5000)
            return None

        return account.get_id()

    def load_info(self, row):
        tokens = self.wallet.get_non_fungible_tokens([self.token_summary_model.get_token(row).get_id()]) if row != -1 else None
        if tokens is None:
            self.token_info.clear()
            return

        self.token_info.setText(tokens[0].symbol)

    def load_summary(self, index = -1):
        account_id = self.get_account_id(self.accounts.currentText()) if index == -1 else self.accounts.itemData(index)
        if account_id is None:
            return

        self.token_summary_model.clear_tokens()

        try:
            for id, count in self.wallet.get_non_fungible_token_summary(account_id).items():
                tokens = self.wallet.get_non_fungible_tokens([id])
                self.token_summary_model.add_token(tokens[0], count)
        except DCore.Exception as e:
            self.statusBar().showMessage(str(e), 5000)

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
