#define FASTLED_ALLOW_INTERRUPTS 0

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <FastLED.h>
#include <ESP8266mDNS.h>
#include <Timezone.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define LED_PIN D6
#define NUM_LEDS 40
#define TIME_FORMAT 12 // 12 = 12 hours format || 24 = 24 hours format
#define BRIGHTNESS 20

WiFiUDP ntpUDP;
//WiFiManager wifiManager;

// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);

NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

int period = 1000; //Update frequency
unsigned long time_now = 0;
int Seconds, Minute, Hour;

CRGB leds[NUM_LEDS];
CHSV colour(0, 255, 180);
int hue = 0;
CHSV colourOfNumbers(160, 255, 255);
int counter = 0;

// now no need to subtract by 1
const int Place1LEDs[2][5] = {
  {},//
  {0,1,2,3,4}//should start from 0
};

const int Place2LEDs[10][11] = {
  {}, // 0 should do nothing
  {5, 11,12,13,14,15}, // 1
  {8, 5,15,14,13,7,8,9,11}, // 2
  {8, 5,15,14,13,7,12,11,9}, // 3
  {6, 15,6,7,13,12,11}, // 4
  {8, 15,5,7,6,13,12,11,9}, // 5
  {8, 15,6,7,8,9,11,12,13}, // 6
  {6, 5,15,14,13,10,9}, // 7
  {10, 5,6,7,8,9,11,12,13,14,15}, // 8
  {8, 5,15,6,7,14,13,10,9} // 9
};

const int Place3LEDs[6][11] = {
  {8, 23,22,19,20,24,25,27,28}, // 0
  {5, 24,25,26,27,28}, // 1
  {8, 23,24,25,26,21,20,19,28}, // 2
  {8, 23,24,25,26,21,19,27,28}, // 3
  {6, 23,22,21,26,27,28}, // 4
  {8, 24,23,22,21,26,27,28,19} // 5
};

const int Place4LEDs[10][11] = {
  {8, 29,30,32,33,34,35,37,38}, // 0
  {5, 34,35,36,37,38}, // 1
  {8, 33,34,35,36,31,30,29,38}, // 2
  {8, 33,24,35,36,31,37,38,29}, // 3
  {6, 33,32,31,36,37,38}, // 4
  {8, 34,33,32,31,36,37,38,29}, // 5
  {9, 34,33,32,31,30,29,38,37,36}, // 6
  {6, 33,34,35,36,39,29}, // 7
  {10, 29,30,31,32,33,34,35,36,37,38}, // 8
  {8, 31,32,33,34,35,36,39,29} // 9
};


void setup()
{

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  clearAll();
  
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  bool res;
  res = wm.autoConnect(); // auto generated AP name from chipid
  
  if(!res) {
    Serial.println("Failed to connect");
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.print("Wifi connected");
  }
    
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  timeClient.begin();
  delay(10);
}

void clearAll()
{
  FastLED.clear();
  FastLED.show();
}


void displaynumber(const int PlaceArr[])
{
  // PlaceArr[0] contains each respective array length
  // start at PlaceArr[1]
  for (int i = 0; i <= PlaceArr[0]; i++)
  {
    leds[PlaceArr[i]] = colour;
    Serial.print(PlaceArr[i]);
    Serial.print(",");
  }
  Serial.print("\n");
}

// hour is in 24
void adjustBrightness(int currentHour)
{
  // Olivia Brightness
  //  if (currentHour >= 22 || currentHour <= 6) {
  //    FastLED.setBrightness(0); // off
  //  } else if (currentHour >= 6 && currentHour <= 11 || currentHour >= 16 && currentHour <= 22) {
  //    FastLED.setBrightness(20);
  //  } else if (currentHour >= 11 && currentHour <= 16) {
  //    FastLED.setBrightness(40);
  //  }

  // Rachel brightness
  if (currentHour >= 20 || currentHour < 7)
  {
    FastLED.setBrightness(4); // off
  }
  else if (currentHour >= 7 && currentHour < 11 || currentHour >= 15 && currentHour < 20)
  {
    FastLED.setBrightness(16);
  }
  else if (currentHour >= 11 && currentHour < 15)
  {
    FastLED.setBrightness(32);
  }
}

void loop()
{

  timeClient.update();
  int Hours;
  unsigned long unix_epoch = timeClient.getEpochTime(); // get UNIX Epoch time
  unsigned long adjusted_unix_epoch = usPT.toLocal(unix_epoch);
  Seconds = second(adjusted_unix_epoch); // get Secondss
  Minute = minute(adjusted_unix_epoch);  // get minutes
  Hours = hour(adjusted_unix_epoch);     // get hours
//  adjustBrightness(Hours);

  if (Hours > 12) {
    Hour = Hours - 12;
  } else {
    Hour = Hours;
  }

  FastLED.clear();
  //changes the colour of background every 10 cycles
  if (counter < 10)
  {
    counter++;
  }
  else
  {
    colour.hue = (colour.hue + 1) % 256;
    counter = 0;
  }

  // Display Number Place 1
  if (Hour > 9) {
    displaynumber(Place1LEDs[1]);
  }

  // Display Number Place 2
  int HourOnesPlace = Hour % 10;
  displaynumber(Place2LEDs[HourOnesPlace]);

  int MinuteOnesPlace = Minute % 10;
  int MinuteTensPlace = (Minute - (MinuteOnesPlace)) / 10;
  // Display Number Place 3
  displaynumber(Place3LEDs[MinuteTensPlace]);
  // Display Number Place 2
  displaynumber(Place4LEDs[MinuteOnesPlace]);

  //display colons
  if (Second % 2 == 0)
  {
    leds[17] = colour;
    leds[19] = colour;
  }
  FastLED.show();
}
