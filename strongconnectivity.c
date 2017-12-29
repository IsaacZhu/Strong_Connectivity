#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "timecount.h"

#define MAXSIZE 256
enum COLOR {WHITE,GRAY,BLACK};

//弧结点
typedef struct arcnode{
    int dest;
    struct arcnode* next;
}arcnode;

//头结点
typedef struct nodehead{
    //这里还可以根据需要添加点的信息
    int nodeindex;
    int d;
    int f;
    int color;
    arcnode* firstarc;
}nodehead;

//邻接表
typedef struct graph{
    nodehead vertex[MAXSIZE+1];
    int vertexnum;
}graph;

int connectcomponent[50][MAXSIZE];
int componentsize[50];
int componentnum;

graph G,Gt;

void nodeinsert(graph *G, int nodeindex, int arcto){
    if (arcto == nodeindex) return;//指向自己的弧
    arcnode *P,*Q; //新结点
    P = (arcnode *)malloc(sizeof(arcnode));
    P->dest = arcto;
    P->next = NULL;
    if (G->vertex[nodeindex].firstarc == NULL){  //是该点的第一条弧
        G->vertex[nodeindex].firstarc = P;
    }
    else {
        Q = G->vertex[nodeindex].firstarc;
        while (Q->next != NULL){
            if (Q->dest == arcto) return; //该弧已经存在 返回
            Q = Q->next;
        }
        if (Q->dest == arcto) return; //该弧已经存在 返回
        Q -> next = P;
    }
}//nodeinsert

void buildgraph(graph *G, int *arcfrom,int *arcto,int nodenum,int arcnum){
    int i;
    //初始化图
    G->vertexnum = nodenum;
    for (i=1;i<=nodenum;++i){
        G->vertex[i].nodeindex = i;    //序号初始化
        G->vertex[i].firstarc = NULL;    //弧初始化
        G->vertex[i].d = 0;
        G->vertex[i].f = 0;
        G->vertex[i].color = WHITE;
    }
    //将弧加入图中
    for (i=0;i<arcnum;++i){
        nodeinsert(G,arcfrom[i],arcto[i]);
    }
}//buildgraph

int timing;

void DFSVisit(graph *G, int nodeindex){
    nodehead *u;
    arcnode *v;
    u = &G->vertex[nodeindex];
    timing++;
    u->d = timing;
    u->color = GRAY;
    
    v = u->firstarc;
    while (v != NULL){
        if (G->vertex[v->dest].color == WHITE){  //未检索
            DFSVisit(G,v->dest);
        }
        v = v->next;
    }

    u->color = BLACK;   //检索完成，设成黑色
    timing++;
    u->f = timing;
}//DFSVisit

void DFS(graph *G){
    int i;
    timing = 0;
    for (i = 1;i <= G->vertexnum;++i){
        if (G->vertex[i].color == WHITE){ //未检索
            DFSVisit(G,i);
        }
    }//for
}//DFS

//对于两个点，如果a.f比b.f大，那么应该先检索a
int compare(const void *a, const void *b){
    int *p = (int *)a;
    int *q = (int *)b;
    if (G.vertex[*p].f > G.vertex[*q].f) return -1;
    else return 1;
}

void BuildReverseGraph(graph *Gt){
    int i,j,nodenum;
    arcnode *arc;
    nodenum = Gt->vertexnum;

    int tmparc[MAXSIZE];//暂时记录某个点为起点的弧
    int tmparcnum = 0;
    for (i=1;i<=nodenum;++i){
        tmparcnum = 0;
        arc = Gt->vertex[i].firstarc;
        //把某个点所有弧记录下来
        while (arc != NULL){
            tmparc[tmparcnum++] = arc->dest;
            arc = arc -> next;
        }
        //对这些弧，按u.f降序排序
        qsort(tmparc,tmparcnum,sizeof(int),compare);
        //把这些弧按顺序插入到Gt中去
        Gt->vertex[i].firstarc = NULL;
        for (j=0;j<tmparcnum;++j){
            nodeinsert(Gt,i,tmparc[j]);
        }//for j
    }//for i
}//BuildReverseGraph

void DFSConnectVisit(graph *Gt, int nodeindex){
    //把当前结点加入连通分量
    connectcomponent[componentnum][componentsize[componentnum]++] = nodeindex;

    nodehead *u;
    arcnode *v;
    u = &Gt->vertex[nodeindex];
    u->color = GRAY;
    
    v = u->firstarc;

    while (v != NULL){
        if (Gt->vertex[v->dest].color == WHITE){  //未检索
            DFSConnectVisit(Gt,v->dest);
        }
        v = v->next;
    }

    u->color = BLACK;   //检索完成，设成黑色
}//DFSConnectVisit

