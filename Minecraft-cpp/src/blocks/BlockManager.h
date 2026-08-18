#ifndef MINECRAFT_CPP_BLOCKMANAGER_H
#define MINECRAFT_CPP_BLOCKMANAGER_H
#include <cstdint>
#include <unordered_map>

#include "block.h"

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

private:
    std::unordered_map<std::uint64_t, std::unique_ptr<Block> > m_blockMap;    // 方块映射表
};
} // block

#endif //MINECRAFT_CPP_BLOCKMANAGER_H