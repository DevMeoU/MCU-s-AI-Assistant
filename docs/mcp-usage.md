# MCP Protocol IoT Control Usage Instructions

> This document describes how to implement IoT control for ESP32 devices based on the MCP protocol. For detailed protocol flow, please refer to [`mcp-protocol.md`](./mcp-protocol.md).

## Introduction

MCP (Model Context Protocol) is a new generation protocol recommended for IoT control. It enables flexible device control by discovering and invoking "Tools" between the backend and devices using the standard JSON-RPC 2.0 format.

## Typical Usage Flow

1.  After the device starts, it establishes a connection with the backend via a basic protocol (e.g., WebSocket/MQTT).
2.  The backend initializes the session using the `initialize` method of the MCP protocol.
3.  The backend obtains all supported tools (functions) and their parameter descriptions from the device using `tools/list`.
4.  The backend invokes specific tools using `tools/call` to control the device.

For detailed protocol format and interaction, please refer to [`mcp-protocol.md`](./mcp-protocol.md).

## Device-side Tool Registration Method Description

Devices register "tools" that can be called by the backend using the `McpServer::AddTool` method. Its common function signature is as follows:

```cpp
void AddTool(
    const std::string& name,           // Tool name, recommended to be unique and hierarchical, e.g., self.dog.forward
    const std::string& description,    // Tool description, concise explanation of functionality, easy for large models to understand
    const PropertyList& properties,    // Input parameter list (can be empty), supported types: boolean, integer, string
    std::function<ReturnValue(const PropertyList&)> callback // Callback implementation when the tool is called
);
```
- name: Unique identifier for the tool, recommended to use "module.function" naming style.
- description: Natural language description, easy for AI/users to understand.
- properties: Parameter list, supported types are boolean, integer, string, can specify range and default values.
- callback: Actual execution logic when a call request is received, return value can be bool/int/string.

## Typical Registration Example (taking ESP-Hi as an example)

```cpp
void InitializeTools() {
    auto& mcp_server = McpServer::GetInstance();
    // Example 1: No parameters, control robot to move forward
    mcp_server.AddTool("self.dog.forward", "机器人向前移动", PropertyList(), [this](const PropertyList&) -> ReturnValue {
        servo_dog_ctrl_send(DOG_STATE_FORWARD, NULL);
        return true;
    });
    // Example 2: With parameters, set LED RGB color
    mcp_server.AddTool("self.light.set_rgb", "设置RGB颜色", PropertyList({
        Property("r", kPropertyTypeInteger, 0, 255),
        Property("g", kPropertyTypeInteger, 0, 255),
        Property("b", kPropertyTypeInteger, 0, 255)
    }), [this](const PropertyList& properties) -> ReturnValue {
        int r = properties["r"].value<int>();
        int g = properties["g"].value<int>();
        int b = properties["b"].value<int>();
        led_on_ = true;
        SetLedColor(r, g, b);
        return true;
    });
}
```

## Common Tool Call JSON-RPC Examples

### 1. Get Tool List
```json
{
  "jsonrpc": "2.0",
  "method": "tools/list",
  "params": { "cursor": "" },
  "id": 1
}
```

### 2. Control Chassis to Move Forward
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.chassis.go_forward",
    "arguments": {}
  },
  "id": 2
}
```

### 3. Switch Light Mode
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.chassis.switch_light_mode",
    "arguments": { "light_mode": 3 }
  },
  "id": 3
}
```

