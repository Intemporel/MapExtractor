#ifndef ADTREADER_H
#define ADTREADER_H

#include <QDir>
#include <QThread>

using namespace std;

class ADTReader : public QThread
{
    Q_OBJECT

public:
    explicit ADTReader(QObject* parent) : QThread(parent) {};
    virtual ~ADTReader() = default;

    void run();
    void setFile(QString f) { file = f; };

signals:
    void runDone(QVector<QString> mdx, QVector<QString> wmo, QVector<QString> blp, QString file);
    void update();

private:
    QString file;
    QVector<QString> BLP;
    QVector<QString> MDX;
    QVector<QString> WMO;

    uint32_t MTEXofs;
    uint32_t MTEXsize;

    uint32_t MMDXofs;
    uint32_t MMDXsize;

    uint32_t MWMOofs;
    uint32_t MWMOsize;
};

#endif // ADTREADER_H
