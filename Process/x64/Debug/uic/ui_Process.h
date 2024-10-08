/********************************************************************************
** Form generated from reading UI file 'Process.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESS_H
#define UI_PROCESS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Process
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *addressLabel;
    QLineEdit *addressInput;
    QLabel *valueLabel;
    QLineEdit *valueInput;
    QPushButton *readButton;
    QPushButton *writeButton;
    QTableWidget *memoryBlockTable;
    QTextEdit *logTextEdit;
    QProgressBar *memoryUsageBar;

    void setupUi(QMainWindow *Process)
    {
        if (Process->objectName().isEmpty())
            Process->setObjectName(QString::fromUtf8("Process"));
        Process->resize(452, 447);
        centralWidget = new QWidget(Process);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        addressLabel = new QLabel(centralWidget);
        addressLabel->setObjectName(QString::fromUtf8("addressLabel"));

        verticalLayout->addWidget(addressLabel);

        addressInput = new QLineEdit(centralWidget);
        addressInput->setObjectName(QString::fromUtf8("addressInput"));

        verticalLayout->addWidget(addressInput);

        valueLabel = new QLabel(centralWidget);
        valueLabel->setObjectName(QString::fromUtf8("valueLabel"));

        verticalLayout->addWidget(valueLabel);

        valueInput = new QLineEdit(centralWidget);
        valueInput->setObjectName(QString::fromUtf8("valueInput"));

        verticalLayout->addWidget(valueInput);

        readButton = new QPushButton(centralWidget);
        readButton->setObjectName(QString::fromUtf8("readButton"));

        verticalLayout->addWidget(readButton);

        writeButton = new QPushButton(centralWidget);
        writeButton->setObjectName(QString::fromUtf8("writeButton"));

        verticalLayout->addWidget(writeButton);

        memoryBlockTable = new QTableWidget(centralWidget);
        if (memoryBlockTable->columnCount() < 3)
            memoryBlockTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        memoryBlockTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        memoryBlockTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        memoryBlockTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        memoryBlockTable->setObjectName(QString::fromUtf8("memoryBlockTable"));

        verticalLayout->addWidget(memoryBlockTable);

        logTextEdit = new QTextEdit(centralWidget);
        logTextEdit->setObjectName(QString::fromUtf8("logTextEdit"));

        verticalLayout->addWidget(logTextEdit);

        memoryUsageBar = new QProgressBar(centralWidget);
        memoryUsageBar->setObjectName(QString::fromUtf8("memoryUsageBar"));
        memoryUsageBar->setValue(24);

        verticalLayout->addWidget(memoryUsageBar);

        Process->setCentralWidget(centralWidget);

        retranslateUi(Process);

        QMetaObject::connectSlotsByName(Process);
    } // setupUi

    void retranslateUi(QMainWindow *Process)
    {
        Process->setWindowTitle(QCoreApplication::translate("Process", "Process", nullptr));
        addressLabel->setText(QCoreApplication::translate("Process", "\345\206\205\345\255\230\345\234\260\345\235\200\357\274\232", nullptr));
        valueLabel->setText(QCoreApplication::translate("Process", "\344\274\240\350\276\223\347\232\204\345\200\274\357\274\232", nullptr));
        readButton->setText(QCoreApplication::translate("Process", "\350\257\273\345\217\226", nullptr));
        writeButton->setText(QCoreApplication::translate("Process", "\345\206\231\345\205\245", nullptr));
        QTableWidgetItem *___qtablewidgetitem = memoryBlockTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Process", "\345\234\260\345\235\200", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = memoryBlockTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Process", "\345\244\247\345\260\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = memoryBlockTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Process", "\347\212\266\346\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Process: public Ui_Process {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESS_H
