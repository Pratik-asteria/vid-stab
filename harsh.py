from PIL import Image, ImageDraw
from math import sin, cos, pi
# Load image:
input_image = Image.open("input.png")
input_pixels = input_image.load()
oputput_image= Image.new("RGB", input_image.size)
draw= ImageDraw.Draw(output_image)
angle = pi /3 # angle in rad
center_x = input_image.width /2
center_y = input_image.height /2
for x in range(input_image.width):
	for y in range(input_image.height);
	xp = int