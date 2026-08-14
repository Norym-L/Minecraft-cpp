#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace mymc
{
    Texture::Texture(const int width, const int height, const int num_channels, const unsigned char* data)
        : m_width(width), m_height(height), m_numChannels(num_channels)
    {
        init(width, height, num_channels, data);
    }

    Texture::~Texture()
    {
        if (m_textureID > 0)
        {
            glDeleteTextures(1, &m_textureID);
        }
    }

    GLuint Texture::getID() const
    {
        return m_textureID;
    }

    void Texture::init(const int width, const int height, const int num_channels, const unsigned char* data)
    {
        //幂等保护：重复初始化时先释放旧纹理，避免泄漏
        if (m_textureID > 0)
        {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
        }

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        GLenum internalFormat, format;
        //注意颜色通道，多颜色通道图片万万不可使用低颜色通道加载，会有残留！！！！
        switch (num_channels)
        {
            case 1: internalFormat = GL_R8;
                format = GL_RED;
                break;
            case 2: internalFormat = GL_RG8;
                format = GL_RG;
                break;
            case 3: internalFormat = GL_RGB8;
                format = GL_RGB;
                break;
            case 4: internalFormat = GL_RGBA8;
                format = GL_RGBA;
                break;
            default: internalFormat = GL_RGB8;
                format = GL_RGB;
                break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width, height, 0, format, GL_UNSIGNED_BYTE,
                     data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void Texture::configureForAtlas() const
    {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::configurePixelArt() const
    {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    int Texture::getWidth() const
    {
        return m_width;
    }

    int Texture::getHeight() const
    {
        return m_height;
    }

    void TextureManager::clear()
    {
        m_textures.clear();
    }
}
