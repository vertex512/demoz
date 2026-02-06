/* @file: umalloc.c
 * #desc:
 *    The implementations of a simple memory allocator.
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/ds/list.h>
#include <demoz/c/sys/umalloc.h>


/* @def: _
 * head | node <--> node <--> node --> end
 *        node  | { list, size, chunk }
 *        chunk | chunk <--> chunk --> end
 *                { prev_size, size|flags }<alloc>
 * aligned: 16
 *  32: ((8+4+12) + (8^+8)+(8^+8))
 *  64: ((16+8)   + (8^+8)+(8^+8))
 *                    2      3
 */
#define CHUNK_NODE_SIZE sizeof(struct umalloc_chunk_node)
#define CHUNK_SIZE sizeof(struct umalloc_chunk)

/* stack 16-byte aligend */
#define CHUNK_ALIGNED_SIZE(x) (((x) + CHUNK_SIZE + 15) & ~15U)
#define CHUNK_NODE_ALIGNED_SIZE(x) (((x) + 15) & ~15UL)

#define NEXT_CHUNK(x) \
	((struct umalloc_chunk *)((char *)(x) + GET_SIZE(x)))
#define PREV_CHUNK(x) \
	((struct umalloc_chunk *)((char *)(x) - (x)->prev_size))

#define GET_INUSE(x) ((x)->size_flags & 0x01)
#define GET_END(x) ((x)->size_flags & 0x02)
#define GET_BIG(x) ((x)->size_flags & 0x04)
#define GET_SIZE(x) ((x)->size_flags & ~0x07U)
#define SET_SIZE(x) ((x) & ~0x07U)
#define FG_INUSE 0x01
#define FG_END 0x02
#define FG_BIG 0x04

#define ALLOC_CHUNK_SIZE (2 << 17)
#define ALLOC_SIZE_MAX (1 << 17)
/* end */

/* @func: _umalloc_new_node (static)
 * #desc:
 *    allocate a memory chunk.
 *
 * #1: umalloc context struct
 * #2: chunk size
 * #r: chunk pointer
 */
static void *_umalloc_new_node(struct umalloc_ctx *ctx, uint32_t size)
{
	size += CHUNK_NODE_SIZE;
	struct umalloc_chunk_node *node = ctx->call_alloc(size, ctx->arg);
	if (!node)
		return NULL;

	node->size = size;
	node->chunk.prev_size = 0;
	node->chunk.size_flags = SET_SIZE(size - CHUNK_NODE_SIZE) | FG_END;

	F_SYMBOL(list_add)(&ctx->chunk, &node->list);
	ctx->idle = &node->chunk;

	return node;
}

/* @func: _umalloc_new_big_node (static)
 * #desc:
 *    allocate large chunk of memory.
 *
 * #1: umalloc context struct
 * #2: chunk size
 * #r: chunk pointer
 */
static void *_umalloc_new_big_node(struct umalloc_ctx *ctx, size_t size)
{
	size += CHUNK_NODE_SIZE;
	struct umalloc_chunk_node *node = ctx->call_alloc(size, ctx->arg);
	if (!node)
		return NULL;

	node->size = size;
	node->chunk.prev_size = 0;
	node->chunk.size_flags = SET_SIZE(size - CHUNK_NODE_SIZE) | FG_INUSE
		| FG_END | FG_BIG;

	F_SYMBOL(list_add)(&ctx->chunk, &node->list);

	return node;
}

/* @func: _umalloc_chunk_merge (static)
 * #desc:
 *    merge unused chunk backward.
 *
 * #1: chunk pointer
 */
static void _umalloc_chunk_merge(struct umalloc_chunk *chunk)
{
	struct umalloc_chunk *next = NULL;
	uint32_t size = 0;

	for (struct umalloc_chunk *pos = chunk;
			!GET_INUSE(pos);
			pos = NEXT_CHUNK(pos)) {
		size += GET_SIZE(pos);
		if (GET_END(pos)) {
			next = NULL;
			break;
		}
		next = NEXT_CHUNK(pos);
	}
	if (!size)
		return;

	chunk->size_flags = SET_SIZE(size) | (next ? 0 : FG_END);
	if (next)
		next->prev_size = size;
}

/* @func: _umalloc_prev (static)
 * #desc:
 *    forward offset unused chunk.
 *
 * #1: chunk pointer
 * #r: chunk pointer
 */
static void *_umalloc_prev(struct umalloc_chunk *chunk)
{
	if (!chunk->prev_size)
		return chunk;

	for (struct umalloc_chunk *pos = PREV_CHUNK(chunk);
			!GET_INUSE(pos);
			pos = PREV_CHUNK(pos)) {
		chunk = pos;
		if (!chunk->prev_size)
			break;
	}

	return chunk;
}

