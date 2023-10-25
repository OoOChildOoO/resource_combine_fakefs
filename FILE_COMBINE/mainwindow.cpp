#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QFileDialog>

#include <QMessageBox>

#include<QDialog>
#include<QColorDialog>
#include<QFontDialog>

#include  <QPainter>
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QPointF>
#include <QColor>

#include <QMenu>
#include <QMenuBar>
#include <QAction>

#include <QGraphicsScene>
#include <QGraphicsView>

#include <QTextCodec>

#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString editTemp;
    editTemp  = "please input a file name";
    ui->listWidget_outPath->setPlaceholderText(editTemp);

    QString editTemp2;
    editTemp2  = "please input a file ver";
    ui->listWidget_ver->setPlaceholderText(editTemp2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString outFilePath;

QList<CatalogTypeDef> fileMsgList;
QStringList filePaths;
void MainWindow::on_pushButton_chooseFile_clicked()
{
    unsigned int res = 0;
    filePaths.clear();
    fileMsgList.clear();
    res = fun_getFileSource();
    if(res)
    {
        ui->pushButton_save->setEnabled(true);
    }
    else
    {
        ui->pushButton_save->setEnabled(false);
    }
}

unsigned int  MainWindow::fun_getFileSource(void)
{

    filePaths = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("请选择数据文件"), NULL, "program file(*.fon *.FON *.bin *.BIN)");
    if (filePaths.isEmpty())
    {
         QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("未选择数据文件"), QString::fromLocal8Bit("确定"));
         return 0;
    }
    else
    {
        outFilePath = QFileInfo(filePaths.at(0)).canonicalPath();
        outFilePath += "/";
        quint32 resource_size = FON_INDEX_OFFSET + FON_INDEX_SIZE;
        for(int i = 0; i < filePaths.length();i++)
        {
            QString filePath = filePaths.at(i);
            QFile openFile(filePath);
            if(!openFile.open(QIODevice::ReadOnly))
            {
                openFile.close();
                QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件打开失败"), QString::fromLocal8Bit("确定"));
                return 0;
            }

            CatalogTypeDef fontMsg;
            memset(&fontMsg,0,sizeof(CatalogTypeDef));
            fontMsg.check = 0;
            int wholeNum = openFile.size()/FLEX_FLASH_SECTOR_SIZE;
            int lostNum = openFile.size()%FLEX_FLASH_SECTOR_SIZE;
            char dataSaveArr[FLEX_FLASH_SECTOR_SIZE];
            for(int j = 0;j < wholeNum;j++)
            {
                memset(dataSaveArr,0,sizeof (dataSaveArr));
                openFile.read(dataSaveArr,FLEX_FLASH_SECTOR_SIZE);
                for(int k = 0; k < FLEX_FLASH_SECTOR_SIZE;k++)
                {
                    fontMsg.check += dataSaveArr[k];
                }
            }
            if(lostNum)
            {
                memset(dataSaveArr,0,sizeof (dataSaveArr));
                openFile.read(dataSaveArr,lostNum);
                for(int k = 0; k < lostNum;k++)
                {
                    fontMsg.check += dataSaveArr[k];
                }
            }
            openFile.close();
            fontMsg.size = openFile.size();
            fontMsg.offset = resource_size;
            QString Name = QFileInfo(filePath).fileName();
            if(Name.length() >= 36)
            {
                openFile.close();
                QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("部分文件文件名过长"), QString::fromLocal8Bit("确定"));
                return 0;
            }
            strcpy(fontMsg.name,Name.toUtf8().data());
            fileMsgList.append(fontMsg);

            quint32 addr_temp = 0;
            addr_temp = (quint32)(fontMsg.size + FLEX_FLASH_SECTOR_SIZE) / FLEX_FLASH_SECTOR_SIZE;
            addr_temp = (quint32)addr_temp * FLEX_FLASH_SECTOR_SIZE;

            resource_size += addr_temp; /* 偏移文件的大小 */

        }
        return 1;
    }
}

