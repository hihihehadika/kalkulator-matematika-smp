<div align="center">
  <h1>📐 Kalkulator Matematika SMP</h1>
  <p><b>Aplikasi kalkulator pintar & interaktif untuk mempermudah belajar Matematika tingkat SMP.</b></p>
</div>

---

## ✨ Tentang Aplikasi
Aplikasi **Kalkulator Matematika SMP** adalah sebuah *desktop application* yang dirancang secara khusus untuk memecahkan berbagai persoalan matematika. Aplikasi ini dilengkapi dengan **Visualisasi 3D (Wireframe & Shading)** yang mulus dan antarmuka (UI) gaya modern berbasis *carousel/cards*, membuat proses belajar matematika dan penyelesaian tugas menjadi jauh lebih interaktif.

Proyek ini aslinya berupa *prototype* dalam Python (PyQt6) yang kemudian telah ditulis ulang sepenuhnya ke dalam **C++** untuk mendapatkan performa maksimal, rendering 60 FPS, dan efisiensi memori yang jauh lebih baik.

## 🚀 Fitur Utama
Aplikasi ini memiliki 7 modul utama kalkulasi:
1. **📦 Bangun Ruang** - Menghitung Volume, Luas Permukaan, dan pencarian variabel balik (Reverse Solving) dilengkapi dengan **Proyeksi 3D** yang bisa berotasi untuk ke-8 bentuk geometri umum (Kubus, Balok, Tabung, Kerucut, Limas, Prisma, dll).
2. **📈 Persamaan Garis Lurus** - Menghitung gradien, persamaan dari 2 titik, dan menggambar graik garis linier secara otomatis.
3. **🎲 Peluang & Probabilitas** - Visualisasi jumlah dadu, koin bersisi, dan ruang sampel untuk menghitung peluang kejadian matematika.
4. **➗ Sistem Persamaan Linear (SPLDV)** - Mencari nilai variabel (x, y) menggunakan perhitungan metode Eliminasi.
5. **📐 Teorema Pythagoras** - Mencari panjang sisi miring (hipotenusa), alas, maupun tinggi segitiga siku-siku.
6. **📊 Statistika Dasar** - Menganalisis sekumpulan data untuk menemukan nilai Rata-rata (Mean), Nilai Tengah (Median), dan Modus.
7. **🧮 Kalkulator Basic** - Fitur kalkulator standar untuk kebutuhan aritmatika harian (pecahan, desimal).

## 🛠️ Teknologi yang Digunakan
- **Bahasa Pemrograman**: C++ (C++17)
- **Framework GUI**: Qt6 (C++ Qt)
- **Modul Qt**: `Qt::Widgets`, `Qt::Core`, `Qt::Gui`, `Qt::Charts`
- **Sistem Build**: CMake
- **Animasi & Render**: `QPainter`, `QPropertyAnimation`, `QGraphicsScene`

## ⚙️ Build & Kompilasi (Untuk Developer)
Bagi Anda yang ingin meng-compile kode sumber ini:
1. Pastikan Anda telah menginstal **Qt6** dan **CMake** di sistem Anda.
2. Buka file `CMakeLists.txt` menggunakan **Qt Creator**.
3. Pilih *Kit* compiler (disarankan MinGW 64-bit untuk Windows).
4. Klik **Build** (Ctrl+B) atau jalankan perintah CLI:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

## 📥 Download / Instalasi
Aplikasi ini telah di-*bundle* menjadi satu file `.exe` *installer* yang mudah dipasang.
> 👉 Buka tab **[Releases]** di samping kanan halaman ini untuk mengunduh versi installer Inno Setup terbaru.

---
*Dibuat oleh Dika.*
