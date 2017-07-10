import cv2
import os
import sys
from random import Random


point_list = []
point_counter = 0
#输入路径
img_path = '/Volumes/SD/BLUE2/'
#输出路径
output_path = '/Volumes/SD/BlueOutput/'
file_list = os.listdir('/Volumes/SD/BLUE3')
cv2.namedWindow('mark',cv2.WND_PROP_OPENGL)
cv2.namedWindow('result')
img =''
temp = ''
file = ''
blue_or_red = 'blue'

def random_str(randomlength=8):
    rstr = ''
    chars = 'AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz'
    length = len(chars) - 1
    random = Random()
    for i in range(randomlength):
        rstr += chars[random.randint(0, length)]
    return rstr

def mouse_Callback(event,x,y,flags,param):
    global point_counter
    x = int(x*2)
    if event == cv2.EVENT_LBUTTONDOWN:
        point_list.append([x, y])
        print(x, y)
        cv2.circle(temp, (x, y), 3, (255, 0, 0), 2)
        cv2.imshow('mark', temp)
        point_counter = point_counter + 1
        if point_counter >= 4:
            minX = min(point_list[i][0] for i in range(point_counter))
            minY = min(point_list[i][1] for i in range(point_counter))
            maxX = max(point_list[i][0] for i in range(point_counter))
            maxY = max(point_list[i][1] for i in range(point_counter))
            cv2.rectangle(temp, (minX, minY), (maxX, maxY), (0,255,0),2)
            ramstr = random_str(5)
            new_filename = 'x' + str(minX) + 'y'+str(minY)+'w' +str(maxX-minX)+'h'+str(maxY-minY)+'-'+ramstr+'.jpg'
            new_view_filename = 'view-'+'x' + str(minX) + 'y' + str(minY) + 'w' + str(maxX - minX) + 'h' + str(
                maxY - minY)+'-' + ramstr + '.jpg'
            new_file = os.path.join(output_path,new_filename)
            new_view_file =os.path.join(output_path,new_view_filename)
            print(new_file)
            cv2.imshow('mark', temp)
            cv2.imwrite(new_file,img)
            cv2.imwrite(new_view_file,temp)












for filename in file_list:
    file = os.path.join(img_path, filename)
    #print(file)
    img = cv2.imread(file)
    cv2.setMouseCallback('mark', mouse_Callback)
    if not img is None:
        point_list = []
        point_counter = 0
        temp = img.copy()
        cv2.imshow('mark', temp)
        cv2.waitKey(0)
        print('下一张 ')
        os.remove(file)

