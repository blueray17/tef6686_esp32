#include "TEF6686.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//#include <Adafruit_SSD1306.h>

#define ENCODER_A 2
#define ENCODER_B 3
#define BUTTON 4

#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SSD1306 display(5);


bool modeautoSW = false;
long lmillis = 0;
int encoderPos, jmlst,  i = 3 , lastEncoderPos = 1, frequency, level, frequencymw = 522, frequencysw = 2300, frequencylw = 144;
//int encoderPos, jmlst,  i = 14 , lastEncoderPos = 1, frequency, level, frequencymw, frequencysw, frequencylw;
int stepp = 10;
String judul = "FM", mod = "FM";
float frk;
int volume = 2, menu = 1;
TEF6686 radio;
bool mode_menu = false;

/*
  const String st[][2] = {
  "8770", "HardRock",
  "8810", "SuaraEmas",
  "8850", "MoraFM",
  "8890", "Auto",
  "8930", "ElShinta",
  "8970", "Global",
  "9010", "Urban Bdg",
  "9050", "Cakra",
  "9090", "X-Channel",
  "9130", "Trijaya", //1
  "9170", "Voks",
  "9210", "SuaraIndh",
  "9250", "Maestro",
  "9330", "Sonora",
  "9370", "Paramuda",
  "9440", "DeltaFM",
  "9480", "FitFM",
  "9520", "Fajri",
  "9560", "B-Radio",
  "9600", "RRI Pro2", //2
  "9640", "PersibFM", //2
  "9680", "Kharisma",
  "9720", "NewShinta",
  "9760", "RRI Pro1",
  "9800", "MayaFM",
  "9840", "Prambors",
  "9880", "Raka",
  "9920", "Mom&Kids",
  "9960", "Unasko",
  "10000", "99ers",
  "10040", "KLCBS",
  "10080", "Kandaga", //3
  "10110", "MGT",
  "10150", "Dahlia",
  "10190", "Cosmo",
  "10230", "RaseFM",
  "10270", "MQFM",
  "10310", "OzRadio",
  "10350", "Chevy",
  "10390", "UNIKOM",
  "10430", "RodjaFM",
  "10470", "RamaFM", //4
  "10510", "I-Radio",
  "10550", "Garuda",
  "10590", "Ardan",
  "10630", "UrbanFM",
  "10670", "Mara FM",
  "10710", "K-lite",
  "10750", "PRFM",
  "10800", "Arduino"
  };*/

const String st[][2] = {
  "8770", "RRI Pro 3",
  "8850", "RRI Pro 4",
  "9010", "Mandala",
  "9090", "RRI Pro 1",
  "9170", "Heartline",
  "9250", "RRI Pro 2",
  "9330", "Trirama",
  "9520", "Rajawali FM",
  "9600", "Brilian", //2
  "9680", "Swara Wajar",
  "9760", "La Nugraha",
  "9840", "Batara FM",
  "10000", "Sai Radio",
  "10190", "Beoli FM",
  "10270", "Andalas FM",
  "10430", "Erbeka",
  "10510", "El Shinta",
  "10590", "Kharisma FM"
};


void setup() {
  Wire.begin(0, 1);
  Serial.begin(9600);
  delay(100);
  radio.init();
  delay(100);
  radio.powerOn();
  radio.setVolume(volume);
  delay(500);
  frequency = st[i][0].toInt();
  level = radio.getLevel();
  jmlst = sizeof(st) / sizeof(st[0]);
  radio.setFrequency(frequency);
  Serial.print("frequency = ");
  Serial.println(frequency);
  delay(100);
  display.begin(i2c_Address, true); // Address 0x3C default
  //  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  setDisplay();
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);

}

