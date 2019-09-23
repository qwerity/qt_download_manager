#pragma once

#include <QtGlobal>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>


class DownloadManagerHTTP : public QObject
{
    Q_OBJECT

public:
    explicit DownloadManagerHTTP(QObject *parent = nullptr);
    ~DownloadManagerHTTP() override;

signals:
    void addLine(const QString &qsLine);
    void downloadComplete();
    void progress(int nPercentage);

public slots:
    void download(const QUrl& url);
    void pause();
    void resume();

private slots:
    void download();
    void finishedHead();
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    static void error(QNetworkReply::NetworkError code);
    static void timeout();

private:
    QString _qsFileName;
    QNetworkAccessManager *_pManager = nullptr;
    QNetworkRequest _CurrentRequest;
    QNetworkReply *_pCurrentReply = nullptr;
    QFile *_pFile = nullptr;
    qint64 _nDownloadTotal;
    bool _bAcceptRanges;
    qint64 _nDownloadSize;
    qint64 _nDownloadSizeAtPause;
    QTimer _Timer;
};
