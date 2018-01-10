#!/usr/bin/python

import sys
from datetime import datetime

def timestamped(s):
	"""
	Prepends a timestamp to a string.
	"""
	return "[{:%Y-%m-%d %H:%M:%S}] {}".format(datetime.now(), s)

def read_loop():
	"""
	Simple REPL skeleton for validating scanned IDs.
	"""
	# Open the log file in append mode
	log_file = open("./access.log", "a")
	log_file.write(timestamped("=== Door security initiated ===\n"))

	# Begin the loop
	while True:
		# TODO: Read in the ID
		
		# TODO: Check if the ID is authorized
		authorized = False

		# If the user is not authorized, deny access
		if input not in authorized:
			s = timestamped("##### denied\n")
			log_file.write(s)
		# If the user is authorized, perform the unlock procedure
		else:
			s = timestamped("##### <Username> validated")
			log_file.write(s)
			# TODO: Play open tone
			# TODO: Run unlock procedure

if __name__ == "__main__":
	read_loop()
