from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
from scipy.spatial import distance
import serial
import time
import sys
import socket

 
import numpy as np

#Activate environment python
# source OpenCV-"$cvVersion"-py3/bin/activate


#Connecting with the bluetooth of rasberry pi and Arduino
port = serial.Serial("/dev/rfcomm1", baudrate=9600)
#
# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
# Set the resolution with 640,480
camera.resolution = (640, 480)
#framerate settting
camera.framerate = 16

#Capturing image using PI library
rawCapture = PiRGBArray(camera, size=(640, 480))
 
# allow the camera to warmup
time.sleep(0.1)
#

# Two arrays to draw region of itnerest on image
#Source = [(180,350) , (550,360),(80,470),(630,470)]
Source = [(100,350) , (550,350),(20,470),(630,470)]
#
Destination = [(200,0) , (440,0),(200,480),(440,480)]
#Source = [(70,500) , (500,500),(0,640),(640,480)]
# 
# while(True):
#     forward = 'J'
#     port.write(forward.encode())
#     time.sleep(2)
#forward = 'I'
#port.write(forward.encode())
#time.sleep(1)
# capture frames from the camera
TCP_IP = '192.168.43.101'
TCP_PORT = 1234
BUFFER_SIZE = 2000  # Normally 1024, but we want fast response
# 

print ("Waiting from master class to give go-ahead")
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()
print ('Connection address:', addr)
data = conn.recv(BUFFER_SIZE)
print ("received data:", data)
conn.send(data)  # echo
conn.close()

forward = 'S'
Last_Action_performed = 'S'
time.sleep(2)
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    # grab the raw NumPy array representing the image, then initialize the timestamp
    # and occupied/unoccupied text
    start_time = time.time()
   
#   Store frame
    image = frame.array
 
#   cfop images to detect signal
    Image_For_Signals = image[380:450,0:640]
   
#    Source = [(100,350) , (550,350),(20,470),(630,470)]
   
#    image = image[70:550, 20:470]
 
    ## convert to hsv
 #  converting image to HSV for signal detection
    hsv_frame = cv2.cvtColor(Image_For_Signals, cv2.COLOR_BGR2HSV)

    #intensity for green
    low_green = np.array([36, 25, 25])
    high_green = np.array([60, 255, 255])
#    captuing green image from cropped image
    green_mask = cv2.inRange(hsv_frame, low_green, high_green)

    green = cv2.bitwise_or(Image_For_Signals, Image_For_Signals, mask=green_mask)
    croped_green = cv2.bitwise_and(Image_For_Signals, Image_For_Signals, mask=green_mask)

   
    ## mask of green (36,25,25) ~ (86, 255,255)
    # mask = cv2.inRange(hsv, (36, 25, 25), (86, 255,255))
#    mask = cv2.inRange(hsv, (36, 25, 25), (60, 255,255))
#
#    ## slice the green
#    imask = mask>0
#    green = np.zeros_like(image, np.uint8)
#    green[imask] = image[imask]
##    
   
 
   
#    same methord for red image
    img_hsv = cv2.cvtColor(Image_For_Signals, cv2.COLOR_BGR2HSV)

    ## Gen lower mask (0-5) and upper mask (175-180) of RED
    mask1 = cv2.inRange(img_hsv, (0,50,20), (5,255,255))
    mask2 = cv2.inRange(img_hsv, (175,50,20), (180,255,255))

    ## Merge the mask and crop the red regions
    mask = cv2.bitwise_or(mask1, mask2 )

#    calculate the number of red pixels
    n_white_pix = np.sum(mask == 255)
   
    if (n_white_pix>100):
        stop='S'
        port.write(stop.encode())
#    else:
#        forward = 'K'
#        port.write(forward.encode())
#        print('Number of white pixels:', n_white_pix)
   
   
   
#    calculate the region of interest and draw lines
   
    cv2.line(image,(Source[0]),(Source[1]),(0,0,255),2)

    cv2.line(image,(Source[1]),(Source[3]),(0,0,255),2)


    cv2.line(image,(Source[3]),(Source[2]),(0,0,255),2)


    cv2.line(image,(Source[2]),(Source[0]),(0,0,255),2)
   

#   #Calculating perspective wrap
    PrespectiveWrapSource = np.array(Source,np.float32)
    PrespectiveWrapDestination = np.array(Destination,np.float32)
    M = cv2.getPerspectiveTransform(PrespectiveWrapSource,PrespectiveWrapDestination  )
    warped = cv2.warpPerspective(image, M, (640, 480))
   
#    print("Size and width of wraped image")
    width1, height1 = warped.shape[:2]
#    
#    print(width1)
#    print(height1)
   

#    
#    convert prespective image to gray scale
    gray_image = cv2.cvtColor(warped, cv2.COLOR_BGR2GRAY)
