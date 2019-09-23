#include "downloadmanager.h"
#include "downloadwidget.h"

#include <QFileInfo>
#include <QDateTime>
#include <utility>


DownloadManager::DownloadManager(QObject *parent) :
        QObject(parent), _pHTTP(nullptr) {
}


void DownloadManager::download(const QUrl& url) {
    qDebug() << __FUNCTION__ << "(" << url.toString() << ")";

    _URL = url;

    {
        _pHTTP = new DownloadManagerHTTP(this);

        connect(_pHTTP, &DownloadManagerHTTP::addLine, this, &DownloadManager::addLine);
        connect(_pHTTP, &DownloadManagerHTTP::progress, this, &DownloadManager::progress);
        connect(_pHTTP, &DownloadManagerHTTP::downloadComplete, this, &DownloadManager::downloadComplete);

        _pHTTP->download(_URL);
    }
}


void DownloadManager::pause()
{
    qDebug() << __FUNCTION__;
    {
        _pHTTP->pause();
    }
}


void DownloadManager::resume()
{
    qDebug() << __FUNCTION__;
    {
        _pHTTP->resume();
    }
}
