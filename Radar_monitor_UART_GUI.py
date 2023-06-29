import sys
from queue import Queue

from PyQt5 import QtGui, QtCore
from PyQt5.QtWidgets import QPushButton, QApplication, QMainWindow, QVBoxLayout, QWidget
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.cm import ScalarMappable
import sys, time, serial


import time
plt.style.use('dark_background')
from PyQt5.QtCore import QIODevice, QByteArray
from PyQt5.QtSerialPort import QSerialPort

class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()
        self.setWindowTitle("Radar Monitor")
        self.setGeometry(200, 200, 1400, 1200)

        # Create the main layout
        main_widget = QWidget(self)
        main_layout = QVBoxLayout(main_widget)
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)

        # Create the matplotlib figure and canvas
        self.fig = plt.figure(figsize=(8, 8))
        self.ax = self.fig.add_subplot(111, polar=True)

        # Set the title and grid
        self.ax.set_title("Radar Monitor", fontsize=20, fontweight='bold')
        self.ax.grid(True)

        # Set the radial tick labels
        self.ax.set_yticklabels([])

        # Set the azimuthal tick labels
        tick_positions = np.arange(0, 181, 10)
        self.ax.set_xticks(np.deg2rad(tick_positions))
        self.ax.set_xticklabels([str(i) for i in tick_positions])

        # Create a ScalarMappable object for the colorbar
        sm = ScalarMappable(cmap=plt.cm.gray)
        sm.set_array([])  # Set an empty array for now

        self.object_data = np.zeros(180) +60
        # Create the canvas
        self.canvas = FigureCanvas(self.fig)
        main_layout.addWidget(self.canvas)

        # Create a button to send data to Arduino
        # self.send_button = QPushButton("Send", self)
        # self.send_button.clicked.connect(self.send_data)
        # main_layout.addWidget(self.send_button)

        # Initialize the Arduino communication
        self.serial = QSerialPort(self)
        self.serial.setPortName('COM9')
        self.serial.setBaudRate(QSerialPort.Baud9600)
        self.serial.readyRead.connect(self.read_data)
        self.serial.open(QIODevice.ReadWrite)

        self.setStyleSheet("background-color: hsl(20, 20%, 5%); color: Aquamarine;")


    def read_data(self):

        # Read the UART signal from Arduino
        line = self.serial.readAll().data().decode().strip()

        line = line.split(' | ')

        phi = int(line[0])
        r = int(line[1])
        # Process the data from Arduino
        #object_data = np.array(line.split(','), dtype=int)
        print(line)
        line = 6
        self.object_data[phi] = min(r, 60)
        # self.object_data[self.object_data > 60] = 60
        # Update the radar plot
        self.update_radar(self.object_data)

    def update_radar(self, object_data):

        # Clear the current plot
        self.ax.clear()

        # Set the theta values for the radar plot
        theta = np.deg2rad(np.arange(0, 180))

        # Plot the data points
        self.ax.plot(theta, object_data)

        # Find the indices where objects are detected
        object_indices = np.where(object_data < 60)[0]

        # Calculate the intensities of the detected objects
        intensities = object_data[object_indices]

        # Plot markers for the detected objects
        self.ax.scatter(np.deg2rad(object_indices), intensities, c=intensities, cmap=plt.cm.jet, alpha=0.5, linestyle='')

        # Fill the area inside the radar plot
        self.ax.fill(theta, object_data, alpha=0.25)

        # Set the title and grid
        self.ax.set_title("Radar Monitor", fontsize=20, fontweight='bold')
        self.ax.grid(True)

        # Set the radial tick labels
        self.ax.set_yticklabels([])

        # Redraw the canvas
        self.canvas.draw()

    def write_data(self, data):
        # Write data to Arduino
        data = QByteArray(data.encode())
        self.serial.write(data)

    def send_data(self):
        # Write '1' to Arduino
        self.write_data('1')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