#    threshold image
# Note::
# tweak these parameters when presenting
#for indoor there will around 120 - 150
#for outdoor they will be around 220 - 240
    retval, threshold = cv2.threshold(gray_image, 120, 150, cv2.THRESH_BINARY)
#    blur = cv2.GaussianBlur(gray_image,(5,5),0)
#    retval,threshold = cv2.threshold(gray_image,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
   
#    Apply canny edge detector
    edges = cv2.Canny(threshold,100,500,False)
   
   
#    merge Threshold image and edge detector imagte
    MergedImage = cv2.add(threshold, edges)
           
#            Draw line between two lines
    cv2.line(MergedImage,(640,60),(640,200),(255,255,255),2)
    height, width = MergedImage.shape[:2]
#    
#    print(width)
#    print(height)

#   Checking right and left image
    Left_Line_Coordinates_row = 0
    Left_Line_Coordinates_col = 0
    Right_Line_Coordinates_row = 0
    Right_Line_Coordinates_col = 0
   
    Point_of_intersection = 470
    Left_Line_Found_For_Lost_One = False
    Right_Line_Found_For_Lost_One = False
    for i in range(int(width/2)):
        pixel= MergedImage[Point_of_intersection, 0+i]
#        print(" Left Line Coordinates are ")
        if(pixel==255):
            Left_Line_Found_For_Lost_One=True
            Coordinates=(Point_of_intersection,i)
            Left_Line_Coordinates_row = Point_of_intersection
            Left_Line_Coordinates_col = i
            print(" Left Line Coordinates are")
            print(Coordinates)
            print (pixel)
            break
           
#            Calculating right line in image
    LoopVar = width-1
    while(True):
        if (LoopVar == int(width/2) ):
            break
        pixel= MergedImage[Point_of_intersection, LoopVar]
        if(pixel==255):
            Right_Line_Found_For_Lost_One = True
            Coordinates=(Point_of_intersection,LoopVar)
            Right_Line_Coordinates_row=Point_of_intersection
            Right_Line_Coordinates_col=LoopVar
            print("Right Line Coordinates are")
            print(Coordinates)
            break
        LoopVar = LoopVar - 1

#    Draw center line between left line and right line
    cv2.line(warped,(Left_Line_Coordinates_col,Point_of_intersection),(Right_Line_Coordinates_col,Point_of_intersection),(0,255,0),2)

    print("Left Line Found _For_Lost_One")
    print(Left_Line_Found_For_Lost_One)
    print("RIght_Line_Found _For_Lost_One")
    print(Right_Line_Found_For_Lost_One)
#    if both lines are not found than apply perations
   
#    if (Left_Line_Found_For_Lost_One == False and Right_Line_Found_For_Lost_One == False):    
#        font = cv2.FONT_HERSHEY_SIMPLEX
#        forward =  'S'
#        port.write(forward.encode())
#        time.sleep(0.5)
#        forward =  'B'
#        port.write(forward.encode())
#        time.sleep(0.1)
#        cv2.putText(image,"Lost" ,(150,250), font, 2,(0,0,255),2,cv2.LINE_AA)  
##      check for past history
#        if (Last_Action_performed == 'Y' or Last_Action_performed=='I'):    
#            forward =  'J'
#            port.write(forward.encode())
#            time.sleep(1)
#        if (Last_Action_performed == 'J' or Last_Action_performed=='G'):    
#            forward =  'Y'
#            port.write(forward.encode())
#            time.sleep(1)
#            
#        if (Last_Action_performed == 'G' or Last_Action_performed=='J'):    
#            forward =  'B'
#            port.write(forward.encode())
#            time.sleep(1)
#        if (Last_Action_performed == 'F'):
#            forward = 'B'
#            port.write(forward.encode())
#            time.sleep(1)
#

#if only left line not found than apply right operation
    if (Left_Line_Found_For_Lost_One == False):
        forward = 'J'
        port.write(forward.encode())
       
#        if only right line not found than apply left operation
    if (Right_Line_Found_For_Lost_One == False):
        forward = 'Y'
        port.write(forward.encode())
#calculating left line and right line for offset

    Left_Line_Coordinates_row = 0
    Left_Line_Coordinates_col = 0
    Right_Line_Coordinates_row = 0
    Right_Line_Coordinates_col = 0
   
    Point_of_intersection = 370
    Left_Line_Found = False
    Right_Line_Found = False
    for i in range(int(width/2)):
        pixel= MergedImage[Point_of_intersection, 0+i]
