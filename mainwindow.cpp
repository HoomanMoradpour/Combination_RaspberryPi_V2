#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QSplitter>
#include <QVBoxLayout>
#include <wiringPi.h>
#include <iostream>
#include <unistd.h> // for sleep()
#include <QTimer>
#include <QDebug>
//#include "DistanceSensor.h"
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

    , chart(new QChart())
    , chartView(new QChartView(chart, this))
    , series(new QLineSeries())
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    ui->ProcessJson->setEnabled(true);

    inigetForecastURL("https://api.weather.gov/points/47.2588,-121.3152");
    inigetForecastURL_2("https://api.weather.gov/points/47.2588,-121.3152");

    connect(ui->ProcessJson, SIGNAL(clicked()), this, SLOT(show_json()));
    connect(ui->listWidget_2, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onlistchanged(QListWidgetItem*)));
    connect(ui->listWidget_3, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onlistchanged(QListWidgetItem*)));

    connect(timer, &QTimer::timeout, this, &MainWindow::updateWeatherData);
    ui->pushButton_Measure_distance->setText("Water Level");
    connect(ui->pushButton_Measure_distance, SIGNAL(clicked()), this, SLOT(on_pushButton_Measure_distance_clicked()));


    ui->pushButton->setText("Turn on");
    connect(ui->pushButton, SIGNAL(clicked()),this, SLOT(on_click()));
    timer1 = new QTimer(this);
    timer2= new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(Turnoff()));
    connect(timer2, SIGNAL(timeout()), this, SLOT(TurnOn()));
    valveCloseTimer = new QTimer(this);
    connect(valveCloseTimer, &QTimer::timeout, this, &MainWindow::closeWaterValve);

    //Prevent warnings from GPIO
    wiringPiSetupGpio(); // Use BCM pin numbering

    DistanceSensor sensor;
   if (!sensor.initialize()) {
        qDebug() << "Something is wrong with the distance sensor!";
  }
   else
  {
      double distance = sensor.getDistance();
       qDebug() << "Measured Distance =" << distance << "cm";
    }

    sensor.cleanup();
    DS.initialize();
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_click()
{
    if (on)
    {
        on = false;
        digitalWrite(18, HIGH);
        ui->pushButton->setText("Turn on");
        //qDebug()<<"Turned off by the user";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Turned off by the user" );
        timer2->start(10000);
    }
    else
    {
        on = true;
        digitalWrite(18, LOW);
        ui->pushButton->setText("Turn off");
        //qDebug()<<"Turned on by the user";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Turned on by the user" );
        timer1->start(10000);
    }
}

void MainWindow::Turnoff()
{
    if (on)
    {
        timer1->stop();
        on = false;
        digitalWrite(18, HIGH);
        //qDebug() << "Turned off by timer";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Turned off by timer" );
        ui->pushButton->setText("Turn on");
        timer2->start(10000);
    }
}

void MainWindow::TurnOn()
{
    if (!on)
    {   timer2->stop();
        on = true;
        digitalWrite(18, LOW);
        ui->pushButton->setText("Turn off");
        //qDebug() << "Turned on by timer";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Turned on by timer" );
        timer1->start(10000);
    }
}




void MainWindow::setupChart()
{
    series = new QLineSeries();

    for (int i = 0; i < weatherData.count(); i++)
    {
        QDateTime momentInTime = weatherData[i].validTime;
        series->append(momentInTime.toMSecsSinceEpoch(), weatherData[i].value);
    }

    chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setTitle("Quantitative of Precipitation Over Time");

    axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("dd-MM-yyyy HH:mm");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    axisX->setLabelsAngle(-90);
    series->attachAxis(axisX);

    axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Quantitative of Precipitation (%)");
    axisY->setRange(0, 100);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);



    QVBoxLayout *layout = new QVBoxLayout(this);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    splitter->addWidget(ui->listWidget_2);
    splitter->addWidget(chartView);
    layout->addWidget(splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 7);
    //centralWidget()->setLayout(layout);
    ui->chartLayout_4->addWidget(chartView);
    resize(800, 1050);

    show();
}



