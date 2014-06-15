/*
 * asciiparser.h
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#ifndef ASCII_PARSER_H_
#define ASCII_PARSER_H_

#include <stdbool.h>
#include <stdint.h>

#define APARSER_COMMAND_BUFFER_SIZE   (6)
#define APARSER_ARGUMENT_BUFFER_SIZE  (32)

#define APARSER_SENTINEL_OPEN   ('(')
#define APARSER_SENTINEL_CLOSE  (')')
#define APARSER_DELIMITER       (' ')

#define APARSER_RETCODE_SUCCESS   (0x00)
#define APARSER_RETCODE_COMMAND_OVERFLOW   (0x01)
#define APARSER_RETCODE_ARGUMENT_OVERFLOW  (0x02)

enum e_aparser_state {
	APARSER_S_INIT,
	APARSER_S_COMMAND,
	APARSER_S_ARGUMENT,
};

struct aparser_item {
	char* command;
	void (*callback)(char* arg, uint8_t arglen);
};

struct aparser_ctx {
	char command_buffer[APARSER_COMMAND_BUFFER_SIZE];
	uint8_t command_count;
	char argument_buffer[APARSER_ARGUMENT_BUFFER_SIZE];
	uint8_t argument_count;

	enum e_aparser_state state;
	struct aparser_item* items;
	uint8_t items_count;
};

void aparser_init(struct aparser_ctx* ctx);
uint8_t aparser_register_commands(struct aparser_ctx* ctx, struct aparser_item* items);
uint8_t aparser_update_and_execute(struct aparser_ctx* ctx, char data);

#endif /* PARSER_H_ */
