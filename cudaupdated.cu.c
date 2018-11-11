#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include <device_functions.h>
#include <cuda_runtime_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
    int start;     // Index of first adjacent node in Ea
    int length;    // Number of adjacent nodes
} Node;


__global__ void CUDA_BFS_KERNEL(Node *Va, int *Ea, bool *Fa, bool *Xa, int *Ca,bool *done)
{

    int id = threadIdx.x + blockIdx.x * blockDim.x;
    if (id > NUM_NODES)
        *done = false;


    if (Fa[id] == true && Xa[id] == false)
    {
        printf("%d ", id); //This printf gives the order of vertices in BFS
        Fa[id] = false;
        Xa[id] = true;
        __syncthreads();
        int k = 0;
        int i;
        int start = Va[id].start;
        int end = start + Va[id].length;
        for (int i = start; i < end; i++)
        {
            int nid = Ea[i];

            if (Xa[nid] == false)
            {
                Ca[nid] = Ca[id] + 1;
                Fa[nid] = true;
                *done = false;
            }

        }

    }

}

// The BFS frontier corresponds to all the nodes being processed at the current level.


int main()
{




     int num_nodes=264346;
    int edge_size=366923;
  int line = edge_size*2;
    int ch[line];
  int ed[edge_size];
  int od[edge_size];
  int le[edge_size];
  FILE *myfile;
  Node node[num_nodes];
    myfile = fopen("soFi.txt","r");
    if (myfile == NULL)
    {
    printf("can not open file \n");
    return 1;
    }
    int i=0;
    for(i=0;i<num_nodes;i++){
        node[i].length=0;


    }
    i=0;

    while(line--){
        fscanf(myfile,"%d",&ch[i]);
        ch[i]=ch[i]-1;
        if(i%2==0){
            node[ch[i]].length+=1;
        }
                    i=i+1;

    //printf("%d\n", ch[i]);
    }
        fclose(myfile);

    int j=1;
    for(i=0;i<edge_size;i++){
        ed[i]=ch[j];
        j=j+2;
    }
    node[0].start=0;
    for(i=1;i<num_nodes;i++){
        node[i].start=node[i-1].start+node[i-1].length;
    }

    bool frontier[NUM_NODES] = { false };
    bool visited[NUM_NODES] = { false };
    int cost[NUM_NODES] = { 0 };

    int source = 0;
    frontier[source] = true;

    Node* Va;
    cudaMalloc((void**)&Va, sizeof(Node)*NUM_NODES);
    cudaMemcpy(Va, node, sizeof(Node)*NUM_NODES, cudaMemcpyHostToDevice);

    int* Ea;
    cudaMalloc((void**)&Ea, sizeof(Node)*NUM_NODES);
    cudaMemcpy(Ea, edges, sizeof(Node)*NUM_NODES, cudaMemcpyHostToDevice);

    bool* Fa;
    cudaMalloc((void**)&Fa, sizeof(bool)*NUM_NODES);
    cudaMemcpy(Fa, frontier, sizeof(bool)*NUM_NODES, cudaMemcpyHostToDevice);

    bool* Xa;
    cudaMalloc((void**)&Xa, sizeof(bool)*NUM_NODES);
    cudaMemcpy(Xa, visited, sizeof(bool)*NUM_NODES, cudaMemcpyHostToDevice);

    int* Ca;
    cudaMalloc((void**)&Ca, sizeof(int)*NUM_NODES);
    cudaMemcpy(Ca, cost, sizeof(int)*NUM_NODES, cudaMemcpyHostToDevice);



    int num_blks = 1;
    int threads = 5;



    bool done;
    bool* d_done;
    cudaMalloc((void**)&d_done, sizeof(bool));
    printf("\n\n");
    int count = 0;

    printf("Order: \n\n");

    cudaEvent_t start,stop;
    float time;
    cudaEventCreate (&start);
    cudaEventCreate (&stop);
    cudaEventRecord(start,0);


    do {
        count++;
        done = true;
        cudaMemcpy(d_done, &done, sizeof(bool), cudaMemcpyHostToDevice);
        CUDA_BFS_KERNEL <<<num_blks, threads >>>(Va, Ea, Fa, Xa, Ca,d_done);
        cudaMemcpy(&done, d_done , sizeof(bool), cudaMemcpyDeviceToHost);

    }while (!done);


    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time,start,stop);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    printf("fuck fuck fuck %f",time);





    cudaMemcpy(cost, Ca, sizeof(int)*NUM_NODES, cudaMemcpyDeviceToHost);

    printf("Number of times the kernel is called : %d \n", count);


    printf("\nCost: ");
    for (int i = 0; i<NUM_NODES; i++)
        printf( "%d    ", cost[i]);
    printf("\n");

}