void MainWindow::setupChart_2()
{
    series = new QLineSeries();

    for (int i = 0; i < weatherData.count(); i++)
    {
        QDateTime momentInTime = weatherData[i].validTime;
        series->append(momentInTime.toMSecsSinceEpoch(), weatherData[i].value);
    }

    chart_1 = new QChart();
    chart_1->addSeries(series_1);
    chart_1->legend()->hide();
    chart_1->setTitle("Probability of Precipitation Over Time");

    axisX_1 = new QDateTimeAxis;
    axisX_1->setTickCount(10);
    axisX_1->setFormat("dd-MM-yyyy HH:mm");
    axisX_1->setTitleText("Date");
    chart_1->addAxis(axisX_1, Qt::AlignBottom);
    axisX_1->setLabelsAngle(-90);
    series_1->attachAxis(axisX_1);

    axisY_1 = new QValueAxis;
    axisY_1->setLabelFormat("%i");
    axisY_1->setTitleText("Probability of Precipitation (%)");
    axisY_1->setRange(0, 100);
    chart_1->addAxis(axisY_1, Qt::AlignLeft);
    series_1->attachAxis(axisY_1);

    chartView_1 = new QChartView(chart_1);
    chartView_1->setRenderHint(QPainter::Antialiasing);



    QVBoxLayout *layout = new QVBoxLayout(this);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    splitter->addWidget(ui->listWidget_3);
    splitter->addWidget(chartView_1);
    layout->addWidget(splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 7);
    //centralWidget()->setLayout(layout);
    ui->chartLayout_4->addWidget(chartView_1);
    resize(800, 1050);

    show();
}






void MainWindow::updateChartData(const QVector<QPair<QDateTime, double>> &data)
{
    series->clear();

    for (const auto &point : data) {
        qreal time = point.first.toMSecsSinceEpoch();
        series->append(time, point.second);
    }

    if (!data.isEmpty()) {
        qDebug()<<data.first().first<<":"<<data.last().first;
        axisX->setRange(data.first().first, data.last().first);
        axisY->setRange(0, 100);
    }
}

void MainWindow::plotForecast()
{
    QVector<QPair<QDateTime, double>> forecastData;

    for (const auto &dataPoint : weatherData) {
        forecastData.append(qMakePair(dataPoint.validTime, dataPoint.value));
    }

    updateChartData(forecastData);
}

void MainWindow::enable_button()
{
    ui->ProcessJson->setEnabled(true);
}

void MainWindow::show_json()
{
    ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Graph updated" );
    //ui->textEdit->setText("\nGraphUpdated");
    //qDebug() << ui->textEdit->toPlainText();
    //qDebug() << ui->textEdit->toPlainText() + "\nGraph updated";
    QJsonObject jsonobject = downloader->loadedJson.object();
    qDebug() << jsonobject;

    forecastArray = jsonobject.value("properties").toObject().value("forecastGridData").toArray();

    QString forecastUrl = jsonobject.value("properties").toObject().value("quantitativePrecipitation").toString();
    qDebug() << "Forecast URL: " << forecastUrl;
    connect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(process_forecast_data()));

    if (!forecastUrl.isEmpty()) {

        disconnect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(enable_button()));


        downloader->setUrl(forecastUrl);
        downloader->execute();

    }

    showDatainList();
    //setupChart();
    plotForecast();
}

void MainWindow::show_json_2()
{
    ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Graph updated" );
    //ui->textEdit->setText("\nGraphUpdated");
    //qDebug() << ui->textEdit->toPlainText();
    //qDebug() << ui->textEdit->toPlainText() + "\nGraph updated";
    QJsonObject jsonobject = downloader_2->loadedJson.object();
    qDebug() << jsonobject;

    forecastArray = jsonobject.value("properties").toObject().value("forecastGridData").toArray();

    QString forecastUrl = jsonobject.value("properties").toObject().value("quantitativePrecipitation").toString();
    qDebug() << "Forecast URL: " << forecastUrl;
    connect(downloader_2, SIGNAL(download_finished_sgnl()), this, SLOT(process_forecast_data()));

    if (!forecastUrl.isEmpty()) {

        disconnect(downloader_2, SIGNAL(download_finished_sgnl()), this, SLOT(enable_button()));


        downloader_2->setUrl(forecastUrl);
        downloader_2->execute();

    }

    showDatainList_2();
    //setupChart();
    plotForecast();
}




















