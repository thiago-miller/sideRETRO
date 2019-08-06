#ifndef ABNORMAL_H
#define ABNORMAL_H

#include "db.h"
#include "chr.h"
#include "exon.h"

#define ABNORMAL_DISTANCE_CUTOFF 10000

enum _AbnormalType
{
	ABNORMAL_NONE          = 0,
	ABNORMAL_DISTANCE      = 1,
	ABNORMAL_CHROMOSOME    = 2,
	ABNORMAL_SUPPLEMENTARY = 4,
	ABNORMAL_EXONIC        = 8
};

typedef enum _AbnormalType AbnormalType;

struct _AbnormalArg
{
	int            tid;
	int            num_threads;
	const char    *sam_file;
	ExonTree      *exon_tree;
	ChrStd        *cs;
	sqlite3_stmt  *alignment_stmt;
	int            queryname_sorted;
	int            either;
	float          exon_frac;
	float          alignment_frac;
};

typedef struct _AbnormalArg AbnormalArg;

void abnormal_filter (AbnormalArg *arg);

#endif /* abnormal.h */
