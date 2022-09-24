# Oparating-Systems-Signals

Goal

In this assignment, you will implement a system call to the linux kernel and then you will build it.
After building, you will call your own system call if your program is working in user space. The
detailed scenario is given in the below.

Implementation Details & Requirements
Your code running into user space should run the system call is named as “ processinfo ”. Your
program has been worked using four different usages without argument and with three arguments
such as -p , -k and -all separately.

Additionally, it may be taken as different time with the following parameters:
● without argument , it prints the right usage of this program. 
● “ -all ”, it lists all the processes and some information about them.  
● “ -p processID ”, it outputs the given process ID and its command line.  
● “ -k processID ”, it kills the given process ID.  

The sample executing formats and its functionality shortly.
./processinfo.out > > program’s right usage
./processinfo.out -all > > all processes some information
./processinfo.out -p 1425 > > process and its cmdline
./processinfo.out -k 1425 > > kill process

The sample outputs should be like that, if the program was run without argument, with -all, -p
and -k arguments, respectively.

  without argument
    Right Usage:
    -all prints some information (process id and its
    argument/s) about all processes
    -p takes process id and prints the details of it
    -k takes process id
  with -all argument
    Process id and the argument list for the processes:
    uid ppid pid name state
  with -p argument
    Argument# [process_id] -Process Information:
    pid commandline
  with -k argument
    Process xx was killed...
