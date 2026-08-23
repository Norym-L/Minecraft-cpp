#include "HotbarWidget.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <glm/vec2.hpp>

#include "blocks/BlockManager.h"
#include "controls/Slot.h"
#include "core/Core.h"
#include "entity/components/InventoryBagComponent.h"
#include "entity/components/InventorySelectorComponent.h"
#include "render/MeshBuilder.h" // mymc::Face

#include <GLFW/glfw3.h> // 必须在 glew 之后（Core.h 链已引入 glew）

namespace ui
{
HotbarWidget::HotbarWidget(const IFont *font)
{
    // 创建 9 个槽位子组件，序号 1-9，点击回调绑定到选择组件
    for (int i = 0; i < entity::HOTBAR_SIZE; ++i)
    {
        auto slot = std::make_unique<Slot>(font);
        slot->m_numberText = std::to_string(i + 1);
        slot->onClickCallback = [this, i]() { selectSlot(i); };
        addChild(std::move(slot));
    }
}

void HotbarWidget::selectSlot(const int index)
{
    auto &core = mymc::Core::GetInstance();
    if (auto *player = core.getEntityManager().getMainCamera())
    {
        if (auto *selector = player->getComponent<entity::InventorySelectorComponent>())
        {
            selector->setSelectedIndex(index);
        }
    }
}

void HotbarWidget::update(const float delta_time)
{
    auto &core = mymc::Core::GetInstance();
    auto &input = core.getInputManager();

    auto *player = core.getEntityManager().getMainCamera();
    auto *bag    = player ? player->getComponent<entity::InventoryBagComponent>() : nullptr;
    auto *sel    = player ? player->getComponent<entity::InventorySelectorComponent>() : nullptr;

    if (bag == nullptr || sel == nullptr)
    {
        return; // 玩家物品栏组件缺失：不刷新（热栏保持隐藏态）
    }

    // 布局：底部居中（每帧按窗口尺寸重算，窗口缩放自适应）
    const float bar_width = static_cast<float>(entity::HOTBAR_SIZE) * kSlotSize +
                            static_cast<float>(entity::HOTBAR_SIZE - 1) * kSlotGap;
    const glm::vec2 origin(static_cast<float>(core.getWindowWidth() - static_cast<int>(bar_width)) * 0.5f, 12.0f);

    // 图标纹理与 UV 查询统一走 Core 持有的 BlockManager 实例（与区块渲染一致；
    // 注意：BlockManager::GetInstance() 单例未被 init，图集为空）
    auto &block_manager = mymc::Core::GetInstance().getBlockManager();
    auto *atlas_texture = block_manager.getAtlasTexture().get();
    const int selected  = sel->getSelectedIndex();

    for (int i = 0; i < entity::HOTBAR_SIZE; ++i)
    {
        auto &slot = *static_cast<Slot *>(getChildren()[static_cast<size_t>(i)].get());
        slot.setPosition(origin + glm::vec2(static_cast<float>(i) * (kSlotSize + kSlotGap), 0.0f));
        slot.setSize(glm::vec2(kSlotSize, kSlotSize));
        slot.m_selected = (i == selected);

        const entity::ItemStack stack = bag->getHotbarSlot(i);
        slot.m_empty = (stack.blockId == 0);
        if (slot.m_empty)
        {
            slot.m_iconTexture = nullptr;
            slot.m_cornerText.clear();
        }
        else
        {
            // 图标：方块图集顶面 UV（未注册方块 id 的防护：getAtlasUV 内部 getBlock 会抛异常）
            mymc::UVRect uv;
            bool         uv_valid = false;
            try
            {
                uv        = block_manager.getAtlasUV(stack.blockId, mymc::Face::Y_PLUS);
                uv_valid  = uv.size.x > 0.0f && uv.size.y > 0.0f;
            }
            catch (const std::out_of_range &)
            {
                uv_valid = false;
            }
            if (uv_valid)
            {
                slot.m_iconTexture = atlas_texture;
                slot.m_iconUV      = uv;
                slot.m_cornerText  = stack.count > 1 ? std::to_string(stack.count) : "";
            }
            else
            {
                slot.m_iconTexture = nullptr; // 未知方块：不显示图标
                slot.m_cornerText.clear();
            }
        }
    }

    // 数字键 1-9：直接选中对应槽位（边沿触发）
    for (int i = 0; i < entity::HOTBAR_SIZE; ++i)
    {
        if (input.wasKeyPressed(GLFW_KEY_1 + i))
        {
            sel->setSelectedIndex(i);
        }
    }

    // 滚轮：正向 selectNext，负向 selectPrevious（clearStates 每帧清零）
    const glm::vec2 scroll = input.getScrollOffset();
    if (scroll.y > 0.0f)
    {
        sel->selectNext();
    }
    else if (scroll.y < 0.0f)
    {
        sel->selectPrevious();
    }
}
} // namespace ui