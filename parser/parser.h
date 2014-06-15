/* Locanimat
 * An animatronics project for Locafore 2013
 *
 * Command parser
 * parser.c
 *
 *  Created on: Aug 16, 2013
 *      Author: tuwuhs (Labtek Indie, Bandung, Indonesia)
 *      (c) 2013 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stdint.h>

#ifndef FALSE
#define FALSE (1 == 0)
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

#define PARSER_COMMAND_BUFFER_SIZE   (6)
#define PARSER_ARGUMENT_BUFFER_SIZE  (32)

#define PARSER_SENTINEL_OPEN   ('(')
#define PARSER_SENTINEL_CLOSE  (')')
#define PARSER_DELIMITER       (' ')

#define PARSER_RETCODE_SUCCESS   (0x00)
#define PARSER_RETCODE_COMMAND_OVERFLOW   (0x01)
#define PARSER_RETCODE_ARGUMENT_OVERFLOW  (0x02)

struct parser_item {
	char* command;
	void (*callback)(char* arg, uint8_t arglen);
};

enum e_parser_state {
	PARSER_S_INIT,
	PARSER_S_COMMAND,
	PARSER_S_ARGUMENT,
};

struct parser_ctx {
	char command_buffer[PARSER_COMMAND_BUFFER_SIZE];
	uint8_t command_count;
	char argument_buffer[PARSER_ARGUMENT_BUFFER_SIZE];
	uint8_t argument_count;

	enum e_parser_state state;
	struct parser_item* items;
	uint8_t items_size;
	uint8_t b_binary_mode;
};

void parser_init(struct parser_ctx* ctx);
uint8_t parser_register_commands(struct parser_ctx* ctx,
		struct parser_item* items, uint8_t items_size);
uint8_t parser_update_and_execute(struct parser_ctx* ctx, char data);

#endif /* PARSER_H_ */
