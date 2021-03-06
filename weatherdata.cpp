#include "weatherdata.h"

WeatherData::WeatherData()
{
    InitUpdateWeatherData();
}

WeatherData::~WeatherData()
{

}

void WeatherData::read()
{
    if (xmlReader.readNextStartElement() && xmlReader.name() == "forecast")
        processForecast();

    if (xmlReader.tokenType() == QXmlStreamReader::Invalid)
        xmlReader.readNext();

    if (xmlReader.hasError()) {
        xmlReader.raiseError();
        qDebug() << errorString();
    }
}

QString WeatherData::GetTemperature()
{
    return temperature;
}

QString WeatherData::GetPressure()
{
    return pressure;
}

QString WeatherData::GetWindSpeed()
{
    return wind_speed;
}

void WeatherData::processForecast()
{
    if (!xmlReader.isStartElement() || xmlReader.name() != "forecast")
        return;
    while (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "fact")
            processFact();
        else
            if (xmlReader.name() == "day")
                processDay();
            else
                xmlReader.skipCurrentElement();
    }
}

void WeatherData::processFact()
{
    if (!xmlReader.isStartElement() || xmlReader.name() != "fact")
        return;

    while (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "pressure")
            pressure = readNextText();
        else if (xmlReader.name() == "wind_speed")
        {
            wind_speed = readNextText();
        }

#ifndef USE_READ_ELEMENT_TEXT
        xmlReader.skipCurrentElement();
#endif
    }
}

void WeatherData::processDay()
{
    if (!xmlReader.isStartElement() || xmlReader.name() != "day")
        return;

    int hour_count = current_hour;
    while (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == "hour" && hour_count > 0)
            hour_count--;
        else if (xmlReader.name() == "hour" && hour_count == 0)
        {
            processHour();
            hour_count = -1;
            break;
        }

#ifndef USE_READ_ELEMENT_TEXT
        xmlReader.skipCurrentElement();
#endif
    }
}

void WeatherData::processHour()
{
    if (!xmlReader.isStartElement() || xmlReader.name() != "hour")
        return;

    while (xmlReader.readNextStartElement()) {
        if(xmlReader.name() == "temperature")
            temperature = readNextText();
    }

}

void WeatherData::InitUpdateWeatherData()
{
    current_hour = QTime::currentTime().hour();
    const QString weatherApi = "http://export.yandex.ru/weather-ng/forecasts/27595.xml";

    QNetworkAccessManager network_mgr;
    QNetworkReply *reply = network_mgr.get(QNetworkRequest(weatherApi));
    QEventLoop loop;
    connect(reply, SIGNAL(finished()),&loop, SLOT(quit()));
    loop.exec();
    xmlReader.addData(reply->readAll());
    read();

    emit trySendWeatherData();
    qDebug() << "signal for weather update is sended";
}

QString WeatherData::readNextText()
{
#ifndef USE_READ_ELEMENT_TEXT
    xmlReader.readNext();
    return xmlReader.text().toString();
#else
    return xmlReader.readElementText();
#endif
}

QString WeatherData::errorString()
{
    return "xml error!";
}






