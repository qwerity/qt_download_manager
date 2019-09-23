#pragma once

#include "downloadmanager.h"

#include <QtGlobal>
#include <QWidget>

namespace Ui {
    class Form;
}

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget *parent = nullptr);

private:
    Ui::Form *ui = nullptr;
    DownloadManager *mManager;
};
