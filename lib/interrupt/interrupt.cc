#include "interrupt.h"

#include "harddrive.h"
#include "process.h"
// 运行在特权级ring0
//初始化中断相关==============================================================================
//初始化8259A,操作顺序不可逆转
//依次向主片和从片中写入ICW1,ICW2,ICW3,ICW4
void init_8259A() {
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
void init_keyboard() {
    kb_buffer.count = 0;
    kb_buffer.p_head = kb_buffer.p_tail = kb_buffer.buffer;
    enable_irq(KEYBOARD_IRQ);
}
void init_clock() {
    //初始化8253PIT
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
    out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
    enable_irq(CLOCK_IRQ);
}
//====================================================================================================
// 进程调度---------------------------------------------------------------------
//在进程队列中寻找没有被阻塞的进程
PROCESS *find_first(PROCESS *head, PROCESS *tail) {
    PROCESS *p = head;
    if (tail == process_tail) return nullptr;

    while (p != tail) {
        if (p->flags == RUNNING) return p;
        p = p->next_pcb;
    }
    if (tail->flags == RUNNING) return p;
    return nullptr;
}

void next_quene(PROCESS *&curr_head, PROCESS *&curr_tail, PROCESS *&next_head,
                PROCESS *&next_tail, PROCESS *&curr, PROCESS *&tail,
                int next_ticks) {
    curr->ticks--;
    if (curr->ticks == 0) {
        curr->queen_number++;
        curr->ticks = next_ticks;
        if (curr_head == curr)
            curr_head = curr->next_pcb;
        else {
            auto p = curr_head;
            while (p->next_pcb != curr) {
                p = p->next_pcb;
            }
            p->next_pcb = curr->next_pcb;
        }
        if (next_tail != tail) {
            next_tail->next_pcb = curr;
            next_tail = curr;
            next_tail->next_pcb = tail;
        } else {
            next_head = curr;
            next_tail = curr;
            next_tail->next_pcb = tail;
        }
        if (curr_tail == curr) {
            // curr = next_head;
            // curr_tail = tail;
            auto p = curr_head;
            while (p->next_pcb != tail) {
                p = p->next_pcb;
            }
            curr_tail = p;
        } else {
            // curr = curr_head;
        }
    }
}
// 在时钟中断以及阻塞某个进程后会调用的进程调度函数
void schedule() {
    if (PROCESS *p_first = find_first(process_queen1_head, process_queen1_tail);
        !p_first) {
        if (PROCESS *p_second =
                find_first(process_queen2_head, process_queen2_tail);
            !p_second) {
            p_proc_ready->ticks--;
            if (p_proc_ready->ticks == 0 or
                p_proc_ready->flags != RUNNING and
                    p_proc_ready->queen_number == 3) {
                p_proc_ready->ticks = LAST_QUENE_SLICE;
                // TODO:这里有问题
                if (p_proc_ready == process_tail) {
                    PROCESS *p = process_queen3_head;

                    while (p->flags != RUNNING) {
                        p = p->next_pcb;
                    }
                    p_proc_ready = p;
                } else {
                    p_proc_ready = p_proc_ready->next_pcb;
                }
            } else if (p_proc_ready->queen_number < 3) {
                p_proc_ready = find_first(process_queen3_head, process_tail);
            }
        } else {
            p_proc_ready = p_second;
            next_quene(process_queen2_head, process_queen2_tail,
                       process_queen3_head, process_queen3_tail, p_proc_ready,
                       process_tail, LAST_QUENE_SLICE);
        }
    } else {
        p_proc_ready = p_first;
        next_quene(process_queen1_head, process_queen1_tail,
                   process_queen2_head, process_queen2_tail, p_proc_ready,
                   process_tail, SECOND_QUENE_SLICE);
    }
}
// ------------------------------------------------------------------

// 中断处理部分=============================================================================
// 入口函数
void interrupt_request(int irq) {
    if (irq == CLOCK_IRQ) {
        clock_handler();
    } else if (irq == KEYBOARD_IRQ) {
        keyboard_handler();
    } else if (irq == AT_WINI_IRQ) {
        hd_handler();
    }
}
//时钟中断处理函数
void clock_handler() {
    schedule();
    ticks++;
}

void keyboard_handler() {
    u8 scan_code = in_byte(KB_DATA);
    if (kb_buffer.count < KB_IN_BYTES) {
        *(kb_buffer.p_head) = scan_code;
        kb_buffer.p_head++;
        if (kb_buffer.p_head == kb_buffer.buffer + KB_IN_BYTES) {
            kb_buffer.p_head = kb_buffer.buffer;
        }
        kb_buffer.count++;
    }
}

void hd_handler() {
    PROCESS *p = proc_table + PID_HD;

    // =======================================================================
    // 一定要加这段代码读取状态寄存器，但我也不知道为什么要加反正加就对了
    u8 disk_status = in_byte(REG_STATUS);
    // =======================================================================

    if ((p->flags & RECEIVING) && /* dest is waiting for the msg */
        ((p->recv_from_record == INTERRUPT) || (p->recv_from_record == ANY))) {
        p->message->source = INTERRUPT;
        p->message->type = INTERRUPT;
        p->message = 0;
        p->has_int_msg = 0;
        p->flags &= ~RECEIVING; /* dest has received the msg */
        p->recv_from_record = NO_TASK;
        unblock(p);
    } else {
        p->has_int_msg = 1;
    }
}
// ===================================================================================