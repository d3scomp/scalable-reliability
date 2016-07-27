import os
import sys
import matplotlib

matplotlib.use('Agg')
matplotlib.rcParams['agg.path.chunksize'] = 10000
import matplotlib.pyplot as plot
from matplotlib import pyplot, colors
import numpy as np


def processLogFile(file: str):
	print("Loading " + file)
	f = open(file, 'r')

	lattencies = []
	distancies = []
	boxLattencies = {}
	data = []

	while True:

		# Read reported name
		name = f.readline()

		if name == '':
			break

		while True:
			line = f.readline()
			if len(line.rstrip()) == 0:
				break

			# Process line
			vals = line.split("\t")
			latt = float(vals[0])
			gtdist = float(vals[1])
			dist = float(vals[2])

			lattencies.append(latt)
			distancies.append(gtdist)

			dist_m = int(gtdist)
			if dist_m not in boxLattencies:
				boxLattencies[dist_m] = []

			boxLattencies[dist_m].append(latt)

	print("Processing " + file)

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


# fig.savefig(file + ".pdf")
# fig.savefig(file + ".ps")
# fig.savefig(file + ".svg")


for file in os.listdir("../logs"):
	if file.endswith(".txt") and file.startswith("delays"):
		processLogFile("../logs/" + file)
