import time
import cv2
import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

import array
import sys,select

from random import randint
from ctypes import *

#设备的Characteristic UUID
#可由读文件录入
NumOfLocker     = 3
BLEServerName   = ["TakoutLocker1","TakoutLocker2","TakoutLocker3"]
# The service we wish to advertise.
BLEServerUUID="00001234-0000-0000-0000-000000000000"
# The characteristic of the remote service we are interested in.
BLECharacteristic1UUID="00001234-0000-0000-0000-000000000001"
BLECharacteristic2UUID="00001234-0000-0000-0000-000000000002"
BLECharacteristic3UUID="00001234-0000-0000-0000-000000000003"


#当然要根据柜子数量
StatusValue     =   [0x00 for i in range(NumOfLocker)]
temperture      =   ["" for i in range(NumOfLocker)]
humidity        =   ["" for i in range(NumOfLocker)]

#导入训练结果
recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.read('face_trainer.yml')#读取前文训练的结果,只对应faceID

#亦可由读文件录入
#NumOfFaces = 3
#facelabels    = ["unkowned", "diaomao", "Jackychan"]#人物名
#userPhoneNUMs = ["0","7337","2048"] #对应手机尾号

# 打开文件
fo = open("Facelist.txt", "r")

wholeline = fo.readline()
NumOfFaces = int(wholeline)
facelabels =['' for i in range(NumOfFaces)]
userPhoneNUMs   =['' for i in range(NumOfFaces)]
print("numofLine:",NumOfFaces)
for i in range(NumOfFaces):
    wholeline=fo.readline()
    #wholeline.strip("\n")
    facelabels[i]=wholeline.split(".")[1]
    userPhoneNUMs[i]=wholeline.split(".")[2].split("\n")[0]
    print("我们有:"+facelabels[i]+" 他的手机尾号是："+userPhoneNUMs[i])

fo.close()

###################################蓝牙客户端读写#############################################
async def RefreshLocker(WhichDevice,th_status):
    #基于设备名查找设备
    print("开始查找设备...")
    device = await BleakScanner.find_device_by_name(BLEServerName[WhichDevice])
    if device is None:
        print("找不到设备 ", BLEServerName[WhichDevice])
        return
    
    print("正在连接设备...")
    #连接至指定设备并做如下，跳出这段代码时断开连接
    async with BleakClient(device) as client:
        print("已连接至: %s"%BLEServerName[WhichDevice])
        print("更新value")
        StatusValue[WhichDevice] = list((await client.read_gatt_char(BLECharacteristic1UUID)))[0]
        if th_status :
            print("更新温度")
            temperture[WhichDevice] = (await client.read_gatt_char(BLECharacteristic2UUID)).decode('utf-8')
            print("更新湿度")
            humidity[WhichDevice]   = (await client.read_gatt_char(BLECharacteristic3UUID)).decode('utf-8')
            
            
        
async def WriteLocker(WhichDevice,new1Value):
    #基于设备名查找设备
    print("开始查找设备...")
    device = await BleakScanner.find_device_by_name(BLEServerName[WhichDevice])
    if device is None:
        print("找不到设备 ", BLEServerName[WhichDevice])
        return
    
    print("正在连接设备...")
    #连接至指定设备并做如下，跳出这段代码时断开连接
    async with BleakClient(device) as client:
        print("已连接至: %s"%BLEServerName[WhichDevice])
        await client.write_gatt_char(BLECharacteristic1UUID, bytearray([new1Value, 0x00]))



async def RefreshALLData():

    tasks=[RefreshLocker(DeviceNumber,True) for DeviceNumber in range(NumOfLocker)]
    await asyncio.gather(*tasks, return_exceptions=True)
    print("全部数值已更新完毕")
