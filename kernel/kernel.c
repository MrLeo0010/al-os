/* kernel.c - AL-OS shell (freestanding) */

#include <stddef.h>
#include "vga.h"
#include "keyboard.h"
#include "fs.h"

#define MAX_CMD_LEN 128

/* --- Globals --- */
char user[32] = "root";
char current_path[128] = "/";

/* --- Small helpers --- */
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strchr(const char* s, int c) {
    while (*s) { if (*s == (char)c) return (char*)s; s++; }
    return NULL;
}

void itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    if (base < 2 || base > 36) { *str = '\0'; return; }
    if (value == 0) { *ptr++ = '0'; *ptr = '\0'; return; }
    int sign = 0;
    if (value < 0 && base == 10) { sign = 1; value = -value; }
    while (value != 0) {
        int digit = value % base;
        *ptr++ = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        value /= base;
    }
    if (sign) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) { char t = *ptr; *ptr-- = *ptr1; *ptr1++ = t; }
}

/* outb/inb */
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned char inb(unsigned short port) {
    unsigned char r;
    __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}
static inline void cpu_halt(void) { __asm__ volatile ("hlt"); }

/* --- Simple expression parser --- */
int parse_expr(const char* s, int* a, char* op, int* b) {
    int i=0;
    while(s[i]==' ') i++;
    int sign=1; if(s[i]=='-'){sign=-1;i++;}
    if(!(s[i]>='0' && s[i]<='9')) return 0;
    int x=0;
    while(s[i]>='0' && s[i]<='9'){x=x*10 + (s[i]-'0'); i++;}
    *a = x*sign;
    while(s[i]==' ') i++;
    if(!(s[i]=='+'||s[i]=='-'||s[i]=='*'||s[i]=='/')) return 0;
    *op = s[i++];
    while(s[i]==' ') i++;
    sign=1; if(s[i]=='-'){sign=-1;i++;}
    if(!(s[i]>='0' && s[i]<='9')) return 0;
    x=0;
    while(s[i]>='0' && s[i]<='9'){x=x*10 + (s[i]-'0'); i++;}
    *b = x*sign;
    return 1;
}

/* --- Beep via PIT + speaker --- */
static void beep_pit(unsigned int frequency, unsigned int duration_ms) {
    if (frequency == 0) return;
    unsigned int div = 1193180 / frequency;
    outb(0x43, 0xB6);
    outb(0x42, (unsigned char)(div & 0xFF));
    outb(0x42, (unsigned char)((div >> 8) & 0xFF));
    unsigned char tmp = inb(0x61);
    if ((tmp & 3) != 3) outb(0x61, tmp | 3);
    for (volatile unsigned long i = 0; i < duration_ms * 15000UL; i++);
    tmp = inb(0x61);
    outb(0x61, tmp & 0xFC);
}

/* --- Slow print per-line --- */
static void slowprint_line(const char* s, unsigned char color, unsigned int delay_loops) {
    vga_print_color((char*)s,color);
    vga_putc('\n');
    for(volatile unsigned int i=0;i<delay_loops;i++);
}

/* --- Update current_path --- */
/*
static void update_current_path(const char* path) {
    if (!path || !path[0]) return;
    if (path[0]=='/'){
        int i=0;
        while(path[i] && i<127){current_path[i]=path[i]; i++;}
        current_path[i]=0;
        return;
    }
    // relative path — упрощённая логика (в оригинале была обрезана)
    // оставим как было, без сложного парсинга
}
*/

/* --- Prompt --- */
static void show_prompt(void) {
    vga_print_color(user, 0x0A);
    vga_print_color("@AL-OS:", 0x09);
    vga_print_color(current_path, 0x0F);
    vga_print_color("> ", 0x07);
}

/* --- Commands --- */
static void cmd_help(void) {
    vga_print_color("=== Command list ===\n", 0x0A);
    vga_print_color("help, about, echo, clear, ls, mkdir, cd, pwd, rm\n", 0x0A);
    vga_print_color("touch, write, cat, calc, chusr, beep, sysinfo, slowfetch\n", 0x0A);
    vga_print_color("reboot, shutdown\n", 0x0A);
}

static void cmd_about(void) {
    vga_print_color("AL-OS Shell (freestanding) - minimal demo\n", 0x0A);
}

static void cmd_calc(const char* expr) {
    if (!expr || !expr[0]) { vga_print_color("Usage: calc <a><op><b>\n",0x0C); return; }
    int a,b; char op;
    if (!parse_expr(expr,&a,&op,&b)) { vga_print_color("Invalid expression\n",0x0C); return; }
    int res=0;
    switch(op){
        case'+':res=a+b;break;
        case'-':res=a-b;break;
        case'*':res=a*b;break;
        case'/':if(b==0){vga_print_color("Division by zero\n",0x0C);return;}res=a/b;break;
        default:vga_print_color("Unknown operator\n",0x0C);return;
    }
    char buf[32]; itoa(res,buf,10);
    vga_print_color(buf,0x0A); vga_putc('\n');
}

