# 💡 Deneyap Kart ile Ses Kontrollü Akıllı Lamba ve Görsel Geri Bildirim Sistemi

<p align="center">
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Mikrodenetleyici-Deneyap%20Kart%20(ESP32)-24292e?style=flat-square&logo=espressif" alt="Deneyap Kart" /></a>
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Sens%C3%B6r-Deneyap%20Mikrofon%20(I2C%200x35)-24292e?style=flat-square" alt="Mikrofon" /></a>
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/G%C3%B6sterge-5x7%20K%C4%B1rm%C4%B1z%C4%B1%20LED%20Matris-24292e?style=flat-square" alt="LED Matris" /></a>
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Akt%C3%BCat%C3%B6r-Deneyap%20R%C3%B6le%20(I2C%200x0C)-24292e?style=flat-square" alt="Röle" /></a>
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Haberle%C5%9Fme-I2C%20Veri%20Yolu-24292e?style=flat-square" alt="I2C" /></a>
  <a href="#lisans"><img src="https://img.shields.io/badge/Lisans-MIT-24292e?style=flat-square" alt="Lisans" /></a>
</p>

> **Temassız Akıllı Ev Otomasyonu:** Ortamdaki alkış, ses dalgaları veya ani ses tepe noktalarını Deneyap Mikrofon ile yakalayan, yazılımsal debounce filtresiyle çift tetiklemeyi önleyerek Deneyap Röle üzerinden aydınlatma yükünü kontrol eden ve anlık durumunu 5x7 kırmızı LED matris üzerinde dinamik piksellerle gösteren gömülü sistem projesi.

---

<a id="icindekiler"></a>
## 📑 İçindekiler

