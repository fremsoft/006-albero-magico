// FFT originale tratto da :
// https://create.arduino.cc/projecthub/abhilashpatel121/easyfft-fast-fourier-transform-fft-for-arduino-9d2677

// suono plink composto dalle seguenti frequenze:
// 1250 Hz
// 4000 Hz
// 6500 Hz
// per almeno 0,5 secondi

// Guarda il video integrale: https://youtu.be/to4RZGu3Jdg

#define USCITA_LED_ALBERO     2    // ci vuole un transistor PN2222 per pilotare 350mA di carico a 5V
#define INGRESSO_MICROFONO   A0    // collegato a MAX4466 alimentato a 3,3V
#define INGRESSO_SENSIBILITA A1    // collegato a un potenziometro

#define FFT_DEPTH 64
int data[FFT_DEPTH];

byte sine_data [91] = {
  0,
  4,    9,    13,   18,   22,   27,   31,   35,   40,   44,
  49,   53,   57,   62,   66,   70,   75,   79,   83,   87,
  91,   96,   100,  104,  108,  112,  116,  120,  124,  127,
  131,  135,  139,  143,  146,  150,  153,  157,  160,  164,
  167,  171,  174,  177,  180,  183,  186,  189,  192,  195,       //Paste this at top of program
  198,  201,  204,  206,  209,  211,  214,  216,  219,  221,
  223,  225,  227,  229,  231,  233,  235,  236,  238,  240,
  241,  243,  244,  245,  246,  247,  248,  249,  250,  251,
  252,  253,  253,  254,  254,  254,  255,  255,  255,  255
};
float f_peaks[5];    // top 5 frequencies peaks in descending order
float a_peaks[5];    // top 5 amplitude   peaks in descending order

void setup() {

  //Serial.begin(9600);

  pinMode( LED_BUILTIN, OUTPUT );
  pinMode( USCITA_LED_ALBERO, OUTPUT );

  // tre lampeggi all'accensione
  for (int n = 0; n < 3; n++) {
    // ACCENSIONE ALBERO DI NATALE
    digitalWrite ( USCITA_LED_ALBERO, HIGH );
    delay (300);

    digitalWrite ( USCITA_LED_ALBERO, LOW );
    delay (300);
  }
}

int durataPlink;
unsigned long t0PlinkMs, tPlinkMs;


void loop() {

  int n;
  //unsigned long t0, t1;


  /* acquisizione dati */
  //t0 = micros();
  for (n = 0; n < FFT_DEPTH; n++) {
    data[n] = analogRead( INGRESSO_MICROFONO );
  }
  //t1 = micros();

  //float f = (1000000.0 * FFT_DEPTH) / (t1 - t0);
  float f = 8900.0;

  FFT(data, f);  //to get top five values of frequencies of DATA[x] having FFT_DEPTH samples at f sampling frequency

  /*
    Serial.print(f);
    Serial.print(",0,");
    Serial.print((int)f_peaks[0]);
    Serial.print(",");
    Serial.print((int)f_peaks[1]);
    Serial.print(",");
    Serial.print((int)f_peaks[2]);
    Serial.print(",");
    Serial.print((int)f_peaks[3]);
    Serial.print(",");
    Serial.println((int)f_peaks[4]);
  */

  // cerchiamo le due frequenze : 8900.00,0,3893,1251,0,0,0
  bool f3893 = false;
  bool f1251 = false;
  for (int i = 0; i < 5; i++) {
    if ((int)(f_peaks[i]) == 1251) {
      f1251 = true;
    }
    if ((int)(f_peaks[i]) == 3893) {
      f3893 = true;
    }
  }

  if (f1251 && f3893) {

    digitalWrite( LED_BUILTIN, HIGH );

    tPlinkMs = millis();
    if (durataPlink == 0) {
      t0PlinkMs = tPlinkMs;
    }
    durataPlink++;
    //Serial.println( durataPlink );

  }
  else {

    digitalWrite ( LED_BUILTIN, LOW );
    digitalWrite ( USCITA_LED_ALBERO, LOW );
        
    if ( (millis() - 100) > tPlinkMs) {
      if ((t0PlinkMs != 0) && (tPlinkMs != 0)) {
        unsigned long durataPlinkMs = millis() - t0PlinkMs;
        //Serial.print( "Durata Tot Ms: " );
        //Serial.println( durataPlinkMs );

        if (durataPlinkMs > 400) {

          for (int n = 0; n < 80; n++) {
            // ACCENSIONE ALBERO DI NATALE
            digitalWrite ( USCITA_LED_ALBERO, HIGH );
            delay (10 + n);

            digitalWrite ( USCITA_LED_ALBERO, LOW );
            delay (10 + n);
          }
        }
      }
      t0PlinkMs = 0;
      tPlinkMs = 0;
      durataPlink = 0;
    }
  }





  /*
     DEBUG MICROFONO

    Serial.print("0,1000,");
    Serial.println( analogRead( INGRESSO_MICROFONO ) );
  */

  /*
     ACCENSIONE ALBERO DI NATALE

    digitalWrite ( USCITA_LED_ALBERO, HIGH );
    delay (300);

    digitalWrite ( USCITA_LED_ALBERO, LOW );
    delay (300);*/

}

//-----------------------------FFT Function----------------------------------------------//

