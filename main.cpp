#include <QCoreApplication>
#include <QVector>
#include <thread>
#include <iostream>
#include <QRunnable>
#include <QThreadPool>
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


class DayTemp : public QRunnable {
private:
    QVector<float>* data_;
    unsigned start_pos_;
    unsigned sample_size_;

public:
    DayTemp(QVector<float>* data, unsigned start_pos, unsigned sample_size) {
        data_ = data;
        start_pos_ = start_pos;
        sample_size_ = sample_size;
    }

    void run()
    {
        std::cout << start_pos_ << " of " << TotalData << " ";
        std::cout << "average temperature " << getMean(&Buffer, start_pos_, SampleSize) << " ";
        std::cout << " with median " << median(&Buffer, start_pos_, SampleSize) << "\n";
    }

    QVector<float>* get_data() { return data_; }
    unsigned get_start_pos() { return start_pos_; }
    unsigned get_sample_size() { return sample_size_; }

    void set_data(QVector<float>* datos) { data_ = datos; }
    void set_start_pos(unsigned start_pos) { start_pos_ = start_pos; }
    void set_sample_size(unsigned sample_size) { sample_size_ = sample_size; }
};


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
    Buffer.resize(TotalData);

    for(unsigned int i = 0; i < TotalData; i++)
        Buffer[i] = random() % 50 + 50;

    if (argc == 3 && ((std::string)argv[1] == "-d")) {
        int depth = atoi(argv[2]);

        auto start = std::chrono::high_resolution_clock::now(); // start   ------------
        std::thread t1(divide_and_conquer, 0, TotalData, depth);
        t1.join();
        auto stop = std::chrono::high_resolution_clock::now(); // stop     ------------

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << duration.count() << "\n";
        std::cout << "Done in divide and conquer mode\n";
    } else if ((argc == 3 || argc == 2) && ((std::string)argv[1] == "-p")) {
        if (argc == 3) {
            int num_threads = atoi(argv[2]);
            QThreadPool::globalInstance()->setMaxThreadCount(num_threads);
        }
        auto start = std::chrono::high_resolution_clock::now(); // start   ------------
        for(unsigned int i = 0; i < TotalData; i += SampleSize) {
            DayTemp *t1 = new DayTemp(&Buffer, i, SampleSize);
            QThreadPool::globalInstance()->start(t1);
        }
        QThreadPool::globalInstance()->waitForDone();
        auto stop = std::chrono::high_resolution_clock::now(); // stop      ------------

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << duration.count() << "\n";
        std::cout << "Done in QThreadPool mode\n";
        std::cout << "Using " << QThreadPool::globalInstance()->maxThreadCount() << " Threads.\n";
    } else if (argc == 1) {
        auto start = std::chrono::high_resolution_clock::now(); // start   ------------
        for(unsigned int i = 0; i < TotalData; i++) 
            if(i % SampleSize == 0 && i != 0) {
                std::cout << i << " of " << TotalData << " ";
                std::cout << "average temperature " << getMean(&Buffer, i - SampleSize, SampleSize) << " ";
                std::cout << " with median " << median(&Buffer, i - SampleSize, SampleSize) << "\n";
            }     
        auto stop = std::chrono::high_resolution_clock::now(); // stop      ------------

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << duration.count() << "\n";
        std::cout << "Done in serial mode\n";
    } else if (argc == 2 && ((std::string)argv[1] == "-h" || (std::string)argv[1] == "--help")) {
        std::cout << "Help guide" << std::endl << std::endl
                  << "This program has a vector with the temperature record of a year every second." << std::endl << std::endl
                  << "-d [VALUE] Executes divide and conquer algorithm with the given value being the depth reached by the algorithm." << std::endl
                  << "-p [VALUE] Executes QThreadPool aproach with the given value being the maximum number of threads in the threadpool." << std::endl
                  << "-h or --help Shows this help guide." << std::endl;
        return 0;
    } else {
        std::cout << "Wrong argument or argument number. Use -h or --help for more information." << std::endl;
        return 1;
    }
    return 0;
}