unsigned long previuosMillis = 0;
const long interval = 2000;

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previuosMillis >= interval) {
    previuosMillis = currentMillis ;
    if (mode_menu == false) setDisplay();
  }

  encoderPos = digitalRead(ENCODER_A);
  if (encoderPos != lastEncoderPos) {
    if (digitalRead(ENCODER_B) != encoderPos  && encoderPos == 0) {//naik
      if (mode_menu == false) {
        if (menu == 1) {
          Serial.println("okeee..");
          i++;
          if (i >= jmlst) {
            i = 0;
          }
          frequency = st[i][0].toInt();;
        } else if (menu == 2 || menu == 3) {
          frequency += stepp;
        } else if (menu == 4) {
          frequency = radio.seekUp();
        } else if (menu == 5 || menu == 6) {
          frequencylw += stepp ;
          //        frequencylw = radio.tuneUp();
        } else if (menu == 7 || menu == 8 || menu == 9) {
          frequencymw += stepp ;
          //        frequencymw = radio.tuneUp();
        } else if (menu == 10 || menu == 11 || menu == 12 || menu == 13) {
          frequencysw += stepp ;
          //       frequencysw = radio.tuneUp();
        } else if (menu == 14) {
          autoSW(1);
        }
        setFrek();
        setDisplay();
        delay(50);
      } else {
        menu++;
        if (menu == 15)menu = 1;
        setDisplayMenu(menu);
      }

    } else if (digitalRead(ENCODER_B) == encoderPos  && encoderPos == 0) {//turun
      if (mode_menu == false) {
        if (menu == 1) {
          i--;
          if (i <= -1) {
            i = jmlst - 1;
          }
          frequency = st[i][0].toInt();;
        } else if (menu == 2 || menu == 3) {
          frequency -= stepp;
        } else if (menu == 4) {
          frequency = radio.seekDown();
        } else if (menu == 5 || menu == 6) {
          //        frequencylw = radio.tuneDown();
          frequencylw -= stepp ;
        } else if (menu == 7 || menu == 8 || menu == 9) {
          //       frequencymw = radio.tuneDown();
          frequencymw -= stepp;
        } else if (menu == 10 || menu == 11 || menu == 12 || menu == 13) {
          //        frequencysw = radio.tuneDown();
          frequencysw -= stepp;
        } else if (menu == 14) {
          autoSW(0);
        }
        setFrek();
        setDisplay();
        delay(50);
      } else {
        menu--;
        if (menu == 0)menu = 14;
        setDisplayMenu(menu);
      }

    }
    //setDisplay();
  }

  lastEncoderPos = encoderPos;

  if (digitalRead(BUTTON) == LOW) {
    if (modeautoSW == true) {
      modeautoSW == false;
      delay(500);
    } else {
      if (mode_menu == true) {
        mode_menu = false;
        setMenu();
        delay(100);
        setDisplay();
      } else {
        mode_menu = true;
        setDisplayMenu(menu);
      }
      delay(500);
    }
  }

}

void setMenu() {
  if (menu == 1 || menu == 2 || menu == 3 || menu == 4) {
    mod = "FM";
    radio.setBand(0);
    radio.setBW (1, 200);
    radio.setFrequency(frequency);
    if (menu == 1) {

    } else if (menu == 2) {
      stepp = 10;
      judul = "Step 0.1 MHz";
    } else if (menu == 3) {
      stepp = 100;
      judul = "Step 1 MHz";
    } else if (menu == 4) {
      judul = "FM Scan...";
    }

  } else if (menu == 5 || menu == 6) { //lw
    radio.setBand(4);
    radio.setBW (0, 3);
    mod = "LW";
    if (menu == 5) {
      judul = "Step 1 kHz";
      stepp = 1;
    } else if (menu == 6) {
      judul = "Step 9 kHz";
      stepp = 9;
    }
    radio.setFrequency(frequencylw);

  } else if (menu == 7 || menu == 8 || menu == 9) { //mw
    radio.setBand(3);
    radio.setBW (0, 3);
    mod = "MW";
    if (menu == 7) {
      judul = "Step 1 kHz";
      stepp = 1;
    } else if (menu == 8) {
      judul = "Step 9 kHz";
      stepp = 9;
    } else if (menu == 9) {
      judul = "Step 0.1 MHz";
      stepp = 100;
    }
    radio.setFrequency(frequencymw);

  } else if (menu == 10 || menu == 11 || menu == 12 || menu == 13 || menu == 14) { //sw
    radio.setBand(5);
    radio.setBW (0, 3);
    mod = "SW";
    if (menu == 10) {
      judul = "Step 1 kHz";
      stepp = 1;
    } else if (menu == 11) {
      judul = "Step 5 kHz";
      stepp = 5;
    } else if (menu == 12) {
      judul = "Step 0.1 MHz";
      stepp = 100;
    } else if (menu == 13) {
      judul = "Step 1 MHz";
      stepp = 1000;
    } else if (menu == 14) {
      judul = "Auto 100x";
      stepp = 1000;
    }
    radio.setFrequency(frequencysw);
  }
}

void setFrek() {
  if (menu < 5) {
    radio.setFrequency(frequency);
  } else if (menu == 5 || menu == 6) {
    //      frequencylw = radio.getFrequency();
    radio.setFrequency(frequencylw);
  } else if (menu == 7 || menu == 8 || menu == 9) {
    //      frequencymw = radio.getFrequency();
    radio.setFrequency(frequencymw);
  } else if (menu == 10 || menu == 11 || menu == 12 || menu == 13 || menu == 14) {
    //      frequencysw = radio.getFrequency();
    radio.setFrequency(frequencysw);
  }
  level = radio.getLevel();
}