#############################人脸识别相关##########################################################
def face_detect_demo(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    face_detector = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
    faces = face_detector.detectMultiScale(gray, 1.2, 6)
    # 如果未检测到面部，则返回原始图像
    if (len(faces) == 0):
        return None, None
    # 目前假设只有一张脸，xy为左上角坐标，wh为矩形的宽高
    (x, y, w, h) = faces[0]
    # 返回图像的脸部部分
    return gray[y:y + w, x:x + h], faces[0]


# 根据给定的人脸（x，y）坐标和宽度高度在图像上绘制矩形
def draw_rectangle(img, rect):
    (x, y, w, h) = rect#矩形框
    cv2.rectangle(img, (x, y), (x + w, y + h), (255, 255, 0), 2)

# 根据给定的人脸（x，y）坐标写出人名
def draw_text(img, text, x, y):
    cv2.putText(img, text, (x, y), cv2.FONT_HERSHEY_COMPLEX, 1, (128, 128, 0), 2)

def whatisyournum(name):
    for i in range(len(facelabels)):
        if facelabels[i]==name:
            return i
        else :
            continue

def whatisyournum_Phone(userPhoneNUM):
    for i in range(len(userPhoneNUMs)):
        if userPhoneNUMs[i]==userPhoneNUM:
            return i
        else :
            continue
    

# 此函数识别图像中的人物并在脸部周围绘制一个矩形及其人名
# 没脸就返回no face  不认识返回0 否则返回人名
def predict(image):
    # 检测人脸区域
    face, rect = face_detect_demo(image)#face_detect_demo前面的人脸检测函数
    # 预测人脸名字
    if face is not None:
        label = recognizer.predict(face)
        print(label)#label[0]为faceid，label[1]可信度数值越低，可信度越高（
        if label[1]<=80 or label[0]>=NumOfFaces:
            # 获取由人脸识别器返回的相应标签的人名
            label_text = facelabels[label[0]]
            print("哦，你是",label_text)
            return label_text
        else:
            print("谁啊？不熟")
            return "unkowned"
    else:
        return "no face"

##############################系统调用部分######################################################
def delay(time):
    #约0.000885/1000
    i=time*1000
    while(i):
        i=i-1



def main(timeout=60):

    LCD = CDLL('./TouchLCD.so')
    LCD.LCDinit()
    CurrentPage = 0 #记录，即可，不用重复画页
    
    LoadingTips="Now we're loading, plz wait ....."
    LCD.LCDDispStringLine_EN(7,LoadingTips.encode("utf-8"))

    

    
    #############################程序开始的地方#########################################
    while(1):
        if CurrentPage != 1 :
            LoadingTips="Now we're loading, plz wait ....."
            LCD.LCDDispStringLine_EN(7,LoadingTips.encode("utf-8"))
            asyncio.run(RefreshALLData())#每个程序改完
            LCD.DrawPage1()
            CurrentPage=1
        waitkey= chr(LCD.GetPage1())
        if waitkey == 'P':
            continue
        ##############################存货程序############################################
        elif waitkey == 'a':
            userPhoneNUM ='0'
            userID=0
            while(1):
                if CurrentPage != 5 :
                    LCD.DrawPage5()
                    CurrentPage=5
            
                waitkey=chr(LCD.GetPage5())
                if waitkey == 'P': #等待输入
                    continue
                elif waitkey == 'S':#返回
                    break
                elif waitkey == 'Y':#确认输入
                    userPhoneNUM=userPhoneNUM[1::1]
                    if   userPhoneNUM in userPhoneNUMs:
                        for WhichLocker in range(NumOfLocker+1):
                            if WhichLocker == NumOfLocker :
                                print('抱歉，没有空的柜子了')
                                PrintTips="          Sorry no Empty Case,"+userPhoneNUM
                                LCD.LCDDispStringLine_EN(3,PrintTips.encode("utf-8"))
                                break
                            elif StatusValue[WhichLocker] & 0xF0 !=0:
                                continue
                            userID = whatisyournum_Phone(userPhoneNUM)
                            asyncio.run(WriteLocker(WhichLocker,(userID<<4|2)&0xFF))
                            print('发送了存货开柜指令，等待 %d 号柜开柜响应...'%WhichLocker)
                            while(StatusValue[WhichLocker]&0x08==0x00):
                                asyncio.run(RefreshLocker(WhichLocker,False))
                                delay(1000)
                            PrintTips="       Door is opened! Welcome User:"+userPhoneNUM
                            LCD.LCDDispStringLine_EN(3,PrintTips.encode("utf-8"))
                            asyncio.run(WriteLocker(WhichLocker,(userID<<4|0)&0xFF))
                            break
                            
                        delay(8000)
                        break
                    else:
                        print('抱歉，你是系统里没记载的人')
                        PrintTips="              Unvaild ID"+userPhoneNUM
                        LCD.LCDDispStringLine_EN(3,PrintTips.encode("utf-8"))
                        userPhoneNUM ='0'
                        continue
                else:#输入全接受
                    userPhoneNUM=userPhoneNUM+waitkey
        #################################取货程序###################################################
        elif waitkey == 'b':
            while(1):
                if CurrentPage != 2 :
                    LCD.DrawPage2()
                    CurrentPage=2
                waitkey=chr(LCD.GetPage2())
                if waitkey =='P' :
                    continue
                elif waitkey =='S':
                    break
                #####################################人脸识别###########################################
                elif waitkey == 'a':
                    detectedState= False
                    CamaraA = cv2.VideoCapture(0)
                    CamaraA.set(3,320);
                    CamaraA.set(4,240);
                    while(1):
                        if CurrentPage != 4 :
                            LCD.DrawPage4()
                            CurrentPage=4
                            
                        #机器识别阶段
                        
                        
                        if detectedState==False:#测到人就停
                            ret, frame = CamaraA.read()#取帧
                            frameR = cv2.flip(frame, 1)#左右翻转
                            WhoseFace = predict(frameR)
                            if WhoseFace == "no face":
                                detectedState = False
                            elif WhoseFace == "unkowned" :
                                detectedState = False
                            else:
                                PrintTips="           Are You "+str(WhoseFace)+" ?(Y/N)"
                                LCD.LCDDispStringLine_EN(7,PrintTips.encode("utf-8"))
                                detectedState = True

                        #客户反应阶段
                        waitkey=chr(LCD.GetPage4())
                        if waitkey =='P' :
                            continue
                        elif waitkey =='N' and detectedState==False:
                            break
                        elif waitkey =='Y' and detectedState==True:
                            detectedState=False
                            DetectedPerson=whatisyournum(WhoseFace)
                            print("你的手机尾号是： "+userPhoneNUMs[DetectedPerson]+" 请前往开启的柜门")
                            for WhichLocker in range(NumOfLocker+1):
                                if WhichLocker == NumOfLocker :
                                    print('压根就没你的货')
                            s        PrintTips="      There is no your take out! User"+str(DetectedPerson)
                                    LCD.LCDDispStringLine_EN(7,PrintTips.encode("utf-8"))
                                    break
                                elif StatusValue[WhichLocker] & 0xF0 !=DetectedPerson<<4:
                                    continue
                            
                                asyncio.run(WriteLocker(WhichLocker,(DetectedPerson<<4|1)&0xFF))
                                print('发送了取货开柜指令，等待 %d 号柜开柜响应...'%WhichLocker)
                                while(StatusValue[WhichLocker]&0x04==0x00):
                                    asyncio.run(RefreshLocker(WhichLocker,False))
                                    delay(1000)
                                PrintTips="       Door is opened! Welcome User"+str(DetectedPerson)
                                LCD.LCDDispStringLine_EN(3,PrintTips.encode("utf-8"))
                                asyncio.run(WriteLocker(WhichLocker,(DetectedPerson<<4|0)&0xFF))
                                break
                            delay(8000)
                            break
                        elif waitkey =='N' and detectedState==True:
                            print("感觉你不熟悉，能不能对准摄像头？或者你是系统里没记载的人")
                            detectedState= False
                            LCD.DrawPage4()
                            continue
                    CamaraA.release()
                #####################################QR码手机APP###########################################
                elif waitkey == 'b':
                    userID = 0 
                    while(1):
                        if CurrentPage != 6 :
                            LCD.DrawPage6()
                            CurrentPage=6
            
                        
                        elif waitkey =='P' :
                            continue
                        elif waitkey =='S' :
                            break
        #################################查柜程序#################################################
        elif waitkey == 'c':
            while(1):
                if CurrentPage != 3 :
                    LCD.DrawPage3()
                    CurrentPage=3
                    for WhichLocker in range(NumOfLocker):
                        if WhichLocker==4:#因为一页也就够三个柜的位置
                            break
                        PrintTips="      Locker "+str(WhichLocker+1)+" :"
                        if(StatusValue[WhichLocker]& 0xF0 ==0):
                           
                            PrintTips=PrintTips+"EMPTY"
                        else:
                            PrintTips=PrintTips+"NOT EMPTY"
                        LCD.LCDDispStringLine_EN(WhichLocker*3+1,PrintTips.encode("utf-8"))
                        PrintTips='      '+temperture[WhichLocker]
                        LCD.LCDDispStringLine_EN(WhichLocker*3+2,PrintTips.encode("utf-8"))
                        PrintTips='      '+humidity[WhichLocker]
                        LCD.LCDDispStringLine_EN(WhichLocker*3+3,PrintTips.encode("utf-8"))


                
                
                waitkey=chr(LCD.GetPage3())
                if waitkey =='P' :
                    continue
                elif waitkey =='S':
                    break
            
            print('\033[0;35;47m')#字体变色
            print("********************************************************")
            for WhichLocker in range(NumOfLocker):
                print("*外卖柜单元"+str(WhichLocker+1)+"：                                         *")
                if(StatusValue[WhichLocker]& 0xF0 ==0):
                    print("*空柜                                                  *")
                else:
                    print("*非空柜,有货                                           *")
                print("*"+str(temperture[WhichLocker]))
                print("*"+str(humidity[WhichLocker]))
                print("*                                                      *")
            print("********************************************************\033[0m")
            
        #######################################结束程序########################################
        elif waitkey == 'S':
            print("\033[0;35;47m*****************程序终止，欢迎您再次使用***************\033[0m")
            break
    #################################程序结束的地方#############################################################
    LCD.LCDRelease()
    

if __name__ == '__main__':
    main()

