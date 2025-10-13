from PIL import Image
import glob
import os

# プレフィックスを指定
PREFIX = "mentako"

# 画像サイズ
WIDTH = 50
HEIGHT = 26
PALETTE = [0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff]  # LVGL 1bitパレット

# 指定プレフィックスのbmpファイルを全て取得
bmp_files = sorted(glob.glob(f"{PREFIX}_*.bmp"))

for bmp_file in bmp_files:
    img = Image.open(bmp_file).convert("1")
    w, h = img.size
    if w != WIDTH or h != HEIGHT:
        print(f"サイズ不一致: {bmp_file} ({w}x{h})")
        continue

    BYTES_PER_ROW = (WIDTH + 7) // 8  # 50→7バイト
    buf = [0] * (BYTES_PER_ROW * HEIGHT)

    for y in range(HEIGHT):
        for x in range(WIDTH):
            pixel = img.getpixel((x, y))
            if pixel == 0:  # 黒ドット
                byte_index = y * BYTES_PER_ROW + (x // 8)
                bit_index = 7 - (x % 8)
                buf[byte_index] |= (1 << bit_index)

    # ファイル名から拡張子を除いた名前を取得
    var_name = os.path.splitext(os.path.basename(bmp_file))[0]
    c_filename = f"{var_name}.c"
    with open(c_filename, "w") as f:
        f.write(f"{var_name}_map[] = {{\n")
        # パレット出力
        for i, b in enumerate(PALETTE):
            f.write(f"  0x{b:02x},")
            if (i+1) % 4 == 0:
                f.write("\t/*Color of index %d*/\n" % ((i//4)))
        # ビットマップ出力（7バイトごとに改行）
        for i, b in enumerate(buf):
            if i % 7 == 0:
                f.write("\n  ")
            f.write(f"0x{b:02x}, ")
        f.write("\n};\n")
    print(f"{c_filename} を保存しました")