void DFSConnect(graph *G, graph *Gt){
    int i, nodeforvisit[G->vertexnum+1];
    //把G中的点按u.f降序排列
    for (i=1;i<=G->vertexnum;++i) nodeforvisit[i] = i;
    qsort(nodeforvisit+1,G->vertexnum,sizeof(int),compare);

    //按排序后的点进行检索
    for (i=1;i<=G->vertexnum;++i){
        if (Gt->vertex[nodeforvisit[i]].color == WHITE){     //未检索，说明是一个新的连通分量
            componentnum++;
            DFSConnectVisit(Gt,nodeforvisit[i]);
        }
    }//for
}//DFSConnect

void listgraph(graph G){
    int i,nodenum;
    nodenum = G.vertexnum;
    nodehead *u;
    arcnode *arc;
    printf("node num is %d\n",nodenum);
    for (i=1;i<=nodenum;++i){
        u = &G.vertex[i];
        arc = u->firstarc;
        printf("node%d ,d:%d f:%d color:%d\n",u->nodeindex,u->d,u->f,u->color);
        printf("arc info:\n");
        while (arc != NULL){
            printf("%d ",arc->dest);
            arc = arc->next;
        }
        printf("\n");
    }
}

int main(){
	int i,j,k,arcnum;
    arcnum = MAXSIZE*log2(MAXSIZE);
    int arcfrom[arcnum],arcto[arcnum];
    int scale[7] = {0,8,16,32,64,128,256};
    char inputpath[100];
    char numstr[5];
    char inputfilename[100] = "/input.txt";
    char outputpath[100];
    char timefilename[100] = "/time1.txt";
    char resultpath[100];
    char resultfilename[100] = "/output1.txt";

	//read arcs
	FILE *fp,*fpt,*fpr;

	fpt=fopen("time1.txt","w+");
	fprintf(fpt,"********result*********\n");

	//calculate
	printf("**************CALCULATION START...****************\n");
	for (i=5;i<=6;++i){
        strcpy(inputpath,"Input/size");
		numstr[0]='0'+i;
        numstr[1]='\0';
        strcat(inputpath,numstr);
        strcat(inputpath,inputfilename);
        fp = fopen(inputpath,"r+");
        
        arcnum = scale[i]*log2(scale[i]);
        //读入弧信息
        for (j=0;j<arcnum;++j){
            fscanf(fp,"%d,%d",&arcfrom[j],&arcto[j]);
        }


        memset(componentsize,0,50*sizeof(int));
        componentnum = 0;

        buildgraph(&G,arcfrom,arcto,scale[i],arcnum);
        timestart();

        DFS(&G);          //DFS
        

        buildgraph(&Gt,arcto,arcfrom,scale[i],arcnum);   //建立Gt

        
        BuildReverseGraph(&Gt);    //把Gt的弧按u.f降序排列

        DFSConnect(&G,&Gt);   //DFS求连通分量

        for (j=0;j<componentnum;++j){
            printf("分量%d:\n",j+1);
            for (k=0;k<componentsize[j+1];++k){
                printf("%d ",connectcomponent[j+1][k]);
            }
            printf("\n");
        }
        timeend();

		//output result

        strcpy(outputpath,"Output/size");
		numstr[0]='0'+i;
        numstr[1]='\0';
        strcat(outputpath,numstr);
        strcpy(resultpath,outputpath);
        strcat(outputpath,timefilename);
        fpt = fopen(outputpath,"w+");
        
		//output time to time.txt
		fprintf(fpt,"\nN:%d time is %.8lfs\n",scale[i],returntime());

        //output connect component
        strcat(resultpath,resultfilename);
        fpr = fopen(resultpath,"w+");
        for (j=0;j<componentnum;++j){
            fprintf(fpr,"\ncomponent %d:\n",j+1);
            for (k=0;k<componentsize[j+1];++k){
                fprintf(fpr,"%d ",connectcomponent[j+1][k]);
            }
        }

		//output time to screen
		outputtime();
	}//for
	printf("\n**************CALCULATION END**********************\n");
	//fclose(fp);
    //fclose(fpr);
	//fclose(fpt);
}