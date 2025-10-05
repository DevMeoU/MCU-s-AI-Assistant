from PIL import Image
import sys

def image_to_c_array(image_path, var_name="emoji"):
    img = Image.open(image_path).convert('1')  # 1-bit monochrome
    width, height = img.size
    pixels = img.load()

    byte_array = []
    for y in range(0, height):
        byte_val = 0
        bit_count = 0
        for x in range(0, width):
            pixel = 0 if pixels[x, y] == 255 else 1  # trắng=0, đen=1
            byte_val |= (pixel << bit_count)
            bit_count += 1
            if bit_count == 8:
                byte_array.append(byte_val)
                byte_val = 0
                bit_count = 0
        if bit_count > 0:
            byte_array.append(byte_val)

    # Sinh code C
    header = []
    header.append(f"#define {var_name.upper()}_WIDTH {width}")
    header.append(f"#define {var_name.upper()}_HEIGHT {height}")
    header.append(f"const unsigned char {var_name}[] PROGMEM = {{")

    for i, val in enumerate(byte_array):
        if i % 12 == 0:
            header.append("\n    ")
        header[-1] += f"0x{val:02X}, "

    header.append("\n};")
    return "\n".join(header)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python gen_emoji.py input.png output.h")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    c_code = image_to_c_array(input_file, "emoji")
    with open(output_file, "w") as f:
        f.write(c_code)

    print(f"Generated {output_file}")