void MainWindow::on_pushButton_save_clicked()
{
    QString fileName = ui->listWidget_outPath->toPlainText();
    QString fileVer = ui->listWidget_ver->toPlainText();
    if(fileName.length() == 0)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("无文件名"), QString::fromLocal8Bit("确定"));
        return;
    }

    if(fileName.length() >= 36)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件名过长"), QString::fromLocal8Bit("确定"));
        return;
    }

    if(fileVer.length() == 0)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("无版本信息"), QString::fromLocal8Bit("确定"));
        return;
    }

    if(fileVer.length() >= 36)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("版本信息过长"), QString::fromLocal8Bit("确定"));
        return;
    }
    fun_saveToFile(outFilePath,fileName,fileVer);
}

unsigned int MainWindow::fun_saveToFile(QString filePath,QString fileName,QString fileVer)
{
    char dataSaveArr[FLEX_FLASH_SECTOR_SIZE];
    FileHeadTypeDef headMsg = {0};
    filePath += fileName;
    QFile saveFile(filePath);
    saveFile.remove();
    if (!saveFile.open(QIODevice::NewOnly|QIODevice::ReadWrite|QIODevice::Truncate))
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件保存失败"), QString::fromLocal8Bit("确定"));
        saveFile.close();
        return 0;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    saveFile.seek(0);
    // 写入头标识符
    memset(dataSaveArr,0,sizeof (dataSaveArr));
    strcpy(headMsg.name,fileVer.toUtf8().data());
    headMsg.offset = 256;
    // 计算文件内容的和校验
    for(int i = 0; i < filePaths.length();i++)
    {
        QFile readFile(filePaths.at(i));
        if(!readFile.open(QIODevice::ReadOnly))
        {
            QApplication::restoreOverrideCursor();
            readFile.close();
            QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件打开失败"), QString::fromLocal8Bit("确定"));
            return 0;
        }
        int wholeNum = readFile.size()/FLEX_FLASH_SECTOR_SIZE;
        int lostNum = readFile.size()%FLEX_FLASH_SECTOR_SIZE;
        for(int j = 0;j < wholeNum;j++)
        {
            memset(dataSaveArr,0,sizeof (dataSaveArr));
            readFile.read(dataSaveArr,FLEX_FLASH_SECTOR_SIZE);
            for(int k = 0; k < FLEX_FLASH_SECTOR_SIZE;k++)
            {
                headMsg.check += dataSaveArr[k];
            }
        }
        if(lostNum)
        {
            memset(dataSaveArr,0,sizeof (dataSaveArr));
            readFile.read(dataSaveArr,lostNum);
            for(int k = 0; k < lostNum;k++)
            {
                headMsg.check += dataSaveArr[k];
            }
        }
        readFile.close();
    }
    //计算文件索引的和校验
    memset(dataSaveArr,0,sizeof (dataSaveArr));
    for(int i = 0, p = 0; i < fileMsgList.length();i++)
    {
        memcpy((void *)&(dataSaveArr[p]),(void *)&fileMsgList.at(i),sizeof(CatalogTypeDef));
        p += sizeof(CatalogTypeDef);
        if(p >= FON_INDEX_SIZE)
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件索引超范围，请减少文件"), QString::fromLocal8Bit("确定"));
            return 0;
        }
    }
    for(int k = 0; k < FLEX_FLASH_SECTOR_SIZE;k++)
    {
        headMsg.check += dataSaveArr[k];
    }
    memset(dataSaveArr,0,sizeof (dataSaveArr));
    //将头文件信息写入文件中
    memcpy((void *)&(dataSaveArr[0]),(void *)&headMsg,sizeof(FileHeadTypeDef));
    saveFile.write(dataSaveArr, 256);

    //写入文件索引
    memset(dataSaveArr,0,sizeof (dataSaveArr));
    for(int i = 0, p = 0; i < fileMsgList.length();i++)
    {
        memcpy((void *)&(dataSaveArr[p]),(void *)&fileMsgList.at(i),sizeof(CatalogTypeDef));
        p += sizeof(CatalogTypeDef);
        if(p >= FON_INDEX_SIZE)
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件索引超范围，请减少文件"), QString::fromLocal8Bit("确定"));
            return 0;
        }
    }
    saveFile.write(dataSaveArr, sizeof (dataSaveArr));

    //写入文件内容
    for(int i = 0; i < filePaths.length();i++)
    {
        QFile readFile(filePaths.at(i));
        if(!readFile.open(QIODevice::ReadOnly))
        {
            QApplication::restoreOverrideCursor();
            readFile.close();
            QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件打开失败"), QString::fromLocal8Bit("确定"));
            return 0;
        }
        if(fileMsgList.at(i).offset + headMsg.offset > saveFile.pos())
        {
            qint64 len = fileMsgList.at(i).offset + headMsg.offset - saveFile.pos();
            int wholeNum = len/FLEX_FLASH_SECTOR_SIZE;
            int lostNum = len%FLEX_FLASH_SECTOR_SIZE;
            for(int j = 0;j < wholeNum;j++)
            {
                memset(dataSaveArr,0,sizeof (dataSaveArr));
                saveFile.write(dataSaveArr, sizeof (dataSaveArr));
            }
            if(lostNum)
            {
                memset(dataSaveArr,0,sizeof (dataSaveArr));
                saveFile.write(dataSaveArr, lostNum);
            }
        }
        int wholeNum = readFile.size()/FLEX_FLASH_SECTOR_SIZE;
        int lostNum = readFile.size()%FLEX_FLASH_SECTOR_SIZE;
        for(int j = 0;j < wholeNum;j++)
        {
            memset(dataSaveArr,0,sizeof (dataSaveArr));
            readFile.read(dataSaveArr,FLEX_FLASH_SECTOR_SIZE);
            saveFile.write(dataSaveArr, sizeof (dataSaveArr));
        }
        if(lostNum)
        {
            memset(dataSaveArr,0,sizeof (dataSaveArr));
            readFile.read(dataSaveArr,lostNum);
            saveFile.write(dataSaveArr, sizeof (dataSaveArr));
        }
        readFile.close();
    }
    ui->pushButton_save->setEnabled(false);
    saveFile.close();
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件保存成功"), QString::fromLocal8Bit("确定"));

    return 1;
}

