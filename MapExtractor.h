#ifndef MAPEXTRACTOR_H
#define MAPEXTRACTOR_H

#include "file/ADTReader.h"
#include "file/MDXReader.h"
#include "file/WMOReader.h"

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QLayout>
#include <QTreeWidget>
#include <QDirIterator>
#include <QMimeData>
#include <QFileInfo>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QTabWidget>
#include <QSpacerItem>
#include <QProgressBar>
#include <QLabel>

class MapExtractor : public QMainWindow
{
    Q_OBJECT

public:
    MapExtractor(QWidget *parent = nullptr);
    ~MapExtractor();

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private:
    // main
    QString patchDir;
    QTabWidget* tabView;

    QVBoxLayout* dropLayout;
    QWidget* dropTab;

    QVBoxLayout* needLayout;
    QWidget* needTab;

    /**** DROP TAB ****/
    // header
    QHBoxLayout* dropHeaderLayout;
    QLabel* nameDir;
    QPushButton* selectDir;
    QPushButton* reloadDir;
    QPushButton* checkDir;
    void selectDirectory();

    // tree
    QHBoxLayout* dropCentralLayout;

    QTreeWidget* dropItem;
    QTreeWidgetItem* dropItemADT;
    QTreeWidgetItem* dropItemMDX;
    QTreeWidgetItem* dropItemWMO;

    QTreeWidget* dropItemTexture;
    QTreeWidgetItem* dropItemBLP;

    void clearDropTree();
    void clearNeedTree();
    void parseFileDirectory();
    void checkFileDirectory();

    // footer
    uint32_t nFile;
    QHBoxLayout* dropFooterLayout;
    QLabel* countProgress;
    QProgressBar* runProgress;

    void updateProgress();

    /**** NEED TAB ****/
    QHBoxLayout* needCentralLayout;

    QTreeWidget* needItem;
    QTreeWidgetItem* needItemMDX;
    QTreeWidgetItem* needItemWMO;

    QTreeWidget* needItemTexture;
    QTreeWidgetItem* needItemBLP;

    /**** LISTFILE ****/
    QMap<QString, QVector<QString>> listfileCache;
    void populateListfileCache();

    /**** READER ****/
    ADTReader* _adt;
    void ADTdata();
    MDXReader* _mdx;
    void MDXdata();
    WMOReader* _wmo;
    void WMOdata();

private slots:
    void populateNeedTree(QVector<QString> MDXs, QVector<QString> WMOs, QVector<QString> BLPs, QString file);
};
#endif // MAPEXTRACTOR_H