void MainWindow::showDatainList() {
    ui->listWidget_contents_2->clear();
    for (const auto &dataPoint : weatherData) {

        QString itemText = QString("ValidTime: %1, Value: %2") //Temperature: %3, windSpeed: %4, Probability of Precipitation: %5, Relative Humidity: %6, shortForecast: %7")
                               .arg(dataPoint.validTime.toString(Qt::ISODate))
                               //.arg(dataPoint.endTime.toString(Qt::ISODate))
                               //.arg(dataPoint.quantitativePrecipitation)
                               //.arg(dataPoint.windSpeed)
                               .arg(dataPoint.value)
            //.arg(dataPoint.relativeHumidity)
            //.arg(dataPoint.shortForecast)
            ;

        ui->listWidget_contents_2->addItem(itemText);
    }
}


void MainWindow::showDatainList_2() {
    ui->listWidget_contents_3->clear();
    for (const auto &dataPoint : weatherData_2) {

        QString itemText = QString("Start Time: 1%, End Time: %2, Temperature: %3, windSpeed: %4, Probability of Precipitation: %5, Relative Humidity: %6, shortForecast: %7")
                               .arg(dataPoint.startTime.toString(Qt::ISODate))
                               .arg(dataPoint.endTime.toString(Qt::ISODate))
                               .arg(dataPoint.temperature)
                               .arg(dataPoint.windSpeed)
                               .arg(dataPoint.probabilityOfPrecipitation)
                               .arg(dataPoint.relativeHumidity)
                               .arg(dataPoint.shortForecast)
            ;
        ui->listWidget_contents_3->addItem(itemText);
    }
}




void MainWindow::process_forecast_data()
{
    connect(downloader, &Downloader::download_finished_sgnl, this, &MainWindow::process_forecast_data);

    //connect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(process_forecast_data()));

    QJsonObject forecastJson = downloader->loadedJson.object();
    qDebug() << forecastJson;

    QJsonArray hourlyForecastArray = forecastJson.value("properties").toObject().value("quantitativePrecipitation").toArray();
    weatherData.clear();

    bool rainExpected = false;

    for (const QJsonValue &value : hourlyForecastArray) {
        QJsonObject forecastObj = value.toObject();
        weatherDatapoint dataPoint;
        dataPoint.validTime = QDateTime::fromString(forecastObj.value("validTime").toString(), Qt::ISODate);
        dataPoint.value = forecastObj.value("value").toDouble();

        if (dataPoint.value > 0) {
            rainExpected = true;
        }

        weatherData.append(dataPoint);
    }

    if (rainExpected) {
        qDebug() << "Rain is expected. Closing the water valve.";
        digitalWrite(18, HIGH);

    } else {
        qDebug() << "No rain expected. Opening the water valve.";
        digitalWrite(18, LOW);
        valveCloseTimer->start(5000);
    }


    QJsonValue forecastGridData = forecastJson.value("properties").toObject().value("quantitativePrecipitation");

    if (forecastGridData.isArray())
    {
        QJsonArray hourlyForecastArray = forecastGridData.toArray();
        ui->listWidget_2->clear();
        weatherData.clear();

        for (const QJsonValue &value : hourlyForecastArray) {
            QJsonObject forecastObj = value.toObject();
            weatherDatapoint dataPoint;

            dataPoint.validTime = QDateTime::fromString(forecastObj.value("validTime").toString(), Qt::ISODate);
            //dataPoint.endTime = QDateTime::fromString(forecastObj.value("endTime").toString(), Qt::ISODate);

            //if (!dataPoint.validTime.isValid() || !dataPoint.endTime.isValid()) {
            qDebug() << "Invalid date-time format in JSON.";
        }

        //dataPoint.temperature = forecastObj.value("temperature").toDouble();
        //dataPoint.windSpeed = forecastObj.value("windSpeed").toString();
        //dataPoint.quantitativePrecipitation = forecastObj.value("quantitativePrecipitation").toObject().value("value").toDouble();
        //dataPoint.relativeHumidity = forecastObj.value("relativeHumidity").toObject().value("value").toDouble();
        //dataPoint.shortForecast = forecastObj.value("shortForecast").toString();


        //qDebug() << "Weather Data Size:" << weatherData.size();
        //for (const auto &dataPoint : weatherData) {
        // qDebug() << "Start Time:" << dataPoint.validTime;
        //qDebug() << "End Time:" << dataPoint.endTime;
        // qDebug() << "Temperature:" << dataPoint.temperature;
        //qDebug() << "Wind Speed:" << dataPoint.windSpeed;
        //qDebug() << "Probability of Precipitation:" << dataPoint.quantitativePrecipitation;
        //qDebug() << "Relative Humidity:" << dataPoint.relativeHumidity;
        // qDebug() << "Short Forecast:" << dataPoint.shortForecast;
    }

    //weatherData.append(dataPoint);
    setupChart();
    plotForecast();
    showDatainList();
}

