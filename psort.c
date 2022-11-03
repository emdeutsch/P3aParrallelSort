#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
int *input;
int fileTotalSize;
int threadTotal;
int threadCount = 0;
/*
    Current Questions:

    format of the file
    correct size of pointer
    merging at the ende
*/
void combineAllMerge(int lowIndex, int middleIndex, int highIndex)
{
    int *leftIndex;
    int leftIndexSize = middleIndex - lowIndex + 1;
    leftIndex = malloc(sizeof(int) * (leftIndexSize));

    int *rightIndex;
    int rightIndexSize = highIndex - middleIndex;
    rightIndex = malloc(sizeof(int) * (rightIndexSize));

    for (int i = 0; i < leftIndexSize; i++)
    {
        leftIndex[i] = input[i];
    }
    for (int j = 0; j < rightIndexSize; j++)
    {
        rightIndex[j] = input[middleIndex + j + 1];
    }

    int indexer = lowIndex;
    int leftCounter = 0;
    int rightCounter = 0;

    while (leftCounter < leftIndexSize && rightCounter < rightIndexSize)
    {
        if (rightIndex[rightCounter] >= leftIndex[leftCounter])
        {
            input[indexer] = leftIndex[leftCounter];
            leftCounter++;
        }
        else
        {
            input[indexer] = rightIndex[rightCounter];
            rightCounter++;
        }
        indexer++;
    }

    while (leftCounter < leftIndexSize)
    {
        input[indexer] = leftIndex[leftCounter];
        leftCounter++;
        indexer++;
    }

    while (rightCounter < rightIndexSize)
    {
        input[indexer] = rightIndex[rightCounter];
        rightCounter++;
        indexer++;
    }

    free(rightIndex);
    free(leftIndex);
}
void merge(int lowIndex, int highIndex)
{
    int middleIndex = lowIndex + (highIndex + lowIndex) / 2;
    if (highIndex > lowIndex)
    {
        merge(lowIndex, highIndex);
        merge(middleIndex + 1, highIndex);
        combineAllMerge(lowIndex, middleIndex, highIndex);
    }
}

void *merge_sort(void *args)
{
    int threadIndex = threadCount;
    threadCount++;
    int lowIndex = (threadIndex) * (fileTotalSize / threadTotal);
    int highIndex = (threadIndex + 1) * (fileTotalSize / threadTotal) - 1;
    int middleIndex = lowIndex + (highIndex - lowIndex) / 2;
    if (highIndex > lowIndex)
    {
        merge(lowIndex, middleIndex);
        merge(middleIndex + 1, highIndex);
        combineAllMerge(lowIndex, middleIndex, highIndex);
    }
    return;
}

int main(int argc, char *argv[])
{
    FILE *inputFile;
    inputFile = fopen(argv[1], "w+");
    fseek(inputFile, 0L, SEEK_END);
    int sizeOfFile = (int)ftell(inputFile);
    fileTotalSize = sizeOfFile / 4;
    input = malloc(sizeof *input * sizeOfFile); // probably not right but you get the idea
    fseek(inputFile, 0, SEEK_SET);
    fread(input, sizeof(input), 1, inputFile);
    threadTotal = get_nprocs();
    pthread_t threads[threadTotal];
    for (int i = 0; i < threadTotal; i++)
    {
        pthread_create(&threads[i], NULL, merge_sort, (void *)NULL);
    }
    for (int i = 0; i < threadTotal; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < threadTotal; i++)
    {
        combineAllMerge(0, ((i + 1) * (fileTotalSize / threadTotal)) / 2, (i + 1) * (fileTotalSize / threadTotal) - 1);
    }

    FILE *outputFile;
    outputFile = fopen(argv[2], "w+");
    fwrite(input, sizeof input, 1, outputFile);
    fclose(inputFile);
    fclose(outputFile);
    free(input);
}