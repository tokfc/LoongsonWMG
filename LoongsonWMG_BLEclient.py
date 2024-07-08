import time
import cv2
import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

import array

from gi.repository import GObject
import sys,select

from random import randint
from ctypes import *
import threading

import dbus
import dbus.exceptions
import dbus.mainloop.glib
import dbus.service


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


Writestatusback=0
#当然要根据柜子数量
StatusValue     =   [0x00 for i in range(NumOfLocker)]
temperture      =   ["" for i in range(NumOfLocker)]
humidity        =   ["" for i in range(NumOfLocker)]

#导入训练结果
recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.read('face_trainer.yml')#读取前文训练的结果,只对应faceID
'''
#亦可由读文件录入
NumOfFaces = 3
facelabels    = ["unkowned", "diaomao", "Jackychan"]#人物名
userPhoneNUMs = ["0","7337","2048"] #对应手机尾号
'''
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
###################################蓝牙服务端QRcode#############################
mainloop = None
BLUEZ_SERVICE_NAME = 'org.bluez'
GATT_MANAGER_IFACE = 'org.bluez.GattManager1'
DBUS_OM_IFACE =      'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE =    'org.freedesktop.DBus.Properties'

GATT_SERVICE_IFACE = 'org.bluez.GattService1'
GATT_CHRC_IFACE =    'org.bluez.GattCharacteristic1'
GATT_DESC_IFACE =    'org.bluez.GattDescriptor1'

LE_ADVERTISEMENT_IFACE = 'org.bluez.LEAdvertisement1'
LE_ADVERTISING_MANAGER_IFACE = 'org.bluez.LEAdvertisingManager1'

#广播服务对象
class Advertisement(dbus.service.Object):
    PATH_BASE = '/org/bluez/LoongsonWMG/advertisement'

    def __init__(self, bus, index, advertising_type):
        self.path = self.PATH_BASE + str(index)
        self.bus = bus
        self.ad_type = advertising_type
        self.service_uuids = None
        self.manufacturer_data = None
        self.solicit_uuids = None
        self.service_data = None
        self.local_name = None
        self.include_tx_power = False
        self.data = None
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        properties = dict()
        properties['Type'] = self.ad_type
        if self.service_uuids is not None:
            properties['ServiceUUIDs'] = dbus.Array(self.service_uuids,
                                                    signature='s')
        if self.solicit_uuids is not None:
            properties['SolicitUUIDs'] = dbus.Array(self.solicit_uuids,
                                                    signature='s')
        if self.manufacturer_data is not None:
            properties['ManufacturerData'] = dbus.Dictionary(
                self.manufacturer_data, signature='qv')
        if self.service_data is not None:
            properties['ServiceData'] = dbus.Dictionary(self.service_data,
                                                        signature='sv')
        if self.local_name is not None:
            properties['LocalName'] = dbus.String(self.local_name)
        if self.include_tx_power:
            properties['Includes'] = dbus.Array(["tx-power"], signature='s')

        if self.data is not None:
            properties['Data'] = dbus.Dictionary(self.data, signature='yv')
        return {LE_ADVERTISEMENT_IFACE: properties}

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_service_uuid(self, uuid):
        if not self.service_uuids:
            self.service_uuids = []
        self.service_uuids.append(uuid)

    def add_solicit_uuid(self, uuid):
        if not self.solicit_uuids:
            self.solicit_uuids = []
        self.solicit_uuids.append(uuid)

    def add_manufacturer_data(self, manuf_code, data):
        if not self.manufacturer_data:
            self.manufacturer_data = dbus.Dictionary({}, signature='qv')
        self.manufacturer_data[manuf_code] = dbus.Array(data, signature='y')

    def add_service_data(self, uuid, data):
        if not self.service_data:
            self.service_data = dbus.Dictionary({}, signature='sv')
        self.service_data[uuid] = dbus.Array(data, signature='y')

    def add_local_name(self, name):
        if not self.local_name:
            self.local_name = ""
        self.local_name = dbus.String(name)

    def add_data(self, ad_type, data):
        if not self.data:
            self.data = dbus.Dictionary({}, signature='yv')
        self.data[ad_type] = dbus.Array(data, signature='y')

    @dbus.service.method(DBUS_PROP_IFACE,in_signature='s',out_signature='a{sv}')
    def GetAll(self, interface):
        print('GetAll')
        if interface != LE_ADVERTISEMENT_IFACE:
            raise InvalidArgsException()
        print('returning props')
        return self.get_properties()[LE_ADVERTISEMENT_IFACE]

    @dbus.service.method(LE_ADVERTISEMENT_IFACE,
                         in_signature='',
                         out_signature='')
    def Release(self):
        print('%s: Released!' % self.path)


