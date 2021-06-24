# import PyQt5 as qt

# designer:
# pyqt5designer
# https://python-scripts.com/pyqt5#signal-slot-editor
# https://pythonworld.ru/gui/pyqt5-painting.html

import sys
from PyQt5.QtWidgets import QWidget, QApplication
from PyQt5.QtGui import QPainter, QColor, QFont
from PyQt5.QtCore import Qt


class Example(QWidget):
	def __init__(self):
		super().__init__()
		self.initUI()


	def initUI(self):
		self.amp = 700
		self.rule = [0, 1, 0, 1, 1, 0, 0, 0]
		#                 x    y          w         h
		self.setGeometry(300, 300, self.amp*2 + 1, self.amp)
		self.setWindowTitle('Draw text')
		self.show()

	def paintEvent(self, event):
		qp = QPainter()
		qp.begin(self)
		self.cellAutomat(event, qp)
		qp.end()

	def cellAutomat(self, event, qp):
		_prev = "0"*self.amp + "1" + "0"*self.amp
		_next = ""

		for i in range(self.amp):
			for cell in enumerate(_prev):
				pn = int(_prev[cell[0]-1])
				cn = int(cell[1])
				nn = int(_prev[(cell[0]+1) % len(_prev)])

				rule_pos = pn * (2**2) + cn * 2 + nn

				_next += str(self.rule[rule_pos])

			for c in enumerate(_next):
				color = 0
				if c[1] == "0":
					color = 255

				qp.setPen( QColor(color, color, color) )
				qp.drawPoint(c[0], i)

			_prev = _next
			_next = ""


if __name__ == '__main__':
	app = QApplication(sys.argv)
	ex = Example()
	sys.exit(app.exec_())
