#include "MapExtractor.h"

MapExtractor::MapExtractor(QWidget *parent)
    : QMainWindow(parent)
{
    setAcceptDrops(true);
    setStyleSheet("background-color: #23313f;");

    tabView = new QTabWidget(this);
    setCentralWidget(tabView);
    tabView->setStyleSheet("background-color: #fff;");

    dropTab = new QWidget(this);
    tabView->addTab(dropTab, "Drop");
    dropLayout = new QVBoxLayout(dropTab);

    needTab = new QWidget(this);
    tabView->addTab(needTab, "Need");
    needLayout = new QVBoxLayout(needTab);

    /**** DROP TAB ****/
    // header
    dropHeaderLayout = new QHBoxLayout();
    nameDir = new QLabel(dropTab);
    nameDir->setText(patchDir);
    checkDir = new QPushButton(dropTab);
    checkDir->setText("Check All");
    reloadDir = new QPushButton(dropTab);
    reloadDir->setText("Reload Directory");
    selectDir = new QPushButton(dropTab);
    selectDir->setText("Select Directory");
    dropHeaderLayout->addWidget(nameDir);
    dropHeaderLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    dropHeaderLayout->addWidget(checkDir);
    dropHeaderLayout->addWidget(reloadDir);
    dropHeaderLayout->addWidget(selectDir);
    connect(checkDir, &QPushButton::clicked, [this]() { checkFileDirectory(); });
    connect(reloadDir, &QPushButton::clicked, [this]() { if (!patchDir.isEmpty()) parseFileDirectory(); } );
    connect(selectDir, &QPushButton::clicked, [this]() { selectDirectory(); });

    // tree
    dropCentralLayout = new QHBoxLayout();
    dropItem = new QTreeWidget(dropTab);
    dropItem->setColumnCount(2);
    dropItem->setHeaderLabels(QStringList() << "File name" << "File Directory");
    dropItemTexture = new QTreeWidget(dropTab);
    dropItemTexture->setColumnCount(2);
    dropItemTexture->setHeaderLabels(QStringList() << "File name" << "File Directory");
    dropCentralLayout->addWidget(dropItem);
    dropCentralLayout->addWidget(dropItemTexture);
    dropItemADT = new QTreeWidgetItem(dropItem, QStringList() << "ADT");
    dropItemMDX = new QTreeWidgetItem(dropItem, QStringList() << "MDX");
    dropItemWMO = new QTreeWidgetItem(dropItem, QStringList() << "WMO");
    dropItemBLP = new QTreeWidgetItem(dropItemTexture, QStringList() << "BLP");

    // footer
    dropFooterLayout = new QHBoxLayout();
    countProgress = new QLabel(dropTab);
    countProgress->setText("0/0");
    runProgress = new QProgressBar(dropTab);
    runProgress->setOrientation(Qt::Horizontal);
    dropFooterLayout->addWidget(countProgress);
    dropFooterLayout->addWidget(runProgress);

    dropLayout->addLayout(dropHeaderLayout);
    dropLayout->addLayout(dropCentralLayout);
    dropLayout->addLayout(dropFooterLayout);

    /**** NEED TAB ****/
    needCentralLayout = new QHBoxLayout();
    needItem = new QTreeWidget(needTab);
    needItem->setColumnCount(2);
    needItem->setHeaderLabels(QStringList() << "File need Directory" << "File Parent");
    needItemTexture = new QTreeWidget(dropTab);
    needItemTexture->setColumnCount(2);
    needItemTexture->setHeaderLabels(QStringList() << "File need Directory" << "File Parent");
    needCentralLayout->addWidget(needItem);
    needCentralLayout->addWidget(needItemTexture);
    needItemMDX = new QTreeWidgetItem(needItem, QStringList() << "MDX");
    needItemWMO = new QTreeWidgetItem(needItem, QStringList() << "WMO");
    needItemBLP = new QTreeWidgetItem(needItemTexture, QStringList() << "BLP");

    needLayout->addLayout(needCentralLayout);

    populateListfileCache();
}

MapExtractor::~MapExtractor() { }

void MapExtractor::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MapExtractor::dropEvent(QDropEvent *event)
{
    foreach (const QUrl& url, event->mimeData()->urls())
    {
        const QFileInfo data(url.toLocalFile());

        if (!data.isDir())
            return;

        patchDir = data.absoluteFilePath();
        parseFileDirectory();
    }
}