class TestAdvertisement(Advertisement):

    def __init__(self, bus, index):
        Advertisement.__init__(self, bus, index, 'peripheral')
        self.add_local_name('Loongson嵌入式')
        self.include_tx_power = True
        self.add_service_uuid('00001234-0000-0001-0000-000000000000')

class Application(dbus.service.Object):
    """
    org.bluez.GattApplication1 interface implementation
    """
    def __init__(self, bus):
        self.path = '/'
        self.services = []
        dbus.service.Object.__init__(self, bus, self.path)
        self.add_service(QRcodeService(bus, 0))
    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_service(self, service):
        self.services.append(service)

    @dbus.service.method(DBUS_OM_IFACE, out_signature='a{oa{sa{sv}}}')
    def GetManagedObjects(self):
        response = {}
        print('GetManagedObjects')

        for service in self.services:
            response[service.get_path()] = service.get_properties()
            chrcs = service.get_characteristics()
            for chrc in chrcs:
                response[chrc.get_path()] = chrc.get_properties()
                descs = chrc.get_descriptors()
                for desc in descs:
                    response[desc.get_path()] = desc.get_properties()

        return response
        
class Service(dbus.service.Object):
    """
    org.bluez.GattService1 interface implementation
    """
    PATH_BASE = '/org/bluez/LoongsonWMG/service'

    def __init__(self, bus, index, uuid, primary):
        self.path = self.PATH_BASE + str(index)
        self.bus = bus
        self.uuid = uuid
        self.primary = primary
        self.characteristics = []
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
            GATT_SERVICE_IFACE: {'UUID': self.uuid,'Primary': self.primary,'Characteristics': dbus.Array(self.get_characteristic_paths(),signature='o')}
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_characteristic(self, characteristic):
        self.characteristics.append(characteristic)

    def get_characteristic_paths(self):
        result = []
        for chrc in self.characteristics:
            result.append(chrc.get_path())
        return result

    def get_characteristics(self):
        return self.characteristics

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_SERVICE_IFACE:
            raise InvalidArgsException()

        return self.get_properties()[GATT_SERVICE_IFACE]


