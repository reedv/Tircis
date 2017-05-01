#!/usr/bin/python

import time
import fnmatch
import re
import os
import subprocess
import distutils.util
import sys

VERSION_PREFIX="tircis_process_cmd"

def getKey():
	import termios, fcntl, sys, os
	fd = sys.stdin.fileno()
	
	oldterm = termios.tcgetattr(fd)
	newattr = termios.tcgetattr(fd)
	newattr[3] = newattr[3] & ~termios.ICANON & ~termios.ECHO
	termios.tcsetattr(fd, termios.TCSANOW, newattr)
	
	oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
	fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)
	
	try:
    		while 1:
        		try:
     		       		c = sys.stdin.read(1)
       		     		return c
        		except IOError: pass
	finally:
    		termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
    		fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
	
def proceed():
	inputchar = getKey()
	if (inputchar == '\n') or (inputchar == 'y') or (inputchar == 'Y'):
		return True
	else:
		return False

def getTerminalWidth():
	rows, columns = os.popen('stty size', 'r').read().split()
	return int(columns)

def printhr():
	print "-"*getTerminalWidth()

def printdhr():
	print "="*getTerminalWidth()

def prettyprint(tabs,string):
	words=string.split()
	width = getTerminalWidth()
	x = 1
	print tabs,
	for word in words:
		if ((x + len(word)) > width-2):
			print ""
			print tabs,
			x = 0
		print word,
		x = x + len(word)+1
	print ""
	return
		

def getAllProjectVersionDirectoryNames(projectname): 
	list = []
	for file in os.listdir('.'):
		if (fnmatch.fnmatch(file, projectname+'_v*') and os.path.isdir(file)):
			list.append(file)
	return list

def getFileList(directory, pattern):
	sourcefiles = subprocess.check_output(["find",directory,"-name",pattern])
	return sourcefiles.split()


def timeVersionWithThreads(version, num_threads):
	# Chdir to the version
        print "cd",version
	os.chdir(version)
	# Do a make clean
        print "make clean"
	if (os.system("make clean 1> /dev/null 2> /dev/null") != 0):
		print version+": Can't 'make clean'... aborting!\n";
	        os.chdir("..")
		return -1.0
	# Set the NUM_THREADS environment variable
	os.environ['NUM_THREADS'] = str(num_threads)
	#os.system("echo $NUM_THREADS")
	# Build the app
        print "make --environment-overrides ..."
	if (os.system("make --environment-overrides 1> /dev/null 2> /dev/null") != 0):
		print version+": Can't 'make'... aborting!\n";
	        os.chdir("..")
		return -1.0


	# Time the app some number of times
	numTrials = 5
	start = time.time()
        os.chdir("../testdata");
	for trial in range(numTrials): 
                print "Running with", num_threads, "threads, trial", trial, "..."
                if (os.system("make run 1> /dev/null 2> /dev/null") != 0):
                    print version+": Can't 'make run'... aborting!\n";
	            os.chdir("..")
                    return -1.0;

	average = 1.0*(time.time() - start) / numTrials
	os.chdir("..")
	return average



###########################################################
###########################################################
###########################################################

# Get all versions
all_versions =  getAllProjectVersionDirectoryNames(VERSION_PREFIX)
if (len(all_versions) < 1):
    print "NO code versions found... aborting!\n"
    exit(0)
else:
    print str(len(all_versions)) + " code versions found."


# Compile the versions
printdhr

print "Proceed with the timing? [Y|n]"
if not proceed():
	print "Bye!\n"
	exit(0)

num_threads = [1,2,4,8,16]
print "Enter a comma-separated list of thread nums (default is",num_threads,"):",
thread_list = raw_input()
if (thread_list != ""):
	num_threads = thread_list.rstrip().split(',')
	try:
		num_threads = map(int,num_threads)
	except ValueError:
		print "Invalid numbers of threads...aborting!\n"
		exit(1)
	if (len([x for x in num_threads if x < 0]) > 0):
		print "Can't have negative numbers of threads...aborting!\n"
		exit(1)


printhr()
fastest = [None,None,None]
for version in all_versions:
	print "Timing version",version+":"
	for num in num_threads:
		average = timeVersionWithThreads(version,num)
                if (average >= 0):
		    print "\t",num,"threads:",average,"seconds"
		    if (fastest == [None,None,None]) or (fastest[0] > average):
			    fastest = [average,version,num]
printhr
print "Fastest version:",fastest[1],"with",fastest[2],"threads:",fastest[0]
printdhr


