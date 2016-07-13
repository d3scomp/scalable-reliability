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
    
    for line in f:
        vals = line.split("\t")
        lattencies.append(vals[0])
        distancies.append(vals[1])

    print("Processing " + file)
    #print(data)

    fig = plot.figure()
    ax = fig.add_subplot(111)
    
    ax.set_xlabel("Distance in meters")
    ax.set_ylabel("Lattency in seconds")

    ax.plot(distancies, lattencies, 'bo')

    print("Storing png")
    fig.savefig(file + ".png", dpi=256, width=20, wight=15)

    #print("Storing eps")
    #fig.savefig(file + ".eps")
    
    #fig.savefig(file + ".pdf")
    #fig.savefig(file + ".ps")
    #fig.savefig(file + ".svg")
        
        
for file in os.listdir("../logs"):
    if file.endswith(".txt"):
        processLogFile("../logs/" + file)