#        print(" Left Line Coordinates are ")
        if(pixel==255):
            Left_Line_Found=True
            Coordinates=(Point_of_intersection,i)
            Left_Line_Coordinates_row = Point_of_intersection
            Left_Line_Coordinates_col = i
            print(" Left Line Coordinates are")
            print(Coordinates)
            print (pixel)
            break
           
    LoopVar = width-1
    while(True):
        if (LoopVar == int(width/2) ):
            break
        pixel= MergedImage[Point_of_intersection, LoopVar]
        if(pixel==255):
            Right_Line_Found = True
            Coordinates=(Point_of_intersection,LoopVar)
            Right_Line_Coordinates_row=Point_of_intersection
            Right_Line_Coordinates_col=LoopVar
            print("Right Line Coordinates are")
            print(Coordinates)
            break
        LoopVar = LoopVar - 1
       
#   Drawing line
    cv2.line(warped,(Left_Line_Coordinates_col,Point_of_intersection),(Right_Line_Coordinates_col,Point_of_intersection),(0,0,255),2)
#

    print("Left Line Found")
    print(Left_Line_Found)
    print("RIght_Line_Found")
    print(Right_Line_Found)
#        

#    Left_Lane_Coordinates = (    Left_Line_Coordinates_row , Left_Line_Coordinates_col)
#    Right_Lane_Coordinates = (    Right_Line_Coordinates_row , Right_Line_Coordinates_col)
#    
#    dst = int (distance.euclidean(Left_Lane_Coordinates, Right_Lane_Coordinates))
#    print("Distance is")
   
#    calculating mid point between two lines
    Mid_Point = int(( Right_Line_Coordinates_col - Left_Line_Coordinates_col ) /2 )
    print("Distance is")
#    setting midpoint according to frame size
    Mid_Point = Mid_Point + Left_Line_Coordinates_col
   
    print(Mid_Point)
    cv2.line(MergedImage,(Mid_Point,60),(Mid_Point ,640),(255,255,255),2)
##
#    drawing line for frame center
    FrameCenter = 324
    cv2.line(MergedImage,(FrameCenter,60),(FrameCenter ,640),(255,255,255),2)
##
#   calculating offset
    offset = Right_Line_Coordinates_col - Mid_Point
    offset_from_left = Mid_Point - Left_Line_Coordinates_col
    print("Offset",offset)
    print("Offeset_From_Left_Line",offset_from_left)
#   Calculating offset between framecenter and mid_point
    Result = FrameCenter - Mid_Point
    print("Result Value")
    print(Result)
   
#    if (n_white_pix<1 and Left_Line_Found == True and Right_Line_Found== True):
    if (n_white_pix<1 and Right_Line_Found_For_Lost_One== True and Left_Line_Found_For_Lost_One == True):
#   Setting range for moving straight
#        time.sleep(1)
        if(Result >= -10 and Result <= 10):
             #move forward
            forward = 'F'
            port.write(forward.encode())
#   Setting offset to move slight left
        elif(Result > 10 and Result <= 20):
             #move forward
            forward = 'H'
            port.write(forward.encode())
#            for quite sharp turn
        elif(Result > 20 and Result <= 40):
             #move forward
            forward = 'G'
            port.write(forward.encode())
 
        elif(Result > 40 ):
             #move forward
            forward = 'J'
            port.write(forward.encode())
           
        elif(Result <= -10  and Result >= -20):
             #move forward
            forward = 'I'
            port.write(forward.encode())
           
        elif(Result <= -20  and Result >= -40):
             #move forward
            forward = 'I'
            port.write(forward.encode())
           
        elif(Result <-40 ):
             #move forward
            forward = 'Y'
            port.write(forward.encode())
#    Display data on screen
   
    font = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(image,"Result" + str(Result),(350,250), font, 2,(0,0,255),2,cv2.LINE_AA)
#  
    cv2.imshow("Green Image", green)
    key = cv2.waitKey(1) & 0xFF

   
    cv2.imshow("REDMASK", mask)
    key = cv2.waitKey(1) & 0xFF
   
    cv2.imshow("Croped_Image_For_Signals", Image_For_Signals)
    key = cv2.waitKey(1) & 0xFF

    Last_Action_performed = forward
 # show the frame
   
   
    cv2.imshow("Frame", image)
    key = cv2.waitKey(1) & 0xFF
##
#   # show the Prespective Image
    cv2.imshow("prespective", warped)
    key = cv2.waitKey(1) & 0xFF
##    
      # show the Gray Scale image
#    cv2.imshow("prespective Gray Image", gray_image)
#    key = cv2.waitKey(1) & 0xFF
##    
#         # show the Threshold Gray Scale image
    cv2.imshow("Threshold prespective Gray Image", threshold)
    key = cv2.waitKey(1) & 0xFF
   
        # show the Edges
#    cv2.imshow("Edges", edges)
#    key = cv2.waitKey(1) & 0xFF
####
#            # show the Edges
    cv2.imshow("Merged_Image", MergedImage)
    key = cv2.waitKey(1) & 0xFF
##    
#    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)
     
    End_Time = time.time() - start_time
   
    Frame_Rate = 1/End_Time
    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break