class Characteristic(dbus.service.Object):
    """
    org.bluez.GattCharacteristic1 interface implementation
    """
    def __init__(self, bus, index, uuid, flags, service):
        self.path = service.path + '/char' + str(index)
        self.bus = bus
        self.uuid = uuid
        self.service = service
        self.flags = flags
        self.descriptors = []
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
                GATT_CHRC_IFACE: {'Service': self.service.get_path(),'UUID': self.uuid,'Flags': self.flags,'Descriptors': dbus.Array(self.get_descriptor_paths(),signature='o')}
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_descriptor(self, descriptor):
        self.descriptors.append(descriptor)

    def get_descriptor_paths(self):
        result = []
        for desc in self.descriptors:
            result.append(desc.get_path())
        return result

    def get_descriptors(self):
        return self.descriptors

    @dbus.service.method(DBUS_PROP_IFACE,in_signature='s',out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_CHRC_IFACE:
            raise InvalidArgsException()

        return self.get_properties()[GATT_CHRC_IFACE]

    @dbus.service.method(GATT_CHRC_IFACE,in_signature='a{sv}',out_signature='ay')
    def ReadValue(self, options):
        print('Default ReadValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE, in_signature='aya{sv}')
    def WriteValue(self, value, options):
        print('Default WriteValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE)
    def StartNotify(self):
        print('Default StartNotify called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE)
    def StopNotify(self):
        print('Default StopNotify called, returning error')
        raise NotSupportedException()

    @dbus.service.signal(DBUS_PROP_IFACE,signature='sa{sv}as')
    def PropertiesChanged(self, interface, changed, invalidated):
        pass

    
class QRcodeService(Service):
    
    #Characteristic存放已确认用户的手机尾号
    def __init__(self, bus, index):
        Service.__init__(self, bus, index, '00001234-0000-0001-0000-000000000000', True)
        self.add_characteristic(QRcodeCharacteristic(bus, 0, self))
        

class QRcodeCharacteristic(Characteristic):
    
    def __init__(self, bus, index, service):
        Characteristic.__init__(self, bus, index,'00001234-0000-0001-0000-000000000001',['read', 'write', 'writable-auxiliaries'],service)
        self.value = ['']
        self.written= False
    
    def ReadValue(self, options):
        #print('TestCharacteristic Read: ' + repr(self.value))
        return self.value

    def WriteValue(self, value, options):
        #print('TestCharacteristic Write: ' + repr(value))
        self.value = value
        self.written= True

def register_ad_cb():
    print('广播服务注册成功！')


def register_ad_error_cb(error):
    print('广播服务注册失败了: ' + str(error))
    mainloop.quit()


def register_app_cb():
    print('GATT服务注册成功！')


def register_app_error_cb(error):
    print('GATT服务注册失败了: ' + str(error))
    mainloop.quit()

def BLESstart():
    print('蓝牙广播与GATT服务已就绪!')
    mainloop.run()

'''
def find_adapter(bus):
    remote_om = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, '/'),
                               DBUS_OM_IFACE)
    objects = remote_om.GetManagedObjects()

    for o, props in objects.items():
        if GATT_MANAGER_IFACE in props.keys() and LE_ADVERTISING_MANAGER_IFACE in props.keys():
            return o

    return None
'''
###################################蓝牙服务端QRcode#############################
###################################蓝牙客户端读写#############################################
async def RefreshLocker(WhichDevice,th_status):

    
        
    #基于设备名查找设备
    print("开始查找设备...", BLEServerName[WhichDevice])
    device = await BleakScanner.find_device_by_name(BLEServerName[WhichDevice])
    if device is None:
        print("找不到设备 ", BLEServerName[WhichDevice])
    else:    
        print("正在连接设备...", BLEServerName[WhichDevice])
        #连接至指定设备并做如下
        client = BleakClient(device)
        await client.connect()
        if client.is_connected : 
            print("已连接至: %s"%BLEServerName[WhichDevice])
            print("更新value")
            StatusValue[WhichDevice] = list((await client.read_gatt_char(BLECharacteristic1UUID)))[0]
            if th_status :
                print("更新温度")
                temperture[WhichDevice] = (await client.read_gatt_char(BLECharacteristic2UUID)).decode('utf-8')
                print("更新湿度")
                humidity[WhichDevice]   = (await client.read_gatt_char(BLECharacteristic3UUID)).decode('utf-8')
            await client.disconnect()
            
        else:
            print("连接失败: %s"%BLEServerName[WhichDevice])
                    
        
async def WriteLocker(WhichDevice,new1Value):
    #基于设备名查找设备
    
    print("开始查找设备...", BLEServerName[WhichDevice])
    device = await BleakScanner.find_device_by_name(BLEServerName[WhichDevice])
    if device is None:
        print("找不到设备 ", BLEServerName[WhichDevice])
        Writestatusback= -1
    
    else:
        print("正在连接设备...", BLEServerName[WhichDevice])
        #连接至指定设备并做如下，跳出这段代码时断开连接
        client = BleakClient(device)
        await client.connect()
        if client.is_connected : 
            print("已连接至: %s"%BLEServerName[WhichDevice])
            Writestatusback=1
            await client.write_gatt_char(BLECharacteristic1UUID, bytearray([new1Value, 0x00]))
            
            await client.disconnect()
            
        else:
            print("连接失败: %s"%BLEServerName[WhichDevice])
            Writestatusback=-2



async def RefreshALLData():
    '''
    tasks=[RefreshLocker(DeviceNumber,True) for DeviceNumber in range(NumOfLocker)]
    await asyncio.gather(*tasks, return_exceptions=True)
    print("全部数值已更新完毕")
    '''
    for DeviceNumber in range(NumOfLocker):
        await RefreshLocker(DeviceNumber,True)
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
            label_text = facelabels[label[0]-1]
            print("哦，你是",label_text)
            return label_text
        else:
            print("谁啊？不熟")
            return "unkowned"
    else:
        return "no face"
#############################人脸识别相关##########################################################
##############################系统调用部分######################################################
def delay(time):
    #约0.000885/1000
    i=time*1000
    while(i):
        i=i-1



def main():
    global mainloop
    LCD = CDLL('./TouchLCD.so')
    LCD.LCDinit()
    CurrentPage = 0 #记录，即可，不用重复画页
    
    LoadingTips="Now we're loading, plz wait ....."
    LCD.LCDDispStringLine_EN(7,LoadingTips.encode("utf-8"))
    try:
        asyncio.run(RefreshALLData())
    except:
        print("不行啦")
    
    #Dbus事件列入mainloop事件序列中循环运行
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    #启动dbus
    bus = dbus.SystemBus()
    #找到蓝牙适配器与GATT广播服务
    adapter = '/org/bluez/hci1'
    #先做广播的准备
    #将dbus的蓝牙adpater在Linux上的服务（一种对象）存为adapter_props
    adapter_props = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter),DBUS_PROP_IFACE)
    #蓝牙设备？启动！(修改其中的参数)
    adapter_props.Set("org.bluez.Adapter1", "Powered", dbus.Boolean(1))
    #然后将dbus的蓝牙adpater的LE广播管理服务（一种对象）存为ad_manager
    ad_manager = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter),LE_ADVERTISING_MANAGER_IFACE)
    
    test_advertisement = TestAdvertisement(bus, 0)
    #然后做GATT的准备
    #启用service_manager位GATT服务管理接口
    service_manager = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter),GATT_MANAGER_IFACE)
    app = Application(bus)
    print('正在注册 GATT 应用...')
    #注册GATT应用
    service_manager.RegisterApplication(app.get_path(), {},reply_handler=register_app_cb,error_handler=register_app_error_cb)
    
    #注册广播管理服务的dbus接口，该接口将循环交换信息
    print('正在注册 广播服务...')    
    ad_manager.RegisterAdvertisement(test_advertisement.get_path(), {},reply_handler=register_ad_cb,error_handler=register_ad_error_cb)
    mainloop = GObject.MainLoop()
    app.services[0].characteristics[0].WriteValue(dbus.Array([dbus.Byte(0x30)], signature=dbus.Signature('y')),True)

    threading.Thread(target=BLESstart).start() #由另外一个线程运行
    #############################程序开始的地方#########################################
    while(1):
        if CurrentPage != 1 :
            LoadingTips="Now we're loading, plz wait ....."
            LCD.LCDDispStringLine_EN(7,LoadingTips.encode("utf-8"))
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
                            userID = whatisyournum_Phone(userPhoneNUM)+1#返回的是用于数组检索的ID，要+1
                            
                            try:
                                Writestatusback=0
                                asyncio.run(WriteLocker(WhichLocker,(userID<<4|2)&0xFF))
                            except:
                                Writestatusback=-3
                            if (Writestatusback<0):
                                print('柜单元失联，已返回'+str(Writestatusback))
                                break
                            print('发送了存货开柜指令，等待 %d 号柜开柜响应...'%WhichLocker)
                            while(StatusValue[WhichLocker]&0x08==0x00):
                                try:
                                    delay(500)
                                    asyncio.run(RefreshLocker(WhichLocker,False))
                                except:
                                    continue
                            PrintTips="       Door is opened! Welcome User:"+userPhoneNUM
                            LCD.LCDDispStringLine_EN(3,PrintTips.encode("utf-8"))
                            asyncio.run(WriteLocker(WhichLocker,(userID<<4|0)&0xFF))
                            StatusValue[WhichLocker]=(userID<<4|0)&0xFF
                            break
                            
                        delay(4000)
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
                            DetectedPerson=whatisyournum(WhoseFace)+1#返回的是用于数组检索的ID，要+1
                            print("你的手机尾号是： "+userPhoneNUMs[DetectedPerson-1]+" 请前往开启的柜门")
                            for WhichLocker in range(NumOfLocker+1):
                                if WhichLocker == NumOfLocker :
                                    print('压根就没你的货')
                                    PrintTips="      There is no your take out! User"+str(DetectedPerson)
                                    LCD.LCDDispStringLine_EN(7,PrintTips.encode("utf-8"))
                                    break
                                elif StatusValue[WhichLocker] & 0xF0 !=DetectedPerson<<4:
                                    continue
                                try:
                                    Writestatusback=0
                                    asyncio.run(WriteLocker(WhichLocker,(DetectedPerson<<4|1)&0xFF))
                                except:
                                    Writestatusback=-3
                                if(Writestatusback<0):
                                    print("柜单元失联，返回上一级"+str(Writestatusback))
                                    break
                                print('发送了取货开柜指令，等待 %d 号柜开柜响应...'%(WhichLocker+1))
                                while(StatusValue[WhichLocker]&0x04==0x00):
                                    try:
                                        delay(500)
                                        asyncio.run(RefreshLocker(WhichLocker,False))
                                    except:
                                        continue
                                PrintTips="       Door is opened! Welcome User"+str(DetectedPerson)
                                LCD.LCDDispStringLine_EN(3,PrintTips.encode("utf-8"))
                                asyncio.run(WriteLocker(WhichLocker,0x00))
                                StatusValue[WhichLocker]=0x00
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
                    userPhoneNUM ='0'
                    userID=0 
                    while(1):
                        if CurrentPage != 6 :
                            LCD.DrawPage6()
                            CurrentPage=6
                            

                        #机器识别阶段
                        
                        ValueBack=app.services[0].characteristics[0].ReadValue(True)
                        '''
                        userPhoneNUM=''
                        for v in ValueBack:
                            if str(v)==0x00 :
                                break
                            userPhoneNUM=userPhoneNUM+''.join(str(v))
                        '''
                        userPhoneNUM=''.join([str(v) for v in ValueBack])
                        userPhoneNUM=userPhoneNUM.split('\0')[0]
                        #userPhoneNUM =userPhoneNUM.split()
                        if userPhoneNUM in userPhoneNUMs:
                            userID=whatisyournum_Phone(userPhoneNUM)+1#返回的是用于数组检索的ID，要+1
                            print("你的手机尾号是： "+userPhoneNUM)
                            for WhichLocker in range(NumOfLocker+1):
                                if WhichLocker == NumOfLocker :
                                    print('压根就没你的货')
                                    PrintTips="      There is no your take out! User"+userPhoneNUM
                                    LCD.LCDDispStringLine_EN(7,PrintTips.encode("utf-8"))
                                    break
                                elif StatusValue[WhichLocker] & 0xF0 !=userID<<4:
                                    continue
                                try:
                                    Writestatusback=0
                                    asyncio.run(WriteLocker(WhichLocker,(userID<<4|1)&0xFF))
                                except:
                                    Writestatusback=-3
                                if(Writestatusback<0):
                                    print("柜单元失联，已返回"+str(Writestatusback))
                                    break
                                print('发送了取货开柜指令，等待 %d 号柜开柜响应...'%(WhichLocker+1))
                                while(StatusValue[WhichLocker]&0x04==0x00):
                                    try:
                                        asyncio.run(RefreshLocker(WhichLocker,False))
                                        delay(1000)
                                    except:
                                        continue
                                print("欢迎使用外卖柜，尾号："+userPhoneNUM)
                                PrintTips="       Door is opened! Welcome User"+userPhoneNUM
                                LCD.LCDDispStringLine_EN(7,PrintTips.encode("utf-8"))
                                asyncio.run(WriteLocker(WhichLocker,0x00))
                                StatusValue[WhichLocker]=0x00
                                break
                            
                            app.services[0].characteristics[0].WriteValue(dbus.Array([dbus.Byte(0x30)], signature=dbus.Signature('y')),True)
                            userPhoneNUM='0'
                            delay(4000)
                            LCD.DrawPage6()
                        elif userPhoneNUM != '0':
                            PrintTips='      Never A guy with'+userPhoneNUM
                            LCD.LCDDispStringLine_EN(7,PrintTips.encode("utf-8"))
                            print("不在系统里面啊你"+userPhoneNUM)
                            app.services[0].characteristics[0].WriteValue(dbus.Array([dbus.Byte(0x30)], signature=dbus.Signature('y')),True)
                            userPhoneNUM='0'
                            delay(4000)
                            LCD.DrawPage6()
                                    
                        
                        waitkey=chr(LCD.GetPage6())
                        if waitkey =='P' :
                            continue
                        elif waitkey =='S' :
                            
                            break
        #################################查柜程序#################################################
        elif waitkey == 'c':
            while(1):
                if CurrentPage != 3 :
                    LCD.DrawPage3()
                    CurrentPage=3
                    try:
                        asyncio.run(RefreshALLData())
                    except:
                        print("不行啦")
    
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
    mainloop.quit()
    ad_manager.UnregisterAdvertisement(test_advertisement)
    print('注销广播服务')
    dbus.service.Object.remove_from_connection(test_advertisement)
    service_manager.UnregisterApplication(app)
    print('注销GATT服务')
    dbus.service.Object.remove_from_connection(app)
    

if __name__ == '__main__':
    main()