void MapExtractor::selectDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        patchDir = dir;
    }
}

void MapExtractor::clearDropTree()
{
    foreach (auto i, dropItemADT->takeChildren()) delete i;
    foreach (auto i, dropItemMDX->takeChildren()) delete i;
    foreach (auto i, dropItemWMO->takeChildren()) delete i;
    foreach (auto i, dropItemBLP->takeChildren()) delete i;
}

void MapExtractor::clearNeedTree()
{
    foreach (auto i, needItemMDX->takeChildren()) delete i;
    foreach (auto i, needItemWMO->takeChildren()) delete i;
    foreach (auto i, needItemBLP->takeChildren()) delete i;
}

void MapExtractor::parseFileDirectory()
{
    nameDir->setText(patchDir);
    clearDropTree();
    clearNeedTree();
    nFile = 0;
    QDirIterator it(patchDir, QStringList() << "*.m2" << "*.adt" << "*.wmo" << "*.blp", QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        it.next();
        QString suffix = it.fileInfo().suffix().toLower();
        QTreeWidgetItem* item;

        if (suffix == "adt") item = new QTreeWidgetItem(dropItemADT);
        else if (suffix == "m2" || suffix == "mdx") item = new QTreeWidgetItem(dropItemMDX);
        else if (suffix == "wmo" ) item = new QTreeWidgetItem(dropItemWMO);
        else item = new QTreeWidgetItem(dropItemBLP);

        item->setText(0, it.fileName());
        item->setText(1, it.filePath().remove(patchDir + "/"));

        if (suffix != "blp")
            nFile++;
    }

    countProgress->setText(QString("0/%1").arg(nFile));
    runProgress->setMaximum(nFile);
    runProgress->setValue(0);
    dropItem->expandAll();
    dropItemTexture->expandAll();

    for (int i = 0; i < 2; ++i)
    {
        dropItem->resizeColumnToContents(i);
        dropItemTexture->resizeColumnToContents(i);
    }
}

void MapExtractor::checkFileDirectory()
{
    runProgress->setValue(0);
    clearNeedTree();

    ADTdata();
    MDXdata();
    WMOdata();

    needItem->expandAll();
    needItemTexture->expandAll();

    for (int i = 0; i < 2; ++i)
    {
        needItem->resizeColumnToContents(i);
        needItemTexture->resizeColumnToContents(i);
    }
}

void MapExtractor::populateListfileCache()
{
    QFile listfile("(listfile)");

    if (listfile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&listfile);
        while (!in.atEnd())
        {
            QString line = in.readLine().replace(R"(\)", R"(/)");

            if (!line.contains(".blp")
                    && !line.contains(".m2")
                    && !line.contains(".mdx")
                    && !line.contains(".wmo"))
                continue;

            QStringList temp = line.remove(R"( )").split(R"(/)");
            QString value = temp.last();
            QString key = temp.join(R"(/)").remove(value);

            listfileCache[key.toLower()].emplace_back(value.toLower());
        }
        listfile.close();
    }
}

void MapExtractor::ADTdata()
{
    for (int i = 0; i < dropItemADT->childCount(); ++i)
    {
        _adt = new ADTReader(this);
        connect(_adt, &ADTReader::runDone,
                [&](QVector<QString> mdx,
                QVector<QString> wmo,
                QVector<QString> blp,
                QString file) {
            populateNeedTree(mdx, wmo, blp, file);
        });
        connect(_adt, &ADTReader::update, [&]() { updateProgress(); });

        _adt->setFile(patchDir + "/" + dropItemADT->child(i)->text(1));
        _adt->run();
    }
}

void MapExtractor::MDXdata()
{
    for (int i = 0; i < dropItemMDX->childCount(); ++i)
    {
        _mdx = new MDXReader(this);
        connect(_mdx, &MDXReader::runDone,
                [&](QVector<QString> blp, QString file) {
            populateNeedTree(QVector<QString>(), QVector<QString>(), blp, file);
        });
        connect(_mdx, &MDXReader::update, [&]() { updateProgress(); });

        _mdx->setFile(patchDir + "/" + dropItemMDX->child(i)->text(1));
        _mdx->run();
    }
}

