#include "MDXReader.h"

void MDXReader::run()
{
    FILE* mdx = fopen(file.toLocal8Bit().data(), "rb");

    if (mdx)
    {
        uint32_t version;
        fseek(mdx, 0x4, SEEK_SET);
        fread(&version, 0x4, 0x1, mdx);

        if (version != 264)
        {
            fclose(mdx);
            emit update();
            return;
        }

        fseek(mdx, 0x50, SEEK_SET);
        fread(&TEXcount, sizeof(uint32_t), 0x1, mdx);

        if (TEXcount == 0)
        {
            fclose(mdx);
            emit update();
            return;
        }

        fread(&TEXofs, sizeof(uint32_t), 0x1, mdx);
        fseek(mdx, TEXofs, SEEK_SET);

        for (uint32_t t = 0; t < TEXcount; ++t)
        {
            fseek(mdx, 0x8, SEEK_CUR);

            QVector<uint32_t> curTex;
            uint32_t temp1, temp2;
            fread(&temp1, sizeof(uint32_t), 0x1, mdx);
            fread(&temp2, sizeof(uint32_t), 0x1, mdx);

            curTex.emplace_back(temp1);
            curTex.emplace_back(temp2);
            TEX.push_back(curTex);
        }

        for (uint32_t t = 0; t < TEX.size(); ++t)
        {
            string temp;
            fseek(mdx, TEX[t][1], SEEK_SET);
            fread(temp.data(), 0x1, TEX[t][0], mdx);

            BLP.emplace_back(QString::fromUtf8(temp.c_str()).replace(R"(\)", R"(/)"));
        }

        fclose(mdx);
        emit runDone(BLP, file);
        emit update();
    }
}
