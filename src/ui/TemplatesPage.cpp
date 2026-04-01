#include "TemplatesPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFile>

TemplatesPage::TemplatesPage(QWidget *parent)
    : QWidget(parent), currentSortIndex(0)
{
    setupUI();
    loadFavoriteWorkflows();
    filterAndSortWorkflows();
}

void TemplatesPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 24, 32, 32);
    mainLayout->setSpacing(16);

    // Modern gradient background
    this->setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FAFBFF, stop:0.5 #F0F6FF, stop:1 #E8F4FF); "
        "}"
    );

    // Header section
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *title = new QLabel("Favorites");
    title->setStyleSheet(
        "font-size: 32px; "
        "font-weight: 700; "
        "color: #212529;"
    );
    
    QLabel *subtitle = new QLabel("Access and manage your saved workflow presets.");
    subtitle->setStyleSheet(
        "font-size: 14px; "
        "color: #6C757D;"
    );
    
    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);
    //titleLayout->addStretch();
    
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    // Search and sort section
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("🔍 Search favorites...");
    searchBox->setFixedHeight(40);
    searchBox->setFixedWidth(300);
    searchBox->setStyleSheet(
        "QLineEdit { "
        "  background-color: white; "
        "  border: 2px solid #E8F0F7; "
        "  border-radius: 8px; "
        "  padding: 0 15px; "
        "  font-size: 14px; "
        "  color: #212529; "
        "} "
        "QLineEdit:focus { "
        "  border: 2px solid #0288D1; "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFFFFF, stop:1 #F0F8FF); "
        "}"
    );
    
    connect(searchBox, &QLineEdit::textChanged, this, &TemplatesPage::onSearchTextChanged);

    QLabel *sortLabel = new QLabel("Sort by:");
    sortLabel->setStyleSheet("font-size: 14px; color: #6C757D;");
    
    sortCombo = new QComboBox();
    sortCombo->addItem("Name (A-Z)");
    sortCombo->addItem("Name (Z-A)");
    sortCombo->addItem("Duration (Short-Long)");
    sortCombo->addItem("Duration (Long-Short)");
    sortCombo->addItem("Recently Created");
    sortCombo->setFixedHeight(40);
    sortCombo->setFixedWidth(200);
    sortCombo->setCursor(Qt::PointingHandCursor);
    sortCombo->setStyleSheet(
        "QComboBox { "
        "  background-color: white; "
        "  border: 2px solid #E8F0F7; "
        "  border-radius: 8px; "
        "  padding: 0 15px; "
        "  font-size: 14px; "
        "} "
        "QComboBox:hover { "
        "  border: 2px solid #0288D1; "
        "} "
        "QComboBox:focus { "
        "  border: 2px solid #0288D1; "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFFFFF, stop:1 #F0F8FF); "
        "} "
        "QComboBox::drop-down { "
        "  border: none; "
        "}"
    );
    
    connect(sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &TemplatesPage::onSortChanged);

    controlsLayout->addWidget(searchBox);
    controlsLayout->addStretch();
    controlsLayout->addWidget(sortLabel);
    controlsLayout->addWidget(sortCombo);

    // Scroll area for workflow cards
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;");
    
    cardsContainer = new QWidget();
    cardsLayout = new QGridLayout(cardsContainer);
    cardsLayout->setSpacing(24);
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    
    scrollArea->setWidget(cardsContainer);

    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(scrollArea);
}

void TemplatesPage::loadFavoriteWorkflows() {
    allWorkflows.clear();
    workflowFileMap.clear();
    
    // Resolve workflows directory (works both from build/ and source root)
    QString appDir = QCoreApplication::applicationDirPath();
    QStringList candidateDirs {
        appDir + "/workflows",
        QDir(appDir).absoluteFilePath("../workflows"),
        QDir(appDir).absoluteFilePath("../../workflows")
    };

    QString workflowsDir;
    for (const QString &cand : candidateDirs) {
        if (QDir(cand).exists()) {
            workflowsDir = QDir(cand).absolutePath();
            break;
        }
    }

    if (workflowsDir.isEmpty()) {
        qDebug() << "Workflows directory not found in candidates:" << candidateDirs;
        return;
    }

    QDir dir(workflowsDir);

    // Get all JSON files
    QStringList filters;
    filters << "*.json";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files, QDir::Time);

    for (const QFileInfo &fileInfo : fileList) {
        try {
            Workflow workflow = Workflow::loadWorkflowFromFile(fileInfo.absoluteFilePath());
            
            // Only add favorite workflows
            if (workflow.getIsFavorite()) {
                QString key = QString::fromStdString(workflow.getDate());
                workflowFileMap.insert(key, fileInfo.absoluteFilePath());
                allWorkflows.append(workflow);
            }
        } catch (...) {
            qDebug() << "Error loading workflow from:" << fileInfo.fileName();
        }
    }

    qDebug() << "Loaded" << allWorkflows.size() << "favorite workflows";
}

