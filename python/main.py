import numpy as np
import cv2
from port import port

def main():

	p = port(port='/dev/ttyUSB0', baudrate=1000000)

	#rx = p.serialRead(1)
	#p.deleteAmbiguity()

	for i in range(3):

		p.serialSendStart()

		rx = p.serialRead(65536).reshape((256,256))
		
		cv2.imshow('frame', rx)

		cv2.waitKey(500)
		
		p.flush()

		

if __name__ == '__main__':
	main()