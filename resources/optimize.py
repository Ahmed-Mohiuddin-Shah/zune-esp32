import os
from PIL import Image

def convert_to_rgb565(image):
    """Convert an image to RGB565 format."""
    print("Converting image to RGB565 format...")
    image = image.convert('RGB')
    pixels = image.load()
    width, height = image.size
    rgb565_data = []

    for y in range(height):
        for x in range(width):
            r, g, b = pixels[x, y]
            r = (r >> 3) & 0x1F
            g = (g >> 2) & 0x3F
            b = (b >> 3) & 0x1F
            rgb565 = (r << 11) | (g << 5) | b
            rgb565_data.append(rgb565)

    print("Conversion to RGB565 format completed.")
    return width, height, rgb565_data

def save_as_zyntex(output_path, width, height, rgb565_data):
    """Save the image data in .zyntex format."""
    with open(output_path, 'w') as file:
        file.write(f"{width} {height}\n")
        for pixel in rgb565_data:
            file.write(f"{pixel}\n")
    print(f"Saved converted image to: {output_path}")

def process_images(input_folder, output_folder):
    """Process images from input_folder and save them to output_folder in .zyntex format."""
    print(f"Processing images from {input_folder} to {output_folder}...")
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
        print(f"Created output folder: {output_folder}")

    for filename in os.listdir(input_folder):
        if filename.lower().endswith(('.jpg', '.jpeg', '.png')):
            image_path = os.path.join(input_folder, filename)
            print(f"Processing file: {image_path}")
            image = Image.open(image_path)
            width, height, rgb565_data = convert_to_rgb565(image)
            output_path = os.path.join(output_folder, os.path.splitext(filename)[0] + '.zyntex')
            save_as_zyntex(output_path, width, height, rgb565_data)

if __name__ == "__main__":
    input_folder = './original_assets/3d_models'
    output_folder = './optimized_assets/3d_models'
    process_images(input_folder, output_folder)