#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>

#define TFT_CS 7
#define TFT_RST 8
#define TFT_DC 5
#define TFT_SCLK 4
#define TFT_MOSI 6

#define PLAY_BUTTON 0
#define NEXT_BUTTON 1
#define PREV_BUTTON 2

#define BATTERY_PIN 3
#define BATTERY_BAR_WIDTH 100
#define BATTERY_BAR_HEIGHT 8

char* SSID = "YOUR WIFI SSID";
const char* PASSWORD = "YOUR WIFI PASSWORD";
const char* CLIENT_ID = "YOUR CLIENT ID";
const char* CLIENT_SECRET = "YOUR CLIENT SECRET";

String lastArtist;
String lastTrackname;

Spotify sp(CLIENT_ID, CLIENT_SECRET);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

float getBatteryVoltage()
{
  int raw = analogRead(BATTERY_PIN);
  float voltage = (raw / 4095.0) * 3.3;
  voltage = voltage * 2;
  return voltage;
}

void drawBatteryBar()
{
  float voltage = getBatteryVoltage();
  float percent = (voltage - 3.0) / (4.2 - 3.0);
  if(percent < 0) percent = 0;
  if(percent > 1) percent = 1;
  int filled = percent * BATTERY_BAR_WIDTH;

  tft.drawRect(20, 0, BATTERY_BAR_WIDTH, BATTERY_BAR_HEIGHT, ST77XX_WHITE);
  tft.fillRect(20, 0, filled, BATTERY_BAR_HEIGHT, ST77XX_GREEN);
  tft.fillRect(20 + filled, 0, BATTERY_BAR_WIDTH - filled, BATTERY_BAR_HEIGHT, ST77XX_BLACK);
}

void drawEqualizer()
{
  int baseY = 150;

  for(int i = 0; i < 6; i++)
  {
    int height = random(10,35);
    int x = 10 + (i * 20);

    tft.fillRect(x,100,10,60,ST77XX_BLACK);
    tft.fillRect(x, baseY - height, 10, height, ST77XX_GREEN);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(PLAY_BUTTON, INPUT_PULLUP);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
  pinMode(PREV_BUTTON, INPUT_PULLUP);

  analogReadResolution(12);
  pinMode(BATTERY_PIN, INPUT);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  randomSeed(analogRead(0));

  WiFi.begin(SSID, PASSWORD);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  tft.setCursor(0,0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.write(WiFi.localIP().toString().c_str());

  sp.begin();

  while(!sp.is_auth())
  {
    sp.handle_client();
  }

  tft.fillScreen(ST77XX_BLACK);
}

void loop()
{
  if(digitalRead(PLAY_BUTTON) == LOW)
  {
    sp.start_resume_playback();
    delay(400);
  }

  if(digitalRead(NEXT_BUTTON) == LOW)
  {
    sp.skip();
    delay(400);
  }

  if(digitalRead(PREV_BUTTON) == LOW)
  {
    sp.previous();
    delay(400);
  }

  String currentArtist = sp.current_artist_names();
  String currentTrackname = sp.current_track_name();

  if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty())
  {
    tft.fillScreen(ST77XX_BLACK);
    lastArtist = currentArtist;
    tft.setCursor(10,15);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.write(lastArtist.c_str());
  }

  if (lastTrackname != currentTrackname && currentTrackname != "Something went wrong" && currentTrackname != "null")
  {
    lastTrackname = currentTrackname;
    tft.setCursor(10,50);
    tft.setTextSize(2);
    tft.write(lastTrackname.c_str());
  }

  drawBatteryBar();
  drawEqualizer();

  delay(150);
}
