import cv2
import numpy as np
import random

O = np.ones((256,256,1,1))
Z = np.zeros((256,256,1,1))

A = np.expand_dims(cv2.imread('./I_sharp.png'),axis=3) / 255
B = np.expand_dims(cv2.imread('./I_blurred.png'),axis=3) / 255

R = np.concatenate((Z,Z,O),axis=2)
G = np.concatenate((Z,O,Z),axis=2)

data = np.concatenate((A,B,R,G),axis=3)

while(1):
	idx = random.randint(0,3)

	frame = data[:,:,:,idx]

	cv2.imshow('Frame', frame)

	if cv2.waitKey(500) & 0xFF == ord('q'):
		break
