/* @file: test_list.c
 * #desc:
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/ds/list.h>


struct T {
	struct list_node node;
	int32_t key;
};

#define GET_KEY(x) ((x) ? (container_of(x, struct T, node)->key) : -1)

void test_list(void)
{
	LIST_NEW(head);
	struct T node[8];

	for (int32_t i = 0; i < 8; i++)
		node[i].key = i;

	/* add */
	printf("-- add %d\n", GET_KEY(&node[0].node));
	F_SYMBOL(list_add)(&head, &node[0].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- add %d\n", GET_KEY(&node[1].node));
	F_SYMBOL(list_add)(&head, &node[1].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- add-tail %d\n", GET_KEY(&node[2].node));
	F_SYMBOL(list_add_tail)(&head, &node[2].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	/* del */
	printf("-- del %d\n", GET_KEY(&node[0].node));
	F_SYMBOL(list_del)(&head, &node[0].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- del %d\n", GET_KEY(&node[1].node));
	F_SYMBOL(list_del)(&head, &node[1].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- del %d\n", GET_KEY(&node[2].node));
	F_SYMBOL(list_del)(&head, &node[2].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	/* insert */
	printf("-- add %d\n", GET_KEY(&node[0].node));
	F_SYMBOL(list_add)(&head, &node[0].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- ins %d %d\n",
		GET_KEY(head.node),
		GET_KEY(&node[1].node));
	F_SYMBOL(list_insert)(&head, head.node, &node[1].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- ins %d %d\n",
		GET_KEY(&node[0].node),
		GET_KEY(&node[2].node));
	F_SYMBOL(list_insert)(&head, &node[0].node, &node[2].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- ins-tail %d %d\n",
		GET_KEY(&node[2].node),
		GET_KEY(&node[3].node));
	F_SYMBOL(list_insert_tail)(&head, &node[2].node, &node[3].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	LIST_FOR_EACH(head.node, pos) {
		F_SYMBOL(list_del)(&head, pos);
	}

	/* swap */
	printf("-- add %d\n", GET_KEY(&node[0].node));
	F_SYMBOL(list_add)(&head, &node[0].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- add %d\n", GET_KEY(&node[1].node));
	F_SYMBOL(list_add)(&head, &node[1].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- swp %d %d\n",
		GET_KEY(&node[1].node),
		GET_KEY(&node[0].node));
	F_SYMBOL(list_swap)(&head, &node[1].node, &node[0].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- swp %d %d\n",
		GET_KEY(&node[1].node),
		GET_KEY(&node[0].node));
	F_SYMBOL(list_swap)(&head, &node[1].node, &node[0].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- add %d\n", GET_KEY(&node[2].node));
	F_SYMBOL(list_add)(&head, &node[2].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- swp %d %d\n",
		GET_KEY(&node[0].node),
		GET_KEY(&node[2].node));
	F_SYMBOL(list_swap)(&head, &node[0].node, &node[2].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- add %d\n", GET_KEY(&node[3].node));
	F_SYMBOL(list_add)(&head, &node[3].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}

	printf("-- swp %d %d\n",
		GET_KEY(&node[0].node),
		GET_KEY(&node[2].node));
	F_SYMBOL(list_swap)(&head, &node[0].node, &node[2].node);
	LIST_FOR_EACH(head.node, pos) {
		printf("n:%d prev:%d next:%d\n",
			GET_KEY(pos),
			GET_KEY(pos->prev),
			GET_KEY(pos->next));
	}
}

int main(void)
{
	test_list();

	return 0;
}
