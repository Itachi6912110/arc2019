from time import sleep
import numpy as np
import serial
import time

class port:
	def __init__(self, port, baudrate):
		if not self.connect(port, baudrate):
			quit()

	def connect(self, port, baudrate):
		print('Connecting...')
		try:
			self.ser = serial.Serial(port, baudrate=baudrate)
			print('Success connect to serial port ' + port)
			print('')
		except serial.serialutil.SerialEXception:
			print('Fail to connect')
			print('')
			return False
		return True

	def is_connect(self):
		return self.ser.is_open()

	def serialRead(self, length, debug=False):
		tic = time.time()
		#self.ser.flushInput()
		#self.ser.read(1)
		#chunk = 256*64
		count = 0
		data = np.zeros(length, dtype=np.uint8)
		while 1:
			waiting = self.ser.inWaiting()
			if waiting:
				'''
				if waiting == 511:
					xx = self.ser.read(2)
					waiting -= 2
					print(xx)
				'''
				#print(waiting)
				rv = ''.join([chr(c) for c in self.ser.read(waiting)])

				value = np.array([ord(c) for c in rv])
				#print(len(value))
				data[count:count+len(value)] = value
				count += len(value)


			if count == length:
				toc = time.time()
				print('-----------------------------------------------------------------------------------')
				print('Transmition time: ' + str(toc-tic) + 'secs')
				return data
	
	def deleteAmbiguity(self):
		tic = time.time()
		for i in range(10):
			waiting = self.ser.inWaiting()
			if waiting:
				rv = self.ser.read(waiting)
				value = int(rv, byteorder='big')
				data[count] = value
				count += 1
			time.sleep(0.000001)
	

	def serialWrite(self, data, length):
		for i in range(length):
			self.ser.write(bytes(data[i]))

	def serialSendStart(self):
		self.ser.write(b'\r')
		self.ser.flushOutput()
		#self.ser.flushInput()

	def flush(self):
		self.ser.flushInput()

				