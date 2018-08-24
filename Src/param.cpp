#include "param.h"
#include "mbed.h"
#include "FATFileSystem.h"
#include "File.h"
#include "Dir.h"

using namespace std;

extern FATFileSystem fatfsInstance;

char* fgets_wrapper(char* buffer, int maxLen, File* fp)
{
    int remain = 0;
    char c;
    while (fp->size() != fp->tell())
    {
        fp->read(&c, 1);

        if (c == '\r')
            continue;
        if (c == '\n' || c == '\0')
            break;
        buffer[remain++] = c;

        if (remain + 1 >= maxLen)
            break;
    }
    buffer[remain] = 0;
    return buffer;
}

_Param::_Param(int bank)
    : iParam(bank)
{
    workPath = "0:/";
    numBank = searchFileCnt(workPath.c_str(), "[Bb][Aa][Nn][Kk]+");

    intParam.resize(sizeof(typeIntParam) / sizeof(string));
    floatParam.resize(sizeof(typeFloatParam) / sizeof(string));

    /** read static parameter */
    if (readStaticParameter() && (size_t(getBankNum()) == staticParam.configRGBIndex.size())) {
        posBank = staticParam.posBank;
    } else {
        size_t rgbNum = sizeof(typeRGBParam) / sizeof(string);
        staticParam.configRGB.reserve(getBankNum() * rgbNum);
        staticParam.configRGBIndex.resize(getBankNum());
        posBank = bank;
    }

    /** Init Color Martrix
     */
    if (!readColorConfigFromFile((workPath + "COLORMATRIX.txt").c_str())) {
        mDebug(1, "Cant open color matrix.txt");
    }

    switchBank(posBank);

    inited = true;
}

_Param::~_Param()
{
    writeStaticParameter();
}

int _Param::searchFileCnt(const char* path, const char* regex) const
{
    Dir directory;
    int cnt = 0;
    dirent ent;

    if (directory.open(&fatfsInstance, path) != 0)
        return 0;

    while (directory.read(&ent)) {
        if (!re_match(regex, ent.d_name)) {
            cnt++;
        }
    }
    directory.close();
    return cnt;
}

bool _Param::searchFileName(const char* path, const char* regex, std::string& out, int pos) const
{
    Dir directory;
    int cnt = 0;
    dirent ent;

    if (directory.open(&fatfsInstance, path) != 0)
        return false;

    while (directory.read(&ent)) {
        if (!re_match(regex, ent.d_name)
            && cnt++ == pos) {
            out = ent.d_name;
            directory.close();
            return true;
        }
    }
    directory.close();
    return false;
}

bool _Param::readConfigFromFile(const char* filepath)
{
    char* pBuffer = NULL;
    const int defaultBufferSize = 512;
    File file;

    if (file.open(&fatfsInstance, filepath))
        return false;

    pBuffer = (char*)malloc(sizeof(char) * defaultBufferSize);

    if (pBuffer == NULL)
        return false;

    while (file.size() != file.tell()) {
        fgets_wrapper(pBuffer, defaultBufferSize, &file);
        setParameterFromLine(pBuffer);
    }

    free(pBuffer);
    file.close();

    return true;
}

bool _Param::readColorConfigFromFile(const char* filepath)
{
    char* pBuffer = NULL;
    const int defaultBufferSize = 512;
    File file;

    if (file.open(&fatfsInstance, filepath))
        return false;

    pBuffer = (char*)malloc(sizeof(char) * defaultBufferSize);

    if (pBuffer == NULL)
        return false;

    while (file.size() != file.tell()) {
        fgets_wrapper(pBuffer, defaultBufferSize, &file);
        setColorParameterFromLine(pBuffer);
    }

    free(pBuffer);
    file.close();

    return true;
}

bool _Param::readStaticParameter()
{
    return false;
}

bool _Param::writeStaticParameter()
{
    return true;
}
