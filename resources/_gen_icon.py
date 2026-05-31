from PIL import Image, ImageDraw

BG   = (0, 0, 0, 255)          # black background (full rectangle)
FACE = (255, 255, 255, 255)    # white letter face
EDGE = (105, 105, 105, 255)    # gray 3d extruded side

N = 32
img = Image.new("RGBA", (N, N), BG)
dr = ImageDraw.Draw(img)

def rect(x0, y0, x1, y1, c, ox=0, oy=0):
    dr.rectangle([x0+ox, y0+oy, x1+ox, y1+oy], fill=c)

def glyphs(col, ox=0, oy=0):
    # lowercase d : tall right stem + lower-left bowl
    rect(12, 1, 15, 27, col, ox, oy)
    rect(2, 11, 15, 14, col, ox, oy)
    rect(2, 24, 15, 27, col, ox, oy)
    rect(2, 11, 5, 27, col, ox, oy)
    # lowercase c : open on the right
    rect(19, 11, 28, 14, col, ox, oy)
    rect(19, 11, 22, 27, col, ox, oy)
    rect(19, 24, 28, 27, col, ox, oy)

# layered extrusion (down-right), then white face, then punched counters
for k in (3, 2, 1):
    glyphs(EDGE, k, k)
glyphs(FACE, 0, 0)
rect(6, 15, 11, 23, BG)
rect(23, 15, 31, 23, BG)

# PNG used for the in-app title-bar logo texture (decoded with stb_image)
img.save("resources/dconstruct.png")

# multi-size .ico used for the window / taskbar / exe icon
sizes = [256, 128, 64, 48, 32, 24, 16]
frames = [img.resize((s, s), Image.NEAREST) for s in sizes]
frames[0].save("resources/dconstruct.ico", format="ICO",
               sizes=[(s, s) for s in sizes], append_images=frames[1:])
print("wrote resources/dconstruct.ico + dconstruct.png")
