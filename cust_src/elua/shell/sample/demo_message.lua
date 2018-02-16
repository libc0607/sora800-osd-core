local msg = {}
local count = 0

-- ��ʼ��keypad ʹ�ü��̱����ȳ�ʼ��
rtos.init_module(rtos.MOD_KEYPAD, 0, 0x1f, 0x1f)

-- ������ʱ��
rtos.timer_start(1, 1000)

while true do
    -- �����ȴ� 
    --msg = rtos.receive(rtos.INF_TIMEOUT)
    -- ��ʱ�ȴ�
    msg = rtos.receive(2000) -- receive ��Ϣ�ӿ� param1: timeout ��msΪ��λ
    
    -- timer��Ϣ���� 
    if(msg.id == rtos.MSG_TIMER) then  
    
        print("timer id: " .. msg.timer_id)
        
        if(1 == msg.timer_id) then 
           rtos.timer_start(2, 200)
        elseif 2 == msg.timer_id then
           rtos.timer_start(3, 3000)
        elseif 3 == msg.timer_id then
           -- nothing to do, just for test
        end
    
    -- uart/atc��Ϣ ͨ��msg.uart_id����
    elseif msg.id == rtos.MSG_UART_RXDATA then 
    
        repeat
            s = uart.read(msg.uart_id, "*l", 0)
            print(s)
        until string.len(s) == 0
    
    -- ������Ϣ���� Ŀǰ��3������ ״̬������ֵ pressed row col
    elseif msg.id == rtos.MSG_KEYPAD then
    
        print(msg.pressed)
        print("key_matrix_row:" .. msg.key_matrix_row)
        print("key_matrix_col:" .. msg.key_matrix_col)
        
    -- �ȴ���Ϣ��ʱ, �����ȴ������ע����Ϣ
    elseif msg.id == rtos.WAIT_MSG_TIMEOUT then
    
        count = count+1
        print("rtos receive msg timeout count:" .. count)
                
    -- ������Ϣ
    else
        print("rtos.receive: msg id(" .. msg.id .. ")")
    end
end
