<h1>Özsoy Asansör IoT Sistemi</h1>
  
  <p>Özsoy Asansör Makina A.Ş. için <strong>Blynk</strong> sistemini kullanarak bir IoT sistemi geliştirdim. Bu sistemin en büyük avantajı, Blynk'in ücretsiz versiyonunda maksimum 2 cihaz ve 10 widget kullanımına izin vermesine rağmen, bunu aşarak daha fazla cihazı kontrol edebilmemdir.</p>
  
  <h2>Sistem Özellikleri</h2>
  <ul>
    <li><strong>ESP32 Cihazı:</strong> Hem Access Point (Erişim Noktası) hem de Station (İstemci) modunda çalışmaktadır.</li>
    <li><strong>Access Point Modu:</strong> Bir Wi-Fi ağı yayınlayarak CNC makinelerinin üzerine bağlanmış ESP8266'ların bu ağa bağlanmasını sağlar.</li>
    <li><strong>Station Modu:</strong> Blynk sunucularına bağlanarak IoT fonksiyonlarını gerçekleştirir.</li>
    <li><strong>ESP8266 Cihazları:</strong> Her biri bir ışık sensörü ile donatılmış olup CNC makinelerinin uyarı lambalarına bağlıdır.</li>
  </ul>
  
  <h2>Sistem İşleyişi</h2>
  <p>ESP32 cihazı, Access Point modu ile bir Wi-Fi ağı oluşturur. CNC makinelerine bağlı ESP8266'lar, bu ağa bağlanır ve Blynk üzerinden kontrol edilir. Blynk'in ücretsiz sürümü sadece 10 widget kullanımına izin verdiği için, 10 ESP8266 cihazı bu ağa bağlanabilir. CNC makineleri bir hata verdiğinde, ışık sensörleri bunu algılar ve Blynk uygulaması aracılığıyla telefonunuza bildirim gönderir.</p>

  <h2>Sonuç</h2>
  <p>Bu sistem, Blynk'in ücretsiz sürümünün kısıtlamalarını aşarak, daha fazla cihazı kontrol etmenizi sağlar ve CNC makinelerindeki hataları anında bildirir. Bu sayede, Özsoy Asansör Makina A.Ş. için verimlilik ve güvenlik artışı sağlanmıştır.</p>
  
  <p>Teşekkürler! Keyifli kullanımlar.</p>
