#include <QCoreApplication>
#include <QVector>
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
   if ((start_pos + sample_size - 1) % 2 == 0)
      return ((*data)[((start_pos + sample_size - 1) / 2) - 1] + (*data)[(start_pos + sample_size - 1) / 2]) / 2.0;
   return (*data)[(start_pos + sample_size - 1) / 2];
}

class DayTemp : public QRunnable
{
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

int main(int argc, char *argv[]) {
    Buffer.resize(TotalData);

    for(unsigned int i = 0; i < TotalData; i++) {
        Buffer[i] = rand() % 50 + 50;
    }

    auto start1 = std::chrono::high_resolution_clock::now();

    for(unsigned int i = 0; i < TotalData; i += SampleSize)
    {
        DayTemp *t1 = new DayTemp(&Buffer, i, SampleSize);
    //TestTask *t2 = new TestTask;
        QThreadPool::globalInstance()->start(t1);
    //QThreadPool::globalInstance()->start(t2);
    }

    QThreadPool::globalInstance()->waitForDone();

    auto stop1 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
    std::cout << duration1.count() << "\n";
    std::cout << "Done in QThreadPool mode\n";
    std::cout << "Using " << QThreadPool::globalInstance()->maxThreadCount() << " Threads.\n";

    return 0;
}
