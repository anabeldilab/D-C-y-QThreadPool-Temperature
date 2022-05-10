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
    return sum/sample_size;
}


double median(QVector<float>* data, unsigned start_pos, unsigned sample_size) {
   std::sort((*data).begin() + start_pos, (*data).begin() + start_pos + sample_size - 1);
   if ((start_pos + sample_size - 1) % 2 == 0) {
      return ((*data)[((start_pos + sample_size - 1) / 2) - 1] + (*data)[(start_pos + sample_size - 1) / 2]) / 2.0;
   }
   return (*data)[(start_pos + sample_size - 1) / 2];
}


void procedure(unsigned begin, unsigned end) {
    for (unsigned i = begin; i < end; i += SampleSize) {
        std::cout << i << " of " << end << " ";
        std::cout << "average temperature " << getMean(&Buffer, i, SampleSize) << " " << std::flush;
        std::cout << " with median " << median(&Buffer, i, SampleSize) << "\n" << std::flush;
    }
}


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



int main(int argc, char *argv[]) {
    int depth = 0;
    if (argc == 3 && ((std::string)argv[1] == "-d" || (std::string)argv[1] == "--depth")) {
        depth = atoi(argv[2]);
    } else if (argc == 1) {
        depth = 0;
    } else if (argc == 2 && ((std::string)argv[1] == "-h" || (std::string)argv[1] == "--help")) {
        std::cout << "Help guide" << std::endl << std::endl
                  << "This program has a vector with the temperature record of a year every second." << std::endl << std::endl
                  << "-d (value) or --depth (value) Executes divide and conquer algorithm with the given value" << std::endl
                  << "-h or --help Shows this help guide." << std::endl;
        return 0;
    } else {
        std::cout << "Wrong argument or argument number" << std::endl;
        return 1;
    }

    Buffer.resize(TotalData);

    for(unsigned int i = 0; i < TotalData; i++)
        Buffer[i] = random() % 50 + 50;

    auto start2 = std::chrono::high_resolution_clock::now(); // start   ------------
    std::thread t1(divide_and_conquer, 0, TotalData, depth);
    t1.join();
    auto stop2 = std::chrono::high_resolution_clock::now(); // stop     ------------
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);
    std::cout << duration2.count() << "\n";
    std::cout << "Done in divide and conquer mode\n";

    return 0;
}
