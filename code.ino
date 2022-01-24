#define BLINKER_WIFI
#define BLINKER_MIOT_SENSOR
#include <Blinker.h>

char auth[] = "";
char ssid[] = "";
char pswd[] = "";

BlinkerNumber HUMI("humi");
BlinkerNumber TEMP("temp");
BlinkerText TEXT1("tex-s"); 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 14
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

uint32_t read_time = 0;

float humi_read, temp_read;

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
    
}

void dataStorage()
{
    Blinker.dataStorage("cha-temp", temp_read);
    Blinker.dataStorage("cha-humi", humi_read);
}
 
String rts()//计算运行时间
{
    int rt =Blinker.runTime();
    int r,e,f,s;
    String fh;
    Blinker.delay(100);
    if(rt >= 86400)//天数
    {r = rt / 86400;
     e = rt / 3600 - r*24;
     f = rt / 60 - r*1440 - e*60;
     s = rt - r*86400 - e*3600 - f*60;}
    else if(rt >= 3600)
    {r = 0;
     e = rt / 3600;
     f = rt / 60 - e*60;
     s = rt - e*3600 - f*60;}
    else
    {r = 0;
     e = 0;
     f = rt / 60;
     s = rt - f*60;}
     
    //BLINKER_LOG(r," ",e," ",f," ",s);//输出数据测试
    
    if(f==0 & e==0 & r==0)
    {fh = String("")+ s +"秒";}
    else if(r == 0 & e == 0 )
    {fh = String("")+ f + "分" + s +"秒";}
    else if(r == 0)
    {fh = String("")+ e + "时" + f + "分" + s +"秒"; }
    else
    {fh = String("")+ r + "天" + e + "时" + f + "分" + s +"秒";}

    return(fh);
}

void heartbeat()//心跳
{ 
    HUMI.print(humi_read);
    TEMP.print(temp_read);
    TEXT1.print(rts());
}

void miotQuery(int32_t queryCode)      //小爱同学语音命令反馈
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

            int humi_read_int=humi_read;     //去掉湿度浮点
            BlinkerMIOT.humi(humi_read_int);     //小爱接收湿度
            BlinkerMIOT.temp(temp_read);      //小爱接收温度
            BlinkerMIOT.print();

}

void setup()
{
    Serial.begin(115200);
  
    BLINKER_DEBUG.stream(Serial);

    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    Blinker.attachHeartbeat(heartbeat);//附加心跳
    Blinker.attachDataStorage(dataStorage);//附加数据存储    
    dht.begin();
    
     if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  BlinkerMIOT.attachQuery(miotQuery);
  Blinker.delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop()
{
     Blinker.run();
     float h = dht.readHumidity();
     float t = dht.readTemperature();
     if (isnan(h) || isnan(t))
     {
        BLINKER_LOG("Failed to read from DHT sensor!");
     }
     else
     {
       //BLINKER_LOG("Humidity: ", h, " %");
       //BLINKER_LOG("Temperature: ", t, " *C");
        humi_read = h;
        temp_read = t;
        float hic = dht.computeHeatIndex(t, h, false);

        // 显示
        display.clearDisplay();
        // 显示温度
        display.setTextSize(1);
        display.setCursor(0,0);
        display.print("Temp: ");
        display.setTextSize(2);
        display.setCursor(0,10);
        display.print(t);
        display.print(" ");
        display.setTextSize(1);
        display.cp437(true);
        display.write(167);
        display.setTextSize(2);
        display.print("C");
        // 显示湿度
        display.setTextSize(1);
        display.setCursor(0, 35);
        display.print("Humi: ");
        display.setTextSize(2);
        display.setCursor(0, 45);
        display.print(h);
        display.print(" %"); 
        //刷新显示
         display.display(); 
     }
  Blinker.delay(1000);
}   
    