void MainWindow::process_forecast_data_2()
{
    connect(downloader, &Downloader::download_finished_sgnl, this, &MainWindow::process_forecast_data);

    //connect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(process_forecast_data()));

    QJsonObject forecastJson = downloader_2->loadedJson.object();
    qDebug() << forecastJson;

    QJsonValue forecastGridData = forecastJson.value("properties").toObject().value("quantitativePrecipitation");

    if (forecastGridData.isArray())
    {
        QJsonArray hourlyForecastArray = forecastGridData.toArray();
        ui->listWidget_3->clear();
        weatherData_2.clear();

        for (const QJsonValue &value : hourlyForecastArray) {
                  QJsonObject forecastObj = value.toObject();
                  weatherDatapoint_2 dataPoint;

                  dataPoint.startTime = QDateTime::fromString(forecastObj.value("startTime").toString(), Qt::ISODate);
                  dataPoint.endTime = QDateTime::fromString(forecastObj.value("endTime").toString(), Qt::ISODate);

                  if (!dataPoint.startTime.isValid() || !dataPoint.endTime.isValid()) {
                      qDebug() << "Invalid date-time format in JSON.";
                  }

        dataPoint.temperature = forecastObj.value("temperature").toDouble();
        dataPoint.windSpeed = forecastObj.value("windSpeed").toString();
        dataPoint.probabilityOfPrecipitation = forecastObj.value("probabilityOfPrecipitation").toObject().value("value").toDouble();
        dataPoint.relativeHumidity = forecastObj.value("relativeHumidity").toObject().value("value").toDouble();
        dataPoint.shortForecast = forecastObj.value("shortForecast").toString();


        qDebug() << "Weather Data Size:" << weatherData_2.size();
        for (const auto &dataPoint : weatherData_2) {
        qDebug() << "Start Time:" << dataPoint.startTime;
        qDebug() << "End Time:" << dataPoint.endTime;
        qDebug() << "Temperature:" << dataPoint.temperature;
        qDebug() << "Wind Speed:" << dataPoint.windSpeed;
        qDebug() << "Probability of Precipitation:" << dataPoint.probabilityOfPrecipitation;
        qDebug() << "Relative Humidity:" << dataPoint.relativeHumidity;
        qDebug() << "Short Forecast:" << dataPoint.shortForecast;
    }

    //weatherData.append(dataPoint);


    setupChart_2();
    plotForecast_2();
    showDatainList_2();
    }
}






//   plotForecast();
//} //else {
//qDebug() << "Unexpected data format for forecast hourly";
//qDebug() << "Calling showDatainList()";
//showDatainList();

//}
//}


//void MainWindow:: closeWaterValve() {
    //  qDebug() << "Closing the water valve after 20 minutes.";
    //   digitalWrite(18, HIGH);
     //  valveCloseTimer->stop();
//}



void MainWindow::onlistchanged(QListWidgetItem *item)
{
    ui->listWidget_contents_2->clear();
    QJsonValue selectedValue = jsonValues[item->text()];

    if (selectedValue.isString()) {
        ui->lineEdit->setText(selectedValue.toString());
    } else if (selectedValue.isObject()) {
        QJsonObject jsonObject = selectedValue.toObject();
        foreach (const QString &key, jsonObject.keys()) {
            QString valueStr = jsonObject.value(key).toString();
            ui->listWidget_contents_2->addItem(key + ": " + valueStr);
        }
    } else if (selectedValue.isArray()) {
        QJsonArray jsonArray = selectedValue.toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            QJsonObject jsonObject = jsonArray[i].toObject();
            foreach (const QString &key, jsonObject.keys()) {
                QString valueStr = jsonObject.value(key).toString();
                ui->listWidget_contents_2->addItem(key + ": " + valueStr);
            }
        }
    } else if (selectedValue.isBool()) {
        ui->lineEdit->setText("Boolean value: " + QString::number(selectedValue.toBool()));
    } else {
        ui->lineEdit->setText("Unsupported JSON value type");
    }
}


