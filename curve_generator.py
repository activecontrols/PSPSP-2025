# Edit this code as needed to generate custom curve files

import math

def sin_wave(num_pts):
	# generates
	pts = []
	for i in range(0, num_pts):
		x = i / num_pts * math.pi
		pts.append(math.sin(x) ** 2)
	return pts

def generate_curve(time_duration, pts):
	with open('curve.csv', 'w+') as f:
		f.write("time (s),x,y,z\n")

		for index, pt in enumerate(pts):
			t = index / len(pts) * time_duration
			f.write(f"{t},{pt},{pt},{pt}\n")

def main():
	pts = []
	# for i in range(0, 10):
	# 	pts.extend(sin_wave(100))
	for i in range(0, 10):
		pts.extend(sin_wave(120 - i * 10))
	generate_curve(10, pts)
	
main()