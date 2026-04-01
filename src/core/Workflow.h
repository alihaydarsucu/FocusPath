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
    std::string icon;


    std::chrono::system_clock::time_point createdAt;


    std::string date;


    void setCurrentDateTime();

public:

    Workflow(long duration, bool isFavorite);


    Workflow(std::string name, long duration, bool isFavorite);

    void addApp(std::string appName);

    void print() const;

    std::string getName() const;
    std::string getDate() const;
    long getDuration() const;
    bool getIsFavorite() const ;
    std::string getIcon() const;
    const std::vector<std::string>& getApps() const ;
    void setDate(const std::string &d);
    void setIcon(const std::string &iconValue);
    static void saveWorkflowToFile(const Workflow &wf, const QString &filePath);
    static Workflow loadWorkflowFromFile(const QString &filePath);


};

#endif
