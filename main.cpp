#include <QCoreApplication>
#include <QVector>
#include <thread>

#include <iostream>
#include <math.h>

const unsigned TotalData = 60*60*24*365;
const unsigned SampleSize = 60*60*24;
QVector<float> Buffer;

double getMean(QVector<float>* data, unsigned start_pos, unsigned sample_size) {
    double sum = 0.0;
    for (int i = start_pos; i < start_pos + sample_size; i++)
        sum += (*data)[i];
    std::cout << "average temperature " << sum/sample_size << " ";
    return sum/sample_size;
}


double median(QVector<float>* data, unsigned start_pos, unsigned sample_size) {
   std::sort((*data).begin() + start_pos, (*data).begin() + start_pos + sample_size - 1);
   if ((start_pos + sample_size - 1) % 2 == 0) {
      std::cout << " with median " << ((*data)[((start_pos + sample_size - 1) / 2) - 1] + (*data)[(start_pos + sample_size - 1) / 2]) / 2.0 << "\n";
      return ((*data)[((start_pos + sample_size - 1) / 2) - 1] + (*data)[(start_pos + sample_size - 1) / 2]) / 2.0;
   }
   std::cout << " with median " << (*data)[(start_pos + sample_size - 1) / 2] << "\n";
   return (*data)[(start_pos + sample_size - 1) / 2];
}


void procedure(unsigned begin, unsigned end) {
    for (unsigned i = begin; i < end; i += SampleSize) {
        std::cout << i << " of " << TotalData << " ";
        getMean(&Buffer, begin, SampleSize);
        median(&Buffer, begin, SampleSize);
    }
}


void divide_and_conquer(unsigned begin, unsigned end, unsigned depth = 0) {
    if (end - begin > SampleSize) {
        const unsigned pivot = (begin + end) / 2;

        if (depth > 0) { // PARALLEL
            std::thread t1(divide_and_conquer, begin, pivot, depth - 1);
            std::thread t2(divide_and_conquer, pivot + SampleSize, end, depth - 1);

            t1.join();
            t2.join();
        } else { // SEQUENTIAL
            std::thread t1(procedure, begin, end);
            t1.join();
        }
    }
}


int main(int argc, char *argv[]) {
    int num_theads = 0;
    if (argc == 3) {

    } else if (argc == 1) {
        num_theads = 0;
    } else if (argc == 2 && ((std::string)argv[1] == "-h" || (std::string)argv[1] == "--help")) {
        std::cout << "Help guide\n";
    } else {
        std::cout << "Wrong number of arguments" << std::endl;
        return 1;
    }

    Buffer.resize(TotalData);

    for(unsigned int i = 0; i < TotalData; i++)
        Buffer[i] = random() % 50 + 50;

    auto start2 = std::chrono::high_resolution_clock::now(); // start   ------------
    std::thread t1(divide_and_conquer, 0, TotalData, num_theads);
    t1.join();
    auto stop2 = std::chrono::high_resolution_clock::now(); // stop     ------------
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);
    std::cout << duration2.count() << "\n";
    std::cout << "Done in divide and conquer mode\n";

    return 0;
}