void MapExtractor::WMOdata()
{
    for (int i = 0; i < dropItemWMO->childCount(); ++i)
    {
        _wmo = new WMOReader(this);
        connect(_wmo, &WMOReader::runDone,
                [&](QVector<QString> mdx,
                QVector<QString> wmo,
                QVector<QString> blp,
                QString file) {
            populateNeedTree(mdx, wmo, blp, file);
        });
        connect(_wmo, &WMOReader::update, [&]() { updateProgress(); });

        _wmo->setFile(patchDir + '/' + dropItemWMO->child(i)->text(1));
        _wmo->setPatchDir(patchDir + "/");
        _wmo->run();
    }
}

void MapExtractor::updateProgress()
{
    runProgress->setValue(runProgress->value() + 1);
    countProgress->setText(QString("%1/%2").arg(runProgress->value()).arg(nFile));
}

void MapExtractor::populateNeedTree(
        QVector<QString> MDXs,
        QVector<QString> WMOs,
        QVector<QString> BLPs,
        QString file)
{
    bool found;
    for (QString mdx : MDXs)
    {
        found = false;

        if ( mdx == "" || mdx.toLower() == "spells/errorcube.mdx")
            continue;

        for (int i = 0; i < dropItemMDX->childCount(); ++i)
        {
            if (mdx.toLower() == dropItemMDX->child(i)->text(1).toLower())
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        QStringList temp = mdx.remove(R"( )").split(R"(/)");
        QString value = temp.last();
        QString key = temp.join(R"(/)").remove(value, Qt::CaseInsensitive);
        for (QString f : listfileCache[key.toLower()])
        {
            if (f.contains(value, Qt::CaseInsensitive))
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        for (int i = 0; i < needItemMDX->childCount(); ++i)
        {
            if (mdx.toLower() == needItemMDX->child(i)->text(0).toLower())
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        QTreeWidgetItem* item = new QTreeWidgetItem(needItemMDX);
        item->setText(0, mdx.toLower());
        item->setText(1, file.remove(patchDir + "/", Qt::CaseInsensitive));
        needItemMDX->addChild(item);
    }

    for (QString wmo : WMOs)
    {
        found = false;

        if ( wmo == "" )
            continue;

        for (int i = 0; i < dropItemWMO->childCount(); ++i)
        {
            if (wmo.toLower() == dropItemWMO->child(i)->text(1).toLower())
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        QStringList temp = wmo.remove(R"( )").split(R"(/)");
        QString value = temp.last();
        QString key = temp.join(R"(/)").remove(value, Qt::CaseInsensitive);
        for (QString f : listfileCache[key.toLower()])
        {
            if (f.contains(value, Qt::CaseInsensitive))
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        for (int i = 0; i < needItemWMO->childCount(); ++i)
        {
            if (wmo.toLower() == needItemWMO->child(i)->text(0).toLower())
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        QTreeWidgetItem* item = new QTreeWidgetItem(needItemWMO);
        item->setText(0, wmo.toLower());
        item->setText(1, file.remove(patchDir + "/", Qt::CaseInsensitive));
        needItemWMO->addChild(item);
    }

    for (QString blp : BLPs)
    {
        found = false;

        if ( blp == "" )
            continue;

        for (int i = 0; i < dropItemBLP->childCount(); ++i)
        {
            if (blp.toLower() == dropItemBLP->child(i)->text(1).toLower())
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        QStringList temp = blp.remove(R"( )").split(R"(/)");
        QString value = temp.last();
        QString key = temp.join(R"(/)").remove(value, Qt::CaseInsensitive);
        for (QString f : listfileCache[key.toLower()])
        {
            if (f.contains(value, Qt::CaseInsensitive))
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        for (int i = 0; i < needItemBLP->childCount(); ++i)
        {
            if (blp.toLower() == needItemBLP->child(i)->text(0).toLower())
            {
                found = true;
                break;
            }
        }

        if (found) continue;

        QTreeWidgetItem* item = new QTreeWidgetItem(needItemBLP);
        item->setText(0, blp.toLower());
        item->setText(1, file.remove(patchDir + "/", Qt::CaseInsensitive));
        needItemBLP->addChild(item);
    }
}

