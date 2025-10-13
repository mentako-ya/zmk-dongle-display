# GLCD/LVGL 画像・フォント変換ツール集

`boards/shields/dongle_display/widgets/bongo_cat_images.c`  
のLVGL画像データ（bongo_cat_xxx_map[]配列）を**作成・確認・編集**するためのツール。

GLCDフォントやLVGL画像データの変換・可視化・編集用Pythonスクリプトが含まれています。


---

## 必要なもの

- Python 3.x
- Pillow ライブラリ  
  インストール:  
  ```
  pip install pillow
  ```

---

## 各ツールの使い方

### 1. lvgl2image.py

**LVGL形式の画像データ（C配列）を複数まとめてビットマップ画像に変換します。**

- `data_list` に複数のLVGL画像データ配列を指定してください（先頭8バイトはパレット）。
- 画像は `PREFIX_インデックス.bmp` というファイル名で保存されます。

**例:**
```python
data_list = [
    [0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, ...],  # 1枚目
    [0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, ...],  # 2枚目
]
```
- 実行方法:
  ```
  python lvgl2image.py
  ```

---

### 2. glcd2image.py

**glcdfont.cのフォントデータ（6バイト/文字）をビットマップ画像に変換します。**

- `font` 配列にglcdfont.cのデータをコピペしてください。
- 1文字6バイト、縦8ドット、横32文字ごとに改行で並べて画像化します。
- 出力ファイル: `glcd_font.bmp`

- 実行方法:
  ```
  python glcd2image.py
  ```

---

### 3. image2pac.py

**ビットマップ画像（bmp）をLVGL用C配列データに変換します。**

- ファイル名が `PREFIX_*.bmp` の画像をすべて処理します。
- 画像サイズは50x26ピクセル、1bitモノクロを想定しています。
- 変換結果は `PREFIX_インデックス.c` 形式のC配列ファイルとして保存されます（パレット＋ビットマップ）。

- 実行方法:
  ```
  python image2pac.py
  ```

---

## 注意事項

- 画像サイズやデータ形式はスクリプト内の定数（WIDTH, HEIGHT, PALETTE_SIZEなど）で調整してください。
- LVGL画像データは「先頭8バイトがパレット、その後に1行7バイトパディング付きビットマップ」が標準です。
- フォントデータや画像データの貼り付けミスにご注意ください。

---
