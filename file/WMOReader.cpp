#include "WMOReader.h"

void WMOReader::run()
{
    FILE* wmo = fopen(file.toLocal8Bit().data(), "rb");

    if (wmo)
    {
        string MAGIC;
        fseek(wmo, 0xC, SEEK_SET);
        fread(MAGIC.data(), sizeof(char), 0x4, wmo);

        if (QString::fromUtf8(MAGIC.c_str()) != "DHOM")
        {
            fclose(wmo);
            emit update();
            return;
        }

        fseek(wmo, 0x4, SEEK_CUR);
        fread(&nTEX, sizeof(uint32_t), 0x1, wmo);
        fread(&nGROUP, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, 0x8, SEEK_CUR);
        fread(&nMDX, sizeof(uint32_t), 0x1, wmo);

        if (nTEX > 0)
        {
            while (true)
            {
                string MOTX;
                fread(MOTX.data(), sizeof(char), 0x4, wmo);

                if (QString::fromUtf8(MOTX.c_str()) == "XTOM")
                    break;
            }

            fread(&sTEX, sizeof(uint32_t), 0x1, wmo);

            QString temp;
            for (uint32_t t = 0; t < sTEX; ++t)
            {
                char c;
                fread(&c, sizeof(char), 0x1, wmo);
                temp += c;
            }

            QStringList nTemp = temp.split('\u0000');
            for (QString tex : nTemp)
            {
                if (tex == "")
                    continue;

                BLP.emplace_back(tex.replace(R"(\)", R"(/)"));
            }
        }

        if (nGROUP > 0)
        {
            QStringList temp = file.split(R"(/)");
            QString name = temp.last().remove(".wmo", Qt::CaseInsensitive);
            QString key = temp.join(R"(/)").remove(name, Qt::CaseInsensitive).remove(patchDir, Qt::CaseInsensitive);
            for (uint32_t g = 0; g < nGROUP; ++g)
            {
                QString format;
                if (g < 10) format = QString("00%1").arg(g);
                else if (g >= 10 && g < 100) format = QString("0%1").arg(g);
                else format = QString::number(g);

                WMO.emplace_back(key + name + "_" + format + ".wmo");
            }
        }

        fseek(wmo, 0x4, SEEK_CUR);
        fread(&sMOMT, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOMT + 0x4, SEEK_CUR);
        fread(&sMOGN, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOGN + 0x4, SEEK_CUR);
        fread(&sMOGI, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOGI + 0x4, SEEK_CUR);
        fread(&sMOSB, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOSB + 0x4, SEEK_CUR);
        fread(&sMOPV, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOPV + 0x4, SEEK_CUR);
        fread(&sMOPT, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOPT + 0x4, SEEK_CUR);
        fread(&sMOPR, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOPR + 0x4, SEEK_CUR);
        fread(&sMOVV, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOVV + 0x4, SEEK_CUR);
        fread(&sMOVB, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOVB + 0x4, SEEK_CUR);
        fread(&sMOLT, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMOLT + 0x4, SEEK_CUR);
        fread(&sMODS, sizeof(uint32_t), 0x1, wmo);
        fseek(wmo, sMODS + 0x4, SEEK_CUR);

        if (nMDX > 0)
        {
            fread(&MDXsize, sizeof(uint32_t), 0x1, wmo);

            QString temp;
            for (uint32_t t = 0; t < MDXsize; ++t)
            {
                char c;
                fread(&c, sizeof(char), 0x1, wmo);
                temp += c;
            }

            QStringList nTemp = temp.split('\u0000');
            for (QString mdx : nTemp)
            {
                if (mdx == "")
                    continue;

                MDX.emplace_back(mdx.replace(R"(\)", R"(/)").replace(".mdx", ".m2", Qt::CaseInsensitive));
            }
        }

        fclose(wmo);
        emit runDone(MDX, WMO, BLP, file);
        emit update();
    }
}