void MainWindow::onlistchanged_2(QListWidgetItem *item)
{
    ui->listWidget_contents_3->clear();
    QJsonValue selectedValue = jsonValues[item->text()];

    if (selectedValue.isString()) {
        ui->lineEdit->setText(selectedValue.toString());
    } else if (selectedValue.isObject()) {
        QJsonObject jsonObject = selectedValue.toObject();
        foreach (const QString &key, jsonObject.keys()) {
            QString valueStr = jsonObject.value(key).toString();
            ui->listWidget_contents_3->addItem(key + ": " + valueStr);
        }
    } else if (selectedValue.isArray()) {
        QJsonArray jsonArray = selectedValue.toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            QJsonObject jsonObject = jsonArray[i].toObject();
            foreach (const QString &key, jsonObject.keys()) {
                QString valueStr = jsonObject.value(key).toString();
                ui->listWidget_contents_3->addItem(key + ": " + valueStr);
            }
        }
    } else if (selectedValue.isBool()) {
        ui->lineEdit->setText("Boolean value: " + QString::number(selectedValue.toBool()));
    } else {
        ui->lineEdit->setText("Unsupported JSON value type");
    }
}



void MainWindow::inigetForecastURL(const QString &url)
{
    downloader = new Downloader(url);
    connect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(getForecastURL()));
}

void MainWindow::inigetForecastURL_2(const QString &url_2)
{
    downloader_2 = new Downloader(url);
    connect(downloader_2, SIGNAL(download_finished_sgnl()), this, SLOT(getForecastURL_2()));
}

void MainWindow::getForecastURL()
{
    forecastURL = downloader->loadedJson.object().value("properties").toObject().value("forecastGridData").toString();
    //delete downloader;
    downloaderGetForcast = new Downloader(forecastURL, this);
    connect(downloaderGetForcast, SIGNAL(download_finished_sgnl()), this, SLOT(getWeatherPrediction()));
}


void MainWindow::getForecastURL_2()
{
    forecastURL = downloader_2->loadedJson.object().value("properties").toObject().value("forecastGridData").toString();
    //delete downloader;
    downloaderGetForcast_2 = new Downloader(forecastURL, this);
    connect(downloaderGetForcast_2, SIGNAL(download_finished_sgnl()), this, SLOT(getWeatherPrediction()));
}





void MainWindow::getWeatherPrediction()
{
    if (!downloaderGetForcast || downloaderGetForcast->loadedJson.isNull()) {
        qDebug() << "No valid JSON data loaded.";
        return;
    }

    QJsonObject jsonObject = downloaderGetForcast->loadedJson.object();
    qDebug() << jsonObject;

    QJsonObject propObj = jsonObject.value("properties").toObject();
    qDebug() << propObj;

    QJsonArray periodsArray = propObj.value("quantitativePrecipitation").toObject().value("values").toArray();
    qDebug() << periodsArray;



    for (const QJsonValue &value : periodsArray) {
        QJsonObject forecastObj = value.toObject();

        weatherDatapoint dataPoint;

        qDebug()<<forecastObj.value("value");
        qDebug()<<forecastObj.value("validTime").toString().split("+")[0];
        dataPoint.validTime = QDateTime::fromString(forecastObj.value("validTime").toString().split("+")[0], Qt::ISODate);

        //dataPoint.endTime = QDateTime::fromString(forecastObj.value("endTime").toString(), Qt::ISODate);
        //dataPoint.temperature = forecastObj.value("temperature").toDouble();
        //dataPoint.windSpeed = forecastObj.value("windSpeed").toString();
        dataPoint.value = forecastObj.value("value").toDouble();
        //dataPoint.relativeHumidity = forecastObj.value("relativeHumidity").toObject().value("value").toDouble();
        //dataPoint.shortForecast = forecastObj.value("shortForecast").toString();



        //qDebug() << "Start Time:" << dataPoint.validTime;
        // qDebug() << "End Time:" << dataPoint.endTime;
        //qDebug() << "Temperature:" << dataPoint.temperature;
        //qDebug() << "Wind Speed:" << dataPoint.windSpeed;
        //qDebug() << "Probability of Precipitation:" << dataPoint.quantitativePrecipitation;
        // qDebug() << "Relative Humidity:" << dataPoint.relativeHumidity;
        //qDebug() << "Short Forecast:" << dataPoint.shortForecast;

        weatherData.append(dataPoint);
    }

}
  setupChart();
  plotForecast();
  showDatainList();

