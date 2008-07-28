from PyQt4 import QtGui, QtCore

class SparesWidget(QtGui.QWidget):
        def __init__(self, sparesInfo, parent = None, parentController = None):
            QtGui.QWidget.__init__(self, parent)
            self.parentController = parentController
            self.gridLayout = QtGui.QGridLayout()
            self.setGeometry(10, 240, 300, 200)
            self.sparesInfo = sparesInfo
            self.createLabels()
            self.createLineEdits()
            self.createButtons()
            self.setLayout(self.gridLayout)
            self.show()
        def setSparesInfo(self, sparesInfo):
            self.sparesInfo = sparesInfo
            self.updateLineEdits()
            self.update()
        def createLabels(self):
            self.equipLabel = QtGui.QLabel(str("Spares in lhcbintegration"), self)
            self.equipLabel.setFixedSize(200, 20)
            #self.equipLabel.setMinimumSize(200, 20)
            self.confLabel = QtGui.QLabel(str("Spares in ConfDB"), self)
            self.confLabel.setMaximumSize(200, 20)
            self.changedLabel = QtGui.QLabel(str("Changed Spares"), self)
            self.changedLabel.setMaximumSize(200, 20)
            self.newLabel = QtGui.QLabel(str("New Spares"), self)
            self.newLabel.setMaximumSize(200, 20)
            self.gridLayout.addWidget(self.equipLabel, 0, 0)
            self.gridLayout.addWidget(self.confLabel, 1, 0)
            self.gridLayout.addWidget(self.changedLabel, 2, 0)
            self.gridLayout.addWidget(self.newLabel, 3, 0)
        def createLineEdits(self):
            self.equipLineEdit = QtGui.QLineEdit(str(self.sparesInfo.SparesInEquipDB), self)
            self.equipLineEdit.setReadOnly(True)
            self.equipLineEdit.setMaximumSize(50, 20)
            self.confLineEdit = QtGui.QLineEdit(str(self.sparesInfo.SparesInConfDB), self)
            self.confLineEdit.setReadOnly(True)
            self.confLineEdit.setMaximumSize(50, 20)
            self.changedLineEdit = QtGui.QLineEdit(str(self.sparesInfo.ChangedSpares), self)
            self.changedLineEdit.setReadOnly(True)
            self.changedLineEdit.setMaximumSize(50, 20)
            self.newLineEdit = QtGui.QLineEdit(str(self.sparesInfo.NewSpares), self)
            self.newLineEdit.setReadOnly(True)
            self.newLineEdit.setMaximumSize(50, 20)
            self.gridLayout.addWidget(self.equipLineEdit, 0, 1)
            self.gridLayout.addWidget(self.confLineEdit, 1, 1)
            self.gridLayout.addWidget(self.changedLineEdit, 2, 1)
            self.gridLayout.addWidget(self.newLineEdit, 3, 1)
        def createButtons(self):
            self.deleteAllSparesButton = QtGui.QPushButton(self)
            self.deleteAllSparesButton.setText("Delete")
            self.deleteAllSparesButton.setToolTip("Delete all spares from ConfDB")
            self.deleteAllSparesButton.setFixedSize(50, 20)
            self.gridLayout.addWidget(self.deleteAllSparesButton, 1, 2)
            self.connect(self.deleteAllSparesButton, QtCore.SIGNAL("clicked()"), self.parentController.onDeleteSpares)
            ############################################################################
            self.updateSparesButton = QtGui.QPushButton(self)
            self.updateSparesButton.setText("Update")
            self.updateSparesButton.setToolTip("Update existing spares in ConfDB")
            self.updateSparesButton.setFixedSize(50, 20)
            self.gridLayout.addWidget(self.updateSparesButton, 2, 2)
            self.connect(self.updateSparesButton, QtCore.SIGNAL("clicked()"), self.parentController.onUpdateSpares)
            ############################################################################
            self.insertNewSparesButton = QtGui.QPushButton(self)
            self.insertNewSparesButton.setText("Insert")
            self.insertNewSparesButton.setToolTip("Insert new spares into ConfDB")
            self.insertNewSparesButton.setFixedSize(50, 20)
            self.gridLayout.addWidget(self.insertNewSparesButton, 3, 2)
            self.connect(self.insertNewSparesButton, QtCore.SIGNAL("clicked()"), self.parentController.onInsertNewSpares)
        def updateLineEdits(self):
            self.equipLineEdit.setText(str(self.sparesInfo.SparesInEquipDB))
            self.confLineEdit.setText(str(self.sparesInfo.SparesInConfDB))
            self.changedLineEdit.setText(str(self.sparesInfo.ChangedSpares))
            self.newLineEdit.setText(str(self.sparesInfo.NewSpares))