#pragma once

typedef struct
{
	int key;
	int wait;
	bool play;
	bool should_exit;
} userinputs_t;


void userinputs_init(userinputs_t &state);
void userinputs_progress(userinputs_t &state);