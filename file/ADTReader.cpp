#include "ADTReader.h"

void ADTReader::run()
{
    FILE* adt = fopen(file.toLocal8Bit().data(), "rb");

    if (adt)
    {
        fseek(adt, 0x4 * 0x7, SEEK_SET);
        fread(&MTEXofs, sizeof(uint32_t), 1, adt);
        fread(&MMDXofs, sizeof(uint32_t), 1, adt);
        fseek(adt, 0x4, SEEK_CUR);
        fread(&MWMOofs, sizeof(uint32_t), 1, adt);

        // texture
        if (MTEXofs > 0)
        {
            fseek(adt, MTEXofs + 0x14 + 0x4, SEEK_SET);
            fread(&MTEXsize, sizeof(uint32_t), 0x1, adt);

            QString temp;
            for (uint32_t i = 0; i < MTEXsize; ++i)
            {
                char c;
                fread(&c, sizeof(char), 0x1, adt);
                temp += c;
            }

            QStringList nTemp = temp.split('\u0000');
            nTemp.removeLast();

            for (QString tex : nTemp)
                BLP.emplace_back(tex.replace(R"(\)", R"(/)"));
        }

        // model .m2
        if (MMDXofs > 0)
        {
            fseek(adt, MMDXofs + 0x14 + 0x4, SEEK_SET);
            fread(&MMDXsize, sizeof(uint32_t), 0x1, adt);

            QString temp;
            for (uint32_t i = 0; i < MMDXsize; ++i)
            {
                char c;
                fread(&c, sizeof(char), 0x1, adt);
                temp += c;
            }

            QStringList nTemp = temp.split('\u0000');
            nTemp.removeLast();

            for (QString model : nTemp)
                MDX.emplaceBack(model.replace(R"(\)", R"(/)").replace(".mdx", ".m2", Qt::CaseInsensitive));
        }

        // wmo
        if (MWMOofs > 0)
        {
            fseek(adt, MWMOofs + 0x14 + 0x4, SEEK_SET);
            fread(&MWMOsize, sizeof(uint32_t), 0x1, adt);

            QString temp;
            for (uint32_t i = 0; i < MWMOsize; ++i)
            {
                char c;
                fread(&c, sizeof(char), 0x1, adt);
                temp += c;
            }

            QStringList nTemp = temp.split('\u0000');
            nTemp.removeLast();

            for (QString wmo : nTemp)
                WMO.emplaceBack(wmo.replace(R"(\)", R"(/)"));
        }

        fclose(adt);
        emit runDone(MDX, WMO, BLP, file);
        emit update();
    }
}
