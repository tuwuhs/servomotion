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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"

static void parser_execute(struct parser_ctx* ctx);

void parser_init(struct parser_ctx* ctx)
{
	ctx->b_binary_mode = false;
	ctx->state = PARSER_S_INIT;
}

uint8_t parser_register_commands(struct parser_ctx* ctx,
		struct parser_item* items)
{
	// Note: the parser engine does not store/copy/allocate any memory
	// The caller has the responsibility to ensure that the context
	//   and parser items are valid

	struct parser_item* item = items;
	uint8_t count = 0;
	while (item->command && item->callback) {
		item++;
		count++;
	}
	ctx->items = items;
	ctx->items_count = count;

	return PARSER_RETCODE_SUCCESS;
}

uint8_t parser_update_and_execute(struct parser_ctx* ctx, char data)
{
	uint8_t ret_code = PARSER_RETCODE_SUCCESS;

	switch (ctx->state) {
	case PARSER_S_INIT:
		if (data == PARSER_SENTINEL_OPEN) {
			ctx->state = PARSER_S_COMMAND;
			ctx->command_count = 0;
		}
		break;

	case PARSER_S_COMMAND:
		if (ctx->command_count >= PARSER_COMMAND_BUFFER_SIZE) {
			ctx->state = PARSER_S_INIT;
			ret_code = PARSER_RETCODE_COMMAND_OVERFLOW;
		} else if (data == PARSER_DELIMITER) {
			ctx->state = PARSER_S_ARGUMENT;
			ctx->command_buffer[ctx->command_count] = '\0';
			ctx->argument_count = 0;
		} else if (data == PARSER_SENTINEL_CLOSE) {
			ctx->state = PARSER_S_INIT;
			ctx->command_buffer[ctx->command_count] = '\0';
			ctx->argument_buffer[0] = '\0';
			parser_execute(ctx);
		} else {
			if (ctx->command_count == 0 && data == 'B') {
				// All command starting with B has 10-byte binary data
				ctx->b_binary_mode = true;
			}
			ctx->command_buffer[ctx->command_count] = data;
			ctx->command_count += 1;
		}
		break;

	case PARSER_S_ARGUMENT:
		if (ctx->argument_count >= PARSER_ARGUMENT_BUFFER_SIZE) {
			ctx->state = PARSER_S_INIT;
			ret_code = PARSER_RETCODE_ARGUMENT_OVERFLOW;
		} if (ctx->b_binary_mode && (ctx->argument_count < 10)) {
			ctx->argument_buffer[ctx->argument_count] = data;
			ctx->argument_count += 1;
		} else if (data == PARSER_SENTINEL_CLOSE) {
			ctx->state = PARSER_S_INIT;
			ctx->b_binary_mode = false;
			ctx->argument_buffer[ctx->argument_count] = '\0';
			parser_execute(ctx);
		} else {
			ctx->argument_buffer[ctx->argument_count] = data;
			ctx->argument_count += 1;
		}
		break;

	default:
		ctx->state = PARSER_S_INIT;
	}

	return ret_code;
}

static void parser_execute(struct parser_ctx* ctx)
{
	uint8_t i;

	for (i = 0; i < ctx->items_count; i++) {
		if (strcmp(ctx->command_buffer, ctx->items[i].command) == 0) {
			(ctx->items[i].callback)(ctx->argument_buffer, ctx->argument_count);
			break;
		}
	}
}
