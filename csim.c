//Tran Van Duy Tue - 1551044
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <math.h>
#include "cachelab.h"

typedef struct {
	int s,b,E,S,B;

	int hits, misses, evicts;
} cacheProperties;

typedef struct {
	 cache_set *sets;
} cache;

typedef struct {
	set_line *lines;
} cache_set;

typedef struct {
	int validBit, lastUsed;
	unsigned long long tag;
	char *block;
} set_line;

//WTF is this shit?
int verbosity;

//calculate 2^exp
long long power(int exp) 
{
	long long result = 1;
	result = result << exp;
	return result;
}

cache buildCache(int numOfSets, int numOfLines, int blockSize) 
{

	cache newCache;	
	set_line line;
	//i : set index, j: line index;
	int i, j;
	newCache.sets = (cache_set*) malloc(sizeof(cache_set) * numOfSets);
	for (i = 0; i < numOfSets; ++i) 
	{
		set.lines =  (set_line*) malloc(sizeof(set_line) * numOfLines);
		cache_set set;
		newCache.sets[i] = set;
		for (j = 0; j < numOfLines; ++j) 
		{
			line.last_used = line.validBit = line.tag = 0;
			set.lines[j] = line;
		}
		free(set);
	} 
	return newCache;	
}

void cleanup(cache cache_, int numOfSets, int numOfLines, int blockSize) 
{
	for (int i = 0; i < numOfSets; ++i) 
	{
		cache_set set = cache_.sets[i];
		if (set.lines != NULL) {	
			free(set.lines);
		}
	} 
	if (cache_.sets != NULL) {
		free(cache_.sets);
	}
}

int find_empty_line(cache_set set_, cacheProperties propeties) {
	int numOfLines = properties.E;
	set_line line;

	for (int i = 0; i < numOfLines; ++i)
	{
		line = set_.lines[i];
		if (line.validBit == 0) {
			return i;
		}
	}
}

int find_evict_line(cache_set set_, cacheProperties par, int *used_lines) {
	
	//Returns index of least recently used line.
	//used_lines[0] gives least recently used line, used_lines[1] gives current lru counter or most recently used line.
	int numOfLines = par.E;
	int max_used = set_.lines[0].last_used;
	int min_used = set_.lines[0].last_used;
	int min_used_index = 0;

	set_line line; 
	int lineIndex;

	for (lineIndex = 1; lineIndex < numOfLines; lineIndex ++) {
		line = set_.lines[lineIndex];

		if (min_used > line.last_used) {
			min_used_index = lineIndex;	
			min_used = line.last_used;
		}

		if (max_used < line.last_used) {
			max_used = line.last_used;
		}
	}

	used_lines[0] = min_used;
	used_lines[1] = max_used;
	return min_used_index;
}

cacheProperties run_sim(cache sim_cache, cacheProperties par, unsigned long long address) {
		
		int lineIndex;
		int cache_full = 1;

		int numOfLines = par.E;
		int prev_hits = par.hits;

		int tag_size = (64 - (par.s + par.b));
		unsigned long long input_tag = address >> (par.s + par.b);
		unsigned long long temp = address << (tag_size);
		unsigned long long setIndex = temp >> (tag_size + par.b);
		
  		cache_set set_ = sim_cache.sets[setIndex];

		for (lineIndex = 0; lineIndex < numOfLines; lineIndex ++) 	{
			
			set_line line = set_.lines[lineIndex];
			
			if (line.validBit) {
					
				if (line.tag == input_tag) {
						
					line.last_used ++;
					par.hits ++;
					set_.lines[lineIndex] = line;
				}

			} else if (!(line.validBit) && (cache_full)) {
				//We found an empty line
				cache_full = 0;		
			}

		}	

		if (prev_hits == par.hits) {
			//Miss in cache;
			par.misses++;
		} else {
			//Data is in cache
			return par;
		}

		//We missed, so evict if necessary and write data into cache.
		
		int *used_lines = (int*) malloc(sizeof(int) * 2);
		int min_used_index = find_evict_line(set_, par, used_lines);	

		if (cache_full) 
		{
			par.evicts++;

			//Found least-recently-used line, overwrite it.
			set_.lines[min_used_index].tag = input_tag;
			set_.lines[min_used_index].last_used = used_lines[1] + 1;
		
		}

		else
	        {
			int empty_index = find_empty_line(set_, par);

			//Found first empty line, write to it.
			set_.lines[empty_index].tag = input_tag;
			set_.lines[empty_index].validBit = 1;
			set_.lines[empty_index].last_used = used_lines[1] + 1;
		}						

		free(used_lines);
		return par;
}

int main(int argc, char **argv)
{
	
	cache sim_cache;
	cacheProperties par;
	bzero(&par, sizeof(par));

	long long numOfSets;
	long long blockSize;	

 
	FILE *read_trace;
	char trace_cmd;
	unsigned long long address;
	int size;
	
	char *trace_file;
	char c;
    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1)
	{
        switch(c)
		{
        case 's':
            par.s = atoi(optarg);
            break;
        case 'E':
            par.E = atoi(optarg);
            break;
        case 'b':
            par.b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    if (par.s == 0 || par.E == 0 || par.b == 0 || trace_file == NULL) 
	{
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

	
	// you need to compute S and B yourself
	numOfSets = pow(2.0, par.s);
	blockSize = power(par.b);	
	par.hits = 0;
	par.misses = 0;
	par.evicts = 0;
	
	sim_cache = buildCache(numOfSets, par.E, blockSize);
 	
	// fill in rest of the simulator routine
	read_trace  = fopen(trace_file, "r");
	
	
	if (read_trace != NULL) {
		while (fscanf(read_trace, " %c %llx,%d", &trace_cmd, &address, &size) == 3) {

		
			switch(trace_cmd) {
				case 'I':
					break;
				case 'L':
					par = run_sim(sim_cache, par, address);
					break;
				case 'S':
					par = run_sim(sim_cache, par, address);
					break;
				case 'M':
					par = run_sim(sim_cache, par, address);
					par = run_sim(sim_cache, par, address);	
					break;
				default:
					break;
			}
		}
	}
	
	
    printSummary(par.hits, par.misses, par.evicts);
	cleanup(sim_cache, numOfSets, par.E, blockSize);
	fclose(read_trace);

    return 0;
}
