#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

struct in_cache {
    int tag;
    int valid;
    int time;
};

struct da_cache {
    int tag;
    int valid;
    int time;
    int dirty;
};

int in_total, in_miss;
int da_total, da_miss, da_write;
int time_count;

struct in_cache* ip;
struct da_cache* dp;

void read_data(int addr, int c_size, int b_size, int assoc);
void write_data(int addr, int c_size, int b_size, int assoc);

int main() {
    int cache[6] = { 1024, 2048, 4096, 8192, 16384, 32768 };
    int block[3] = { 100, 200, 300 };
    int associative[4] = { 1, 2, 3, 3 };
    int x, y, z;
    int input = 0;

    while (input != 3) {
        printf("1. print all\n2. select\n3. exit\n: ");
        scanf_s("%d", &input);

        if (input == 1) {
            printf("cache size | block size | associative | d-miss rate | i-miss rate | mem write\n");
            for (x = 0; x < 2; x++) {
                for (y = 0; y < 5; y++) {
                    for (z = 0; z < 4; z++) {
                        simulation(cache[y], block[x], associative[z]);

                    }
                }
            }
        }

        else if (input == 2) {
            printf("cache size | block size | associative | d-miss rate | i-miss rate | mem write\n");
            printf("cache size : ");
            scanf_s("%d", &x);
            printf("block size : ");
            scanf_s("%d", &y);
            printf("associative : ");
            scanf_s("%d", &z);

            simulation(x, y, z);
        }
    }
    return 0;
}

void read_data(int addr, int c_size, int b_size, int assoc) {
    int num_of_sets, set;
    int x, ev = 0, avail = 10;
    struct da_cache* p;

    num_of_sets = c_size / (b_size * assoc);
    set = (addr / b_size) % num_of_sets;

    for (x = 0; x < assoc; x++) {
        p = &dp[set * assoc + x];

        if (p->valid == 1 && p->tag == (addr / b_size) / num_of_sets) {
            p->time = time_count;
            return;
        }

        else if (p->valid == 0) {
            if (x < avail)
                avail = x;
        }
    }

    da_miss++;
    
    if (avail == 10) {
        ev = evict(set, assoc, 'd');
        p = &dp[set * assoc + ev];

        if (p->dirty)
            da_write++;

        p->valid = 1;
        p->time = time_count;
        p->tag = (addr / b_size) / num_of_sets;
        p->dirty = 0;

    }
    else {
        p = &dp[set * assoc + avail];

        p->valid = 1;
        p->time = time_count;
        p->tag = (addr / b_size) / num_of_sets;
        p->dirty = 0;
    }
}

void write_data(int addr, int c_size, int b_size, int assoc) {
    int num_of_sets, set;
    int x, ev = 0, avail = 10;
    struct da_cache* p;

    num_of_sets = c_size / (b_size * assoc);
    set = (addr / b_size) % num_of_sets;

    for (x = 0; x < assoc; x++) {
        p = &dp[set * assoc + x];
        
        if (p->valid == 1 && p->tag == (addr / b_size) / num_of_sets) {
            p->time = time_count;
            p->dirty = 1;
            return;
        }
        else if (p->valid == 0) {
            if (x < avail)
                avail = x;
        }
    }
    da_miss++;

    if (avail == 10) {
        ev = evict(set, assoc, 'd');
        p = &dp[set * assoc + ev];

        if (p->dirty)
            da_write++;

        p->valid = 1;
        p->time = time_count;
        p->tag = (addr / b_size) / num_of_sets;
        p->dirty = 1;
    }
    else {
        p = &dp[set * assoc + avail];
        p->valid = 1;
        p->time = time_count;
        p->tag = (addr / b_size) / num_of_sets;
        p->dirty = 1;
    }
}
