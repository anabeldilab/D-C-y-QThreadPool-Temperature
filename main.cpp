#include <QCoreApplication>
#include <QVector>
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
   if ((start_pos + sample_size - 1) % 2 == 0)
      return ((*data)[((start_pos + sample_size - 1) / 2) - 1] + (*data)[(start_pos + sample_size - 1) / 2]) / 2.0;
   return (*data)[(start_pos + sample_size - 1) / 2];
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Buffer.resize(TotalData);

    auto start1 = std::chrono::high_resolution_clock::now();
    for(unsigned int i = 0; i < TotalData; i++) {
        Buffer[i] = random() % 50 + 50;
        if(i % SampleSize == 0 && i != 0) {
            std::cout << i << " of " << TotalData << " ";
            std::cout << "average temperature " << getMean(&Buffer, i - SampleSize, SampleSize) << " ";
            std::cout << " with median " << median(&Buffer, i - SampleSize, SampleSize) << "\n";
        }
    }
    auto stop1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
    std::cout << duration1.count() << "\n";
    std::cout << "Done in serial mode\n";

    return 0;
}
