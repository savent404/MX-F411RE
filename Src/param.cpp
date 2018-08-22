#include "param.h"

using namespace std;

_Param::_Param(int bank)
    : iParam(bank)
{
    workPath = "0:/";
    numBank = searchFileCnt(workPath.c_str(), "BANK*");

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
    FATFS_DIR dj;
    FRESULT fr;
    FILINFO fno;

    int cnt = 0;

    fr = f_findfirst(&dj, &fno, path, regex);

    while (fr == FR_OK && fno.fname[0]) {
        cnt += 1;
        fr = f_findnext(&dj, &fno);
    }
    return cnt;
}

bool _Param::searchFileName(const char* path, const char* regex,std::string& out, int pos) const
{
    FATFS_DIR dj;
    FRESULT fr;
    FILINFO fno;

    fr = f_findfirst(&dj, &fno, path, regex);

    while (fr == FR_OK && fno.fname[0]) {
        if (pos-- <= 0) {
            out = fno.fname;
            return true;
        }
        fr = f_findnext(&dj, &fno);
    }
    return false;
}

bool _Param::readConfigFromFile(const char* filepath)
{
    FRESULT fr;
    FIL file;
    char *pBuffer = NULL;
    const int defaultBufferSize = 512;

    fr = f_open(&file, filepath, FA_READ);
    pBuffer = (char*)malloc(sizeof(char)*defaultBufferSize);

    if (fr != FR_OK)
        return false;
    if (pBuffer == NULL)
        return false;

    while (!f_eof(&file)) {
        if (f_gets((TCHAR*)pBuffer, defaultBufferSize, &file) == NULL)
            break;
        setParameterFromLine(pBuffer);
    }

    free(pBuffer);
    f_close(&file);
    return true;
}

bool _Param::readColorConfigFromFile(const char* filepath)
{

    FRESULT fr;
    FIL file;
    char *pBuffer = NULL;
    const int defaultBufferSize = 512;

    fr = f_open(&file, filepath, FA_READ);
    pBuffer = (char*)malloc(sizeof(char)*defaultBufferSize);

    if (fr != FR_OK)
        return false;
    if (pBuffer == NULL)
        return false;

    while (!f_eof(&file)) {
        if (f_gets((TCHAR*)pBuffer, defaultBufferSize, &file) == NULL)
            break;
        setColorParameterFromLine(pBuffer);
    }

    free(pBuffer);
    f_close(&file);
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

