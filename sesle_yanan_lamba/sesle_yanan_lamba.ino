#include <Deneyap_Mikrofon.h>           
#include <Deneyap_5x7LedEkran.h>         
#include <Deneyap_Role.h>               

Microphone mic;
DotMatrix ledMatrix;
Relay role;

const int SES_ESIGI = 400;
const unsigned long DEBOUNCE_SURESI = 800;

bool lambaDurumu = false;
unsigned long sonTetiklemeZamani = 0;


const byte HAZIR_BMP[35] = {
  1, 0, 0, 0, 1,
  1, 0, 0, 0, 1,
  1, 1, 1, 1, 1,
  1, 0, 0, 0, 1,
  1, 0, 0, 0, 1,
  0, 0, 0, 0, 0,
  1, 1, 1, 1, 1
};

const byte ACILDI_BMP[35] = {
  0, 1, 1, 1, 0,
  1, 0, 0, 0, 1,
  1, 1, 1, 1, 1,
  1, 0, 0, 0, 0,
  1, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 1, 1, 1, 0
};

const byte KAPANDI_BMP[35] = {
  1, 0, 1, 0, 1,
  1, 0, 1, 0, 1,
  1, 1, 1, 1, 1,
  1, 0, 1, 0, 1,
  1, 0, 1, 0, 1,
  0, 0, 0, 0, 0,
  1, 1, 1, 1, 1
};

void setup() {
  Serial.begin(115200);

  if (!mic.begin(0x35)) {
    Serial.println("Mikrofon baslatilamadi!");
    while (1);
  }

  if (!ledMatrix.begin(0x0A)) {
    Serial.println("LED Matris baslatilamadi!");
    while (1);
  }

  if (!role.begin(0x0C)) {
    Serial.println("Role baslatilamadi!");
    while (1);
  }

  role.RelayDrive(0);  // Röleyi kapalı başlat
  ledMatrix.resetDotRows();
  ledMatrix.drawLedMatrix(HAZIR_BMP, 500);
}


void loop() {
  sesAlgila();
  delay(100);
}

void sesAlgila() {
  uint16_t sesDegeri = mic.ReadMicrophoneAnalog();
  Serial.print("Ses: ");
  Serial.println(sesDegeri);

  if (sesDegeri > SES_ESIGI && (millis() - sonTetiklemeZamani) > DEBOUNCE_SURESI) {
    lambaDegistir();
    sonTetiklemeZamani = millis();
  }
}

void lambaDegistir() {
  lambaDurumu = !lambaDurumu;
  role.RelayDrive(lambaDurumu);
  ledMatrix.resetDotRows();

  if (lambaDurumu) {
    Serial.println("Lamba AÇILDI");
    ledMatrix.drawLedMatrix(ACILDI_BMP, 500);
  } else {
    Serial.println("Lamba KAPANDI");
    ledMatrix.drawLedMatrix(KAPANDI_BMP, 500);
  }
}
