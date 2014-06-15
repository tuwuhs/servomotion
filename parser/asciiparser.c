/*
 * asciiparser.c
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "asciiparser.h"
#include "timer.h"

static void aparser_eval(struct aparser_ctx* ctx);

void aparser_init(struct aparser_ctx* ctx, int timeout_ms)
{
	ctx->state = APARSER_S_INIT;
	timer_set_period_ms(&ctx->timeout, timeout_ms);
}

uint8_t aparser_register_commands(struct aparser_ctx* ctx,
		struct aparser_item* items)
{
	// Note: the parser engine does not store/copy/allocate any memory
	// The caller has the responsibility to ensure that the context
	//   and parser items are valid

	struct aparser_item* item = items;
	uint8_t count = 0;
	while (item->command && item->callback) {
		item++;
		count++;
	}
	ctx->items = items;
	ctx->items_count = count;

	return APARSER_RETCODE_SUCCESS;
}

uint8_t aparser_update_and_execute(struct aparser_ctx* ctx, char data)
{
	uint8_t ret_code = APARSER_RETCODE_SUCCESS;

	if (timer_has_expired(&ctx->timeout)) {
		ctx->state = APARSER_S_INIT;
	}

	switch (ctx->state) {
	case APARSER_S_INIT:
		if (data == APARSER_SENTINEL_OPEN) {
			timer_restart(&ctx->timeout);
			ctx->state = APARSER_S_COMMAND;
			ctx->command_count = 0;
		}
		break;

	case APARSER_S_COMMAND:
		if (ctx->command_count >= APARSER_COMMAND_BUFFER_SIZE) {
			ctx->state = APARSER_S_INIT;
			ret_code = APARSER_RETCODE_COMMAND_OVERFLOW;
		} else if (data == APARSER_DELIMITER) {
			ctx->state = APARSER_S_ARGUMENT;
			ctx->command_buffer[ctx->command_count] = '\0';
			ctx->argument_count = 0;
		} else if (data == APARSER_SENTINEL_CLOSE) {
			ctx->state = APARSER_S_INIT;
			ctx->command_buffer[ctx->command_count] = '\0';
			ctx->argument_buffer[0] = '\0';
			aparser_eval(ctx);
		} else {
			timer_restart(&ctx->timeout);
			ctx->command_buffer[ctx->command_count] = data;
			ctx->command_count += 1;
		}
		break;

	case APARSER_S_ARGUMENT:
		if (ctx->argument_count >= APARSER_ARGUMENT_BUFFER_SIZE) {
			ctx->state = APARSER_S_INIT;
			ret_code = APARSER_RETCODE_ARGUMENT_OVERFLOW;
		} else if (data == APARSER_SENTINEL_CLOSE) {
			ctx->state = APARSER_S_INIT;
			ctx->argument_buffer[ctx->argument_count] = '\0';
			aparser_eval(ctx);
		} else {
			timer_restart(&ctx->timeout);
			ctx->argument_buffer[ctx->argument_count] = data;
			ctx->argument_count += 1;
		}
		break;

	default:
		ctx->state = APARSER_S_INIT;
	}

	return ret_code;
}

static void aparser_eval(struct aparser_ctx* ctx)
{
	uint8_t i;

	for (i = 0; i < ctx->items_count; i++) {
		if (strcmp(ctx->command_buffer, ctx->items[i].command) == 0) {
			(ctx->items[i].callback)(ctx->argument_buffer, ctx->argument_count);
			break;
		}
	}
}
