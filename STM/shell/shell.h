#include <stdint.h>

#define UART_DEVICE huart2

#define ARGC_MAX 8
#define BUFFER_SIZE 60
#define SHELL_FUNC_LIST_MAX_SIZE 64

struct h_shell_struct;

typedef int (*shell_func_pointer_t)(struct h_shell_struct * h_shell, int argc, char ** argv);
typedef uint8_t (* drv_shell_transmit_t)(const char * pData, uint16_t size);
typedef uint8_t (* drv_shell_receive_t)(char * pData, uint16_t size);

typedef uint8_t (* drv_shell_led)(uint8_t led_num, uint8_t gpio);
typedef void (* drv_shell_MCP23S17_write)( uint8_t reg, uint8_t value);
typedef void (* drv_shell_MCP23S17_init)(void);
typedef uint8_t (* drv_shell_led_reset)(uint8_t rst);
typedef uint8_t (* drv_shell_sgtl_id)(uint8_t rst);

typedef struct shell_func_struct
{
	char c;
	shell_func_pointer_t func;
	char * description;
} shell_func_t;

typedef struct drv_shell_struct
{
	drv_shell_transmit_t transmit;
	drv_shell_receive_t receive;
	drv_shell_MCP23S17_write write;
	drv_shell_MCP23S17_init init;
	drv_shell_led led;
	drv_shell_led_reset reset;
} drv_shell_t;


typedef struct h_shell_struct
{
	int func_list_size;
	shell_func_t func_list[SHELL_FUNC_LIST_MAX_SIZE];
	char print_buffer[BUFFER_SIZE];
	char cmd_buffer[BUFFER_SIZE];
	uint8_t led_num;
	uint8_t gpio;
	uint8_t rst;
	drv_shell_t drv;
} h_shell_t;

void shell_init(h_shell_t * h_shell);
int shell_add(h_shell_t * h_shell, char c, shell_func_pointer_t pfunc, char * description);
int shell_run(h_shell_t * h_shell);
// commentaire documentation!
void shell_uart_receive_irq_cb(void);
