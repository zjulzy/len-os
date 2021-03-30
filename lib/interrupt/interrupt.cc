#include "interrupt.h"

//初始化中断相关==============================================================================
//初始化8259A,操作顺序不可逆转
//依次向主片和从片中写入ICW1,ICW2,ICW3,ICW4
void init_8259A()
{
    // Master 8259, ICW1
    out_byte(INT_M_CTL, 0x11);

    // Slave  8259, ICW1
    out_byte(INT_S_CTL, 0x11);

    // Master 8259, ICW2. 设置 '主8259' 的中断入口地址为 0x20
    out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0);

    // Slave  8259, ICW2. 设置 '从8259' 的中断入口地址为 0x28
    out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8);

    // Master 8259, ICW3
    //意思是从芯片接在主芯片的IR2位置
    out_byte(INT_M_CTLMASK, 0x4);

    // Slave  8259, ICW3
    //意思是从芯片接在主芯片的IR2位置
    out_byte(INT_S_CTLMASK, 0x2);

    // Master 8259, ICW4
    // 设置芯片工作在80x86模式
    out_byte(INT_M_CTLMASK, 0x1);

    // Slave  8259, ICW4
    // 设置芯片工作在80x86模式
    out_byte(INT_S_CTLMASK, 0x1);

    // Master 8259, OCW1
    // 主芯片关闭所有中断
    out_byte(INT_M_CTLMASK, 0xFF);

    // Slave  8259, OCW1
    //从芯片关闭所有中断
    out_byte(INT_S_CTLMASK, 0xFF);
    init_keyboard();
    init_clock();
}
void init_keyboard()
{
    kb_buffer.count = 0;
    kb_buffer.p_head = kb_buffer.p_tail = kb_buffer.buffer;
    enable_irq(KEYBOARD_IRQ);
}
void init_clock()
{
    //初始化8253PIT
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
    out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
    enable_irq(CLOCK_IRQ);
}
//====================================================================================================

//中断处理==============================================================================================
void next_quene(PROCESS *&curr_head, PROCESS *&curr_tail, PROCESS *&next_head, PROCESS *&next_tail, PROCESS *&curr, PROCESS *&tail, int next_ticks)
{
    curr->ticks--;
    if (curr->ticks == 0)
    {
        curr->ticks = next_ticks;
        curr_head = curr->next_pcb;
        if (next_tail != tail)
        {
            next_tail->next_pcb = curr;
            next_tail = curr;
            next_tail->next_pcb = tail;
        }
        else
        {
            next_head = curr;
            next_tail = curr;
            next_tail->next_pcb = tail;
        }
        if (curr_tail == curr)
        {
            curr = next_head;
            curr_tail = tail;
        }
        else
        {
            curr = curr_head;
        }
    }
}

//时钟中断处理函数
void clock_handler()
{
    //disp_str("#");
    if (process_queen1_tail == process_tail)
    {
        if (process_queen2_tail == process_tail)
        {

            p_proc_ready->ticks--;
            if (p_proc_ready->ticks == 0)
            {
                p_proc_ready->ticks = LAST_QUENE_SLICE;

                if (p_proc_ready == process_tail)
                {
                    p_proc_ready = process_queen3_head;
                }
                else
                {
                    p_proc_ready = p_proc_ready->next_pcb;
                }
            }
        }
        else
        {
            next_quene(process_queen2_head, process_queen2_tail, process_queen3_head, process_queen3_tail, p_proc_ready, process_tail, LAST_QUENE_SLICE);
        }
    }
    else
    {
        next_quene(process_queen1_head, process_queen1_tail, process_queen2_head, process_queen2_tail, p_proc_ready, process_tail, SECOND_QUENE_SLICE);
    }

    ticks++;
}
void interrupt_request(int irq)
{
    if (irq == 0)
    {
        clock_handler();
    }
    if (irq == 1)
    {
        keyboard_handler();
    }
}
void keyboard_handler()
{
    u8 scan_code = in_byte(KB_DATA);
    if (kb_buffer.count < KB_IN_BYTES)
    {
        *(kb_buffer.p_head) = scan_code;
        kb_buffer.p_head++;
        if (kb_buffer.p_head == kb_buffer.buffer + KB_IN_BYTES)
        {
            kb_buffer.p_head = kb_buffer.buffer;
        }
        kb_buffer.count++;
    }
}