1. [Proje Özeti ve Çalışma Prensibi](#proje-ozeti)
2. [Donanım Mimarisi ve I2C Adresleme](#donanim-ve-baglantilar)
3. [Yazılım Mimarisi ve Algoritma Akışı](#yazilim-mimarisi)
4. [5x7 LED Matris Piksel Desenleri (Bitmap Haritası)](#piksel-haritasi)
5. [Proje Dosya Yapısı](#dosya-yapisi)
6. [Kurulum ve Çalıştırma Rehberi](#kurulum-ve-calistirma)
7. [Sorun Giderme (Troubleshooting)](#sorun-giderme)
8. [Geliştirici](#gelistirici)
9. [Lisans](#lisans)

---

<a id="proje-ozeti"></a>
## 🎙️ 1. Proje Özeti ve Çalışma Prensibi

**Sesle Yanan Lamba**, geleneksel mekanik duvar anahtarlarına ihtiyaç duymadan, fiziksel temas gerektirmeyen bir akıllı oda ve ev aydınlatma konseptidir:

- **Akustik Tepe Algılama (Peak Detection):** Deneyap Mikrofon modülü I2C üzerinden analog ses genliğini gerçek zamanlı örnekler.
- **Yazılımsal Filtreleme & Debounce:** Ani bir ses (örneğin el çırpma / alkış) algılandığında, ortamdaki yankılanma veya gürültü sebebiyle lambanın anında kapanıp tekrar açılmasını engellemek için **800 ms'lik `millis()` tabanlı debounce kilidi** uygulanır.
- **Durum Değişimi (Toggle Mantığı):** Her geçerli akustik tetiklemede sistem `lambaDurumu = !lambaDurumu` işlemi ile lambayı açık ise kapalıya, kapalı ise açığa geçirir.
- **Güvenli Röle Sürüşü:** I2C tabanlı Deneyap Röle modülü kullanılarak 220V AC şebeke gerilimi veya 12V/24V DC lamba hatları mikrodenetleyiciden galvanik olarak izole biçimde kontrol edilir.
- **Görsel Durum Geri Bildirimi:** Sistem hazır olduğunda, lamba açıldığında ve kapandığında ortadaki 5x7 kırmızı LED matris ekranda özel grafik desenler çizdirilir.

---

<a id="donanim-ve-baglantilar"></a>
## 🔌 2. Donanım Mimarisi ve I2C Adresleme

Projedeki tüm çevresel birimler (sensör, ekran ve röle) **I2C haberleşme protokolü** üzerinden tek bir SDA/SCL hattına zincirleme (daisy-chain) bağlanmıştır. Bu sayede karmaşık kablolama ortadan kalkmıştır.

| Donanım Bileşeni | Görev / İşlev | İletişim Arayüzü | I2C Adresi | Çalışma Gerilimi |
| :--- | :--- | :---: | :---: | :---: |
| **Deneyap Kart** | ESP32 tabanlı ana mikrodenetleyici | Master (I2C) | — | 3.3V / 5V |
| **Deneyap Mikrofon Modülü** | Ortam ses genliğini okuyan analog sensör | I2C Slave | `0x35` | 3.3V |
| **5x7 Kırmızı LED Matris** | Lamba durumunu (Hazır/Açık/Kapalı) gösteren ekran | I2C Slave | `0x0A` | 3.3V |
| **Deneyap Röle Modülü** | Lambanın elektrik devresini açıp kapatan anahtar | I2C Slave | `0x0C` | 3.3V / 5V |
| **Lamba / Aydınlatma** | Kontrol edilen yük (AC veya DC lamba) | Röle COM - NO Uçları | — | 220V AC / 12V DC |

> 💡 **Mühendislik Notu:** Tek bir I2C veri yolu üzerinde `0x35` (Mikrofon), `0x0A` (LED Matris) ve `0x0C` (Röle) adresleri çakışmasız olarak doğrudan paralel çalışır. GPIO bacakları meşgul edilmez.

---

<a id="yazilim-mimarisi"></a>
## 🧠 3. Yazılım Mimarisi ve Algoritma Akışı (`sesle_yanan_lamba.ino`)

### Akış Şeması Mantığı
```text
           [ Mikrofon Dinle (0x35) ]
                       │
            sesDegeri > SES_ESIGI (400) ?
                 ├── HAYIR ──> Döngüye devam et (100 ms)
                 └── EVET
                       │
     (millis() - sonTetiklemeZamani) > 800 ms ?
                 ├── HAYIR ──> Gürültü/Yankı sayılır (Yoksay)
                 └── EVET
                       │
           ┌───────────┴───────────┐
           │  lambaDurumu Tersle   │
           │  role.RelayDrive(...) │
           └───────────┬───────────┘
                       │
           ┌───────────┴───────────┐
           │    Lamba Durumu?      │
           ├── AÇIK  ──> ACILDI_BMP (0x0A)
           └── KAPALI──> KAPANDI_BMP (0x0A)
```

### Kritik Kod Blokları

#### 1. Hassasiyet Eşiği ve Debounce Filtresi
```cpp
const int SES_ESIGI = 400;                 // Tetikleyici ses genlik seviyesi
const unsigned long DEBOUNCE_SURESI = 800; // İki komut arasındaki minimum süre (ms)

void sesAlgila() {
  uint16_t sesDegeri = mic.ReadMicrophoneAnalog();
  Serial.print("Ses: ");
  Serial.println(sesDegeri);

  if (sesDegeri > SES_ESIGI && (millis() - sonTetiklemeZamani) > DEBOUNCE_SURESI) {
    lambaDegistir();
    sonTetiklemeZamani = millis();
  }
}
```

#### 2. Röle ve LED Matris Eş Zamanlı Kontrolü
```cpp
void lambaDegistir() {
  lambaDurumu = !lambaDurumu;
  role.RelayDrive(lambaDurumu); // 1 = Röle Çekili (Açık), 0 = Röle Bırakıldı (Kapalı)
  ledMatrix.resetDotRows();

  if (lambaDurumu) {
    Serial.println("Lamba AÇILDI");
    ledMatrix.drawLedMatrix(ACILDI_BMP, 500);
  } else {
    Serial.println("Lamba KAPANDI");
    ledMatrix.drawLedMatrix(KAPANDI_BMP, 500);
  }
}
```

---

<a id="piksel-haritasi"></a>
## 🎨 4. 5x7 LED Matris Piksel Desenleri (Bitmap Haritası)

Sistem durumları 35 elemanlı (`5x7`) tek boyutlu ikili dizi formatında tanımlanmıştır:

| Durum | Dizi Adı | Temsili Piksel Çizimi | Açıklama |
| :--- | :--- | :--- | :--- |
| **Sistem Hazır** | `HAZIR_BMP[35]` | `H` Harfi Motifi | Başlatma tamamlandığında 500 ms gösterilir |
| **Lamba Açık** | `ACILDI_BMP[35]` | Lamba Işık Saçma Motifi | Röle tetiklenip ışık yandığında ekranda belirir |
| **Lamba Kapalı** | `KAPANDI_BMP[35]` | Çizgili / Kapalı Motifi | Röle bırakılıp ışık söndüğünde ekranda belirir |

---

<a id="dosya-yapisi"></a>
## 📁 5. Proje Dosya Yapısı

```text
sesle_yanan_lamba/
├── sesle_yanan_lamba/             # Arduino kaynak kod klasörü
│   └── sesle_yanan_lamba.ino      # Mikrofon, LED Matris ve Röleyi yöneten ana kaynak kod
├── .gitattributes                # Git yapılandırma dosyası
└── README.md                     # Proje teknik dokümantasyonu
```

---

<a id="kurulum-ve-calistirma"></a>
## 🚀 6. Kurulum ve Çalıştırma Rehberi

### Adım 1: Gerekli Kütüphanelerin Yüklenmesi
Arduino IDE üzerinde **Araçlar ➔ Kütüphaneleri Yönet** (Ctrl+Shift+I) menüsünü açın ve aşağıdaki kütüphaneleri aratarak kurun:
- `Deneyap_Mikrofon`
- `Deneyap_5x7LedEkran`
- `Deneyap_Role`

### Adım 2: Devre Bağlantısı
1. Deneyap Kart'ın I2C konnektöründen çıkan kabloyu sırasıyla **Mikrofon**, **5x7 LED Matris** ve **Röle** modüllerine zincirleme bağlayın.
2. Lambanın faz (L) kablosunu keserek rölenin **COM** ve **NO (Normally Open)** klemenslerine bağlayın.
3. Nötr (N) hattını doğrudan lambaya iletin.

> ⚠️ **Yüksek Gerilim Güvenlik Uyarısı:** 220V AC şebeke elektriğiyle çalışırken elektrik fişinin prize takılı olmadığından emin olun. Gerekli yalıtım önlemlerini alınız.

### Adım 3: Kodu Yükleme ve Kalibrasyon
1. `sesle_yanan_lamba/sesle_yanan_lamba.ino` dosyasını Arduino IDE ile açın.
2. **Araçlar ➔ Kart ➔ Deneyap Kart** modelinizi ve doğru portu seçin.
3. Kodu karta yükleyin.
4. **Seri Port Ekranı'nı (115200 baud)** açın. Ortam sessizken gelen ses değerini gözlemleyin.
5. Eğer ortam çok gürültülü veya fazla sessiz ise `const int SES_ESIGI = 400;` değerini kendi ortamınıza göre artırıp azaltabilirsiniz.

---

<a id="sorun-giderme"></a>
## 🔧 7. Sorun Giderme (Troubleshooting)

| Karşılaşılan Sorun | Olası Neden | Çözüm Adımı |
| :--- | :--- | :--- |
| `Mikrofon baslatilamadi!` hatası | I2C bağlantı temassızlığı | I2C kablolarını kontrol edin; mikrofon I2C adresinin `0x35` olduğunu teyit edin. |
| `Role baslatilamadi!` hatası | Röle modülüne güç gitmemesi | Röle adresinin `0x0C` olduğunu ve Deneyap I2C hattının takılı olduğunu doğrulayın. |
| Lamba istemsizce kendi kendine yanıp sönüyor | `SES_ESIGI` değeri çok düşük | Eşik değerini `500` veya `600` seviyesine yükseltin. |
| Alkış yapıldığında tepki vermiyor | `SES_ESIGI` değeri çok yüksek | Eşik değerini `300` veya `350` seviyesine düşürün. |
| Tek alkışta lamba iki kez açılıp kapanıyor | Yankı / kısa debounce süresi | `DEBOUNCE_SURESI` değişkenini `1000` (1 saniye) olarak güncelleyin. |

---

<a id="gelistirici"></a>
## 👩‍💻 8. Geliştirici

**Kevser ÜSTÜN**  
🎓 Giresun Üniversitesi — Bilgisayar Mühendisliği  
🔗 [GitHub Profilim](https://github.com/kvsrustun)

---

<a id="lisans"></a>
## 📄 9. Lisans

Bu proje [MIT Lisansı](LICENSE) kapsamında açık kaynak olarak yayımlanmıştır.
