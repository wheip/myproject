#ifndef CLASSLIST_H
#define CLASSLIST_H

#include <QByteArray>
#include <QString>
#include <vector>
#include <math.h>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include <QMessageBox>
#include <QVBoxLayout>
#include "imageviewer.h"
#include "irimagedisplay.h"
#include <QStackedWidget>
#include <QFileDialog>
#include <QDir>
#include <QMenu>
#include <QAction>
#include "opencv2/opencv.hpp"

#define PI              3.1415926535898
using namespace std;

class Waveform {
public:
    virtual double generate(int sampleIndex, int totalSamples) = 0;
};

class SineWave : public Waveform {
    double amplitude;
    double frequency;
public:
    SineWave(double amp, double freq) : amplitude(amp), frequency(freq) {}
    double generate(int sampleIndex, int totalSamples) override {
        double phaseIncrement = 2 * M_PI * frequency / totalSamples;
        return amplitude * sin(phaseIncrement * sampleIndex);
    }
};

class SquareWave : public Waveform {
    double amplitude;
    double frequency;
    double dutyCycle; // 占空比
public:
    SquareWave(double amp, double freq, double duty) : amplitude(amp), frequency(freq), dutyCycle(duty) {}
    double generate(int sampleIndex, int totalSamples) override {
        double cycleIncrement = frequency / totalSamples;
        double position = cycleIncrement * sampleIndex;
        if (position - floor(position) < dutyCycle) {
            return amplitude;
        } else {
            return -amplitude;
        }
    }
};

class TriangleWave : public Waveform {
    double amplitude;
    double frequency;
public:
    TriangleWave(double amp, double freq) : amplitude(amp), frequency(freq) {}
    double generate(int sampleIndex, int totalSamples) override {
        double cycleIncrement = frequency / totalSamples;
        double position = cycleIncrement * sampleIndex;
        double phase = position - floor(position);
        if (phase < 0.5) {
            return 4 * amplitude * phase - amplitude;
        } else {
            return -4 * amplitude * phase + 3 * amplitude;
        }
    }
};

class StepWave : public Waveform {
    double amplitude;
public:
    StepWave(double amp) : amplitude(amp) {}
    double generate(int sampleIndex, int totalSamples) override {
        return amplitude;
    }
};

class HighLevelWave : public Waveform {
    double amplitude;
public:
    HighLevelWave(double amp) : amplitude(amp) {}
    double generate(int sampleIndex, int totalSamples) override {
        return amplitude;
    }
};

class LowLevelWave : public Waveform {
    double amplitude;
public:
    LowLevelWave(double amp) : amplitude(amp) {}
    double generate(int sampleIndex, int totalSamples) override {
        return -amplitude;
    }
};

class Device
{
public:
    QString id;
    QByteArray device_name;
    double device_driver_Voltage;
    double device_driver_Current;
    double device_driver_Power;
    QByteArray image;
};

class PXIe5711Waveform
{
public:
    QString id;
    QString step_id;
    int channel;
    QString waveform_type;
    double amplitude;
    double frequency = 0;
    double dutyCycle = 0;
    int positive_connect_location = 0;
    int negative_connect_location = 0;
};

class PXIe5320Waveform
{
public:
    QString id;
    QString step_id;
    int device;
    int port;
    vector<float> data;
    int positive_connect_location = 0;
    int negative_connect_location = 0;
};

class Step
{
public:
    QString id;
    QString test_task_id;
    double collecttime;
    int step_number;
    bool continue_step;
    bool isthermometry;
    double thermometry_pause_time;
};

class TestTask
{
public:
    QString id;
    int element_id;
    QByteArray connection_image_data;
};

class Image
{
public:
    QString id;
    QString step_id;
    QString device;
    QString task_table_name;
    QByteArray image_data;
    QByteArray temp_data = QByteArray();
    int temp_width = 0;
    int temp_height = 0;
};

class Data5320
{
public:
    QString id;
    int serial_number;
    QString waveform_id;
    vector<float> data;
};

class Data8902
{
public:
    QString id;
    QString step_id;
    QString test_type;
    int positive_connect_location = 0;
    int negative_connect_location = 0;
};

class Label
{
public:
    int id;
    QString label;
    double point_x, point_y;
    double width, height;
    QString position_number;
    QByteArray notes;

    Label() : id(0), label(""), point_x(0), point_y(0), width(0), height(0), position_number(""), notes("") {}
    Label(const int &idx, const QString &n, double px, double py, double w, double h, const QString &pn, const QByteArray &nt)
        : id(idx), label(n), point_x(px), point_y(py), width(w), height(h), position_number(pn), notes(nt) {}
};

#endif // CLASSLIST_H
