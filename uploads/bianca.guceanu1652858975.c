#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define inf 999
typedef struct listR {
	char nameRouter[3];
	int bandwidth;
	int latenta;
	struct listR* next;
}listNodeR;
typedef struct listPC {
	char namePC[4];
	struct listPC* next;
}listNodePC;
typedef struct network {
	char nameCurrentRouter[3];
	int numberPCs;
	int numberConectionsForEachRouter;
	listNodeR* listRouter;
	listNodePC* listPCs;
}structNetwork;
typedef struct graphMatrix {
	int** costMatrix;
	int numR;
}graphMatrix;
typedef struct nodeDP{
	int node;
	int d;
	int parent;
} nodeDP;
//pentru PC-uri
listNodePC* getNodePC(listNodePC* listNode, int poz)
{
	for (int i = 0; i < poz; i++) {
		listNode = listNode->next;
		if (listNode->next == NULL)
			break;
	}
	return listNode;
}
void insertNodeHeadOfListPC(listNodePC** listStart, char* name)
{
	listNodePC* node = (listNodePC*)malloc(sizeof(listNodePC));
	if (node == NULL) {
		printf("ERROR: CAN NOT ALLOCATE RAM\n");
		return;
	}
	node->next = *listStart;
	strcpy(node->namePC, name);
	*listStart = node;
}
void insertNodeInListPC(listNodePC** listStart, char* name, int poz)
{
	if (poz == 0) {
		insertNodeHeadOfListPC(listStart, name);
		return;
	}
	listNodePC* node = (listNodePC*)malloc(sizeof(listNodePC));
	if (node == NULL) {
		printf("ERROR: CAN NOT ALLOCATE RAM\n");
		return;
	}
	listNodePC* aux = getNodePC(*listStart, poz - 1);
	node->next = aux->next;
	strcpy(node->namePC, name);
	aux->next = node;
}
//pentru routere
listNodeR* getNodeR(listNodeR* listNode, int poz)
{
	for (int i = 0; i < poz; i++) {
		listNode = listNode->next;
		if (listNode->next == NULL)
			break;
	}
	return listNode;
}
void insertNodeHeadOfListR(listNodeR** listStart, char* name, int band, int lat)
{
	listNodeR* node = (listNodeR*)malloc(sizeof(listNodeR));
	if (node == NULL) {
		printf("ERROR: CAN NOT ALLOCATE RAM\n");
		return;
	}
	node->next = *listStart;
	strcpy(node->nameRouter, name);
	node->bandwidth = band;
	node->latenta = lat;
	*listStart = node;
}
void insertNodeInListR(listNodeR** listStart, char* name,int band,int lat, int poz)
{
	if (poz == 0) {
		insertNodeHeadOfListR(listStart, name,band,lat);
		return;
	}
	listNodeR* node = (listNodeR*)malloc(sizeof(listNodeR));
	if (node == NULL) {
		printf("ERROR: CAN NOT ALLOCATE RAM\n");
		return;
	}
	listNodeR* aux = getNodeR(*listStart, poz - 1);
	node->next = aux->next;
	strcpy(node->nameRouter, name);
	node->bandwidth = band;
	node->latenta = lat;
	aux->next = node;
}
void printListR(listNodeR* listStart)
{
	printf("[");
	for (; listStart != NULL; listStart = listStart->next) {
		printf("%s ", listStart->nameRouter);
		printf("%d ", listStart->bandwidth);
		printf("%d ", listStart->latenta);
		if (listStart->next != NULL)
			printf(",");
	}
	printf("]\n");
}
void printListPC(listNodePC* listStart)
{
	printf("[");
	for (; listStart != NULL; listStart = listStart->next) {
		printf("%s", listStart->namePC);
		if (listStart->next != NULL)
			printf(",");
	}
	printf("]\n");
}
graphMatrix graphMatrixBandwitdth(int numarRoutere, structNetwork* structura)
{
	graphMatrix graph;
	graph.numR = numarRoutere;
	graph.costMatrix = (int**)malloc(sizeof(int*) * graph.numR);
	for (int i = 0; i < graph.numR; i++) {
		graph.costMatrix[i] = (int*)malloc(sizeof(int) * graph.numR);
	}
	for (int i = 0; i < graph.numR; i++) {
		for (int j = 0; j < graph.numR; j++) {
			graph.costMatrix[i][j] = inf;
		}
	}
	for (int cnt = 0; cnt < numarRoutere; cnt++) {
		for (int i = 0; i < structura[cnt].numberConectionsForEachRouter; i++) {
			listNodeR* aux=getNodeR(structura[cnt].listRouter,i);
			int cifra = aux->nameRouter[1] - '0';
			graph.costMatrix[cnt][cifra] = aux->bandwidth;
		}
	}
	return graph;
}
graphMatrix graphAdiacenta(int numarRoutere, structNetwork* structura)
{
	graphMatrix graph;
	graph.numR = numarRoutere;
	graph.costMatrix = (int**)malloc(sizeof(int*) * graph.numR);
	for (int i = 0; i < graph.numR; i++) {
		graph.costMatrix[i] = (int*)malloc(sizeof(int) * graph.numR);
	}
	for (int i = 0; i < graph.numR; i++) {
		for (int j = 0; j < graph.numR; j++) {
			graph.costMatrix[i][j] = inf;
		}
	}
	for (int cnt = 0; cnt < numarRoutere; cnt++) {
		for (int i = 0; i < structura[cnt].numberConectionsForEachRouter; i++) {
			listNodeR* aux = getNodeR(structura[cnt].listRouter, i);
			int cifra = aux->nameRouter[1] - '0';
			graph.costMatrix[cnt][cifra] = 1;
		}
	}
	return graph;
}
graphMatrix graphMatrixLatenta(int numarRoutere, structNetwork* structura)
{
	graphMatrix graph;
	graph.numR = numarRoutere;
	graph.costMatrix = (int**)malloc(sizeof(int*) * graph.numR);
	for (int i = 0; i < graph.numR; i++) {
		graph.costMatrix[i] = (int*)malloc(sizeof(int) * graph.numR);
	}
	for (int i = 0; i < graph.numR; i++) {
		for (int j = 0; j < graph.numR; j++) {
			graph.costMatrix[i][j] = inf;
		}
	}
	for (int cnt = 0; cnt < numarRoutere; cnt++) {
		for (int i = 0; i < structura[cnt].numberConectionsForEachRouter; i++) {
			listNodeR* aux = getNodeR(structura[cnt].listRouter, i);
			int cifra = aux->nameRouter[1] - '0';
			graph.costMatrix[cnt][cifra] = aux->latenta;
		}
	}
	return graph;
}
void printGraph(graphMatrix graph) {
	for (int i = 0; i < graph.numR; i++) {
		for (int j = 0; j < graph.numR; j++) {
			printf("%d ", graph.costMatrix[i][j]);
		}
		printf("\n");
	}
}
nodeDP* d(graphMatrix graph, int source)
{
	nodeDP* NDP = (nodeDP*)malloc(sizeof(nodeDP) * graph.numR);
	if (NDP == NULL)
		return NULL;
	for (int i = 0; i < graph.numR; i++)
	{
		NDP[i].node = i;
		NDP[i].d = inf;
		NDP[i].parent = -1;
	}
	NDP[source].d = 0;
	int position = source;
	int* visited;
	visited = (int*)malloc(graph.numR * sizeof(int));
	for (int i = 0; i < graph.numR; i++)
		visited[i] = 0;
	int count = 0;
	while (count < graph.numR - 1)
	{
		int min_distance = inf;
		for (int i = 0; i < graph.numR; i++)
			if (NDP[i].d < min_distance && !visited[i])
			{
				min_distance = NDP[i].d;
				position = i;
			}
		visited[position] = 1;
		for (int i = 0; i < graph.numR; i++)
			if (!visited[i])
				if (min_distance + graph.costMatrix[position][i] < NDP[i].d)
				{
					NDP[i].d = min_distance + graph.costMatrix[position][i];
					NDP[i].parent = position;
				}
		count++;
	}
	for (int i = 0; i < graph.numR; i++)
		for (int j = i + 1; j < graph.numR; j++)
		{
			if (NDP[i].d > NDP[j].d)
			{
				nodeDP aux = NDP[i];
				NDP[i] = NDP[j];
				NDP[j] = aux;
			}
		}
	return NDP;
}
void vectParents(nodeDP* NDP, int source, int dest, int nrNoduri,const char* filename)
{
	char router[3];
	for (int i = 0; i < nrNoduri; i++) {
		if (NDP[i].node == dest) {
			router[0] = 'R';
			router[1] = NDP[i].parent + '0';
			router[2] = '\0';
			fprintf(filename,"%s ", router);
			vectParents(NDP, source, NDP[i].parent, nrNoduri,filename);
		}
		if (NDP[dest].parent == source) {
			router[0] = 'R';
			router[1] = NDP[dest].parent + '0';
			router[2] = '\0';
			fprintf(filename, "%s ", router);
			return;
		}
	}
}
void printNDP(nodeDP* NDP, int source, int dest, int nrNoduri,const char* filename)
{
	vectParents(NDP, source, dest, nrNoduri,filename);
}
int main(int argc, char* argv[])
{
	structNetwork* routere = NULL;
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		printf("Eroare la deschiderea fisierului");
		exit(-1);
	}
	FILE* f1 = fopen(argv[2], "w");
	int numarRoutere = 0;
	fscanf(f, "%d", &numarRoutere);
	routere = (structNetwork*)malloc(sizeof(structNetwork) * numarRoutere);
	if (routere == NULL) {
		printf("Eroare la alocarea structurii");
		exit(-1);
	}
	char* numePC = (char*)malloc(sizeof(char) * 3);
	char* numeR = (char*)malloc(sizeof(char) * 2);
	int b = 0;
	int l = 0;
	int i = 0;
	for (int cnt = 0; cnt < numarRoutere;cnt++) {
		listNodeR* list1Routers = NULL;
		listNodePC* list1PCs = NULL;
		routere[cnt].nameCurrentRouter[0] = 'R';
		routere[cnt].nameCurrentRouter[1] = cnt+'0';
		routere[cnt].nameCurrentRouter[2] = '\0';
		fscanf(f, "%d", &routere[cnt].numberPCs);
		if (routere[cnt].numberPCs == 1) {
			fscanf(f, "%s", numePC);
			insertNodeHeadOfListPC(&list1PCs, numePC);
		}
		else {
			for (int i = 0; i < routere[cnt].numberPCs; i++) {
				if (i == 0)
				{
					fscanf(f, "%s", numePC);
					insertNodeHeadOfListPC(&list1PCs, numePC);
				}
				else {
					fscanf(f, "%s", numePC);
					insertNodeInListPC(&list1PCs, numePC, i);
				}
			}
		}
		routere[cnt].listPCs = list1PCs;
		fscanf(f, "%d", &routere[cnt].numberConectionsForEachRouter);
		if (routere[cnt].numberConectionsForEachRouter == 1) {
			fscanf(f, "%s", numeR);
			fscanf(f, "%d", &b);
			fscanf(f, "%d", &l);
			insertNodeHeadOfListR(&list1Routers, numeR, b, l);
		}
		else {
			for (int i = 0; i < routere[cnt].numberConectionsForEachRouter; i++) {
				if (i == 0) {
					fscanf(f, "%s", numeR);
					fscanf(f, "%d", &b);
					fscanf(f, "%d", &l);
					insertNodeHeadOfListR(&list1Routers, numeR, b, l);
				}
				else {
					fscanf(f, "%s", numeR);
					fscanf(f, "%d", &b);
					fscanf(f, "%d", &l);
					insertNodeInListR(&list1Routers, numeR, b, l, i);
				}
			}
		}
		routere[cnt].listRouter = list1Routers;
	}
	int tabelulDeRutare = 0;
	fscanf(f, "%d", &tabelulDeRutare);
	int nr = 0;
	char router1[3];
	char router2[3];
	fscanf(f, "%d", &nr);
	fscanf(f, "%s", router1);
	fscanf(f, "%s", router2);
	int source = router1[1] - '0';
	int destination = router2[1] - '0';
	graphMatrix graphA = graphAdiacenta(numarRoutere, routere);
	/*printGraph(graphA);*/
	nodeDP* NDP = d(graphA, source);
	char router[3];
	router[0] = 'R';
	router[1] = destination + '0';
	router[2] = '\0';
	fprintf(f1,"%s ", router);
	printNDP(NDP, source, destination, numarRoutere,f1);
	fprintf(f1, "%d\n", NDP[destination].d);
	graphMatrix graphL = graphMatrixLatenta(numarRoutere, routere);
	//printGraph(graphL);
	nodeDP* NDP1 = d(graphL, source,destination);
	fprintf(f1, "%s ", router);
	printNDP(NDP, source, destination, numarRoutere,f1);
	fprintf(f1, "%d\n", NDP1[destination].d);
	int subretea = 0;
	fscanf(f, "%d", &subretea);
	//verificare fisier
	/*for (int i = 0; i < numarRoutere; i++) {
		printf("%s\n", routere[i].nameCurrentRouter);
		printf("%d %d\n", routere[i].numberPCs, routere[i].numberConectionsForEachRouter);
		printListPC(routere[i].listPCs);
		printListR(routere[i].listRouter);
		printf("\n");
	}*/
}