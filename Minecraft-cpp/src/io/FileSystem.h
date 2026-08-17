/**
 * @file FileSystem.h
 * @brief 文件系统模块：负责可执行目录/资源目录解析与文件读写，提供跨平台实现。
 */
#pragma once
#include <filesystem>
#include <vector>

namespace mymc
{
/// @brief 文件系统类，负责路径解析与文件读写，提供跨平台实现。
class FileSystem
{
public:
    /**
     * @brief 获取可执行文件所在目录（多平台实现）
     * @return std::filesystem::path 可执行文件所在目录路径
     */
    static std::filesystem::path GetExecutableFolder();

    /**
     * @brief 获取资源文件夹
     * @return std::filesystem::path 资源文件夹路径
     */
    static std::filesystem::path GetAssetsFolder() ;

    /**
     * @brief 读取文件内容
     * @param path 文件路径
     * @return std::vector<char> 文件内容（字符）；无法打开返回空值
     */
    static std::vector<char> LoadFile(const std::filesystem::path &path) ;

    /**
     * @brief 通过相对路径读取资源文件内容
     * @param relative_path 文件相对路径（相对于资源文件夹）
     * @return std::vector<char> 文件内容（字符）；无法打开返回空值
     */
    static std::vector<char> LoadAssetFile(const std::string &relative_path) ;

    /**
     * @brief 通过相对路径读取资源文件文本
     * @param relative_path 文件相对路径（相对于资源文件夹）
     * @return std::string 文件字符串；无法打开返回空值
     */
    static std::string LoadAssetFileText(const std::string &relative_path) ;
};
}
