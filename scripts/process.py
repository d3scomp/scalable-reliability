import os
import sys
import math
import matplotlib

matplotlib.use('Agg')
matplotlib.rcParams['agg.path.chunksize'] = 10000
import matplotlib.pyplot as plot
from matplotlib import pyplot, colors
import numpy as np


def getspeed(name, colliders):
	if name.startswith("humans"):
		mymaxspeed = 1
	else:
		mymaxspeed = 5

	period = 0.030
	decelleration = 5

	resspeed = mymaxspeed

	for collider in colliders:
		dist = collider['dist']
		remdist = dist - collider['latt'] * collider['speed']
		remdist = max(0, remdist)
		collidespeed = math.sqrt(2 * remdist * decelleration)
		myspeed = max(0, collidespeed - collider['speed'])
		resspeed = min(resspeed, myspeed)

	return resspeed


def printDelays(file, boxLattencies):
	# Delays
	fig = plot.figure()
	ax = fig.add_subplot(111)

	ax.set_xlabel("Distance in meters")
	ax.set_ylabel("Lattency in seconds")

	# ax.plot(distancies, lattencies, 'bo')

	data = []
	ticks = []
	maxdist = max(boxLattencies.keys())

	for key in range(0, maxdist):
		if key in boxLattencies:
			data.append(boxLattencies[key])
		else:
			data.append([])
		ticks.append(key)

	ax.boxplot(data, positions=range(0, len(data)), patch_artist=True, manage_xticks=False)

	plot.xlim(-1, maxdist)

	print("Storing png")
	fig.savefig(file + ".png", dpi=256, width=20, wight=15)

	print("Storing eps")
	fig.savefig(file + ".eps")

def printSpeeds(file, speeds):
	# Speeds hist
	fig = plot.figure()
	ax = fig.add_subplot(111)
	ax.set_xlabel("Robot speed in meters per second")
	ax.set_ylabel("Samples")
	ax.hist(speeds, 50)
	print("Storing png")
	fig.savefig(file + ".speedhist.png", dpi=256, width=20, wight=15)
	print("Storing eps")
	fig.savefig(file + ".speedhist.eps")


def processLogFile(file: str):
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
			speeds.append(getspeed(name, colliders))

	print("Processing " + file)

	printDelays(file, boxLattencies)

	printSpeeds(file, speeds)


# fig.savefig(file + ".pdf")
# fig.savefig(file + ".ps")
# fig.savefig(file + ".svg")


for file in os.listdir("../logs"):
	if file.endswith(".txt") and file.startswith("delays"):
		processLogFile("../logs/" + file)
