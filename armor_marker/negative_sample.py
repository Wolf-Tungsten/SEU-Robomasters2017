import cv2
import os
import numpy as np

camera = cv2.VideoCapture(0)
camera.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
path = '/Users/WolfTungsten/Development/SEU-Robomasters2017/armor-sample/blue/negative'
bg_file = open(os.path.join(path,'bg.dat'), 'w')

for i in range(600):
    ret, frame = camera.read()
    ROI = frame[300:420, 300:420]
    img_name = 'img' + str(i) +'.jpg'
    cv2.imwrite(os.path.join(path,img_name),ROI)
    bg_file.write(os.path.join(path,img_name)+ '\n')
    cv2.waitKey(10)
    print('捕获第', i,'张')

bg_file.close()
