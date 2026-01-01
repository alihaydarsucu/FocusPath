#ifndef WORKFLOW_H
#define WORKFLOW_H
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <string>
#include <vector>
#include <chrono>

class Workflow {
private:
    std::string name;
    std::vector<std::string> apps;
    long duration;
    bool isFavorite;


    std::chrono::system_clock::time_point createdAt;


    std::string date;


    void setCurrentDateTime();

public:

    Workflow(long duration, bool isFavorite);


    Workflow(std::string name, long duration, bool isFavorite);

    void addApps(std::string appName);

    void print() const;

    std::string getName() const;
    std::string getDate() const;
    long getDuration() const;
    bool getisFavorite() const ;
    const std::vector<std::string>& getApps() const ;
    void setDate(const std::string &d);
    static void saveWorkflowToFile(const Workflow &wf, const QString &filePath);
    static Workflow loadWorkflowFromFile(const QString &filePath);


};

#endif
