#include <devicedrivers.h>

ParallelILI9486::ParallelILI9486(void)
{
    {                                      // 设置总线控制。
        auto cfg = _bus_instance.config(); // 获取用于总线设置的结构。

        // 8位并行总线设置
        cfg.freq_write = 30000000; // 发送时钟（最大可将20MHz、80MHz舍入为整数）
        cfg.pin_wr = 12;           // 连接WR的针脚编号
        cfg.pin_rd = 46;           // 连接RD的针脚编号
        cfg.pin_rs = 13;           // 连接RS（D/C）的针脚编号

        cfg.pin_d0 = 4;  // 连接D0的管脚编号
        cfg.pin_d1 = 5;  // 连接D1的管脚编号
        cfg.pin_d2 = 6;  // 连接D2的管脚编号
        cfg.pin_d3 = 7;  // 连接D3的管脚编号
        cfg.pin_d4 = 15; // 连接D4的管脚编号
        cfg.pin_d5 = 16; // 连接D5的管脚编号
        cfg.pin_d6 = 10; // 连接D6的管脚编号
        cfg.pin_d7 = 11; // 连接D7的管脚编号

        _bus_instance.config(cfg);              // 在总线上反映设置值。
        _panel_instance.setBus(&_bus_instance); // 把总线设置在面板上。
    }

    {                                        // 设置显示面板控制。
        auto cfg = _panel_instance.config(); // 获取用于设置显示面板的结构。
        cfg.pin_cs = 14;                     // 连接CS的管脚编号（-1=disable）
        cfg.pin_rst = -1;                    // 连接RST的管脚编号（-1=disable）
        cfg.pin_busy = -1;                   // 连接BUSY的管脚编号（-1=disable）
        cfg.memory_width = 320;              // 驱动器IC支持的最大宽度
        cfg.memory_height = 480;             // 驱动器IC支持的最大高度
        cfg.panel_width = 320;               // 实际可见宽度
        cfg.panel_height = 480;              // 实际可见高度
        cfg.offset_x = 0;                    // 面板X方向偏移量
        cfg.offset_y = 0;                    // 面板Y方向偏移量
        cfg.offset_rotation = 0;             // 旋转方向值的偏移0~7（4~7上下反转）
        cfg.dummy_read_pixel = 8;            // 像素读取前虚拟读位数
        cfg.dummy_read_bits = 1;             // 读取非像素数据之前的虚拟读位数
        cfg.readable = true;                 // 可读取数据时设置为真
        cfg.invert = false;                  // 如果面板的明暗颠倒，则设置为真
        cfg.rgb_order = false;               // 如果面板中的红色和蓝色替换为真
        cfg.dlen_16bit = false;              // 在以16 bit为单位发送数据长度的面板的情况下设定为真
        cfg.bus_shared = true;               // 与SD卡共享总线时设置为真（通过drawJpgFile等进行总线控制）

        _panel_instance.config(cfg);
    }

    setPanel(&_panel_instance); // 设置要使用的面板。

    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
}