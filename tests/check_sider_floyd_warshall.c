/*
 * sideRETRO - A pipeline for detecting Somatic Insertion of DE novo RETROcopies
 * Copyright (C) 2019-2020 Thiago L. A. Miller <tmiller@mochsl.org.br
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <check.h>
#include <math.h>
#include "check_sider.h"
#include "../src/wrapper.h"
#include "../src/utils.h"
#include "../src/floyd_warshall.c"

static double weights[4][4] = {
	{NAN, NAN, -2,   NAN},
	{4,   NAN,  3,   NAN},
	{NAN, NAN,  NAN, 2  },
	{NAN,  -1,  NAN, NAN}
};

static double weights_res[4][4] = {
	{0, -1, -2, 0},
	{4,  0,  2, 4},
	{5,  1,  0, 2},
	{3, -1,  1, 0}
};

static int nodes[4] = {0, 1, 2, 3};

static int path2to0[] = {2, 3, 1, 0};

static double
weight_fun (void *vertex1, void *vertex2)
{
	int i = * (int *) vertex1;
	int j = * (int *) vertex2;
	return weights[i][j];
}

START_TEST (test_floyd_warshall_run)
{
	Graph *graph = NULL;
	FloydWarshal *fw = NULL;
	List *path = NULL;
	ListElmt *cur = NULL;
	int i, j;

	graph = graph_new_full (int_hash, int_equal, NULL);

	for (i = 0; i < 4; i++)
		graph_ins_vertex (graph, &nodes[i]);

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (!isnan (weights[i][j]))
				graph_ins_edge (graph, &nodes[i], &nodes[j]);

	ck_assert_int_eq (graph_vcount (graph), 4);
	ck_assert_int_eq (graph_ecount (graph), 5);

	fw = floyd_warshall_new (graph, int_hash, int_equal, weight_fun);
	floyd_warshall_run (fw);

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			ck_assert (fequal (weights_res[i][j],
						floyd_warshall_dist (fw, &i, &j)));

	i = j = 100;
	ck_assert (isnan (floyd_warshall_dist (fw, &i, &j)));

	// See path
	i = 2; j = 0;
	path = floyd_warshall_path (fw, &i, &j);
	ck_assert (path != NULL);

	cur = list_head (path);
	j = 0;

	for (; cur != NULL; cur = list_next (cur))
		{
			i = * (int *) list_data (cur);
			ck_assert_int_eq (path2to0[j++], i);
		}

	list_free (path);
	graph_free (graph);
	floyd_warshall_free (fw);
	floyd_warshall_free (NULL);
}
END_TEST

Suite *
make_floyd_warshall_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("FloydWarshall");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_floyd_warshall_run);
	suite_add_tcase (s, tc_core);

	return s;
}