//![CRC-16数组]
quint8  crc_ta_ArrayHi[]={
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

quint8  crc_ta_ArrayLo[]={
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40};
//![CRC-16计算]
quint16 MainWindow:: GetCRC16(char *puchMsg,unsigned int usDataLen)
{
    quint8 uchCRCHi = 0xFF;  /* CRC  的高字节初始化  */
    quint8 uchCRCLo = 0xFF;  /* CRC  的低字节初始化  */
    quint8 uIndex ;  	   /* CRC 查询表索引       */
    quint16 u16Result;
    quint16 i = 0;
    while(usDataLen--)  			 /* 完成整个报文缓冲区   */
    {
        uIndex = uchCRCLo ^ (quint8)puchMsg[i] ;  /*  计算  CRC      */
        i++;
        uchCRCLo = uchCRCHi ^ crc_ta_ArrayHi[uIndex];
        uchCRCHi = crc_ta_ArrayLo[uIndex];
    }
    u16Result = uchCRCHi;
    u16Result <<= 8;
    u16Result += uchCRCLo;
    return u16Result;
}

//![SUM-16计算]
quint16 MainWindow:: GetSUM16(char *puchMsg,unsigned int usDataLen)
{
    quint16 sumCheck = 0;
    unsigned int i = 0;
    for(i = 0;i<usDataLen;i++)
    {
        sumCheck += (quint8)(*(puchMsg + i));
    }
    return sumCheck;
}
