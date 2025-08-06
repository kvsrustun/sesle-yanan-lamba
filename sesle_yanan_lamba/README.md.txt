# Ses Kontrollü Lamba ve Görsel Geri Bildirim Sistemi

Bu proje, Deneyap modülleri kullanarak sesle kontrol edilen bir lamba sistemi sunar. Mikrofon ile ortamdan gelen ses seviyesi ölçülür ve belirlenen eşik değerinin üzerinde bir ses algılandığında, bağlı röle tetiklenerek lambanın açılıp kapanması sağlanır. Aynı zamanda LED matris üzerinde lamba durumuna göre farklı görseller gösterilir.

---

## İçindekiler

- `ses_kontrol_lamba.ino` - Ana Arduino kodu
- Kullanılan Deneyap kütüphaneleri:
  - `Deneyap_Mikrofon`
  - `Deneyap_5x7LedEkran`
  - `Deneyap_Role`

---

## Kullanılan Donanımlar

- **Deneyap Mikrofon Modülü** (I2C adres: 0x35)
- **Deneyap 5x7 LED Matris** (I2C adres: 0x0A)
- **Deneyap Röle Modülü** (I2C adres: 0x0C)
- Arduino Deneyap Kart veya uyumlu Arduino modeli

---

## Çalışma Prensibi

- Mikrofon modülü ortam sesini analog olarak ölçer.
- Ölçülen ses seviyesi `SES_ESIGI` eşik değerinin üzerindeyse ve debounce süresi dolmuşsa,
  - Röle tetiklenerek lambanın açma/kapama durumu değiştirilir.
  - LED matris üzerinde lamba durumu görsel olarak gösterilir:
    - Lamba açıkken “AÇILDI” görseli
    - Lamba kapalıyken “KAPANDI” görseli
- Sistem ses tetiklemeleri arasındaki gereksiz tekrarları önlemek için debounce süresi uygular.

---

## Pin ve Adres Bilgileri

| Donanım           | I2C Adresi / Pin   |
|-------------------|--------------------|
| Mikrofon Modülü   | I2C 0x35           |
| 5x7 LED Matris    | I2C 0x0A           |
| Röle Modülü       | I2C 0x0C           |

---

## Kurulum ve Kullanım

1. Modülleri belirtilen I2C adreslerine ve pinlere doğru şekilde bağlayın.
2. Arduino IDE'de gerekli Deneyap kütüphanelerini yükleyin.
3. `ses_kontrol_lamba.ino` dosyasını açın.
4. Kodda eşik değer (`SES_ESIGI`) ve debounce süresi (`DEBOUNCE_SURESI`) ihtiyacınıza göre ayarlayın.
5. Kodu Arduino kartınıza yükleyin.
6. Seri monitörden ses değerlerini izleyerek sistemin çalışmasını takip edin.

---

## Sorun Giderme

- **Modüller başlatılamıyor:** I2C adreslerini ve bağlantıları kontrol edin.
- **Lamba açılıp kapanmıyor:** Röle bağlantısını ve güç kaynağını kontrol edin.
- **LED matris görsel göstermiyor:** Kütüphane ve I2C adres uyumluluğunu kontrol edin.
- **Ses algılamada sorun:** Mikrofon hassasiyetini ve eşik değerini ayarlayın.

---

## 👩‍💻 Geliştirici

**Kevser ÜSTÜN**  
Giresun Üniversitesi – Bilgisayar Mühendisliği  
