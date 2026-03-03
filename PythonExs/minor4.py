#	Author: Giovanni Rebosio gdr0051 10849933
#	Description: Asks for rectangle lengths and prints a rectangle
#	Date: 4/28/2025

def rect(x, y):
	xMax = x
	yMax = y
	for x in range(0, xMax):
		for y in range(0, yMax):
			if (x == 0 or x == xMax - 1) and y == 0:
				print("+", end="")
			elif (x == 0 or x == xMax - 1) and y == yMax - 1:
				print("+")
			elif (x == 0 or x == xMax - 1) and (y != 0 or y != yMax - 1):
				print("-", end="")
			elif (x != 0 or x != xMax - 1) and y == 0:
				print("|", end="")
			elif (x != 0 or x != xMax - 1) and y == yMax - 1:
				print("|")
			else:
				print(" ", end="")

while 1:
	a = int(input("Enter height of rectangle: "))
	if a < 2:
		print("Height must be > 1")
		exit()
	while 1:
		b = int(input("Enter width of rectangle: "))
		if b < 2:
			print("Width must be > 1")
			exit()
		rect(a, b)
		break
	break
