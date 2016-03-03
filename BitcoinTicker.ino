#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <Adafruit_ILI9341.h>
#include <Fonts/FreeSansBold9pt7b.h>  //Within the ADAFRUIT GFX Library
#include <Fonts/FreeSansBold18pt7b.h> //Within the ADAFRUIT GFX Library

#include "RTClib.h"

#include <SPI.h>
#include <Wire.h>

//#include <EEPROM.h>  <--not currently in use

//BitcoinTicker includes
#include "BitcoinTicker.h"
#include "BlockchainInfoAPI.h"
#include "CoinbaseAPI.h"

#define BUTTON_PIN D1

#define TFT_DC D4
#define TFT_CS D8

#define SDA_PIN D2
#define SCL_PIN D3
        
//#define COINBASE_FINGERPRINT        "35 3E 28 E0 75 8B B7 61 33 6E 23 67 38 CA 6E 74 2A 63 17 B5"
const char coinbase_fingerprint[] PROGMEM = "35 3E 28 E0 75 8B B7 61 33 6E 23 67 38 CA 6E 74 2A 63 17 B5";

//#define BLOCKCHAIN_INFO_FINGERPRINT "94 10 81 EB E4 62 B5 BD 7B 03 DE 79 C7 A6 4D 91 30 13 7B E0"
const char blockchain_info_fingerprint[] PROGMEM = "94 10 81 EB E4 62 B5 BD 7B 03 DE 79 C7 A6 4D 91 30 13 7B E0";

#define UPDATE_TIMER_DELAY    60000
#define DAILY_TIMER_DELAY     60000
#define DISPLAY_UPDATE_DELAY  30000
#define TIME_UPDATE_DELAY     5000
#define EPOCH_UPDATE_DELAY    1800000

//#define UPDATE_TIMER_DELAY  5000
//#define DAILY_TIMER_DELAY   5000

//#define DISPLAY_UPDATE_DELAY  1000

char *deviceSSID = "BitcoinTicker";
char *devicePassword = "a123fasdwerhjj";

const char* ssid = "AgemoNet";
const char* password = "olliepoop";

int httpCode;
String getData, priceMarketCapString;

unsigned long updateTimer = millis() + UPDATE_TIMER_DELAY;
unsigned long dailyTimer = millis() + DAILY_TIMER_DELAY;
unsigned long displayTimer = millis() + DISPLAY_UPDATE_DELAY;
unsigned long timeTimer = millis() + TIME_UPDATE_DELAY;
unsigned long epochTimer = millis() + EPOCH_UPDATE_DELAY;

HTTPClient http;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

float priceBid, priceAsk, priceOpen, priceHigh, priceLow, rateDelta, priceDelta, epochCoinbase, priceMarketCap, count24Transactions, countUnconfirmed;

//bitmap variables
int16_t bx, by, bw, bh, clearx, cleary;
uint16_t bcolor, bgcolor, clearw, clearh;

RTC_DS1307 RTC;
DateTime now;

void updateBitcoinData();
void updateDailyBitcoinData();
void initDisplay();
void updateMarketData();
void updateTime();
void updateCoinbaseEpoch();
void updateDisplay();
void forceUpdate();

//Initialize Unit
void setup(void){

  /*
  EEPROM.begin(512);
  red_value = EEPROM.read(0);
  green_value = EEPROM.read(1);
  blue_value = EEPROM.read(2);
  */

  pinMode(BUTTON_PIN, INPUT);
  
  Serial.begin(115200);

  WiFi.softAP(deviceSSID, devicePassword);
  delay(500);
  
  WiFi.begin(ssid, password);
   
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("macAddress: ");
  Serial.println(WiFi.macAddress());

  Wire.begin(SDA_PIN, SCL_PIN);
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  //RTC.adjust(DateTime(__DATE__, __TIME__));
  //-21600
  //RTC.adjust(1455160779.488-21600);
  //Date |Feb 10 2016| Time |21:09:01|

  tft.begin();

  tft.setRotation(1);

  //get initial bitcoin data from coindesk
  updateDailyBitcoinData();
  updateBitcoinData();
  //print initial bitcoin data to display
  initDisplay();
  updateMarketData();
  updateCoinbaseEpoch();
  updateTime();  // this should be called after updateCoinbaseEpoch();
  
  //Update Last
  updateDisplay();
}

void updateBitcoinData(){

  http.begin("api.exchange.coinbase.com", 443, "/products/BTC-USD/book/", true, coinbase_fingerprint);
  //http.begin("api.exchange.coinbase.com", 443, "/products/BTC-USD/book/", true, fingerprint);
  
  httpCode = http.GET();
  getData = http.getString();

  if(httpCode){

    Serial.print("HTTP Code (Book): ");
    Serial.println(httpCode);

    if(httpCode == 200){

      //Serial.println(getData);

      priceBid = get_coinbase_bid(getData);
      priceAsk = get_coinbase_ask(getData); 
      
      Serial.print("Bid Price: ");
      Serial.print(priceBid);
      Serial.print(" | Ask Price: ");
      Serial.print(priceAsk);      
    }
    
  }
  else{
    Serial.println("HTTP Current GET failed.");
  }

  http.end();

  rateDelta = (1 - (priceOpen / priceAsk)) * 100;

  Serial.print(" (");
  if(rateDelta > 0){Serial.print("^");}
  else             {Serial.print("v");}
  Serial.print(rateDelta);
  Serial.println("%)");

}

