#include "downloadmanagerHTTP.h"
#include "downloadwidget.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDebug>


DownloadManagerHTTP::DownloadManagerHTTP(QObject *parent) :
        QObject(parent), _pManager(nullptr), _pCurrentReply(nullptr), _pFile(nullptr), _nDownloadTotal(0),
        _bAcceptRanges(false), _nDownloadSize(0), _nDownloadSizeAtPause(0) {
}


DownloadManagerHTTP::~DownloadManagerHTTP() {
    if (_pCurrentReply != nullptr) {
        pause();
    }
}


void DownloadManagerHTTP::download(const QUrl& url) {
    qDebug() << "download: URL=" << url.toString();

    {
        QFileInfo fileInfo(url.toString());
        _qsFileName = fileInfo.fileName();
    }
    _nDownloadSize = 0;
    _nDownloadSizeAtPause = 0;

    _pManager = new QNetworkAccessManager(this);
    _CurrentRequest = QNetworkRequest(url);

    _pCurrentReply = _pManager->head(_CurrentRequest);

    _Timer.setInterval(5000);
    _Timer.setSingleShot(true);
    connect(&_Timer, SIGNAL(timeout()), this, SLOT(timeout()));
    _Timer.start();

    connect(_pCurrentReply, &QNetworkReply::finished, this, &DownloadManagerHTTP::finishedHead);
    connect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}


void DownloadManagerHTTP::pause() {
    qDebug() << "pause() = " << _nDownloadSize;
    if (_pCurrentReply == nullptr) {
        return;
    }
    _Timer.stop();
    disconnect(&_Timer, SIGNAL(timeout()), this, SLOT(timeout()));
    disconnect(_pCurrentReply, &QNetworkReply::finished, this, &DownloadManagerHTTP::finished);
    disconnect(_pCurrentReply, &QNetworkReply::downloadProgress, this, &DownloadManagerHTTP::downloadProgress);
    disconnect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    _pCurrentReply->abort();
//    _pFile->write( _pCurrentReply->readAll());
    _pFile->flush();
    _pCurrentReply = nullptr;
    _nDownloadSizeAtPause = _nDownloadSize;
    _nDownloadSize = 0;
}


void DownloadManagerHTTP::resume() {
    qDebug() << "resume() = " << _nDownloadSizeAtPause;

    download();
}


void DownloadManagerHTTP::download() {
    qDebug() << "download()";

    if (_bAcceptRanges) {
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(_nDownloadSizeAtPause) + "-";
        if (_nDownloadTotal > 0) {
            rangeHeaderValue += QByteArray::number(_nDownloadTotal);
        }
        _CurrentRequest.setRawHeader("Range", rangeHeaderValue);
    }

    _pCurrentReply = _pManager->get(_CurrentRequest);

    _Timer.setInterval(5000);
    _Timer.setSingleShot(true);
    connect(&_Timer, SIGNAL(timeout()), this, SLOT(timeout()));
    _Timer.start();

    connect(_pCurrentReply, &QNetworkReply::finished, this, &DownloadManagerHTTP::finished);
    connect(_pCurrentReply, &QNetworkReply::downloadProgress, this, &DownloadManagerHTTP::downloadProgress);
    connect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}


void DownloadManagerHTTP::finishedHead() {
    _Timer.stop();
    _bAcceptRanges = false;

    QList<QByteArray> list = _pCurrentReply->rawHeaderList();
            foreach (QByteArray header, list) {
            QString qsLine = QString(header) + " = " + _pCurrentReply->rawHeader(header);
            addLine(qsLine);
        }

    if (_pCurrentReply->hasRawHeader("Accept-Ranges")) {
        QString qstrAcceptRanges = _pCurrentReply->rawHeader("Accept-Ranges");
        _bAcceptRanges = (qstrAcceptRanges.compare("bytes", Qt::CaseInsensitive) == 0);
        qDebug() << "Accept-Ranges = " << qstrAcceptRanges << _bAcceptRanges;
    }

    _nDownloadTotal = _pCurrentReply->header(QNetworkRequest::ContentLengthHeader).toInt();

//    _CurrentRequest = QNetworkRequest(url);
    _CurrentRequest.setRawHeader("Connection", "Keep-Alive");
    _CurrentRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    _pFile = new QFile(_qsFileName + ".part");
    if (!_bAcceptRanges) {
        _pFile->remove();
    }
    _pFile->open(QIODevice::ReadWrite | QIODevice::Append);

    _nDownloadSizeAtPause = _pFile->size();
    download();
}


void DownloadManagerHTTP::finished() {
    qDebug() << __FUNCTION__;

    _Timer.stop();
    _pFile->close();
    QFile::remove(_qsFileName);
    QFile::rename(_qsFileName + ".part", _qsFileName);
    _pFile = nullptr;
    _pCurrentReply = nullptr;
    emit downloadComplete();
}


void DownloadManagerHTTP::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    _Timer.stop();
    _nDownloadSize = _nDownloadSizeAtPause + bytesReceived;
    qDebug() << "Download Progress: Received=" << _nDownloadSize << ": Total=" << _nDownloadSizeAtPause + bytesTotal;

    _pFile->write(_pCurrentReply->readAll());
    int nPercentage = static_cast<int>((static_cast<float>(_nDownloadSizeAtPause + bytesReceived) * 100.0) /
                                       static_cast<float>(_nDownloadSizeAtPause + bytesTotal));
    qDebug() << nPercentage;
    emit progress(nPercentage);

    _Timer.start(5000);
}


void DownloadManagerHTTP::error(QNetworkReply::NetworkError code) {
    qDebug() << __FUNCTION__ << "(" << code << ")";
}


void DownloadManagerHTTP::timeout() {
    qDebug() << __FUNCTION__;
}
