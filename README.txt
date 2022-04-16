##################################################################
##							        ##
##         How to use the Secheduling Simulator program         ##
##							        ##
##################################################################

1. Input File Format & Requirements
	A. The file is expected to have three columns of data.
		a. The first column is of process PIDs.

		b. The second column is of the process
		   arrival times.

		c. The third column is of the process burst times.

		d. Example:
			1   0   10 
			2   0   9
			3   3   5
			4   7   4
			5   10  6
			6   10  7

	B. Format Requirements:
		a. No headers or labels. This is to minimize the
		   possibility of errors.

		b. The amount of rows should not exceed 20. Any
		   rows past that will be ignored.

		c. Each column is expected to be separated at least
		   by one space.

	C. Data Requirements:
		a. The numbers are to be integers. Decimals will
		   cause errors.

		b. The PIDs must be a value of 1 or greater. 
			i. Any value below 1 will still be read, but 
			   will be ignored by the scheduler.

		c. The Arrival Time must be a value of 0 or greater.
		   	i. Negative arrival times are used by the scheduler
		   	   to represent a process that has already been
		   	   placed in the ready queue, so it will be ignored.

		d. The Burst Time must be a value of 1 or greater.
			i. Process with a burst time of 0 or less will be
			   considered as complete, and will be ignored.

	C. Allowed:
		a. There can be more than one space between each column.

		b. There is no limitation on the file's name.

		c. The file can have any arbitrary extension, such as
		   input.1, numbers.erf, input.txt, input.input, etc.

		d. Data does not need to be sorted by PID, arrival time,
		   nor burst time. Having data sorted will not make a
		   difference in the time calculations.

	D. File location:
		a. Ensure the file is located in the same folder as as 
		   the program.


2. Compiling the Application
	A. Type the following command in the command-promt:

		gcc scheduler.c -o scheduler -lm

	B. The -lm flag is required for the the math library to
	   be linked, allowing the use of mathematical functions.


3. Executing the Application
	A. Version 1
		a. Execute the program without any arguments:

			./scheduler

		b. The output will be instructions on the format
		   and the available scheduling algorithms.
	
	B. Version 2
		a. Execute the program with two additional arguments.
		   The first argument is the name of the input file.
		   The second argument is the abbreviation of the 
		   scheduling algorithm.

		b. Type the following to execute the program using 
		   the First Come First Serve algorithm:
			
			./scheduler [file name] FCFS
		
		c. Type the following to execute the program using 
		   the Shortest Remaining Time First algorithm:
		
			./scheduler [file name] SRTF

	C. Version 3
		a. Execute the program with an additional argument from
		   Version 2. This additional argument is an integer 
		   that will set the quantum time to be used for the
		   Round Robin algorithm.
		
		b. Type the following to execute the program using 
		   the Round Robin algorithm:

			./scheduler [file name] RR [quantum time]

		c. Example: ./scheduler input.txt RR 2

		d. The quantum time cannot be a value of 0 or lower. It
		   must also be an integer value. No decimals. The only
		   limitation on the highest quantum value is what an
		   integer can hold, which is of 2,147,483,647. Keep in
		   mind that if the quantum is equal or greater to the
		   highest burst among processes, then it will simply
		   become a FCFS algorithm.

4. Changing the Limitation of the Size of the Ready Queue.
	A. Currently, the size is limited to 20. If you want to have the 
	   scheduler read more rows of data from your file, then do the
	   following:
		a. Open the scheduler.c file.
		
		b. Search for the global variable: SIZE.
		
		c. Change the SiZE to the amount of processes you want
		   to work with.

		d. Below the SIZE variable, you will find the three data
		   arrays: pidArray, arrivalArray, and burstArray.
		
		e. Change the memory allocation for these arrays from 20
		   to the same value of the SIZE variable.

