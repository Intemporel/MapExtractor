#ifndef WMOREADER_H
#define WMOREADER_H

#include <QDir>
#include <QThread>

using namespace std;

class WMOReader : public QThread
{
    Q_OBJECT

public:
    explicit WMOReader(QObject *parent = nullptr) : QThread(parent) {};
    virtual ~WMOReader() = default;

    void run();
    void setFile(QString f) { file = f; };
    void setPatchDir(QString p) { patchDir = p; };

signals:
    void runDone(QVector<QString> mdx, QVector<QString> wmo, QVector<QString> blp, QString file);
    void update();

private:
    QString file;
    QString patchDir;
    QVector<QString> WMO;
    QVector<QString> MDX;
    QVector<QString> BLP;

    uint32_t nTEX;
    uint32_t sTEX;

    uint32_t nGROUP;

    uint32_t sMOMT;
    uint32_t sMOGN;
    uint32_t sMOGI;
    uint32_t sMOSB;
    uint32_t sMOPV;
    uint32_t sMOPT;
    uint32_t sMOPR;
    uint32_t sMOVV;
    uint32_t sMOVB;
    uint32_t sMOLT;
    uint32_t sMODS;

    uint32_t nMDX;
    uint32_t MDXsize;
};

#endif // WMOREADER_H
