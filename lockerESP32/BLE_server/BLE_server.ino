/**
 * A BLE client example that is rich in capabilities.d:\Wen\ESP32\Exp\BLE_advertising_DOOR\BLE_advertising_DOOR\BLE_advertising_DOOR.ino
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
//#include "BLEScan.h"
#include <dhtESP32-rmt.h>

#define BLEServerName "TakoutLocker3"
// The remote service we wish to connect to.
#define BLEServerUUID "00001234-0000-0000-0000-000000000000"
// The characteristic of the remote service we are interested in.
#define BLECharacteristic1UUID "00001234-0000-0000-0000-000000000001"
#define BLECharacteristic2UUID "00001234-0000-0000-0000-000000000002"
#define BLECharacteristic3UUID "00001234-0000-0000-0000-000000000003"

// 1/20 秒，50Hz 的频率，20ms 的周期，这个变量用来存储时钟基准。
#define FREQ        50     
// 通道(高速通道（0 ~ 7）由 80MHz 时钟驱动，低速通道（8 ~ 15）由 1MHz 时钟驱动。) 
#define CHANNEL     0    
// 分辨率设置为 1，就是 2 的 1 次方，用 2 的数值来映射角度。   
#define RESOLUTION  8      
// 定义舵机 PWM 控制引脚。 
#define SERVO       13      

#define Closepin 15  //红外测关门，暂无
#define DHT11pin 14

static uint8_t userID=0;
static uint16_t value;
static float temperature = 0.0;
static float humidity = 0.0;


static boolean BLE_connect_status = false;
static BLEClient*  pClient=NULL;
static BLEAdvertisedDevice* myDevice          = NULL;
static BLEService *Locker                     = NULL;
static BLECharacteristic  *Lockertemperature  = NULL;
static BLECharacteristic  *Lockerhumidity     = NULL;

int calculatePWM(int degree) 
{ 
    //20ms 周期内，高电平持续时长 0.5-2.5 ms，对应 0-180 度舵机角度。
    //对应 0.5ms（0.5ms/(20ms/256)）
    float min_width = 0.6 / 20 * pow(2, RESOLUTION);
    //对应 2.5ms（2.5ms/（20ms/256））
    float max_width = 2.5 / 20 * pow(2, RESOLUTION);
    if (degree < 0)
      degree = 0;
    if (degree > 180)
      degree = 180;
    //返回度数对应的高电平的数值
    return (int)(((max_width - min_width) / 180) * degree + min_width); 
}

void openthegate()
{
  ledcWrite(CHANNEL, calculatePWM(120));
  delay(2000);
  ledcWrite(CHANNEL, calculatePWM(0));
}

void WaitTilclose()
{


}

class MyServerCallbacks: public BLEServerCallbacks 
{
    void onConnect(BLEServer* pServer) {
      Serial.println("现在有设备接入~");
      BLE_connect_status = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("现在有设备断开连接~");
      // 在有设备接入后Advertising广播会被停止，所以要在设备断开连接时重新开启广播
      // 不然的话只有重启ESP32后才能重新搜索到
      BLE_connect_status = false;
      pServer->startAdvertising(); //该行效果同 BLEDevice::startAdvertising();
    }
};

//读写操作回调
class LockerstatusCallbacks: public BLECharacteristicCallbacks 
{
  void onRead(BLECharacteristic* pCharacteristic) { // 客户端读取事件回调函数
    Serial.println("触发读取事件");
  }

  void onWrite(BLECharacteristic *pCharacteristic) 
  { // 客户端写入事件回调函数
    Serial.println("触发写入事件");
    value =pCharacteristic->getValue().data()[0];
    Serial.print("柜子状态的characteristic的值被修改:0x");
    Serial.println(value,HEX);
    //几种情况：空柜子存货（非0ID存货），空柜子取货（非0ID取货），对的人取货（对上ID的开门），错的人取不了货（对不上ID的开门）
    if((value & 0x0f) == 0x02)
    {
      Serial.println("触发空柜存货程序");
      userID=value&0xf0;
      openthegate();
      WaitTilclose();
      value=userID+8;//存货完成响应位
      Serial.print("存货成功!使用者为ID"+String(userID>>4));
      Serial.println(",将其中的Value设置为: \"" + String(value,HEX) + "\"");
      pCharacteristic->setValue(value);
    }
    else if(((value&0x01)==0x01)&&((value&0xf0)==userID))
    {
      Serial.print("触发有货取货程序");
      userID=0;//取货变空柜
      openthegate();
      WaitTilclose();
      value= userID+4;//取货响应位
      Serial.println("取货成功!将其中的Value设置为: \"" + String(value,HEX) + "\"");
      pCharacteristic->setValue(value);
    }
    else if(((value&0x01)==0x01)&&((value&0xf0)!=userID))
    {
      Serial.println("你这取货人对不上啊！");
      value= value+3;//取货响应位
      Serial.println("取货失败!将其中的Value设置为: \"" + String(value,HEX) + "\"");
      pCharacteristic->setValue(value);
    }
  }
};
class LockertemperatureCallbacks: public BLECharacteristicCallbacks 
{
    void onRead(BLECharacteristic* pCharacteristic) { // 客户端读取事件回调函数
      Serial.println("触发读取事件");
    }

    void onWrite(BLECharacteristic *pCharacteristic) { // 客户端写入事件回调函数
      Serial.println("触发写入事件");
    }
};
class LockerhumidityCallbacks: public BLECharacteristicCallbacks 
{
    void onRead(BLECharacteristic* pCharacteristic) { // 客户端读取事件回调函数
      Serial.println("触发读取事件");
    }

    void onWrite(BLECharacteristic *pCharacteristic) { // 客户端写入事件回调函数
      Serial.println("触发写入事件");
    }
};

void setup() 
{  

  Serial.begin(115200);
  Serial.println("正在启用arduinoBLE应用...");
  // 用于设置 LEDC 通道的频率和分辨率
  ledcSetup(CHANNEL, FREQ, RESOLUTION);
  // 将通道与对应的引脚连接
  ledcAttachPin(SERVO, CHANNEL);   
  ledcWrite(CHANNEL, calculatePWM(0)); 

  BLEDevice::init(BLEServerName);
  BLEServer *pServer = BLEDevice::createServer(); // 创建服务器
  pServer->setCallbacks(new MyServerCallbacks()); // 绑定回调函数

  //创建服务LED，以及服务内的特性LEDstatus，可读可写。
  Locker =  pServer->createService(BLEServerUUID);
  BLECharacteristic  *Lockerstatus = Locker->createCharacteristic(BLECharacteristic1UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  Lockerstatus->setCallbacks(new LockerstatusCallbacks());
  value = 0;
  Lockerstatus->setValue(value);
  Lockertemperature = Locker->createCharacteristic(BLECharacteristic2UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  Lockertemperature->setCallbacks(new LockertemperatureCallbacks());
  Lockertemperature->setValue(" ");
  Lockerhumidity = Locker->createCharacteristic(BLECharacteristic3UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  Lockerhumidity->setCallbacks(new LockerhumidityCallbacks());
  Lockerhumidity->setValue(" ");
  Locker->start();
  BLEDevice::setPower(ESP_PWR_LVL_P9,ESP_BLE_PWR_TYPE_ADV);
  BLEDevice::startAdvertising();
} // End of setup.



// This is the Arduino main loop function.
void loop() 
{

  //每轮都要读一次DHT11
  //read_dht(temperature, humidity, pin, type (DHT11, DHT21, DHT22, AM2301, AM2302), RMT channel (0-7) 
	uint8_t error=read_dht(temperature, humidity, DHT11pin, DHT11, 0);
  if(error)
  {
    Serial.print("DHT11发生错误");
		Serial.println(error);
	}
  else
  {
    String newValue="temperature:"+String(temperature)+" \C";
    Serial.println("将其中的Value设置为: \"" + newValue + "\"\0");
    Lockertemperature->setValue(newValue.c_str());
    newValue="humidity:"+String(humidity)+"\%";
    Serial.println("将其中的Value设置为: \"" + newValue + "\"\0");
    Lockerhumidity->setValue(newValue.c_str());
  }

  
  delay(1000); // Delay a second between loops.
} // End of loop
