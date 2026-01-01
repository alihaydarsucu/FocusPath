#include "workflow.h"
#include <ctime>
#include <iostream>

void Workflow::setCurrentDateTime()
{
    createdAt = std::chrono::system_clock::now();

    std::time_t time = std::chrono::system_clock::to_time_t(createdAt);
    std::tm* localTime = std::localtime(&time);

    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

    date = buffer;


    if (name.empty()) {
        name = date;
    }
}


Workflow::Workflow(long duration, bool isFavorite)
    : name(""), duration(duration), isFavorite(isFavorite), icon("")
{
    setCurrentDateTime();
}


Workflow::Workflow(std::string name, long duration, bool isFavorite)
    : name(std::move(name)), duration(duration), isFavorite(isFavorite), icon("")
{
    setCurrentDateTime();
}

void Workflow::addApps(std::string appName)
{
    apps.push_back(appName);
}

const std::vector<std::string>& Workflow::getApps() const {
    return apps;
}

std::string Workflow::getDate() const
{
    return date;
}

std::string Workflow::getName() const
{
    return name;
}

bool Workflow::getisFavorite() const {
    return isFavorite;
}

std::string Workflow::getIcon() const {
    return icon;
}

long Workflow::getDuration() const
{
    return duration;
}

void Workflow::setDate(const std::string &d) {
    date = d;
}

void Workflow::setIcon(const std::string &iconValue) {
    icon = iconValue;
}

void Workflow::print() const
{
    std::cout << name << std::endl;
    std::cout << duration << std::endl;
    std::cout << date << std::endl;
    std::cout << isFavorite << std::endl;
    for(auto app: apps){
        std::cout << app << std::endl;
    }
}
QJsonObject workflowToJson(const Workflow &wf) {
    QJsonObject obj;
    obj["name"] = QString::fromStdString(wf.getName());
    obj["duration"] = static_cast<qint64>(wf.getDuration());
    obj["isFavorite"] = wf.getisFavorite();
    obj["date"] = QString::fromStdString(wf.getDate());
    if (!wf.getIcon().empty()) {
        obj["icon"] = QString::fromStdString(wf.getIcon());
    }

    QJsonArray appsArray;
    for (const std::string &app : wf.getApps()) {
        appsArray.append(QString::fromStdString(app));
    }
    obj["apps"] = appsArray;

    return obj;
}

void Workflow::saveWorkflowToFile(const Workflow &wf, const QString &filePath) {
    QJsonObject obj = workflowToJson(wf);
    QJsonDocument doc(obj);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open file for writing:" << filePath;
        qDebug() << "Error:" << file.errorString();
        return;
    }

    file.write(doc.toJson());
    file.close();
    
    qDebug() << "Workflow saved to:" << filePath;
}

Workflow Workflow::loadWorkflowFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) throw std::runtime_error("Dosya açılamadı!");

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    file.close();

    if (!doc.isObject()) throw std::runtime_error("Geçersiz JSON formatı!");

    QJsonObject obj = doc.object();

    std::string name = obj["name"].toString().toStdString();
    long duration = obj["duration"].toVariant().toLongLong();
    bool isFavorite = obj["isFavorite"].toBool();
    std::string date = obj["date"].toString().toStdString();
    std::string iconValue = obj.contains("icon") ? obj["icon"].toString().toStdString() : "";

    Workflow wf(name, duration, isFavorite);
    wf.setDate(date);
    wf.setIcon(iconValue);

    QJsonArray appsArray = obj["apps"].toArray();
    for (const auto &val : appsArray) {
        wf.addApps(val.toString().toStdString());
    }

    return wf;
}

