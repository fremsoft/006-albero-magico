/*
   Il DF-player è connesso ai seguenti pin tramite Software Serial:

   PIN D2 RX
   PIN D3 TX

   Un LED è connesso al pin #4

   I tasti delle note sono connessi a:
   
   PIN D5   DO
   PIN D6   RE
   PIN D7   MI
   PIN D8   FA
   PIN D9   SOL
   PIN D10  LA
   PIN D11  SI

   Altri tasti aggiuntivi sono connessi a:

   PIN D14  F1
   PIN D15  F2
   PIN D16  F3
   PIN D17  F4

   Il LED BUILTIN effettua un lampeggio a 1,5Hz per indicarci che
   Arduino sta lavorando correttamente

   Da usare con l'albero magico
   Guarda il video integrale: https://youtu.be/to4RZGu3Jdg

*/

#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

#define DFPLAYER_TX 3
#define DFPLAYER_RX 2

#define OUT_LED     4

#define IN_F4      14
#define IN_F3      15
#define IN_F2      16
#define IN_F1      17

#define IN_NOTA_1  5
#define IN_NOTA_2  6
#define IN_NOTA_3  7
#define IN_NOTA_4  8
#define IN_NOTA_5  9
#define IN_NOTA_6  10
#define IN_NOTA_7  11

const int in_func[4] = { IN_F1,  IN_F2,  IN_F3,  IN_F4 };
const int in_nota[7] = { IN_NOTA_1, IN_NOTA_2, IN_NOTA_3, IN_NOTA_4, IN_NOTA_5, IN_NOTA_6, IN_NOTA_7 };

int watchdog;

int gruppo;

class Mp3Notify;

SoftwareSerial dfplayerSerial( DFPLAYER_RX, DFPLAYER_TX ); // RX, TX
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;

DfMp3 dfmp3(dfplayerSerial);

class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {
      if (source & DfMp3_PlaySources_Sd)
      {
        Serial.print("SD Card, ");
      }
      if (source & DfMp3_PlaySources_Usb)
      {
        Serial.print("USB Disk, ");
      }
      if (source & DfMp3_PlaySources_Flash)
      {
        Serial.print("Flash, ");
      }
      Serial.println(action);
    }
    static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
    {
      // see DfMp3_Error for code meaning
      Serial.println();
      Serial.print("Com Error ");
      Serial.println(errorCode);
    }
    static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
    {
      Serial.print("Play finished for #");
      Serial.println(track);
    }
    static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "online");
    }
    static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "inserted");
    }
    static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "removed");
    }
};


void setup() {

  pinMode( LED_BUILTIN, OUTPUT);
  digitalWrite( LED_BUILTIN, LOW );

  // lampeggio led iniziale
  pinMode( OUT_LED, OUTPUT );
  digitalWrite( OUT_LED, HIGH );
  delay(300);
  digitalWrite( OUT_LED, LOW );
  delay(300);

  digitalWrite( OUT_LED, HIGH );
  delay(300);
  digitalWrite( OUT_LED, LOW );
  delay(300);

  digitalWrite( OUT_LED, HIGH );
  delay(300);
  digitalWrite( OUT_LED, LOW );
  delay(300);

  for (int i = 0; i < 4; i++) {
    pinMode( in_func[i], INPUT_PULLUP);
  }
  
  for (int i = 0; i < 7; i++) {
    pinMode( in_nota[i], INPUT_PULLUP);
  }

  Serial.begin(115200);
  Serial.println("Initializing DF-Player...");
  dfmp3.begin();

  uint16_t volume = dfmp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  dfmp3.setVolume(25);

  uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);

  Serial.println("starting...");

  watchdog = 0;
  gruppo   = 0;
  
  Serial.println("Track 1");
  dfmp3.playMp3FolderTrack(1);  // sd:/mp3/0001.mp3

}

void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();

  while ((millis() - start) < msWait)
  {
    // if you have loops with delays, its important to
    // call dfmp3.loop() periodically so it allows for notifications
    // to be handled without interrupts
    dfmp3.loop();
    delay(1);
  }
}


void loop()
{
  delay(1); /* il loop viene eseguito ogni millisecondo */
  watchdog++; digitalWrite( LED_BUILTIN, (watchdog > 300)?(HIGH):(LOW)); if (watchdog > 600) { watchdog = 0;} 
  
  for (int i = 0; i < 4; i++) {
    if ( digitalRead(in_func[i]) == LOW ) {
      Serial.print("Premuto F");
      Serial.println(i+1);

      gruppo = i;

      Serial.print("Track ");
      Serial.println(i+2);
      dfmp3.playMp3FolderTrack(i+2);  // sd:/mp3/000i+2.mp3

      digitalWrite( OUT_LED, HIGH );
      delay(300);
      digitalWrite( OUT_LED, LOW );
      delay(300);

      digitalWrite( OUT_LED, HIGH );
      delay(300);
      digitalWrite( OUT_LED, LOW );
      delay(300);

    }
  }
  
  for (int i = 0; i < 7; i++) {
    if ( digitalRead(in_nota[i]) == LOW ) {
      Serial.print("Premuto Nota ");
      Serial.println(i+1);

      int n = 6 + gruppo*7 + i;
      Serial.print("Track ");
      Serial.println(n);
      dfmp3.playMp3FolderTrack(n);  // sd:/mp3/000n.mp3

      digitalWrite( OUT_LED, HIGH );
      delay(300);
      digitalWrite( OUT_LED, LOW );
      delay(700);

    }
  }  
}
