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
			line.lastUsed = line.validBit = line.tag = 0;
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

int findEmptyLine(cache_set set_, cacheProperties properties) {
	int numOfLines = properties.E;
	set_line line;

	for (int i = 0; i < numOfLines; ++i)
	{
		line = set_.lines[i];
		if (line.validBit == 0) 
		{
			return i;
		}
	}
}

//this function returns the index of the least used line
int findLine2Evict(cache_set set_, cacheProperties properties, int *used_lines) 
{
	//used_lines[0] gives least recently used line, used_lines[1] gives current lru counter or most recently used line.
	int numOfLines = properties.E;
	int mostUsed = set_.lines[0].lastUsed;
	int leastUsed = set_.lines[0].lastUsed;
	int leastUsedIndex = 0;
	set_line line;

	for int (i = 1; i < numOfLines; ++i) {
		line = set_.lines[i];

		if (leastUsed > line.lastUsed) {
			leastUsedIndex = i;	
			leastUsed = line.lastUsed;
		}

		if (mostUsed < line.lastUsed) {
			mostUsed = line.lastUsed;
		}
	}

	used_lines[0] = leastUsed;
	used_lines[1] = mostUsed;
	return leastUsedIndex;
}

cacheProperties run_sim(cache simCache, cacheProperties properties, unsigned long long address) 
{
		
		int cache_full = 1;
		int numOfLines = properties.E;
		int lastHit = properties.hits;

		int tagSize = (64 - (properties.s + properties.b));
		unsigned long long inputTag = address >> (properties.s + properties.b);
		unsigned long long setIndex = (address << tagSize) >> (tagSize + properties.b);
		
  		cache_set set_ = simCache.sets[setIndex];

		for (int i = 0; i < numOfLines; ++i) 	
		{
			
			set_line line = set_.lines[i];
			if (set_.lines[i].validBit)
			{
				if (set_line.[i].tag == inputTag)
				{
					++set_line.[i].lastUsed;
					++properties.hits;
				}
			}
			//a "blank line"
			 else if (!(set_line.[i].validBit) && (cache_full)) 
			{
				cache_full = 0;		
			}
		}	

		if (lastHit == properties.hits) 
		{
			//Miss in cache;
			properties.misses++;
		} 
		else 
		{
			return properties;
		}

		//We missed, so evict if necessary and write data into cache.
		
		int *used_lines = (int*) malloc(sizeof(int) * 2);
		int index = findLine2Evict(set_, properties, used_lines);	

		if (cache_full) 
		{
			properties.evicts++;
			//Found least-recently-used line, overwrite it.
			set_.lines[index].tag = inputTag;
			set_.lines[index].lastUsed = used_lines[1] + 1;
		}
		else
	    {
			int empty = findEmptyLine(set_, properties);

			//Found first empty line, write to it.
			set_.lines[empty].tag = inputTag;
			set_.lines[empty].validBit = 1;
			set_.lines[empty].lastUsed = used_lines[1] + 1;
		}						

		free(used_lines);
		return properties;
}

int main(int argc, char **argv)
{
	
	cache simCache;
	cacheProperties properties;
	bzero(&properties, sizeof(properties));

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
            properties.s = atoi(optarg);
            break;
        case 'E':
            properties.E = atoi(optarg);
            break;
        case 'b':
            properties.b = atoi(optarg);
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

    if (properties.s == 0 || properties.E == 0 || properties.b == 0 || trace_file == NULL) 
	{
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

	
	// you need to compute S and B yourself
	numOfSets = pow(2.0, properties.s);
	blockSize = power(properties.b);	
	properties.hits = 0;
	properties.misses = 0;
	properties.evicts = 0;
	
	simCache = buildCache(numOfSets, properties.E, blockSize);
 	
	// fill in rest of the simulator routine
	read_trace  = fopen(trace_file, "r");
	
	
	if (read_trace != NULL) {
		while (fscanf(read_trace, " %c %llx,%d", &trace_cmd, &address, &size) == 3) {

		
			switch(trace_cmd) {
				case 'I':
					break;
				case 'L':
					properties = run_sim(simCache, properties, address);
					break;
				case 'S':
					properties = run_sim(simCache, properties, address);
					break;
				case 'M':
					properties = run_sim(simCache, properties, address);
					properties = run_sim(simCache, properties, address);	
					break;
				default:
					break;
			}
		}
	}
	
	
    printSummary(properties.hits, properties.misses, properties.evicts);
	cleanup(simCache, numOfSets, properties.E, blockSize);
	fclose(read_trace);

    return 0;
}
