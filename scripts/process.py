import os
import sys
import math
import matplotlib
matplotlib.use('Agg')
matplotlib.rcParams['agg.path.chunksize'] = 10000
import matplotlib.pyplot as plot
from matplotlib.ticker import FuncFormatter


def get_speed(name, colliders):
	if name.startswith("humans"):
		my_max_speed = 1
	else:
		my_max_speed = 5

	period = 0.030
	deceleration = 10

	resulting_speed = my_max_speed

	for collider in colliders:
		dist = collider['dist']
		rem_dist = dist - collider['latt'] * collider['speed']
		rem_dist = max(0, rem_dist)
		collide_speed = math.sqrt(2 * rem_dist * deceleration)
		my_speed = max(0, collide_speed - collider['speed'])
		resulting_speed = min(resulting_speed, my_speed)

	return resulting_speed


def print_delays(name, box_lattencies):
	deadline = 0.072

	# Delays
	fig = plot.figure()
	ax = fig.add_subplot(111)

	ax.set_xlabel("Distance (meters)")
	ax.set_ylabel("Lattency (seconds)")

	# ax.plot(distancies, lattencies, 'bo')

	data = []
	ticks = []
	maxdist = 25# max(box_lattencies.keys())

	for key in range(0, maxdist):
		if key in box_lattencies:
			data.append(box_lattencies[key])
		else:
			data.append([])
		ticks.append(key)

	hline = ax.axhline(y = deadline)
	hline.set_color("red")

	ax.boxplot(data, positions=range(0, len(data)), patch_artist=True, manage_xticks=False)

	plot.xlim(-1, maxdist)

	print("Storing png")
	fig.savefig(name + "_lattency.png", dpi=256, width=20, wight=15)

	print("Storing pdf")
	fig.savefig(name + "_lattency.pdf")


def print_speeds(name, speeds):
	# Speeds hist
	fig = plot.figure()
	ax = fig.add_subplot(111)
	ax.set_xlabel("Robot speed (meters per second)")
	ax.set_ylabel("Relative frequency")

	ax.hist(speeds, 10, normed=False, weights=[1 / len(speeds) for s in speeds])
	ax.set_ylim([0, 1])

	ax.yaxis.set_major_formatter(FuncFormatter(lambda y, pos: str(int(y * 100)) + "%"))

	print("Storing png")
	fig.savefig(name + "_speedhist.png", dpi=256, width=20, wight=15)
	print("Storing pdf")
	fig.savefig(name + "_speedhist.pdf")


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

	name = file.replace(".txt", "")

	print_delays(name, boxLattencies)

	print_speeds(name, speeds)


def process():
	for file in os.listdir("../logs"):
		if file.endswith(".txt"):
			try:
				process_log_file("../logs/" + file)
			except Exception as e:
				print("Processing failed")
				print(e)

process()