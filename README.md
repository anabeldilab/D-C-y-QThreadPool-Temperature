# SOA D&C y QThreadPool: Temperature
Made by Jaime Pablo Pérez Moro and Anabel Díaz Labrador

# Description
In this project we have programmed a CPU temperature statistics calculator for the whole year for each **day**.

To carry out this practice we have worked in QTCreator.

## QTCreator
We have worked on 6.0.2v although it can possibly run on 5v.

We have used QVector and QThreadPool.

# How to use?
To run it, you have to compile the project with cmake and execute the executable with the following commands:

- By default the sequential mode is executed.
- -d [VALUE] This option is to use divide and conquer algorithm. "VALUE" is the depth of the algorithm. It will be explained later.
- -p [VALUE] This option is to use QThreadPool aproximation. "VALUE" is the number of thread that the algorithm will use.


# Secuential mode 
The serial mode is a for loop that starts at 0 and ends at *TotalData* in which "every second" We put a random value, between 50 and 100, into the Buffer.

We have an *if* that searches if the iterator is in another day to calculate the mean and the median of a previous day. We have been careful to skip the 0 day since we haven't data yet.


# Divide and Conquer
The Divide and Conquer mode has a depth value, that it will determine the number of times that the array is going to divide in differents sub-lists.

The algorithm has a partition part that depends on the depth. Later, the threads create a new thread that executes the procedure function.

The procedure function travels the vector from the beginning to end correspondent to each thread calculating the mean and the median of each day.

