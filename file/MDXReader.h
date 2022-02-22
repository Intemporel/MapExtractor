#ifndef MDXREADER_H
#define MDXREADER_H

#include <QMap>
#include <QThread>

using namespace std;

class MDXReader : public QThread
{
    Q_OBJECT

public:
    explicit MDXReader(QObject *parent = nullptr) : QThread(parent) {};
    virtual ~MDXReader() = default;

    void run();
    void setFile(QString f) { file = f; };

signals:
    void runDone(QVector<QString> blp, QString file);
    void update();

private:
    QString file;
    QVector<QString> BLP;

    uint32_t TEXcount;
    uint32_t TEXofs;

    QVector<QVector<uint32_t>> TEX;
};

#endif // MDXREADER_H
