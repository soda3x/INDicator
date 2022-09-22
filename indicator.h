#pragma once
#ifndef INDICATOR_H
#define INDICATOR_H
#include <string.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <cctype>
#include <algorithm>

void printUsageString();
void toLowerCase(std::string);
void replaceBytesInExec(std::string, std::string, bool, bool);
void writeBytes(int, std::string, int, std::string);
void makeBackup(std::string);
bool validatePlatformArg(std::string);
bool validateVersionArg(std::string);
const char *makePaddedCString(std::string, std::string, int);
std::string truncateString(std::string, int, const char *);

const int PSX_WAD_BYTE_OFFSET_1 = 0x00000918;
const int PSX_WAD_BYTE_OFFSET_2 = 0x00000CF8;
const int PSX_IND_BYTE_OFFSET_1 = 0x00000924;
const int PSX_IND_BYTE_OFFSET_2 = 0x00000D04;

const int PC_M2K_WAD_BYTE_OFFSET_1 = 0x00133FAC;
const int PC_M2K_WAD_BYTE_OFFSET_2 = 0x0013CDDC;
const int PC_M2K_WAD_BYTE_OFFSET_3 = 0x0010528D;
const int PC_M2K_IND_BYTE_OFFSET_1 = 0x00133F9C;
const int PC_M2K_IND_BYTE_OFFSET_2 = 0x0013CDCC;

const int PC_MTVMG_WAD_BYTE_OFFSET_1 = 0x00133FC4;
const int PC_MTVMG_WAD_BYTE_OFFSET_2 = 0x0013CDF4;
const int PC_MTVMG_WAD_BYTE_OFFSET_3 = 0x001052BD;
const int PC_MTVMG_IND_BYTE_OFFSET_1 = 0x00133FB4;
const int PC_MTVMG_IND_BYTE_OFFSET_2 = 0x0013CDE4;

const int PSX_WAD_IND_REF_SIZE_BYTES = 12;
const int PC_WAD_IND_REF_SIZE_BYTES = 16;

const std::string USAGE_STR = "INDicator: Replace the reference to wad/ind file in a M2K or MTV Music Generator executable\n\tUsage: indicator <exec-file> <new-wad-file-to-use> <platform - pc | psx> <version - m2k | mtvmg>";
const std::string PLAT_PSX_STR = "psx";
const std::string PLAT_PC_STR = "pc";
const std::string VERS_MTVMG_STR = "mtvmg";
const std::string VERS_M2K_STR = "m2k";

#endif /* INDICATOR_H */