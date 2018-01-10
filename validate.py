#!/usr/bin/python

import sys
from datetime import datetime
import time
import RPi.GPIO as IO

MOTOR_PIN = 19

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
	p = IO.PWM(MOTOR_PIN, 100) # Second argument is frequency, I think
	return p

def unlock_door(p):
	"""
	Uses PWM to turn the motor connected to the door handle.
	"""
	p.start(50)
	time.sleep(1.0)
	p.stop()

def read_loop():
	"""
	Simple REPL skeleton for validating scanned IDs.
	"""
	# Open the log file in append mode
	log_file = open("./access.log", "a")
	log_file.write(timestamped("=== Door security initiated ===\n"))
	# Initialize the PWM pin
	pwm_pin = setup_pwm()

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
			unlock_door(pwm_pin)

if __name__ == "__main__":
	read_loop()