static void cmd_chusr(const char* name) {
    if(!name||!name[0]){vga_print_color("Usage: chusr <name>\n",0x0C);return;}
    int i=0; while(name[i]&&i<31){user[i]=name[i];i++;} user[i]=0;
    vga_print_color("User changed.\n",0x0A);
}

static void cmd_sysinfo(void) {
    vga_print_color("=== System Info ===\n",0x0D);
    vga_print_color("CPU: i686-compatible\n",0x0A);
    vga_print_color("Memory: BIOS/e820 later\n",0x0A);
    vga_print_color("Display: VGA text mode\n",0x0A);
}

static void cmd_slowfetch(void) {
    const char* art[]={" /\\ "," / \\ "," / /\\ \\ ","/ ____ \\","/_/ \\_\\",NULL};
    for(int i=0;art[i];i++) slowprint_line(art[i],0x0B,100000u);
    slowprint_line("",0x07,20000u);
    slowprint_line("=== System Info ===",0x05,20000u);
    cmd_sysinfo();
}

/* --- Kernel main --- */
void kernel_main(void){
    vga_clear();
    vga_print_color("Kernel initialized.\n",0x0A);
    for(volatile int i=0;i<20000000;i++);

    fs_init();
    fs_cd("/home");
    vga_print_color("AL-OS Shell\nType 'help' for commands.\n",0x0A);

    char cmd[MAX_CMD_LEN];
    while(1){
        show_prompt();
        keyboard_read_line(cmd,MAX_CMD_LEN);
        if(cmd[0]==3){ vga_putc('\n'); continue; } // ctrl-c

        char* space = strchr(cmd,' ');
        char* args = "";
        if(space){ *space=0; args=space+1; }

        if(strcmp(cmd,"help")==0) cmd_help();
        else if(strcmp(cmd,"about")==0) cmd_about();
        else if(strcmp(cmd,"echo")==0){ vga_print_color(args,0x09); vga_putc('\n'); }
        else if(strcmp(cmd,"clear")==0) vga_clear();
        else if(strcmp(cmd,"ls")==0){ if(args[0]) fs_list(args); else fs_list(NULL); }
        else if(strcmp(cmd,"mkdir")==0){ if(!args[0]) vga_print_color("Usage: mkdir <name>\n",0x0C); else fs_mkdir(args); }
        else if(strcmp(cmd,"cd")==0){ if(!args[0]) vga_print_color("Usage: cd <dir>\n",0x0C); else { fs_cd(args); } }
        else if(strcmp(cmd,"pwd")==0){ vga_print_color(current_path,0x0F); vga_putc('\n'); }
        else if(strcmp(cmd,"rm")==0){ if(!args[0]) vga_print_color("Usage: rm <name>\n",0x0C); else fs_rm(args); }
        else if(strcmp(cmd,"touch")==0){ if(!args[0]) vga_print_color("Usage: touch <name>\n",0x0C); else fs_touch(args); }
        else if(strcmp(cmd,"write")==0){ char* s2=strchr(args,' '); if(!s2) vga_print_color("Usage: write <name> <text>\n",0x0C); else { *s2=0; fs_write(args,s2+1); } }
        else if(strcmp(cmd,"cat")==0){ if(!args[0]) vga_print_color("Usage: cat <name>\n",0x0C); else fs_cat(args); }
        else if(strcmp(cmd,"calc")==0) cmd_calc(args);
        else if(strcmp(cmd,"chusr")==0) cmd_chusr(args);
        else if(strcmp(cmd,"beep")==0){
            if(args && args[0]){
                int f=1000,ms=300,val=0,i=0;
                while(args[i]>='0'&&args[i]<='9'){val=val*10+(args[i++]-'0');} f=val;
                while (args[i] == ' ') i++;
                val = 0;
                while(args[i]>='0'&&args[i]<='9'){val=val*10+(args[i++]-'0');} ms=val;
                beep_pit(f,ms);
            } else beep_pit(1000,300);
        }
        else if(strcmp(cmd,"sysinfo")==0) cmd_sysinfo();
        else if(strcmp(cmd,"slowfetch")==0) cmd_slowfetch();
        else if(strcmp(cmd,"reboot")==0){ 
            vga_print_color("Rebooting...\n",0x0C); 
            __asm__ volatile("cli"); 
            // Triple fault → настоящий reset (перезагрузка)
            __asm__ volatile(".byte 0x0F, 0x0B" ::: "memory"); // UD2 — гарантированный #UD
            __asm__ volatile("lidt %0" :: "m"(*(short[3]){0,0,0})); // нулевой IDT → triple fault
            for(;;) __asm__("hlt");
        }
        else if(strcmp(cmd,"shutdown")==0 || strcmp(cmd,"poweroff")==0){ 
            vga_print_color("Shutting down...\n",0x0C); 
            __asm__ volatile("cli"); 
            outb(0x64, 0xFE);           // Это теперь настоящий poweroff в QEMU с -no-reboot
            for(;;) __asm__("hlt");
        }
        else if(cmd[0]==0) {}
        else vga_print_color("Error: command not found\n",0x0C);
    }
}