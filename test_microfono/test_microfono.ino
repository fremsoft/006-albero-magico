// suono plink composto dalle seguenti frequenze:
// 1250 Hz
// 4000 Hz
// 6500 Hz
// per almeno 0,5 secondi

// Guarda il video integrale: https://youtu.be/to4RZGu3Jdg

#define USCITA_LED_ALBERO     2    // ci vuole un transistor PN2222 per pilotare 350mA di carico a 5V
#define INGRESSO_MICROFONO   A0    // collegato a MAX4466 alimentato a 3,3V

void setup() {

  Serial.begin(115200);
  
  pinMode( USCITA_LED_ALBERO, OUTPUT );
  
}


void loop() {

  Serial.print("0,1000,");
  Serial.println( analogRead( INGRESSO_MICROFONO ) );
  
  /*digitalWrite ( USCITA_LED_ALBERO, HIGH );
  delay (300);

  digitalWrite ( USCITA_LED_ALBERO, LOW );
  delay (300);*/

}
