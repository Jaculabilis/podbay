#!/usr/bin/python

import sys
from datetime import datetime
import time
import json
import RPi.GPIO as IO
import zmq

# Magic numbers for PWM activation
MOTOR_PIN = 19
FREQUENCY = 100
DUTYCYCLE = 100
DURATION = 1.0
# Address of the scanning service
SCANSERVADDRESS = "tcp://localhost:5000"


def timestamped(s):
	"""
	Prepends a timestamp to a string.
	"""
	return "[{:%Y-%m-%d %H:%M:%S}] {}".format(datetime.now(), s)

def setup_pwm():
	"""
	Performs initialization for pulse width modulation.
	Returns the PWM object for the motor pin.
	"""
	IO.setwarnings(False)
	IO.setmode(IO.BCM)
	IO.setup(MOTOR_PIN, IO.OUT)
	p = IO.PWM(MOTOR_PIN, FREQUENCY)
	return p

def unlock_door(p):
	"""
	Uses PWM to turn the motor connected to the door handle.
	"""
	try:
		p.start(DUTYCYCLE)
		time.sleep(DURATION)
	finally:
		p.stop()

def read_loop():
	"""
	Simple REPL skeleton for validating scanned IDs.
	"""
	# Initialize zmq socket for reading in scanned barcodes
	context = zmq.Context()
	socket = context.socket(zmq.SUB)
	socket.setsockopt(zmq.SUBSCRIBE, "")
	socket.connect(SCANSERVADDRESS)
	# Initialize the PWM pin for opening the door
	pwm_pin = setup_pwm()
	# Open the log file in append mode
	log_file = open("./access.log", "a")
	log_file.write(timestamped("=== Door security initiated ===\n"))

	# Begin the loop
	while True:
		# Read in the ID
		code = socket.recv()

		# Load the access database
		# As long as the access list is small and the speed is unimportant,
		# the flexibility and update ability is worth reading from disk.
		raw = open("access.json", "r").read()
		try:
			access = json.loads(raw)
		except:
			log_file.write(timestamped("Could not load access file!"))
			sys.stderr.write("Could not load access file!")
			access = {}

		# Determine ID authorization
		if code not in access:
			authorized = False
			user = "unknown barcode"
		else:
			authorized = "authorized" in access[code] and access[code]["authorized"]
			user = access[code]["name"] if "name" in access[code] else code

		# If the user is not authorized, deny access
		if not authorized:
			s = timestamped("Denied {} ({})\n".format(code, user))
			log_file.write(s)
			print s,
		
		# If the user is authorized, perform the unlock procedure
		else:
			s = timestamped("Validated {} ({})\n".format(code, user))
			log_file.write(s)
			print s,
			# TODO: Play open tone
			if "sound" in access[code]:
				pass
			# Run unlock procedure
			unlock_door(pwm_pin)

if __name__ == "__main__":
	read_loop()
