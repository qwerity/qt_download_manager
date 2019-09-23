#pragma once

#include <QtGlobal>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>

#include "downloadmanagerHTTP.h"

class DownloadManager : public QObject {
Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);

signals:
    void addLine(const QString &qsLine);
    void progress(int nPercentage);
    void downloadComplete();

public slots:
    void download(const QUrl& url);
    void pause();
    void resume();

private:
    QUrl _URL;
    DownloadManagerHTTP *_pHTTP = nullptr;
};
