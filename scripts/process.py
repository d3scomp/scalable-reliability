import os
import sys
import math
import matplotlib
matplotlib.use('Agg')
matplotlib.rcParams['agg.path.chunksize'] = 10000
import matplotlib.pyplot as plot


def get_speed(name, colliders):
	if name.startswith("humans"):
		my_max_speed = 1
	else:
		my_max_speed = 5

	period = 0.030
	deceleration = 5

	resulting_speed = my_max_speed

	for collider in colliders:
		dist = collider['dist']
		rem_dist = dist - collider['latt'] * collider['speed']
		rem_dist = max(0, rem_dist)
		collide_speed = math.sqrt(2 * rem_dist * deceleration)
		my_speed = max(0, collide_speed - collider['speed'])
		resulting_speed = min(resulting_speed, my_speed)

	return resulting_speed


def print_delays(file, box_lattencies):
	# Delays
	fig = plot.figure()
	ax = fig.add_subplot(111)

	ax.set_xlabel("Distance in meters")
	ax.set_ylabel("Lattency in seconds")

	# ax.plot(distancies, lattencies, 'bo')

	data = []
	ticks = []
	maxdist = max(box_lattencies.keys())

	for key in range(0, maxdist):
		if key in box_lattencies:
			data.append(box_lattencies[key])
		else:
			data.append([])
		ticks.append(key)

	ax.boxplot(data, positions=range(0, len(data)), patch_artist=True, manage_xticks=False)

	plot.xlim(-1, maxdist)

	print("Storing png")
	fig.savefig(file + ".png", dpi=256, width=20, wight=15)

	print("Storing pdf")
	fig.savefig(file + ".pdf")


def print_speeds(file, speeds):
	# Speeds hist
	fig = plot.figure()
	ax = fig.add_subplot(111)
	ax.set_xlabel("Robot speed in meters per second")
	ax.set_ylabel("Samples")
	ax.hist(speeds, 100)
	print("Storing png")
	fig.savefig(file + ".speedhist.png", dpi=256, width=20, wight=15)
	print("Storing pdf")
	fig.savefig(file + ".speedhist.pdf")


def process_log_file(file: str):
	print("Loading " + file)
	f = open(file, 'r')

	lattencies = []
	distancies = []
	speeds = []
	boxLattencies = {}
	data = []

	while True:
		# Read reported name
		name = f.readline()

		if name == '':
			break

		colliders = []

		while True:
			line = f.readline()
			if len(line.rstrip()) == 0:
				break

			# Process line
			vals = line.split("\t")
			latt = float(vals[0])
			gtdist = float(vals[1])
			dist = float(vals[2])
			maxspeed = float(vals[3])

			lattencies.append(latt)
			distancies.append(gtdist)

			colliders.append({"dist": dist, "speed": maxspeed, "latt": latt})

			dist_m = int(gtdist)
			if dist_m not in boxLattencies:
				boxLattencies[dist_m] = []

			boxLattencies[dist_m].append(latt)

		if name.startswith("robot"):
			speeds.append(get_speed(name, colliders))

	print("Processing " + file)

	print_delays(file, boxLattencies)

	print_speeds(file, speeds)


def process():
	for file in os.listdir("../logs"):
		if file.endswith(".txt") and file.startswith("delays"):
			process_log_file("../logs/" + file)

process()