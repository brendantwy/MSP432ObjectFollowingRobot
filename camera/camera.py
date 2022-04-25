import cv2
from picamera.array import PiRGBArray
from picamera import PiCamera
import numpy as np
import serial
import time

reso_width = 640
reso_height = 480

def nothing(x):
        pass
    
def midpoint(ptA, ptB):
    return (int((ptA[0] + ptB[0]) * 0.5), int((ptA[1] + ptB[1]) * 0.5))

camera = PiCamera()
camera.resolution = (reso_width, reso_height)
camera.framerate = 30
frame_rate_calc = 1
freq = cv2.getTickFrequency()
border_x_start = int(0.30 * reso_width)
border_x_minor_start = int(0.15 * reso_width)
border_x_end = int(reso_width - border_x_start)
border_x_minor_end = int(reso_width - border_x_minor_start)

ser = serial.Serial("/dev/ttyAMA0", baudrate = 9600,timeout=1)
ser.reset_input_buffer()
ser.reset_output_buffer()

rawCapture = PiRGBArray(camera, size=(reso_width, reso_height))

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    image = frame.array

    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    color = np.uint8([[[255, 255, 80]]])
    hsvColor = cv2.cvtColor(color, cv2.COLOR_BGR2HSV)
    lowerLimit = np.uint8([hsvColor[0][0][0]-10,100,100])
    upperLimit = np.uint8([hsvColor[0][0][0] + 10,255,255])

    mask = cv2.inRange(hsv, lowerLimit, upperLimit)
    
    result = cv2.bitwise_and(image, image, mask=mask)

    image2, contours, hierarchy = cv2.findContours(mask, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    if len(contours) != 0:
        c = max(contours, key = cv2.contourArea)
        x,y,w,h = cv2.boundingRect(c)
        object_area = w * h
        cv2.rectangle(result, (x,y), (x+w, y+h), (0,255,0), 2)
        cv2.circle(result, (midpoint((x,y), (x+w, y+h))), radius=1, color=(0,255,0), thickness=-1)
        cv2.line(result, (border_x_start, 0), (border_x_start,reso_height), color=(0,255,0), thickness=1)
        cv2.line(result, (border_x_end, 0), (border_x_end, reso_height), color=(0,255,0), thickness=1)
        
        cv2.line(result, (border_x_minor_start, 0), (border_x_minor_start,reso_height), color=(0,255,0), thickness=1)
        cv2.line(result, (border_x_minor_end, 0), (border_x_minor_end, reso_height), color=(0,255,0), thickness=1)
        if(midpoint((x,y), (x+w, y+h))[0] < border_x_start):
            ser.write('l'.encode())
            print("left")
            #if (midpoint((x,y), (x+w, y+h))[0] < border_x_minor_start):
                #ser.write('t'.encode())
            
        elif(midpoint((x,y), (x+w, y+h))[0] > border_x_end):
            ser.write('r'.encode())
            print("right")
            #if (midpoint((x,y),(x+w, y+h))[0] > border_x_minor_end):
                #ser.write('y'.encode())
            
        elif(midpoint((x,y),(x+w, y+h))[0] > border_x_start and midpoint((x,y), (x+w, y+h))[0] < border_x_end):
            print(object_area)
            if(object_area > 9000):
                ser.write('s'.encode())
                print("stop")
                
            elif(object_area < 100000 and object_area > 2000):
                ser.write('f'.encode())
                print("forward")
                
            
  
    else:
        ser.write('s'.encode())
        print("stop")
        


    result = cv2.imshow('result', result)     
    
    key = cv2.waitKey(1)
    rawCapture.truncate(0)
    if key == 27:
        break

cv2.destroyAllWindows()
