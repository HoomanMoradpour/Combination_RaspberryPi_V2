#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "downloader.h"
#include "QListWidgetItem"
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QListWidgetItem>
#include <QChart>
#include <QtCharts/QDateTimeAxis>
#include <QTimer>
#include <QSplitter>
#include <QMainWindow>
#include <wiringPi.h>
#include "DistanceSensor.h"

#ifdef _version5
using namespace QtCharts;
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct weatherDatapoint
{
    QString validTime;
    double values;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool on = false;

    QJsonArray forecastArray;

private:
    Ui::MainWindow *ui;
    Downloader *downloader;
    Downloader *downloader_2;
    Downloader *downloaderGetForcast;
    Downloader *downloaderGetForcast_2;
    QMap<QString, QJsonValue> jsonValues;
    void showDatainList();
    void showDatainList_2();
    void inigetForecastURL(const QString &url);
    void inigetForecastURL_2(const QString &url);
    struct weatherDatapoint
    {
        QDateTime validTime;
        //QDateTime endTime;
        //double quantitativePrecipitation;
        //QString windSpeed;
        double value;
        //double relativeHumidity;
        //QString shortForecast;
    };

    struct weatherDatapoint_2
    {
        QDateTime startTime;
        QDateTime endTime;
        double temperature;
        QString windSpeed;
        double probabilityOfPrecipitation;
        double relativeHumidity;
        QString shortForecast;
    };

    QJsonArray getForecastArray(const QString &url);
    QString forecastURL;
    QString forecastURL_2;
    QVector<weatherDatapoint> weatherData;
    QVector<weatherDatapoint_2> weatherData_2;

    QChart *chart;
    QChart *chart_1;
    QChartView *chartView;
    QChartView *chartView_1;
    QLineSeries *series;
    QLineSeries *series_1;
    QDateTimeAxis *axisX;
    QDateTimeAxis *axisX_1;
    QValueAxis *axisY;
    QValueAxis *axisY_1;
    QTimer *timer;
    QTimer *timer1;
    QTimer *timer2;
    QTimer *valveCloseTimer;
    bool isRainExpected = false;
    DistanceSensor DS;

    void setupChart();
    void setupChart_2();
    void updateChartData(const QVector<QPair<QDateTime, double>> &data);

public slots:
    void enable_button();
    void show_json();
    void show_json_2();
    void onlistchanged(QListWidgetItem *item);
    void onlistchanged_2(QListWidgetItem *item);
    void process_forecast_data();
    void process_forecast_data_2();
    void getForecastURL(); 
    void getForecastURL_2();
    void getWeatherPrediction();
    void getWeatherPrediction_2();
    void plotForecast();
    void plotForecast_2();
    void on_click();
    void Turnoff();
    void TurnOn();
    void closeWaterValve();

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_listWidget_contents_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_ProcessJson_clicked();
    void on_comboBox_activated(int index);
    void updateWeatherData();
    void updateWeatherData_2();
    void on_pushButton_Measure_distance_clicked();
};
#endif