void autoSW(int updown) { //
  modeautoSW = true;
  if (updown == 0) { //turun
    for (int i = 1; i <= 100; i++) {
      if (digitalRead(BUTTON) == LOW) {
        modeautoSW = false;
        delay(2000);
        return;
      }
      frequencysw -= 5;
      radio.setFrequency(frequencysw);
      setFrek();
      setDisplay();
      delay(600);
    }
  } else { //naik
    for (int i = 1; i <= 100; i++) {
      if (digitalRead(BUTTON) == LOW) {
        modeautoSW = false;
        delay(2000);
        return;
      }
      frequencysw += 5;
      radio.setFrequency(frequencysw);
      setFrek();
      setDisplay();
      delay(600);
    }
  }
  modeautoSW = false ;
}

void setDisplayMenu(int menu) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print(" < Menu >");
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 15);
  display.print(" -------------------");
  if (menu == 1) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" FM");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Saved Memory");
  } else if (menu == 2) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" FM");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step : ");
    display.print("0.1 MHz");
  } if (menu == 3) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" FM");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 1 MHz");
  } else if (menu == 4) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" FM");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Search");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" Auto");

  } else if (menu == 5) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" LW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 1 kHz");
  } else if (menu == 6) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" LW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 9 kHz");

  } else if (menu == 7) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" MW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 1 kHz");
  } else if (menu == 8) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" MW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 9 kHz");
  } else if (menu == 9) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" MW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 0.1 MHz");

  } else if (menu == 10) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" SW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 1 kHz");
  } else if (menu == 11) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" SW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 5 kHz");
  } else if (menu == 12) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" SW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 0.1 MHz");
  } else if (menu == 13) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" SW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Manual");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 1 MHz");
  } else if (menu == 14) {
    display.setCursor(0, 23);
    display.print(" Band :");
    display.print(" SW");
    display.setCursor(0, 35);
    display.print(" Mode :");
    display.print(" Auto 100x");
    display.setCursor(0, 47);
    display.print(" Step :");
    display.print(" 5 kHz");
  }
  /*
    else if (menu == 14) {
      display.setCursor(6, 20);
      display.setTextSize(2);
      display.print("EXIT");
      display.setTextSize(1);
    }
  */

  display.setCursor(0, 60);
  display.print(" -------------------");
  display.display();
}

void setDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 3);
  display.print("   ** Radio ");
  display.print(mod);
  display.println(" **");
  //display.setCursor(0, 8);
  //  display.println(" -------------------");
  display.drawLine(5, 13, display.width() - 5, 13 , SH110X_WHITE);
  display.setCursor(0, 17);
  if (menu == 1) {
    display.print(" Name : ");
    display.println(st[i][1]);
  } else {
    display.print(" Mode : ");
    display.println(judul);
  }
  display.setCursor(0, 28);
  display.print(" Freq : ");
  frk = frequency / 100.0;
  if (menu == 5 || menu == 6) {
    display.print(frequencylw);
    display.println(" kHz");
  } else if (menu == 7 || menu == 8 || menu == 9) {
    display.print(frequencymw);
    display.println(" kHz");
  } else if (menu == 10 || menu == 11 || menu == 12 || menu == 13 || menu == 14) {
    display.print(frequencysw);
    display.println(" kHz");
  } else {
    display.print(frk, 2);
    display.println(" MHz");
  }
  //display.println("");

  //display.display();
  display.setCursor(0, 39);
  display.print(" Sign : ");
  if (level > 800) {
    display.print("1");
  } else {
    display.print(radio.getLevel());
  }
  display.println(" dB");

  display.setCursor(0, 50);
  if (menu < 5) {
    display.print(" Stat : ");
    if (Radio_CheckStereo() < 0) {
      display.print("Mono");
    } else {
      display.print("Stereo");
    }
  } else {
    display.print(" -------------------");
  }


  //  display.setCursor(0, 49);
  //  display.print(" -------------------");
  display.drawRect(0, 0, display.width(), display.height(), SH110X_WHITE);
  display.display();

  /*
    Serial.println();
    Serial.print("Band = "); Serial.println(Radio_GetCurrentBand());
    Serial.print("Frequency = "); Serial.println(Radio_GetCurrentFreq());
    Serial.print("isFM = "); Serial.println(TEF665X_Is_FM_Freq(Radio_GetCurrentFreq()));
    Serial.print("Level = "); Serial.println(Radio_Get_Level(1));
    Serial.print("FM?? = "); Serial.println(Radio_IsFMBand());
    Serial.print("BW = "); Serial.println(Radio_Get_Bandwidth());
    Serial.print("Stereo = "); Serial.println(Radio_CheckStereo());
  */
}
