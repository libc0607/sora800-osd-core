
-- debug assert
uart.setup(uart.ATC, 0, 0, uart.PAR_NONE, uart.STOP_1)
uart.write(uart.ATC, "at*exassert=1\r\n")

local lcd_init_cmd = {
0x00020011,
0x00010078,
0x000200B1,
0x00030002,
0x00030035,
0x00030036,
0x000200B2,
0x00030002,
0x00030035,
0x00030036,
0x000200B3,
0x00030002,
0x00030035,
0x00030036,
0x00030002,
0x00030035,
0x00030036,
0x000200B4,
0x00030003,
0x000200C0,
0x000300A2,
0x00030002,
0x00030084,
0x000200C1,
0x000300C5,
0x000200C2,
0x0003000D,
0x00030000,
0x000200C3,
0x0003008D,
0x0003002A,
0x000200C4,
0x0003008D,
0x000300EE,
0x000200C5,
0x00030003,
0x00020036,
0x000300C8,
0x000200E0,
0x00030012,
0x0003001C,
0x00030010,
0x00030018,
0x00030033,
0x0003002C,
0x00030025,
0x00030028,
0x00030028,
0x00030027,
0x0003002F,
0x0003003C,
0x00030000,
0x00030003,
0x00030003,
0x00030010,
0x000200E1,
0x00030012,
0x0003001C,
0x00030010,
0x00030018,
0x0003002D,
0x00030028,
0x00030023,
0x00030028,
0x00030028,
0x00030026,
0x0003002F,
0x0003003B,
0x00030000,
0x00030003,
0x00030003,
0x00030010,
0x0002003A,
0x00030005,
0x00020029,
}

local lcd_param ={
  width = 128,
  height = 128,
  bpp = 16, -- bits per pixel 16bit color lcd or 1bit mono lcd
  yoffset = 32, -- 160-128=32
  bus = disp.BUS_SPI4LINE, -- support disp.BUS_SPI4LINE or disp.PARALLEL Ĭ��spi4line
  pinrst = pio.P0_16, -- lcd reset pin
}

-- ����ϵͳ��ldo
pmd.deepsleep(0)
pmd.ldoset(1, pmd.LDO_KEYPAD, pmd.LDO_LCD)

-- ��ʼ����ʾ�ӿ�
disp.init(lcd_param, lcd_init_cmd)

disp.clear() -- ���������������,����ˢ��
disp.puttext("hello,lcd!overflow,1234567890",0,48) -- just write to ram cache ������update֮��Ż�ˢ����
disp.update()

-- �ر���ʾ�ӿ� �����Ĳ�Ʒһ��ʹ�ò���Ҫ�ر� һֱ��ϵͳ�ػ�
disp.close()

-- ����10��
rtos.sleep(10000)

-- �ر�ldo ����ϵͳ����˯��
pmd.ldoset(0, pmd.LDO_KEYPAD, pmd.LDO_LCD)
pmd.deepsleep(1)