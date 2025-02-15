#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

std::string removeQuotes(std::filesystem::directory_entry entry)
{
    std::string fileName = entry.path().filename().string();
    if(!fileName.empty() && fileName.front() == '"' && fileName.back() == '"') 
    {
        fileName.erase(0, 1);
        fileName.erase(fileName.size() - 1, 1);
    }
    return fileName;
}

std::string calculateSpaces(std::string fileName)
{
    int16_t num = 32 - fileName.length();
    if(num<0) { return " "; }
    return std::string(num, ' ');
}

std::pair<float, std::string> humanReadable(float bytes)
{
    float num;
    std::string unit;

    if(bytes<1024){ num = bytes; unit = " bytes";}
    else if (bytes>1024 && bytes<1024*1024){ num = bytes/1024; unit = " kilobytes";}
    else if (bytes>1024*1024 && bytes<1024*1024*1024){ num = bytes/1048576; unit = " megabytes";}
    return std::make_pair(num, unit);
}


int main(int argc, char* argv[])
{
    bool showHidden = false;
    bool showSize = false;
    bool showSizeB = false;
    bool showSizeK = false;
    bool showSizeM = false;
    bool showSizeHR = false;

    for(int i=1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg.contains("-h")) { showHidden = true; }
        else if (arg.contains("-sB") && !arg.contains("-sK") && !arg.contains("-sM") && !arg.contains("-sH")) { showSize = true; showSizeB = true; }
        else if (arg.contains("-sK") && !arg.contains("-sB") && !arg.contains("-sM") && !arg.contains("-sH")) { showSize = true; showSizeK = true; }
        else if (arg.contains("-sM") && !arg.contains("-sK") && !arg.contains("-sB") && !arg.contains("-sH")) { showSize = true; showSizeM = true; }
        else if (arg.contains("-sH") && !arg.contains("-sK") && !arg.contains("-sB") && !arg.contains("-sM")) { showSize = true; showSizeHR = true; }
    }

    std::vector<std::string> dirs;
    std::vector<std::string> files;
    std::vector<std::string> other;
    
    std::vector<float> filesSize;
    std::vector<float> otherSize;

    auto currentDir = std::filesystem::current_path();
    for(const auto &entry : std::filesystem::directory_iterator(currentDir))
    {
        std::string fileName = removeQuotes(entry);
        if(fileName.starts_with(".") && !showHidden) { continue; }

        if(std::filesystem::is_directory(entry.path()))
        {
            dirs.push_back(fileName);   
        } 
        else if (std::filesystem::is_regular_file(entry.path()))
        {
            if(fileName.contains(".") && !fileName.starts_with(".")) 
            { 
                other.push_back(fileName);
                if(showSize) { otherSize.push_back(std::filesystem::file_size(entry.path())); }
                continue; 
            }

            if(showSize) { filesSize.push_back(std::filesystem::file_size(entry.path())); }
            files.push_back(fileName);
        }
        else 
        {
            if(showSize) { otherSize.push_back(std::filesystem::file_size(entry.path())); }
            other.push_back(fileName);
        }
    }

    std::vector<std::string> sizeUnitFiles;
    std::vector<std::string> sizeUnitOther;

    if(showSizeB)
    {
        for(int i = 0; i < filesSize.end() - filesSize.begin(); i++)
        {
            sizeUnitFiles.push_back(" bytes");
        }
        for(int i = 0; i < otherSize.end() - otherSize.begin(); i++)
        {
            sizeUnitOther.push_back(" bytes");
        }
    }
    if(showSizeK)
    {
        for(int i = 0; i < filesSize.end() - filesSize.begin(); i++)
        {
            filesSize[i] /= 1024;
            filesSize[i] = std::round(filesSize[i]*10)/10;
            sizeUnitFiles.push_back(" kilobytes");
        }
        for(int i = 0; i < otherSize.end() - otherSize.begin(); i++)
        {
            otherSize[i] /= 1024;
            otherSize[i] = std::round(otherSize[i]*10)/10;
            sizeUnitOther.push_back(" kilobytes");
        }
    }
    if(showSizeM)
    {
        for(int i = 0; i < filesSize.end() - filesSize.begin(); i++)
        {
            filesSize[i] /= 1048576;
            filesSize[i] = std::round(filesSize[i]*10)/10;
            sizeUnitFiles.push_back(" megabytes");
        }
        for(int i = 0; i < otherSize.end() - otherSize.begin(); i++)
        {
            otherSize[i] /= 1048576;
            otherSize[i] = std::round(otherSize[i]*10)/10;
            sizeUnitOther.push_back(" megabytes");
        }
    }
    if(showSizeHR)
    {
        for(int i = 0; i < filesSize.end() - filesSize.begin(); i++)
        {
            std::pair<float, std::string> hr = humanReadable(filesSize[i]);
            filesSize[i] = hr.first;
            sizeUnitFiles.push_back(hr.second);
        }
        for(int i = 0; i < otherSize.end() - otherSize.begin(); i++)
        {
            std::pair<float, std::string> hr = humanReadable(otherSize[i]);
            otherSize[i] = hr.first;
            sizeUnitOther.push_back(hr.second);
        }
    }


    for(int i = 0; i< dirs.end() - dirs.begin(); i++)
    {
        std::cout<< "   " << "\033[34m" <<dirs[i]<< "\033[0m"<<std::endl;
    }
    for(int i = 0; i< files.end() - files.begin(); i++)
    {
        std::cout<<  "   " << "\033[32m" <<files[i]<< "\033[0m";
        if(showSize) { std::cout << calculateSpaces(files[i]) << std::round(filesSize[i]*10)/10 << sizeUnitFiles[i]; }
        std::cout<<std::endl;
    }
    for(int i = 0; i< other.end() - other.begin(); i++)
    {
        std::cout<< "   " << other[i];
        if(showSize) { std::cout << calculateSpaces(other[i]) << std::round(otherSize[i]*10)/10 << sizeUnitOther[i]; }
        std::cout<<std::endl;
    }

    return EXIT_SUCCESS;
}