/* @file:   kcd_proc.h
 * @brief:  Keyboard Command Decoder Process Management
 */
 
#ifndef KCD_PROC_H_
#define KCD_PROC_H_

/* initialization table item */
typedef struct Command {
   int command_pid;
   char msgtext;
}Command;

void initializeKCDProcess(void);
void kcd_proc(void);
void addCommand(char mtext, int register_pid);
void checkMsgInfo(Letter* get_msginfo);
int searchCommand(char mtext);

#endif /* ! KCD_PROC_H_ */
