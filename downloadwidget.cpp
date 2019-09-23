#include "downloadwidget.h"
#include "ui_form.h"
#include <QProgressBar>
#include <QHBoxLayout>
#include <QStandardItemModel>


DownloadWidget::DownloadWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Form)
{
    ui->setupUi(this);

    ui->urlEdit->setText("http://mirrors.dotsrc.org/qtproject/official_releases/qt/5.13/5.13.1/qt-opensource-windows-x86-5.13.1.exe");

    auto *model = new QStandardItemModel(0, 1, this);
    ui->listView->setModel(model);

    mManager = new DownloadManager(this);

    connect(mManager, &DownloadManager::addLine, [&](const QString& qsLine){
        int nRow = ui->listView->model()->rowCount();
        ui->listView->model()->insertRow(nRow);
        ui->listView->model()->setData(ui->listView->model()->index(nRow, 0), qsLine);
    });

    connect(mManager, &DownloadManager::downloadComplete, [&](){
        ui->downloadUrl->setEnabled(true);
        ui->pauseDownload->setEnabled(false);
        ui->resumeDownload->setEnabled(false);
    });

    connect(mManager, &DownloadManager::progress, [&](int nPercentage){
        ui->progressBar->setValue(nPercentage);
    });

    connect(ui->downloadUrl, &QPushButton::clicked, [&](){
        ui->listView->reset();

        QUrl url(ui->urlEdit->text());
        mManager->download(url);
        ui->downloadUrl->setEnabled(false);
        ui->pauseDownload->setEnabled(true);
    });

    connect(ui->pauseDownload, &QPushButton::clicked, [&](){
        mManager->pause();
        ui->pauseDownload->setEnabled(false);
        ui->resumeDownload->setEnabled(true);
    });

    connect(ui->resumeDownload, &QPushButton::clicked, [&](){
        mManager->resume();
        ui->pauseDownload->setEnabled(true);
        ui->resumeDownload->setEnabled(false);
    });
}
