#    USAGE : python3 split-skymap-into-images.py <chose>.<extension>
#supprimer : rm ??.png 

#The author takes no responsibility for any damage this script might cause,
# feel free to use, change and or share this script.
# 2013-07, CanOfColliders, m@canofcolliders.com
#							   __
#							__|+Y|_____
#NOTE : suit la convention |-X|+Z|+X|-Z|
#							  |-Y|

from PIL import Image
import sys, os

path = os.path.abspath("") + "/"
processed = False
	

def processImage(path, name):
	img = Image.open(os.path.join(path, name))
	size = img.size[0] / 4 # on split le width par 4, en se fiant sur la layout du cube minecraft

	arr = []
	if extension == ".png":
		arr = ["px", "nz", "nx", "ny", "py", "pz"]
	elif extension == ".jpg":
		arr = ["posx", "negz", "negx", "negy", "posy", "posz"]

	#on commence DANS LE VIDE, en haut à gauche, à 0, 0
	splitAndSave(img, size*2, size,   size, addToFilename(name, arr[0])) #px OK
	splitAndSave(img, size*3, size,   size, addToFilename(name, arr[1])) #nz OK
	splitAndSave(img, 0     , size,   size, addToFilename(name, arr[2])) #nx OK
	splitAndSave(img, size  , size*2, size, addToFilename(name, arr[3])) #ny OK
	splitAndSave(img, size  , 0   ,   size, addToFilename(name, arr[4])) #py OK
	splitAndSave(img, size  , size,   size, addToFilename(name, arr[5])) #pz OK

def addToFilename(name, add):
	name = name.split('.')
	return add + "." + name[1]

def splitAndSave(img, startX, startY, size, name):
	area = (startX, startY, startX + size, startY + size)

	croppedImg = img.crop(area)

	#faire la rotation des images py et py et nz 
	#if (name == "px.png" or name == "posx.jpg"):
	#	croppedImg = croppedImg.transpose(Image.ROTATE_180) 
		
	saveImage(croppedImg, path, name)

def saveImage(img, path, name):
	try:
		img.save(os.path.join(path, name))
	except:
		print("*   ERROR: Could not convert image.")
		pass

extension = ""

for arg in sys.argv:
	if ".png" in arg:
		extension = ".png"
		processImage(path, arg)
		processed = True
		
	elif ".jpg" in arg:
		extension = ".jpg"
		processImage(path, arg)
		processed = True

if not processed:
	print("*  ERROR: No Image")
	print("   usage: 'python3 split-skymap-into-images.py <chose>.<extension>'")