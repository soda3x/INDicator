#include "indicator.h"

int main(int argc, char *argv[])
{
    // INDicator <exec-file> <new-wad-file-to-use> <platform> <version>
    if (argc != 5)
    {
        printUsageString();
        return -1;
    }

    std::string executable = std::string(argv[1]);
    std::string chosenWad = std::string(argv[2]);
    std::string platform = std::string(argv[3]);
    std::string version = std::string(argv[4]);
    std::string explainStr = "PC executable ";
    bool isPSX = false;
    bool isM2K = false;

    if (validatePlatformArg(platform) && validateVersionArg(version))
    {
        if (0 == PLAT_PSX_STR.compare(platform))
        {
            isPSX = true;
            explainStr = "PSX executable ";
        }

        if (0 == VERS_M2K_STR.compare(version))
        {
            isM2K = true;
        }

        makeBackup(executable);
        replaceBytesInExec(executable, chosenWad, isPSX, isM2K);
        std::cout << "INDicator: " << explainStr << "\"" << executable << "\" now using \"" << chosenWad << "\" as the loaded wad file." << std::endl;
    }

    else
    {
        printUsageString();
        return -1;
    }

    return 0;
}

/**
 * @brief Convert Platform argument to lowercase and validate that it is either PC or PSX
 *
 * @param arg to compare
 * @return true if platform argument is either pc or psx,
 * @return false if platform argument is anything else
 */
bool validatePlatformArg(std::string arg)
{
    toLowerCase(arg);
    return 0 == PLAT_PSX_STR.compare(arg) || 0 == PLAT_PC_STR.compare(arg);
}

/**
 * @brief Convert Version argument to lowercase and validate that it is either MTVMG or M2K
 *
 * @param arg to compare
 * @return true if version argument is either mtvmg or m2k,
 * @return false if version argument is anything else
 */
bool validateVersionArg(std::string arg)
{
    toLowerCase(arg);
    return 0 == VERS_M2K_STR.compare(arg) || 0 == VERS_MTVMG_STR.compare(arg);
}

/**
 * @brief Print Usage String
 *
 */
void printUsageString()
{
    std::cout << USAGE_STR << std::endl;
}

/**
 * @brief Convert string to lowercase
 *
 * @param s string to convert
 */
