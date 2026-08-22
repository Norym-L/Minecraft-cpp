/**
 * @file Texture.h
 * @brief 贴图模块：封装 OpenGL 纹理对象的创建、上传与句柄管理，并提供带缓存的贴图管理器。
 */
#pragma once

#include <GL/glew.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mymc
{
/// @brief 贴图类，封装 OpenGL 纹理对象的创建、上传与句柄管理。
class Texture
{
public:
    /**
         * @brief 创建一个贴图。
         * @param width 宽度
         * @param height 高度
         * @param num_channels 通道数量
         * @param data 图片数据指针
         */
    Texture(int width, int height, int num_channels, const unsigned char *data);

    /// @brief 析构函数，释放贴图资源。
    ~Texture();

    /**
         * @brief 获取贴图 ID。
         * @return GLuint OpenGL 纹理句柄
         */
    [[nodiscard]] GLuint getID() const;

    /**
         * @brief 初始化贴图对象。
         * @param width 宽度
         * @param height 高度
         * @param num_channels 通道数量
         * @param data 图片数据指针
         */
    void init(int width, int height, int num_channels, const unsigned char *data);

    /**
         * @brief 获取贴图宽度。
         * @return int 宽度
         */
    [[nodiscard]] int getWidth() const;

    /**
         * @brief 获取贴图高度。
         * @return int 高度
         */
    [[nodiscard]] int getHeight() const;

    /**
         * @brief 获取通道数量。
         * @return int 通道数
         */
    [[nodiscard]] int getChannels() const;

    /**
         * @brief 获取 CPU 端像素副本（RGBA 顺序）。
         * @return 像素数据引用；未保留时为空
         */
    [[nodiscard]] const std::vector<unsigned char> &getPixels() const;

    /**
         * @brief CPU 像素是否可用。
         * @return true 可用（供图集拼接等）
         */
    [[nodiscard]] bool hasPixels() const;

    /**
     * 获取贴图
     * @param path 贴图路径
     * @return 贴图共享指针
     */
    static std::shared_ptr<Texture> Load(const std::string &path);

    /**
         * @brief 将贴图配置为图集模式：关闭 mipmap、使用 CLAMP_TO_EDGE，避免字形渗色。
         */
    void configureForAtlas() const;

    /**
         * @brief 将贴图配置为像素风模式：最近邻过滤 + 边缘钳制，关闭 mipmap，避免像素模糊与图集渗色。
         */
    void configurePixelArt() const;

private:
    int m_width = 0;        ///< 贴图宽度
    int m_height = 0;       ///< 贴图高度
    int m_numChannels = 0;  ///< 通道数量
    GLuint m_textureID = 0; ///< OpenGL 纹理句柄
    std::vector<unsigned char> m_pixels; ///< CPU 端像素副本（供图集拼接等用途）
};

/// @brief 贴图管理器，维护贴图缓存，避免同一路径重复加载。
class TextureManager
{
public:
    /**
     * @brief 获取或加载贴图（带缓存）。
     * @param path 图片路径
     * @return std::shared_ptr<Texture> 贴图对象共享指针；加载失败返回 nullptr
     */
    std::shared_ptr<Texture> getOrLoadTexture(const std::string& path);

    /// @brief 清空贴图缓存（需在 GL 上下文销毁前调用）。
    void clear();

private:
    /** @brief 从文件实际加载贴图（不经过缓存）。 */
    static std::shared_ptr<Texture> LoadFromFile(const std::string &path) ;

private:
    std::unordered_map<std::string, std::shared_ptr<Texture> > m_textures; ///< 贴图路径到贴图对象的缓存表
};
}
