#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>

#define FLEX_FLASH_SECTOR_SIZE (4 * 1024) /* 扇区大小4K */

#define FON_INDEX_OFFSET (0)
#define FON_INDEX_SIZE (FLEX_FLASH_SECTOR_SIZE)//索引大小

/* 字库目录信息类型 */
typedef struct
{
    char name[36];   /* 资源的名字 */
    quint32 check;   /* 资源的和校验 */
    quint32 size;   /* 资源的大小 */
    quint32 offset; /* 资源相对于基地址的偏移 */
} CatalogTypeDef;


typedef struct
{
    char name[40];   /* 资源的头名字 */
    quint32 offset; /* 资源相对于基地址的偏移 */
    quint32 check;   /* 资源的校验 *///未使用

} FileHeadTypeDef;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    unsigned int fun_getFileSource(void);
    unsigned int fun_saveToFile(QString filePath,QString fileName,QString fileVer);
    quint16 GetCRC16(char *puchMsg,unsigned int usDataLen);
    quint16 GetSUM16(char *puchMsg,unsigned int usDataLen);

private slots:
    void on_pushButton_chooseFile_clicked();

    void on_pushButton_save_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