timer->start(10000);
}


void MainWindow::getWeatherPrediction_2()
    {
        if (!downloaderGetForcast_2 || downloaderGetForcast_2->loadedJson.isNull()) {
            qDebug() << "No valid JSON data loaded.";
            return;
        }

        QJsonObject jsonObject = downloaderGetForcast_2->loadedJson.object();
        qDebug() << jsonObject;

        QJsonObject propObj = jsonObject.value("properties").toObject();
        qDebug() << propObj;

        QJsonArray periodsArray = propObj.value("quantitativePrecipitation").toObject().value("values").toArray();
        qDebug() << periodsArray;

    for (const QJsonValue &value : periodsArray) {
        QJsonObject forecastObj = value.toObject();

        weatherDatapoint_2 dataPoint;

        qDebug()<<forecastObj.value("value");
        qDebug()<<forecastObj.value("validTime").toString().split("+")[0];
        //dataPoint.validTime = QDateTime::fromString(forecastObj.value("validTime").toString().split("+")[0], Qt::ISODate);
        dataPoint.startTime = QDateTime::fromString(forecastObj.value("startTime").toString(), Qt::ISODate);
        dataPoint.endTime = QDateTime::fromString(forecastObj.value("endTime").toString(), Qt::ISODate);
        dataPoint.temperature = forecastObj.value("temperature").toDouble();
        dataPoint.windSpeed = forecastObj.value("windSpeed").toString();
        //dataPoint.value = forecastObj.value("value").toDouble();
        dataPoint.relativeHumidity = forecastObj.value("relativeHumidity").toObject().value("value").toDouble();
        dataPoint.shortForecast = forecastObj.value("shortForecast").toString();



        qDebug() << "Start Time:" << dataPoint.startTime;
        qDebug() << "End Time:" << dataPoint.endTime;
        qDebug() << "Temperature:" << dataPoint.temperature;
        qDebug() << "Wind Speed:" << dataPoint.windSpeed;
        qDebug() << "Probability of Precipitation:" << dataPoint.probabilityOfPrecipitation;
        qDebug() << "Relative Humidity:" << dataPoint.relativeHumidity;
        qDebug() << "Short Forecast:" << dataPoint.shortForecast;

        weatherData_2.append(dataPoint);
    }

    setupChart_2();
    plotForecast_2();
    showDatainList_2();

    timer->start(10000);
}


void MainWindow::updateWeatherData()
{
    qDebug() << "Updating weather data...";


    downloader->setUrl(forecastURL);
    downloader->execute();
}

void MainWindow::updateWeatherData_2()
{
    qDebug() << "Updating weather data...";


    downloader_2->setUrl(forecastURL_2);
    downloader_2->execute();
}

void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}

void MainWindow::on_listWidget_contents_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}

void MainWindow::on_ProcessJson_clicked()
{

}

void MainWindow::on_comboBox_activated(int index)
{

}






void MainWindow::on_pushButton_Measure_distance_clicked()
{

    double distance = DS.getDistance();
    ui->pushButton_Measure_distance;
    ui->label->setText("Water Level: "  +  (QString::number(distance) +  " Cm" ));
}