void toLowerCase(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

/**
 * @brief Cut the tail end of a string if it exceeds the max character length, the final character at index of 'maxSize - 1' will be a null char
 *
 * @param s string to process
 * @param maxSize maximum length of the string (including extension), anything beyond this size will be cut
 * @param extension file extension to keep at the end of the truncated string
 * @return Truncated string
 */
std::string truncateString(std::string s, int maxSize, const char *extension)
{
    // First, strip the path in case we have one as we only want the filename
    std::string strippedStr = s;
    int index = strippedStr.find_last_of("/\\");
    strippedStr = s.substr(index + 1);
    // Now remove any extension the file might have, we're gonna put it back on after. This just prevents 'ANDY.WAD.wad'
    int extIndex = strippedStr.find_last_of(".");
    strippedStr = strippedStr.substr(0, extIndex);

    // Now perform the truncating if need be
    int sizeData = maxSize - strlen(extension) - 1; // -1 to allow for NULL terminator to be within range when calling std::string.c_str()
    std::string truncatedStr = strippedStr.substr(0, sizeData) + extension;

    if (strippedStr.length() + strlen(extension) > maxSize)
    {
        std::cout << "INDicator: " << extension << " file reference in the executable has been shortened to \"" << truncatedStr << "\". Please rename the actual file when moving the file(s) to the target location." << std::endl;
    }
    return truncatedStr;
}

/**
 * @brief Pad a std::string with characters if size > string length and return it is a C string
 *
 * @param s to pad
 * @param pad character (as std::string)
 * @param size of string, if input string is shorter than this, padding occurs
 * @return padded C string
 */
const char *makePaddedCString(std::string s, std::string pad, int size)
{
    if (s.length() >= size)
    {
        return s.c_str();
    }
    else
    {
        std::string pads;
        int sizeDiff = size - s.length();
        for (int i = 0; i < sizeDiff; ++i)
        {
            pads.append(pad);
        }
        return s.append(pads).c_str();
    }
}

/**
 * @brief Write bytes to file at position offset, size will determine how many bytes are written. If data is < size, data is null-padded.
 *
 * @param offset location in file to write bytes to
 * @param file to write bytes to
 * @param size of data to write
 * @param data to write at location in file
 */
void writeBytes(int offset, std::string file, int size, std::string data)
{
    std::fstream stream(file.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::in);
    stream.seekp(offset, std::ios_base::beg);
    stream.write(makePaddedCString(data, "\0", size), size);
    stream.flush();
    stream.close();
}

/**
 * @brief Backup executable as <executable-file>.bak
 *
 * @param executable to backup
 */
void makeBackup(std::string executable)
{
    std::ifstream src(executable, std::ios::binary);
    std::ofstream dst(executable + ".bak", std::ios::binary);
    dst << src.rdbuf();
    std::cout << "INDicator: Backing up \"" << executable << "\" as \"" << executable + ".bak"
              << "\"" << std::endl;
}

/**
 * @brief Replace bytes with the wad filename at the specific offset based on whether the PC or PSX version
 *
 * @param executable to perform the replacement on
 * @param wad filename to inject
 * @param isPSX True if this is a PSX executable, False if PC
 * @param isM2K True if this is a Music 2000 executable, False if MTV Music Generator
 * (offsets are different so we need to differentiate between the two)
 */
void replaceBytesInExec(std::string executable, std::string wad, bool isPSX, bool isM2K)
{
    std::string wadFname;
    std::string indFname;
    std::string nullPad = "\0";
    std::fstream execF(executable.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::in);

    if (isPSX)
    {
        wadFname = truncateString(wad, PSX_WAD_IND_REF_SIZE_BYTES, ".wad");
        indFname = truncateString(wad, PSX_WAD_IND_REF_SIZE_BYTES, ".ind");

        // Both MTV Music Generator and Music 2000 have the same offsets for the wad/ind files,
        // so we don't need to check version for PSX
        writeBytes(PSX_WAD_BYTE_OFFSET_1, executable, PSX_WAD_IND_REF_SIZE_BYTES, wadFname);
        writeBytes(PSX_WAD_BYTE_OFFSET_2, executable, PSX_WAD_IND_REF_SIZE_BYTES, wadFname);
        writeBytes(PSX_IND_BYTE_OFFSET_1, executable, PSX_WAD_IND_REF_SIZE_BYTES, indFname);
        writeBytes(PSX_IND_BYTE_OFFSET_2, executable, PSX_WAD_IND_REF_SIZE_BYTES, indFname);
    }
    // PC
    else
    {
        wadFname = truncateString(wad, PC_WAD_IND_REF_SIZE_BYTES, ".wad");
        indFname = truncateString(wad, PC_WAD_IND_REF_SIZE_BYTES, ".ind");

        if (isM2K)
        {
            writeBytes(PC_M2K_WAD_BYTE_OFFSET_1, executable, PC_WAD_IND_REF_SIZE_BYTES, wadFname);
            writeBytes(PC_M2K_WAD_BYTE_OFFSET_2, executable, PC_WAD_IND_REF_SIZE_BYTES, wadFname);
            writeBytes(PC_M2K_WAD_BYTE_OFFSET_3, executable, PC_WAD_IND_REF_SIZE_BYTES, wadFname);
            writeBytes(PC_M2K_IND_BYTE_OFFSET_1, executable, PC_WAD_IND_REF_SIZE_BYTES, indFname);
            writeBytes(PC_M2K_IND_BYTE_OFFSET_2, executable, PC_WAD_IND_REF_SIZE_BYTES, indFname);
        }
        else
        {
            writeBytes(PC_MTVMG_WAD_BYTE_OFFSET_1, executable, PC_WAD_IND_REF_SIZE_BYTES, wadFname);
            writeBytes(PC_MTVMG_WAD_BYTE_OFFSET_2, executable, PC_WAD_IND_REF_SIZE_BYTES, wadFname);
            writeBytes(PC_MTVMG_WAD_BYTE_OFFSET_3, executable, PC_WAD_IND_REF_SIZE_BYTES, wadFname);
            writeBytes(PC_MTVMG_IND_BYTE_OFFSET_1, executable, PC_WAD_IND_REF_SIZE_BYTES, indFname);
            writeBytes(PC_MTVMG_IND_BYTE_OFFSET_2, executable, PC_WAD_IND_REF_SIZE_BYTES, indFname);
        }
    }
}