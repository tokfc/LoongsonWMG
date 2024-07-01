from ctypes import *

LCD = CDLL('./TouchLCD.so')
LCD.LCDinit()
CurrentPage = 0 #记录，即可，不用重复画页
PrintTips="Now we're loading, plz wait ....."
LCD.LCDDispStringLine_EN(7,PrintTips.encode("utf-8"))

while(1):
    if CurrentPage != 1 :
        LCD.DrawPage1()
        CurrentPage=1
        
    waitkey= chr(LCD.GetPage1())
    if waitkey == 'P':
        
        continue
    ##############################存货程序############################################
    elif waitkey == 'a':
        userID = 0 
        while(1):
            if CurrentPage != 5 :
                LCD.DrawPage5()
                CurrentPage=5
                
            waitkey=chr(LCD.GetPage5())
            if waitkey == 'P':
                continue
            elif waitkey == 'S':
                break
            elif waitkey == 'Y':
                if userID>=3 or userID <=0:
                    print("Error:不支持这样的ID，系统不存在，返回主菜单%d"%userID)
                    PrintTips="              Unvaild ID"+str(userID)
                    LCD.LCDDispStringLine_EN(3,PrintTips.encode("utf-8"))
                    userID =0
                    continue
            else:
                userID=userID*10+int(waitkey)
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
                while(1):
                    if CurrentPage != 4 :
                        LCD.DrawPage4()
                        CurrentPage=4
                    #每帧检测，有人脸的时候中间触发detectState为True
                    waitkey=chr(LCD.GetPage4())
                    if waitkey =='P' :
                        continue
                    elif waitkey =='N' and detectedState==False:
                        break
                    elif waitkey =='Y' and detectedState==True:
                        #开门
                        break
                    
                        
                        
                        
            #####################################QR码手机APP###########################################
            elif waitkey == 'b':
                print("Error:你等等，我还在开发")
                continue
                
    #################################查柜程序#################################################
    elif waitkey == 'c':
        while(1):
            if CurrentPage != 3 :
                LCD.DrawPage3()
                CurrentPage=3
                
            waitkey=chr(LCD.GetPage3())
            if waitkey =='P' :
                continue
            elif waitkey =='S':
                break
    #######################################结束程序########################################
    elif waitkey == 'S':
        print("\033[0;35;47m*****************程序终止，欢迎您再次使用***************\033[0m")
        break

#################################程序结束的地方#############################################################
LCD.LCDRelease()
