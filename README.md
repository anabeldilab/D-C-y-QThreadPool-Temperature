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

# QThreadPool aproach

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

|        | Serial mode   | Divide & Conquer mode    | QThreadPool mode      |      VALUE     |
| :----: | :-----------: | :----------------------: | :-------------------: | :------------: |
| µs     | 12208960      | 12147838                 |           -           |    0(Serial)   |
| µs     |      -        | 6123903                  | 12141932              |    1(Serial)   |
| µs     |      -        | 3113816                  | 6132674               |        2       |
| µs     |      -        | 2086753                  | 4098260               |        3       |
| µs     |      -        | 1667251                  | 3093433               |        4       |
| µs     |      -        | 1600163                  | 2495920               |        5       |
| µs     |      -        | 1584571                  | 2122245               |        6       |
| µs     |      -        |            -             | 1978396               |        7       |
| µs     |      -        |            -             | 1874429               |        8       |
| µs     |      -        |            -             | 1773957               |        9       |
| µs     |      -        |            -             | 1691725               |       10       |
| µs     |      -        |            -             | 1612826               |       11       |
| µs     |      -        |            -             | 1593345               |       12       |

<font size="1"> In QThreadPool mode VALUE = maxThreadCount*</font> 

<font size="1"> In Divide & Conquer mode VALUE = depth*</font> 

<font size="1"> After depth 6 too many threads are created making it hard to test.*</font>

