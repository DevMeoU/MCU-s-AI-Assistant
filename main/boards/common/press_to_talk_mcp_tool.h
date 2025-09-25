#ifndef PRESS_TO_TALK_MCP_TOOL_H
#define PRESS_TO_TALK_MCP_TOOL_H

#include "mcp_server.h"
#include "settings.h"

// Lớp công cụ MCP chế độ nhấn để nói có thể tái sử dụng
class PressToTalkMcpTool {
private:
    bool press_to_talk_enabled_;

public:
    PressToTalkMcpTool();
    
    // Khởi tạo công cụ, đăng ký với máy chủ MCP
    void Initialize();
    
    // Lấy trạng thái chế độ nhấn để nói hiện tại
    bool IsPressToTalkEnabled() const;

private:
    // Hàm gọi lại công cụ MCP
    ReturnValue HandleSetPressToTalk(const PropertyList& properties);
    
    // Phương thức nội bộ: đặt trạng thái nhấn để nói và lưu vào cài đặt
    void SetPressToTalkEnabled(bool enabled);
};

#endif // PRESS_TO_TALK_MCP_TOOL_H