//Possible bitfinex info
//https://api.bitfinex.com/v1/pubticker/BTCUSD


void updateDailyBitcoinData(){
  
  http.begin("api.exchange.coinbase.com", 443, "/products/BTC-USD/stats/", true, coinbase_fingerprint);
  //http.begin("api.exchange.coinbase.com", 443, "/products/BTC-USD/stats/", true, fingerprint);  

  httpCode = http.GET();
  getData = http.getString();
  
  if(httpCode){

    Serial.print("HTTP Code (24Hr): ");
    Serial.println(httpCode);

    if(httpCode == 200){

      //Serial.println(getData);
      priceOpen = get_coinbase_open(getData);
      priceHigh = get_coinbase_high(getData);
      priceLow  = get_coinbase_low(getData);
      
      Serial.print("Open Price: ");
      Serial.print(priceOpen);
      Serial.print(" | High Price: ");
      Serial.print(priceHigh);      
      Serial.print(" | Low Price: ");
      Serial.println(priceLow);      

    }

  }
  else{
    Serial.println("HTTP Previous GET failed.");
  }

  http.end();

}

void updateCoinbaseEpoch(){
  
  http.begin("api.exchange.coinbase.com", 443, "/time/", true, coinbase_fingerprint);
  //http.begin("api.exchange.coinbase.com", 443, "/time/", true, fingerprint);  

  httpCode = http.GET();
  getData = http.getString();
  
  if(httpCode){

    Serial.print("HTTP Code (CB Time): ");
    Serial.println(httpCode);

    if(httpCode == 200){

      //Serial.println(getData);
      epochCoinbase = get_coinbase_epoch(getData);
      
      Serial.print("Coinbase Epoch: ");
      Serial.println(epochCoinbase-21570,0);
      RTC.adjust(epochCoinbase-21570);
    }

  }
  else{
    Serial.println("HTTP Previous GET failed.");
  }

  http.end();  
}

void updateMarketData(){

  //START -- MarketCap --
  http.begin("blockchain.info", 443, "/q/marketcap", true, blockchain_info_fingerprint);
  //http.begin("blockchain.info", 443, "/q/marketcap", true, blockchain_info_fingerprint);  

  httpCode = http.GET();
  getData = http.getString();

  if(httpCode){

    Serial.print("blockchain.info (MCAP) HTTPCode: ");
    Serial.println(httpCode);

    //httpCode 429 = blockchain.info max connections, try again
    if(httpCode == 200){
      
      priceMarketCap = sci_to_float(getData);
      priceMarketCapString = String(priceMarketCap);
      priceMarketCapString = priceMarketCapString.substring(0, priceMarketCapString.indexOf("."));


      
      Serial.print("Market Cap: ");
      Serial.println(add_commas(priceMarketCapString));
      //Serial.println(priceMarketCapString);
    }

  }
  else{
    Serial.println("HTTP Previous GET failed.");
  }

  http.end();
//END -- MarketCap --

//START -- Transaction Count --
  http.begin("blockchain.info", 443, "/q/24hrtransactioncount", true, blockchain_info_fingerprint);
  //http.begin("blockchain.info", 443, "/q/24hrtransactioncount", true, blockchain_info_fingerprint);  

  httpCode = http.GET();
  getData = http.getString();

  if(httpCode){

    Serial.print("blockchain.info (24Hr Transaction Count) HTTPCode: ");
    Serial.println(httpCode);

    if(httpCode == 200){

      count24Transactions = getData.toFloat();
      Serial.print("24Hr Transaction Count: ");
      Serial.println(count24Transactions, 0);

    }

  }
  else{
    Serial.println("HTTP Previous GET failed.");
  }

  http.end();
//END -- Transaction Count --

//START -- Unconfirmed Count --
  http.begin("blockchain.info", 443, "/q/unconfirmedcount", true, blockchain_info_fingerprint);
  //http.begin("blockchain.info", 443, "/q/unconfirmedcount", true, blockchain_info_fingerprint);  

  httpCode = http.GET();
  getData = http.getString();

  if(httpCode){

    Serial.print("blockchain.info (Unconfirmed) HTTPCode: ");
    Serial.println(httpCode);

    if(httpCode == 200){

      countUnconfirmed = getData.toFloat();
      Serial.print("Unconfirmed Transactions: ");
      Serial.println(countUnconfirmed, 0);

    }

  }
  else{
    Serial.println("HTTP Previous GET failed.");
  }

  http.end();
//END -- Unconfirmed Count --


}

