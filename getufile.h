#ifndef GETUFILE_H
#define GETUFILE_H

#include <QWidget>
#include <QDebug>
#include<QStorageInfo>
#include<QString>
#include<QException>
#include<windows.h>    //windows下调用识别可移动的盘
#include <dbt.h>       //windows磁盘的插入和退出用到
#include<QDateTime>
#include<QSettings>
#include<QCoreApplication>
#include "JlCompress.h"


class GetUFile : public QWidget
{
    Q_OBJECT

public:
    GetUFile(QWidget *parent = 0);
    ~GetUFile();
    void test();
    QStringList getRemoveDisk();
    QString getDestFileName(const QString& src_file_name);
    void readPath(QString path);
    bool copyFile(QString src_file,QString dest_file );
    void run();
    void zipFile();
    void readSet();
signals:
    void sigUDiskCome(QString uDiskName);
    void sigUDiskRemove();
protected:
    // 可移动磁盘的插入和拔出检测
     bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
     // 可移动磁盘的插入和拔出检测
    char FirstDriveFromMask(ULONG unitmask);
private:
    QString backup_path;
    QStringList absolute_files_name;
    QStringList accept_file_types;


};

#endif // GETUFILE_H