### 4. Flip Camera
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.camera.set_camera_flipped",
    "arguments": {}
  },
  "id": 4
}
```

## Remarks
- Tool names, parameters, and return values should be based on the `AddTool` registration on the device side.
- It is recommended that all new projects uniformly adopt the MCP protocol for IoT control.
- For detailed protocol and advanced usage, please refer to [`mcp-protocol.md`](./mcp-protocol.md).

---

# Hướng dẫn sử dụng điều khiển IoT giao thức MCP

> Tài liệu này mô tả cách triển khai điều khiển IoT cho các thiết bị ESP32 dựa trên giao thức MCP. Để biết chi tiết luồng giao thức, vui lòng tham khảo [`mcp-protocol.md`](./mcp-protocol.md).

## Giới thiệu

MCP (Model Context Protocol) là một giao thức thế hệ mới được khuyến nghị cho điều khiển IoT. Nó cho phép điều khiển thiết bị linh hoạt bằng cách khám phá và gọi "Công cụ" giữa backend và các thiết bị sử dụng định dạng JSON-RPC 2.0 tiêu chuẩn.

## Luồng sử dụng điển hình

1.  Sau khi thiết bị khởi động, nó thiết lập kết nối với backend thông qua một giao thức cơ bản (ví dụ: WebSocket/MQTT).
2.  Backend khởi tạo phiên bằng phương thức `initialize` của giao thức MCP.
3.  Backend lấy tất cả các công cụ (chức năng) được hỗ trợ và mô tả tham số của chúng từ thiết bị bằng cách sử dụng `tools/list`.
4.  Backend gọi các công cụ cụ thể bằng cách sử dụng `tools/call` để điều khiển thiết bị.

Để biết chi tiết định dạng giao thức và tương tác, vui lòng tham khảo [`mcp-protocol.md`](./mcp-protocol.md).

## Mô tả phương pháp đăng ký công cụ phía thiết bị

Các thiết bị đăng ký "công cụ" có thể được backend gọi bằng phương thức `McpServer::AddTool`. Chữ ký hàm phổ biến của nó như sau:

```cpp
void AddTool(
    const std::string& name,           // Tên công cụ, khuyến nghị là duy nhất và có phân cấp, ví dụ: self.dog.forward
    const std::string& description,    // Mô tả công cụ, giải thích ngắn gọn chức năng, dễ hiểu cho các mô hình lớn
    const PropertyList& properties,    // Danh sách tham số đầu vào (có thể trống), các loại được hỗ trợ: boolean, integer, string
    std::function<ReturnValue(const PropertyList&)> callback // Triển khai callback khi công cụ được gọi
);
```
- name: Định danh duy nhất cho công cụ, khuyến nghị sử dụng kiểu đặt tên "module.function".
- description: Mô tả ngôn ngữ tự nhiên, dễ hiểu cho AI/người dùng.
- properties: Danh sách tham số, các loại được hỗ trợ là boolean, integer, string, có thể chỉ định phạm vi và giá trị mặc định.
- callback: Logic thực thi thực tế khi nhận được yêu cầu gọi, giá trị trả về có thể là bool/int/string.

## Ví dụ đăng ký điển hình (lấy ESP-Hi làm ví dụ)

```cpp
void InitializeTools() {
    auto& mcp_server = McpServer::GetInstance();
    // Ví dụ 1: Không có tham số, điều khiển robot di chuyển về phía trước
    mcp_server.AddTool("self.dog.forward", "Robot di chuyển về phía trước", PropertyList(), [this](const PropertyList&) -> ReturnValue {
        servo_dog_ctrl_send(DOG_STATE_FORWARD, NULL);
        return true;
    });
    // Ví dụ 2: Có tham số, đặt màu RGB của đèn LED
    mcp_server.AddTool("self.light.set_rgb", "Đặt màu RGB", PropertyList({
        Property("r", kPropertyTypeInteger, 0, 255),
        Property("g", kPropertyTypeInteger, 0, 255),
        Property("b", kPropertyTypeInteger, 0, 255)
    }), [this](const PropertyList& properties) -> ReturnValue {
        int r = properties["r"].value<int>();
        int g = properties["g"].value<int>();
        int b = properties["b"].value<int>();
        led_on_ = true;
        SetLedColor(r, g, b);
        return true;
    });
}
```

## Ví dụ JSON-RPC gọi công cụ phổ biến

### 1. Lấy danh sách công cụ
```json
{
  "jsonrpc": "2.0",
  "method": "tools/list",
  "params": { "cursor": "" },
  "id": 1
}
```

### 2. Điều khiển khung gầm di chuyển về phía trước
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.chassis.go_forward",
    "arguments": {}
  },
  "id": 2
}
```

### 3. Chuyển đổi chế độ đèn
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.chassis.switch_light_mode",
    "arguments": { "light_mode": 3 }
  },
  "id": 3
}
```

### 4. Lật camera
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.camera.set_camera_flipped",
    "arguments": {}
  },
  "id": 4
}
```

## Lưu ý
- Tên công cụ, tham số và giá trị trả về phải dựa trên đăng ký `AddTool` ở phía thiết bị.
- Khuyến nghị tất cả các dự án mới thống nhất áp dụng giao thức MCP để điều khiển IoT.
- Để biết chi tiết giao thức và cách sử dụng nâng cao, vui lòng tham khảo [`mcp-protocol.md`](./mcp-protocol.md).