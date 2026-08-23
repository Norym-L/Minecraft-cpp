#ifndef MINECRAFT_CPP_BLOCKMANAGER_H
#define MINECRAFT_CPP_BLOCKMANAGER_H
#include <cstdint>
#include <unordered_map>

#include "block.h"
#include "graphics/TextureAtlas.h"
#include "render/MeshBuilder.h" // Face 枚举

namespace block
{
class BlockManager
{
public:
    BlockManager();

    BlockManager(const BlockManager &) = delete;

    BlockManager &operator=(const BlockManager &) = delete;

    BlockManager(BlockManager &&) = delete;

    BlockManager &operator=(BlockManager &&) = default;

public:
    /**
         * 获取方块管理器单例
         * @return 方块管理器引用
         */
    static BlockManager &GetInstance();

    /**
         * 初始化方块管理器（注册内置方块）。
         * 注意：必须在 OpenGL 上下文创建完成后调用，切勿在构造函数中构造 Mesh。
         */
    void init();

    /**
         * 注册方块
         * @param block 方块的独特指针
         */
    void registerBlock(std::unique_ptr<Block> block);

    /**
         * 注册方块（直接构造）
         * @tparam Args 构造参数类型
         * @param args 参数
         */
    template<typename... Args>
    void registerBlock(Args &&... args)
    {
        registerBlock(std::make_unique<Block>(std::forward<Args>(args)...));
    }

    /**
     * 通过id获取方块
     * @param block_id 方块注册id
     * @return 方块
     */
    const Block &getBlock(std::uint64_t block_id) const;

    /**
     * 从json注册方块
     * @param path json路径
     */
    void loadFromJson(const std::string& path = "mymc/block/block_register.json");

    /**
     * 获取全局方块材质（所有方块共用的图集材质）
     * @return 材质共享指针；图集构建失败时为 nullptr
     */
    [[nodiscard]] std::shared_ptr<mymc::Material> getAtlasMaterial() const { return m_atlasMaterial; }

    /**
     * 获取全局方块图集纹理（供 UI 图标等使用）
     * @return 图集纹理共享指针；图集构建失败时为 nullptr
     */
    [[nodiscard]] const std::shared_ptr<mymc::Texture> &getAtlasTexture() const { return m_atlasTexture; }

    /**
     * 获取某方块某面的图集 UV 矩形
     * @param block_id 方块注册id
     * @param face     面方向
     * @return UV 矩形；未找到时返回全零矩形
     */
    [[nodiscard]] mymc::UVRect getAtlasUV(std::uint64_t block_id, mymc::Face face) const;

    /**
     * 把网格面方向映射到 6 贴图数组下标（顶/底/前(+Z)/后(-Z)/右(+X)/左(-X)）
     * @param face 面方向
     * @return 0~5
     */
    static int faceToTextureIndex(mymc::Face face);

private:
    /// 构建贴图图集并创建全局方块材质（init 内部调用）
    void buildAtlas();

private:
    std::unordered_map<std::uint64_t, std::unique_ptr<Block> > m_blockMap;      // 方块映射表
    std::shared_ptr<mymc::TextureAtlas> m_atlas;                                // 贴图图集
    std::shared_ptr<mymc::Texture> m_atlasTexture;                              // 图集纹理（构建成功后持有）
    std::shared_ptr<mymc::Material> m_atlasMaterial;                            // 全局方块材质
};
} // block

#endif //MINECRAFT_CPP_BLOCKMANAGER_H