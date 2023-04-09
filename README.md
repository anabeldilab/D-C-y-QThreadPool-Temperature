# SOA D&C y QThreadPool: Temperature
Made by Jaime Pablo Pérez Moro and Anabel Díaz Labrador

# Description
In this project we have programmed a CPU temperature statistics calculator for the whole year for each **day**.

To carry out this practice we have worked in QT Creator.

## QT Creator
We have worked on version 6.0.2 although it can possibly run on version 5.

We have used QVector and QThreadPool.

# How to use?
To run it, you have to compile the project with cmake and execute the executable with the following commands:

- By default the sequential mode is executed.
- -d [VALUE] This option is to use divide and conquer algorithm. "VALUE" is the depth of the algorithm. It will be explained later.
- -p [VALUE] This option is to use QThreadPool aproximation. "VALUE" is the number of thread that the algorithm will use, when left blank maxThreadCount will take its maximum value.


# Sequential mode 
The serial mode is a for loop that starts at 0 and ends at *TotalData* in which "every second" We put a random value, between 50 and 100, into the Buffer.

We have an *if* that searches if the iterator is in another day to calculate the mean and the median of a previous day. We have been careful to skip the 0 day since we have no data yet.


# Divide and Conquer
The Divide and Conquer mode has a depth value, that will determine the number of times that the array is going to divide in differents sub-lists.


```c++
void divide_and_conquer(unsigned begin, unsigned end, unsigned depth = 0) {
    if ((end - begin > SampleSize)) {

        const unsigned pivot = ((begin + end) / 2) % SampleSize != 0 ? ((begin + end) / 2) + (SampleSize/2) : (begin + end) / 2;

        if (pivot < TotalData - SampleSize) {
            if (depth > 0) { // PARALLEL
                std::thread t1(divide_and_conquer, begin, pivot, depth - 1);
                std::thread t2(divide_and_conquer, pivot, end, depth - 1);

                t1.join();
                t2.join();
            } else { // SEQUENTIAL
                std::thread t1(procedure, begin, end);
                t1.join();
            }
        }
    }
}
```

The algorithm has a partition part that depends on the depth. Later, the threads create a new thread that executes the procedure function.

```c++
void procedure(unsigned begin, unsigned end) {
    for (unsigned i = begin; i < end; i += SampleSize) {
        std::cout << i << " of " << end << " ";
        std::cout << "average temperature " << getMean(&Buffer, i, SampleSize) << " " << std::flush;
        std::cout << " with median " << median(&Buffer, i, SampleSize) << "\n" << std::flush;
    }
}
```

The procedure function travels the vector from beginning to end havinh each thread calculate the mean and the median of each day.

# QThreadPool approach

Using the same data as the other two methods explained earlier, we can calculate the mean and median using QThreadPool and QRunnable.

All we had to do was implement a class that inherited from QRunnable as we can see in the following code extract:
```c++
class DayTemp : public QRunnable
```

The idea of this was to implement the pure virtual function run() in our subclass.

```c++
    void run()
    {
        std::cout << start_pos_ << " of " << TotalData << " ";
        std::cout << "average temperature " << getMean(&Buffer, start_pos_, SampleSize) << " ";
        std::cout << " with median " << median(&Buffer, start_pos_, SampleSize) << "\n";
    }
```

This way we get every thread to calculate the mean and the median for every day.

When executing the program you can specify the number of threads in the threadpool. If this option is left blank the program will run with default number of threads depending on the system its being run on.

# Timing Comparison Table

All tests were run on Linux OS.

- Sequential mode (without threads): 12208960 

|        | QThreadPool mode      |      Threads     |
| :----: | :-------------------: | :--------------: |
| µs     | 12141932              |    1(Serial)     |
| µs     | 6132674               |        2         |
| µs     | 4098260               |        3         |
| µs     | 3093433               |        4         |
| µs     | 2495920               |        5         |
| µs     | 2122245               |        6         |
| µs     | 1978396               |        7         |
| µs     | 1874429               |        8         |
| µs     | 1773957               |        9         |
| µs     | 1691725               |       10         |
| µs     | 1612826               |       11         |
| µs     | 1593345               |       12         |


|        | Divide & Conquer mode    |      Depth       |      Threads                           |
| :----: | :----------------------: | :--------------: | :------------------------------------: |
| µs     | 12147838                 |      0 (Serial)  |        1 + 1(executes procedure)       |
| µs     | 6123903                  |      1           |        3 + 2(execute procedure)        |
| µs     | 3113816                  |      2           |        7 + 4(execute procedure)        |
| µs     | 2086753                  |      3           |        15 + 8(execute procedure)       |
| µs     | 1667251                  |      4           |        31 + 16(execute procedure)      |
| µs     | 1600163                  |      5           |        63 + 32(execute procedure)      |
| µs     | 1584571                  |      6           |        127 + 64(execute procedure)     |


As it can be observed in the tables above, when using the QThreadPool aproach to the problem, better times are taken. Leaving the OS to handle threads seems to be the best aproach as compared with divide and conquer which is at some point inefficient due to the excessive creation of threads. This happens because QThreadPool uses already created threads (if there are any) while being able to put a limit to the number of threads used and divide and conquer doesn't. Having in mind the amount of threads that this aproach needs, divide and conquer becomes increasingly inefficient. 

During the tests, when setting the depth above 5 we started to notice the effect of creating too many threads, the programs lagged out and the entire system was affected by this. However in the thread pool aproach it never happened. 



<font size="1"> After depth 6 too many threads are created making it hard to test.*</font>