/* @func: _umalloc_chunk (static)
 * #desc:
 *    allocate memory chunk.
 *
 * #1: umalloc context struct
 * #2: chunk pointer
 * #3: allocate size
 * #r: allocated chunk pointer
 */
static void *_umalloc_chunk(struct umalloc_ctx *ctx,
		struct umalloc_chunk *chunk, uint32_t size)
{
	size = CHUNK_ALIGNED_SIZE(size);
	struct umalloc_chunk *next = NULL;

	for (struct umalloc_chunk *pos = chunk;
			pos;
			pos = NEXT_CHUNK(pos)) {
		if (GET_INUSE(pos)) {
			if (GET_END(pos))
				return NULL;
			continue;
		}

		_umalloc_chunk_merge(pos);
		uint32_t psize = GET_SIZE(pos), pend = GET_END(pos);
		if (psize < size) {
			if (pend)
				return NULL;
			continue;
		}

		ctx->idle = NULL;
		if (psize == size) {
			pos->size_flags |= FG_INUSE;
			return pos;
		}
		if (psize >= (size + CHUNK_ALIGNED_SIZE(8))) {
			pos->size_flags = SET_SIZE(size) | FG_INUSE;

			next = NEXT_CHUNK(pos);
			next->prev_size = size;
			next->size_flags = SET_SIZE(psize - size);
			ctx->idle = next;

			if (pend) { /* pos | cur -> next */
				next->size_flags |= FG_END;
			} else { /* pos | cur -> next -> next */
				next = NEXT_CHUNK(next);
				next->prev_size = SET_SIZE(psize - size);
			}

			return pos;
		}

		pos->size_flags |= FG_INUSE;
		return pos;
	}

	return NULL;
}

/* @func: ___umalloc
 * #desc:
 *    memory allocator.
 *
 * #1: umalloc context struct
 * #2: allocate size
 * #r: allocated pointer
 */
void *C_SYMBOL(___umalloc)(struct umalloc_ctx *ctx, size_t size)
{
	struct umalloc_chunk_node *node;
	struct umalloc_chunk *chunk;

	if (size >= ALLOC_SIZE_MAX) { /* big */
		node = _umalloc_new_big_node(ctx, size);
		if (!node)
			return NULL;
		chunk = &node->chunk;
		return ++chunk;
	} else if (!size) {
		size = 1;
	}

	LIST_FOR_EACH(ctx->chunk.node, pos) {
		node = container_of(pos, struct umalloc_chunk_node, list);
		chunk = &node->chunk;
		if (GET_BIG(chunk))
			continue;
		if (ctx->idle)
			chunk = ctx->idle;

		chunk = _umalloc_chunk(ctx, chunk, size);
		if (chunk)
			return ++chunk;
		if (ctx->idle)
			break;
	}

	node = _umalloc_new_node(ctx, ALLOC_CHUNK_SIZE);
	if (!node)
		return NULL;
	chunk = _umalloc_chunk(ctx, &node->chunk, size);

	return chunk ? ++chunk : NULL;
}

/* @func: ___ufree
 * #desc:
 *    free allocated memory.
 *
 * #1: umalloc context struct
 * #2: allocated pointer
 * #r: 0: no error, -1: double free
 */
int32_t C_SYMBOL(___ufree)(struct umalloc_ctx *ctx, void *p)
{
	struct umalloc_chunk_node *node;
	struct umalloc_chunk *chunk = p;

	chunk--;
	if (!GET_INUSE(chunk))
		return -1;

	if (GET_BIG(chunk)) {
		node = container_of(chunk, struct umalloc_chunk_node, chunk);
		F_SYMBOL(list_del)(&ctx->chunk, &node->list);
		ctx->call_free(node, node->size, ctx->arg);
		return 0;
	}

	chunk->size_flags &= ~FG_INUSE;
	chunk = _umalloc_prev(chunk);
	_umalloc_chunk_merge(chunk);

	ctx->idle = NULL;
	if (!chunk->prev_size && GET_END(chunk)) {
		node = container_of(chunk, struct umalloc_chunk_node, chunk);
		F_SYMBOL(list_del)(&ctx->chunk, &node->list);
		ctx->call_free(node, node->size, ctx->arg);
	}

	return 0;
}

/* @func: ___ufree_all
 * #desc:
 *    free all allocated memory.
 *
 * #1: umalloc context struct
 */
void C_SYMBOL(___ufree_all)(struct umalloc_ctx *ctx)
{
	struct umalloc_chunk_node *node;
	while (ctx->chunk.node) {
		LIST_FOR_EACH(ctx->chunk.node, pos) {
			node = container_of(pos,
				struct umalloc_chunk_node, list);
			F_SYMBOL(list_del)(&ctx->chunk, &node->list);
			ctx->call_free(node, node->size, ctx->arg);
			break;
		}
	}
}
