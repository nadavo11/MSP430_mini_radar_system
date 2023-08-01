import re
import sys
from queue import Queue
PORT = 'COM13'
MODE = 0
TH = 10
MAX_DIST = 12000
from PyQt5 import QtGui, QtCore
from PyQt5.QtWidgets import QPushButton, QApplication, QMainWindow, QVBoxLayout, QWidget, QFileDialog, QTextEdit
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

def detect_lights(l0,l1,direction):
    """
    detect lights by recording the direction of the light
    according to weather the input of sensor 0 or 1 is greater
    and finding turning points where l0 ~== l1
    :param l0: light sensor 0
    :param l1: light sensor 1
    :param direction: scanning direction

    return: None if no light detected
            1 if light is in the same direction as the scanning direction
            -1 if light is in the opposite direction as the scanning direction
    """
    TH = 10
    if l0 < TH or l1 <TH:
        return




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
        self.light_data1 = np.zeros(180)
        self.light_data2 = np.zeros(180)
        self.light = np.zeros(180)
        self.previous_l = 0

        # Create the canvas
        self.canvas = FigureCanvas(self.fig)
        main_layout.addWidget(self.canvas)


        # Create a button to send data to Arduino
        # red stop button

        self.button_stop = QPushButton("stop", self)
        self.button_stop.setStyleSheet("font: bold 16px; color: red; background-color: hsl(20, 20%, 5%);")
        self.button_stop.pressed.connect(self.stop)

        self.button1 = QPushButton("Ultra Sonc Scan", self)
        self.button1.pressed.connect(self.scan_obj)

        self.button2 = QPushButton("LDR scan", self)
        self.button2.pressed.connect(self.scan_light)

        self.button3 = QPushButton("Integrated scan", self)
        self.button3.pressed.connect(self.scan_both)

        self.button_mode_toggle = QPushButton("Script Mode", self)
        self.button_mode_toggle.pressed.connect(self.toggle_mode)


        # Initialize the Arduino communication
        self.serial = QSerialPort(self)
        self.serial.setPortName(PORT)
        self.serial.setBaudRate(QSerialPort.Baud9600)
        self.serial.readyRead.connect(self.read_data)
        self.serial.open(QIODevice.ReadWrite)

        self.setStyleSheet("background-color: hsl(20, 20%, 5%); color: Aquamarine;")

        main_layout.addWidget(self.button1)

        main_layout.addWidget(self.button2)

        main_layout.addWidget(self.button3)

        main_layout.addWidget(self.button_mode_toggle)

        main_layout.addWidget(self.button_stop)

        """
        ______________________________________________
        
                        script mode
        
        ______________________________________________
        """

        # Text box for displaying script content
        self.text_box = QTextEdit(self)
        self.text_box.setReadOnly(True)
        self.text_box.hide()

        self.button_compile = QPushButton("Compile script", self)
        self.button_compile.pressed.connect(self.compile)
        self.button_compile.hide()

        self.button_load = QPushButton("⬇️Load Script⬇️", self)
        self.button_load.pressed.connect(self.load_script)
        self.button_load.hide()

        self.button_burn = QPushButton("🔥burn Script🔥", self)
        self.button_burn.pressed.connect(self.burn_script)
        self.button_burn.hide()

        main_layout.addWidget(self.text_box)
        main_layout.addWidget(self.button_compile)
        main_layout.addWidget(self.button_load)
        main_layout.addWidget(self.button_burn)


        # Flag to track the current mode (True for radar mode, False for script mode)
        self.radar_mode = True
    @QtCore.pyqtSlot()

    def read_data(self):
        """
        Read the data from MCU and update the radar plot
        if an object is detected, the radar data will be updated
        if
        """

        # Read the UART signal from Arduino
        line = self.serial.readLine().data().decode().strip()
        if len(line) == 1:
            line.join(self.serial.readLine().data().decode().strip())
        print(line)
        print("_______________________")
        line = line.split('|')
        r = MAX_DIST
        if not line[0]:
            line.pop(0)
        print(line)

        if len(line) == 5 and not line[-1] and line[0] and line[1] and line[2] and line[3]:
            phi = int(line[0])
            l1 = int(line[1])
            l2 = int(line[2])
            r = int(line[3])
            self.object_data[phi : phi+3] = min(abs(r), MAX_DIST)
            self.light_data1[phi] = 1023 - l1
            self.light_data2[phi] = 1023 - l2

            argmax = (np.argmax(self.light_data1) + np.argmax(self.light_data2))//2
            print(argmax)
            self.light[phi : phi+3] = 0
            self.light[argmax] = (np.max(self.light_data1) + np.max(self.light_data2))




        self.update_radar(self.object_data)
    @QtCore.pyqtSlot()
    def update_radar(self, object_data):

        # Clear the current plot
        self.ax.clear()

        # Set the theta values for the radar plot
        theta = np.deg2rad(np.arange(0, 180))
        """
        #############################
        #   objects plot            #
        #############################
        """

        # Plot the data points
        self.ax.plot(theta, object_data)

        # Find the indices where objects are detected
        object_indices = np.where(object_data < MAX_DIST)

        # Calculate the intensities of the detected objects
        intensities = object_data[object_indices]

        # Plot markers for the detected objects
        self.ax.scatter(np.deg2rad(object_indices), intensities, c=intensities, cmap=plt.cm.jet, alpha=0.5, linestyle='')
        # Fill the area inside the radar plot
        self.ax.fill(theta, object_data, alpha=0.25)
        """
       #############################
       #   lights plot            #
       #############################
       """

        # plot one light on the argmax of the light data
        #self.ax.scatter(np.deg2rad(np.argmax(self.light)), self.light[np.argmax(self.light)], c="yellow", s=1000, alpha=0.5, linestyle='')

        self.ax.scatter(np.deg2rad(np.where(self.light >0)), self.light[np.where(self.light >0)]+7000, c="yellow", s=self.light[np.where(self.light >0)],
                        alpha=0.5, linestyle='')


        #self.ax.scatter(np.deg2rad(object_indices), intensities + 10, s=0, c="yellow", alpha=0.5, linestyle='')


        # Set the title and grid
        self.ax.set_title("Radar Monitor", fontsize=20, fontweight='bold')
        self.ax.grid(True)

        # Set the radial tick labels
        self.ax.set_yticklabels([])

        # Redraw the canvas
        self.canvas.draw()
    @QtCore.pyqtSlot()
    def write_data(self, data):
        # Write data to Arduino
        data = QByteArray(data.encode())
        print(data)
        self.serial.write(data)

    def stop(self):
        self.write_data('0')
    def scan_obj(self):
        # Write '1' to Arduino
        state = 0
        self.write_data('1')
    def scan_light(self):
        # Write '1' to Arduino
        self.write_data('2')
    def scan_both(self):
        # Write '1' to Arduino
        self.write_data('3')
    def toggle_mode(self):
        self.radar_mode = not self.radar_mode

        if self.radar_mode:
            self.write_data('r')
            # Show radar mode

            self.text_box.hide()
            self.canvas.show()
            self.ax.clear()
            self.update_radar(self.object_data)

            self.button_compile.hide()
            self.button_load.hide()
            self.button_burn.hide()
        else:
            self.write_data('s')
            # Show script mode

            self.text_box.show()
            self.canvas.hide()
            self.ax.clear()
            self.text_box.clear()
            self.button_compile.show()
            self.button_load.show()
            self.button_burn.show()

    #def load_txt(self):


    def load_script(self):
        # UART send script mode
        self.write_data('s')
        # Open a file dialog to choose a .txt file
        file_dialog = QFileDialog(self)
        file_dialog.setNameFilter("Text Files (*.txt)")
        if file_dialog.exec_():
            file_path = file_dialog.selectedFiles()[0]
            with open(file_path, "r") as file:
                script_content = file.read()
                self.text_box.setPlainText(script_content)
    def compile(self):
        txt = self.text_box.toPlainText()
        txt = re.sub(r'\d+', lambda match: str(format(int(match.group()),'02x')), txt)
        txt = txt.replace("inc_lcd ", "01")
        txt = txt.replace("dec_lcd ", "02")
        txt = txt.replace("rra_lcd ", "03")
        txt = txt.replace("set_delay ", "04")
        txt = txt.replace("clear_lcd", "05")
        txt = txt.replace("servo_deg ", "06")
        txt = txt.replace("servo_scan ", "07")
        txt = txt.replace("sleep", "08")
        txt = txt.replace(",", "")


        print(txt)
        self.text_box.setPlainText(txt)

        # Write '1' to Arduino
        self.write_data('3')

    def burn_script(self):
        txt = self.text_box.toPlainText().replace("\n", "")
        self.write_data(txt + '\n')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