void FFT(int in[], float Frequency)
{
  /*
    Code to perform FFT on arduino,
    setup:
    paste sine_data [91] at top of program [global variable], paste FFT function at end of program
    Term:
    1. in[]     : Data array,
    2. N        : Number of sample (recommended sample size 2,4,8,16,32,64,128...)
    3. Frequency: sampling frequency required as input (Hz)

    If sample size is not in power of 2 it will be clipped to lower side of number.
    i.e, for 150 number of samples, code will consider first 128 sample, remaining sample  will be omitted.
    For Arduino nano, FFT of more than 128 sample not possible due to mamory limitation (64 recomended)
    For higher Number of sample may arise Mamory related issue,
    Code by ABHILASH
    Contact: abhilashpatel121@gmail.com
    Documentation:https://www.instructables.com/member/abhilash_patel/instructables/
    2/3/2021: change data type of N from float to int for >=256 samples
  */

  unsigned int data[13] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  int a, c1, f, o, x;
  a = FFT_DEPTH;

  for (int i = 0; i < 12; i++)          //calculating the levels
  {
    if (data[i] <= a) {
      o = 6;
    }
  }

  int in_ps[data[o]] = {};   //input for sequencing
  float out_r[data[o]] = {}; //real part of transform
  float out_im[data[o]] = {}; //imaginory part of transform

  x = 0;
  for (int b = 0; b < o; b++)              // bit reversal
  {
    c1 = data[b];
    f = data[o] / (c1 + c1);
    for (int j = 0; j < c1; j++)
    {
      x = x + 1;
      in_ps[x] = in_ps[j] + f;
    }
  }


  for (int i = 0; i < data[o]; i++)     // update input array as per bit reverse order
  {
    if (in_ps[i] < a)
    {
      out_r[i] = in[in_ps[i]];
    }
    if (in_ps[i] > a)
    {
      out_r[i] = in[in_ps[i] - a];
    }
  }


  int i10, i11, n1;
  float e, c, s, tr, ti;

  for (int i = 0; i < o; i++)                             //fft
  {
    i10 = data[i];            // overall values of sine/cosine  :
    i11 = data[o] / data[i + 1]; // loop with similar sine cosine:
    e = 360 / data[i + 1];
    e = 0 - e;
    n1 = 0;

    for (int j = 0; j < i10; j++)
    {
      c = cosine(e * j);
      s = sine(e * j);
      n1 = j;

      for (int k = 0; k < i11; k++)
      {
        tr = c * out_r[i10 + n1] - s * out_im[i10 + n1];
        ti = s * out_r[i10 + n1] + c * out_im[i10 + n1];

        out_r[n1 + i10] = out_r[n1] - tr;
        out_r[n1] = out_r[n1] + tr;

        out_im[n1 + i10] = out_im[n1] - ti;
        out_im[n1] = out_im[n1] + ti;

        n1 = n1 + i10 + i10;
      }
    }
  }

  /*
    for(int i=0;i<data[o];i++)
    {
    Serial.print(out_r[i]);
    Serial.print("\t");                                     // un comment to print RAW o/p
    Serial.print(out_im[i]); Serial.println("i");
    }
  */


  //---> here onward out_r contains amplitude and our_in conntains frequency (Hz)
  for (int i = 0; i < data[o - 1]; i++)      // getting amplitude from complex number
  {
    out_r[i] = sqrt(out_r[i] * out_r[i] + out_im[i] * out_im[i]); // to  increase the speed delete sqrt
    out_im[i] = i * Frequency / FFT_DEPTH;
    /*
      Serial.print(out_im[i]); Serial.print("Hz");
      Serial.print("\t");                            // un comment to print freuency bin
      Serial.println(out_r[i]);
    */
  }




  x = 0;     // peak detection
  for (int i = 1; i < data[o - 1] - 1; i++)
  {
    if (out_r[i] > out_r[i - 1] && out_r[i] > out_r[i + 1])
    { in_ps[x] = i;  //in_ps array used for storage of peak number
      x = x + 1;
    }
  }


  s = 0;
  c = 0;
  for (int i = 0; i < x; i++)      // re arraange as per magnitude
  {
    for (int j = c; j < x; j++)
    {
      if (out_r[in_ps[i]] < out_r[in_ps[j]])
      { s = in_ps[i];
        in_ps[i] = in_ps[j];
        in_ps[j] = s;
      }
    }
    c = c + 1;
  }



  for (int i = 0; i < 5; i++) // updating f_peak array (global variable)with descending order
  {
    f_peaks[i] = out_im[in_ps[i]];
    a_peaks[i] = out_r[in_ps[i]];
    //if (a_peaks[i] < analogRead( INGRESSO_SENSIBILITA )) { f_peaks[i] = 0; }
    if (a_peaks[i] < 15) {
      f_peaks[i] = 0;
    }
  }
}


float sine(int i)
{
  int j = i;
  float out;
  while (j < 0) {
    j = j + 360;
  }
  while (j > 360) {
    j = j - 360;
  }
  if (j > -1   && j < 91) {
    out = sine_data[j];
  }
  else if (j > 90  && j < 181) {
    out = sine_data[180 - j];
  }
  else if (j > 180 && j < 271) {
    out = -sine_data[j - 180];
  }
  else if (j > 270 && j < 361) {
    out = -sine_data[360 - j];
  }
  return (out / 255);
}

float cosine(int i)
{
  int j = i;
  float out;
  while (j < 0) {
    j = j + 360;
  }
  while (j > 360) {
    j = j - 360;
  }
  if (j > -1   && j < 91) {
    out = sine_data[90 - j];
  }
  else if (j > 90  && j < 181) {
    out = -sine_data[j - 90];
  }
  else if (j > 180 && j < 271) {
    out = -sine_data[270 - j];
  }
  else if (j > 270 && j < 361) {
    out = sine_data[j - 270];
  }
  return (out / 255);
}
