
print("start display chinese.lua")

local function lcdinit()
	local lcdparam = {
		width = 128,
		height = 64,
		bpp = 1, -- bits per pixel 1Ϊ�ڰ���
		xoffset = 1, -- ��Ϊ0�ɲ����ø���
		yoffset = 32, -- ��Ϊ0�ɲ����ø���
		bus = disp.BUS_SPI4LINE,
		pinrst = pio.P0_12, -- LCD_RST GPIO_12
		pincs = pio.P1_7, -- LCD_CS GPO_7 ��ʹ��Ĭ��Ƭѡ�����ø���
		initcmd = { -- ��ʼ��������
			0xE2,
			0xA3,
			0xA0,
			0xC8,
			0xA6,
			0x2F,
			0x23,
			0x81,
			0x2E,
			0xAF,
		},
	}

	-- ��ʼ����ʾ�ӿ�
	disp.init(lcdparam)
end

pmd.ldoset(1,pmd.LDO_LCD) -- backlight on

lcdinit()

disp.clear()
-- �ָ�16
disp.puttext("����������������������������������������������������������",0,0)

disp.update()

local msg

while true do
	msg = rtos.receive(rtos.INF_TIMEOUT)
	print("msg:",msg.id)
end

print("done.")
