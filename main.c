#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define FLAG_DIGIT  0x01
#define FLAG_ALPUP  0x02
#define FLAG_ALPLW  0x04
#define FLAG_SIGN   0x08

const char table[] =
{
    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
    58,59,60,61,62,63,64,
    91,92,93,94,95,96,
    123,124,125,126
};

#define SIZE_DIGIT  ('9' - '0' + 1)
#define SIZE_ALPUP  ('Z' - 'A' + 1)
#define SIZE_ALPLW  ('z' - 'a' + 1)
#define SIZE_SIGN   (sizeof(table) / sizeof(table[0]))

#define START_DIGIT 0
#define START_ALPUP (START_DIGIT + SIZE_DIGIT)
#define START_ALPLW (START_ALPUP + SIZE_ALPUP)
#define START_SIGN  (START_ALPLW + SIZE_ALPLW)

#define END_DIGIT   SIZE_DIGIT
#define END_ALPUP   (END_DIGIT + SIZE_ALPUP)
#define END_ALPLW   (END_ALPUP + SIZE_ALPLW)
#define END_SIGN    (END_ALPLW + SIZE_SIGN)

#define DEFAULT_SIZE    64

uint8_t* gen(uint8_t config,uint8_t* buffer,size_t length);
uint8_t random_max(uint8_t max);
#define random() rand() & 0xFF

int main(int argc,char** argv)
{
    unsigned int i = ((uint64_t)argv) * time(NULL) ^ ((uint64_t)argv[0]);
    printf("Seed: %u\n",i);
    srand(i);

    uint8_t* result;
    size_t size = DEFAULT_SIZE;
    uint8_t flags = 0;

    if(argc > 2)
    {
        for(i = 2; i < argc; i++)
        {
            if(argv[i][0] == '-' && argv[i][2] == '\0')
            {
                switch(tolower(argv[i][1]))
                {
                    case 'd':
                    {
                        flags |= FLAG_DIGIT;
                        break;
                    }
                    case 'u':
                    {
                        flags |= FLAG_ALPUP;
                        break;
                    }
                    case 'l':
                    {
                        flags |= FLAG_ALPLW;
                        break;
                    }
                    case 's':
                    {
                        flags |= FLAG_SIGN;
                        break;
                    }
                }
            }
            else
                break;
        }
    }
    if(flags == 0)
        flags = FLAG_DIGIT | FLAG_ALPUP | FLAG_ALPLW;
    if(argc > 1)
    {
        sscanf_s(argv[1],"%zu",&size);
        size++;
    }
    printf("Mode: ");
    if(flags & FLAG_DIGIT)
        printf("digits");
    if(flags & FLAG_ALPUP)
        printf(", upper case");
    if(flags & FLAG_ALPLW)
        printf(", lower case");
    if(flags & FLAG_SIGN)
        printf(", sign");

    result = (uint8_t*)malloc(sizeof(uint8_t) * size);
    size--;
    printf("\nLength: %zu\n\n",size);
    result[size] = '\0';

    gen(flags,result,size);
    printf("Result: %s\n",result);

    free(result);

    return 0;
}

uint8_t* gen(uint8_t config,uint8_t* buffer,size_t length)
{
    if(buffer == NULL)
        return NULL;
    uint8_t start,end;
    start = UINT8_MAX;
    end = 0;

    if(config & FLAG_DIGIT)
    {
        if(start > START_DIGIT)
            start = START_DIGIT;
        end = END_DIGIT;
    }
    if(config & FLAG_ALPUP)
    {
        if(start > START_ALPUP)
            start = START_ALPUP;
        end = END_ALPUP;
    }
    if(config & FLAG_ALPLW)
    {
        if(start > START_ALPLW)
            start = START_ALPLW;
        end = END_ALPLW;
    }
    if(config & FLAG_SIGN)
    {
        if(start > START_SIGN)
            start = START_SIGN;
        end = END_SIGN;
    }
    if(end == 0)
        return NULL;
    end -= start + 1;

    uint8_t tmp;
    size_t i;
    for(i = 0; i < length; i++)
    {
        tmp = random_max(end) + start;
        if(tmp >= START_DIGIT && tmp < END_DIGIT)
            buffer[i] = tmp + '0';
        else if(tmp < END_ALPUP)
            buffer[i] = tmp - START_ALPUP + 'A';
        else if(tmp < END_ALPLW)
            buffer[i] = tmp - START_ALPLW + 'a';
        else
            buffer[i] = table[tmp - START_SIGN];
    }
    return buffer;
}

uint8_t random_max(uint8_t max)
{
    uint8_t result = random();
    if(max < 0xFF)
        result = result % (max + 1);
    return result;
}