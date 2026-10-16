// modelgen.h -- header file for model generation program

#ifndef MODELGEN_H
#define MODELGEN_H

typedef enum { ST_SYNC=0, ST_RAND } synctype_t;

typedef struct {
	int		onseam;
	int		s;
	int		t;
} stvert_t;

// This mirrors trivert_t in trilib.h, is present so Quake knows how to
// load this data

typedef struct {
	byte	v[3];
	byte	lightnormalindex;
} trivertx_t;

#endif // MODELGEN_H
