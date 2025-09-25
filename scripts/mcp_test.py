# server.py
from mcp.server.fastmcp import FastMCP
import logging

logger = logging.getLogger("mcp_demo")

# Tạo MCP server
mcp = FastMCP("MyAssistant")

# Tool mở nhạc Spotify (giả lập)
@mcp.tool()
def play_music(song: str) -> dict:
    """
    Phát nhạc từ Spotify hoặc YouTube theo tên bài hát.
    """
    logger.info(f"Yêu cầu phát nhạc: {song}")
    # TODO: gọi Spotify API thật ở đây
    return {"success": True, "result": f"Đã phát bài hát {song} trên Spotify 🎶"}

# Tool bật đèn
@mcp.tool()
def turn_on_light(room: str = "phòng khách") -> dict:
    """
    Bật đèn trong phòng chỉ định.
    """
    logger.info(f"Bật đèn ở {room}")
    # TODO: gửi lệnh tới Home Assistant/MQTT
    return {"success": True, "result": f"Đèn {room} đã bật 💡"}

# Tool điều khiển động cơ
@mcp.tool()
def set_motor_speed(speed: int) -> dict:
    """
    Điều chỉnh tốc độ động cơ (0-100%).
    """
    logger.info(f"Đặt tốc độ động cơ = {speed}%")
    # TODO: gửi lệnh tới vi điều khiển
    return {"success": True, "result": f"Động cơ chạy ở {speed}% ⚙️"}

if __name__ == "__main__":
    # Chạy server theo chuẩn MCP (qua stdio)
    mcp.run(transport="stdio")
