#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>

#define TFT_CS 1
#define TFT_RST 2
#define TFT_DC 3
#define TFT_SCLK 4
#define TFT_MOSI 5

// Buttons
#define PLAY_BUTTON 12
#define NEXT_BUTTON 13
#define PREV_BUTTON 14

char* SSID = "YOUR WIFI SSID";
const char* PASSWORD = "YOUR WIFI PASSWORD";
const char* CLIENT_ID = "YOUR CLIENT ID FROM THE SPOTIFY DASHBOARD";
const char* CLIENT_SECRET = "YOUR CLIENT SECRET FROM THE SPOTIFY DASHBOARD";

String lastArtist;
String lastTrackname;

Spotify sp(CLIENT_ID, CLIENT_SECRET);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



void drawEqualizer()
{
  int baseY = 150;

  for(int i = 0; i < 6; i++)
  {
    int height = random(10,35);
    int x = 10 + (i * 20);

    // clear previous bar
    tft.fillRect(x,100,10,60,ST77XX_BLACK);

    // draw bar
    tft.fillRect(x, baseY - height, 10, height, ST77XX_GREEN);
  }
}


void setup() {

    Serial.begin(115200);

    pinMode(PLAY_BUTTON, INPUT_PULLUP);
    pinMode(NEXT_BUTTON, INPUT_PULLUP);
    pinMode(PREV_BUTTON, INPUT_PULLUP);

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    Serial.println("TFT Initialized!");
    tft.fillScreen(ST77XX_BLACK);

    randomSeed(analogRead(0));

    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to WiFi...");

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.printf("\nConnected!\n");

    tft.setCursor(0,0);
    tft.write(WiFi.localIP().toString().c_str());

    sp.begin();

    while(!sp.is_auth()){
        sp.handle_client();
    }

    Serial.println("Authenticated");
}


void loop()
{

    // BUTTON CONTROLS

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


    if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty()) {

        tft.fillScreen(ST77XX_BLACK);

        lastArtist = currentArtist;

        Serial.println("Artist: " + lastArtist);

        tft.setCursor(10,10);
        tft.write(lastArtist.c_str());
    }

    if (lastTrackname != currentTrackname && currentTrackname != "Something went wrong" && currentTrackname != "null") {

        lastTrackname = currentTrackname;

        Serial.println("Track: " + lastTrackname);

        tft.setCursor(10,40);
        tft.write(lastTrackname.c_str());
    }

    // Equalizer animation
    drawEqualizer();

    delay(150);
}
