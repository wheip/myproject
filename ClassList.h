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

enum class PXIe5711_testtype {
    HighLevelWave,
    LowLevelWave,
    SineWave,
    SquareWave,
    StepWave,
    TriangleWave,
};

inline QString PXIe5711_testtype_to_string(PXIe5711_testtype testtype) {
    switch (testtype) {
        case PXIe5711_testtype::HighLevelWave:
            return "HighLevelWave";
        case PXIe5711_testtype::LowLevelWave:
            return "LowLevelWave";
        case PXIe5711_testtype::SineWave:
            return "SineWave";
        case PXIe5711_testtype::SquareWave:
            return "SquareWave";
        case PXIe5711_testtype::StepWave:
            return "StepWave";
        case PXIe5711_testtype::TriangleWave:
            return "TriangleWave";
    }
}

enum class PXIe8902_testtype {
    PXIe8902_voltage,
    PXIe8902_current,
    PXIe8902_resistance,
};

inline QString PXIe8902_testtype_to_string(PXIe8902_testtype testtype) {
    switch (testtype) {
        case PXIe8902_testtype::PXIe8902_voltage:
            return "voltage";
        case PXIe8902_testtype::PXIe8902_current:
            return "current";
        case PXIe8902_testtype::PXIe8902_resistance:
            return "resistance";
    }
}

class Device // 待测设备
{
public:
    int id;
    QByteArray device_name;
    double device_driver_Voltage;
    double device_driver_Current;
    double device_driver_Power;
    QByteArray image;
};

class PXIe5711Waveform
{
public:
    int id;
    int step_id;
    int channel;
    PXIe5711_testtype waveform_type;
    double amplitude;
    double frequency = 0;
    double dutyCycle = 0;
    int positive_connect_location = 0;
    int negative_connect_location = 0;
    PXIe5711Waveform() : id(-1), step_id(-1), channel(0), waveform_type(PXIe5711_testtype::HighLevelWave), amplitude(0), frequency(0), dutyCycle(0), positive_connect_location(0), negative_connect_location(0) {}
    PXIe5711Waveform(const int &id, const int &step_id, int channel, PXIe5711_testtype waveform_type, double amplitude, double frequency, double dutyCycle, int positive_connect_location, int negative_connect_location)
        : id(id), step_id(step_id), channel(channel), waveform_type(waveform_type), amplitude(amplitude), frequency(frequency), dutyCycle(dutyCycle), positive_connect_location(positive_connect_location), negative_connect_location(negative_connect_location) {}
};

class PXIe5320Waveform
{
public:
    int id;
    int step_id;
    int device;
    int port;
    vector<float> data;
    int positive_connect_location = 0;
    int negative_connect_location = 0;
    PXIe5320Waveform() : id(-1), step_id(-1), device(0), port(0), data(vector<float>()), positive_connect_location(0), negative_connect_location(0) {}
    PXIe5320Waveform(const int &id, const int &step_id, int device, int port, const vector<float> &data, int positive_connect_location, int negative_connect_location)
        : id(id), step_id(step_id), device(device), port(port), data(data), positive_connect_location(positive_connect_location), negative_connect_location(negative_connect_location) {}
};

class Step
{
public:
    int id;
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
    int id;
    int step_id;
    QString task_table_name;
    QByteArray image_data;
    QByteArray temp_data = QByteArray();
    int temp_width = 384;
    int temp_height = 288;
};

class Data5320
{
public:
    int id;
    int serial_number;
    int waveform_id;
    vector<float> data;
    Data5320() : id(0), serial_number(0), waveform_id(0), data(vector<float>()) {}
    Data5320(const int &id, int serial_number, const int &waveform_id, const vector<float> &data)
        : id(id), serial_number(serial_number), waveform_id(waveform_id), data(data) {}
};

class Data8902
{
public:
    int id;
    int step_id;
    PXIe8902_testtype test_type;
    bool model; //true:DC,false:AC
    int positive_connect_location = 0;
    int negative_connect_location = 0;
    Data8902() : id(0), step_id(0), test_type(PXIe8902_testtype::PXIe8902_voltage), model(false), positive_connect_location(0), negative_connect_location(0) {}
    Data8902(const int &id, const int &step_id, PXIe8902_testtype test_type, bool model, int positive_connect_location, int negative_connect_location)
        : id(id), step_id(step_id), test_type(test_type), model(model), positive_connect_location(positive_connect_location), negative_connect_location(negative_connect_location) {}
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