void loop(void){

  WiFi.mode(WIFI_STA);


  if(digitalRead(BUTTON_PIN) == LOW){
    forceUpdate();
  }

  
  // unsigned long 4,294,967,295
  // 4,294,900,000 reset
  // 1 minute = 60000 millis
  if(millis() >= updateTimer){
    updateBitcoinData();
    updateMarketData();
    updateTimer = millis()+ UPDATE_TIMER_DELAY;
  }

  if(millis() >= dailyTimer){
    updateDailyBitcoinData();
    dailyTimer = millis()+ DAILY_TIMER_DELAY;
  }
  

  if(millis() >= timeTimer){
    updateTime();
    timeTimer = millis() + TIME_UPDATE_DELAY;
  }

  if(millis() >= epochTimer){
    updateCoinbaseEpoch();
    epochTimer = millis() + EPOCH_UPDATE_DELAY;
  }

  if(millis() >= displayTimer){
    updateDisplay();
    displayTimer = millis() + DISPLAY_UPDATE_DELAY;
  }

  //if millis counter is approaching overflow reset the unit
  if(millis() >= 4294900000){ESP.reset();}

}

void forceUpdate(){
  updateTimer = millis();
  dailyTimer = millis();
  displayTimer = millis();
  timeTimer = millis();
  epochTimer = millis();
}

void initDisplay() {

  tft.fillScreen(ILI9341_LIGHTBLUE);
  
}

void updateTime(){

 now = RTC.now();
 
 Serial.print("Time: ");
 /*
 Serial.print(now.hour(), DEC);
 Serial.print(":");
 Serial.print(now.minute(), DEC);
 Serial.print(":"); 
 Serial.print(now.second(), DEC);
 */
 
  //See RTClib > ds1307 for examples of how to calculate ahead and behind in time
  Serial.print(formatTime(now.hour(), now.minute()));
  Serial.print("  Date: ");
  Serial.print(now.month(), DEC);
  Serial.print("-");
  Serial.print(now.day(), DEC);
  Serial.print("-"); 
  Serial.println(now.year(), DEC);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(36, 200);
  tft.getTextBounds("22:22 PM", 36, 200, &clearx, &cleary, &clearw, &clearh);
  tft.fillRect(clearx, cleary, clearw, clearh, ILI9341_LIGHTBLUE);
  tft.print(formatTime(now.hour(), now.minute()));

 //36, 200
  
}

void updateDisplay(){

  rateDelta = (1 - (priceOpen / priceAsk)) * 100;
  priceDelta = priceAsk - priceOpen;
  
  //tft.drawRect(0, 0, 319, 239, ILI9341_LIGHTBLUE);
  tft.fillRect(0, 0, 319, 239, ILI9341_LIGHTBLUE);

  tft.setFont(&FreeSansBold18pt7b);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(36, 82);
  tft.print(priceAsk);  
  tft.setCursor(38, 82);
  tft.print(priceAsk);  
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(37, 82);
  tft.print(priceAsk);

  tft.setFont(&FreeSansBold9pt7b);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(36, 20);
  tft.print("Bid ");
  tft.print(priceBid);
  tft.print(" Ask ");
  tft.print(priceAsk);      

  tft.setCursor(36, 110);
  tft.print(priceDelta);
  tft.print(" (");
  if(rateDelta > 0){tft.setTextColor(ILI9341_GREEN); }
  else             {tft.setTextColor(ILI9341_RED); }
  tft.print(rateDelta);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("%) 24 Hr ");
  tft.print(priceOpen);

  tft.setCursor(36, 130);
  tft.print("High ");
  tft.print(priceHigh);
  tft.print("  Low ");
  tft.print(priceLow);

  tft.setCursor(36, 155);
  tft.print("MCAP $");
  tft.print(add_commas(priceMarketCapString));

  tft.setCursor(24, 175);
  tft.print("Trns Ct: 24 Hr ");
  tft.print(count24Transactions, 0);
  tft.print(" Uncf ");
  tft.print(countUnconfirmed, 0);
  
  tft.setCursor(36, 200);
  tft.print(formatTime(now.hour(), now.minute()));
  tft.print("  ");
  tft.print(now.month(), DEC);
  tft.print("-");
  tft.print(now.day(), DEC);
  tft.print("-"); 
  tft.print(now.year(), DEC);


//34w 56h
//BitcoinSymbol
//320x240
/*
bx = 200;
by = 80;
bw = 34;
bh = 56;
bcolor = 0xFFFF;
bgcolor = 0x2B96;
*/

  //tft.drawBitmap(200, 80, BitcoinSymbol, 34, 56, 0x0000, 0x2B96);
  //tft.drawBitmap(198, 78, BitcoinSymbol, 34, 56, 0xFFFF);

}