void TemplatesPage::displayWorkflows() {
    // Clear existing cards
    QLayoutItem *item;
    while ((item = cardsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    if (filteredWorkflows.isEmpty()) {
        // Show "no workflows" message
        QLabel *emptyLabel = new QLabel("No favorite workflows found.\nCreate your first workflow template!");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(
            "font-size: 16px; "
            "color: #999; "
            "padding: 60px;"
        );
        cardsLayout->addWidget(emptyLabel, 0, 0, 1, 3, Qt::AlignCenter);
        return;
    }

    int row = 0;
    int col = 0;
    const int columns = 3;

    for (const Workflow &workflow : filteredWorkflows) {
        WorkflowCard *card = new WorkflowCard(workflow);
        connect(card, &WorkflowCard::startSessionClicked, 
                this, &TemplatesPage::onWorkflowStartRequested);
        connect(card, &WorkflowCard::deleteClicked,
                this, &TemplatesPage::onWorkflowDeleteRequested);
        
        cardsLayout->addWidget(card, row, col);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }

    // Add stretch to push cards to top
    cardsLayout->setRowStretch(row + 1, 1);
}

void TemplatesPage::filterAndSortWorkflows() {
    filteredWorkflows = allWorkflows;

    // Filter by search text
    if (!currentSearchText.isEmpty()) {
        QVector<Workflow> filtered;
        for (const Workflow &wf : filteredWorkflows) {
            QString name = QString::fromStdString(wf.getName()).toLower();
            if (name.contains(currentSearchText.toLower())) {
                filtered.append(wf);
            }
        }
        filteredWorkflows = filtered;
    }

    // Sort based on selection
    switch (currentSortIndex) {
        case 0: // Name A-Z
            std::sort(filteredWorkflows.begin(), filteredWorkflows.end(),
                     [](const Workflow &a, const Workflow &b) {
                         return QString::fromStdString(a.getName()).toLower() < 
                                QString::fromStdString(b.getName()).toLower();
                     });
            break;
        case 1: // Name Z-A
            std::sort(filteredWorkflows.begin(), filteredWorkflows.end(),
                     [](const Workflow &a, const Workflow &b) {
                         return QString::fromStdString(a.getName()).toLower() > 
                                QString::fromStdString(b.getName()).toLower();
                     });
            break;
        case 2: // Duration Short-Long
            std::sort(filteredWorkflows.begin(), filteredWorkflows.end(),
                     [](const Workflow &a, const Workflow &b) {
                         return a.getDuration() < b.getDuration();
                     });
            break;
        case 3: // Duration Long-Short
            std::sort(filteredWorkflows.begin(), filteredWorkflows.end(),
                     [](const Workflow &a, const Workflow &b) {
                         return a.getDuration() > b.getDuration();
                     });
            break;
        case 4: // Recently Created
            std::sort(filteredWorkflows.begin(), filteredWorkflows.end(),
                     [](const Workflow &a, const Workflow &b) {
                         return QString::fromStdString(a.getDate()) > 
                                QString::fromStdString(b.getDate());
                     });
            break;
    }

    displayWorkflows();
}

void TemplatesPage::onSearchTextChanged(const QString &text) {
    currentSearchText = text;
    filterAndSortWorkflows();
}

void TemplatesPage::onSortChanged(int index) {
    currentSortIndex = index;
    filterAndSortWorkflows();
}

void TemplatesPage::onWorkflowStartRequested(const Workflow &workflow) {
    // Create a mutable copy to pass to the signal
    Workflow mutableWorkflow = workflow;
    emit startWorkflow(mutableWorkflow);
}

void TemplatesPage::onWorkflowDeleteRequested(const Workflow &workflow) {
    QMessageBox msg(this);
    msg.setWindowTitle("Delete Favorite");
    msg.setText("Are you sure you want to delete this favorite workflow?");
    msg.setInformativeText(QString::fromStdString(workflow.getName()));
    msg.setIcon(QMessageBox::Warning);

    QPushButton *yesBtn = msg.addButton("Delete", QMessageBox::DestructiveRole);
    msg.addButton("Cancel", QMessageBox::RejectRole);
    msg.exec();

    if (msg.clickedButton() != yesBtn) return;

    QString key = QString::fromStdString(workflow.getDate());
    QString path = workflowFileMap.value(key);

    if (path.isEmpty()) {
        // fallback: look up by name
        for (auto it = workflowFileMap.constBegin(); it != workflowFileMap.constEnd(); ++it) {
            if (it.key().contains(QString::fromStdString(workflow.getName()))) {
                path = it.value();
                break;
            }
        }
    }

    if (!path.isEmpty()) {
        QFile::remove(path);
    }

    refreshWorkflows();
}

void TemplatesPage::refreshWorkflows() {
    loadFavoriteWorkflows();
    filterAndSortWorkflows();
}
