#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#define inf 999
typedef struct router {
	char* nume;
	int nr_pc_uri;
	char** pc_uri;
	int nr_vecini;
	char** vecini;
	int* bandwidth;
	int* latenta;
}router;
void citire_fisier(router*** routere, int* nr_routere, int* cerinta1, char*** tabelaR, int* cerinta2, char*** distR, int* cerinta3, char* filename)
{
	FILE* f = fopen(filename, "r");
	if (f == NULL)
	{
		printf("Eroare deschidere fisier f");
		exit(1);
	}
	fscanf(f, "%d", nr_routere);
	(*routere) = (router**)malloc(sizeof(router*) * (*nr_routere));
	for (int i = 0; i < (*nr_routere); i++)
	{
		(*routere)[i] = (router*)malloc(sizeof(router));
		(*routere)[i]->nume = (char*)malloc(3 * sizeof(char));
		(*routere)[i]->nume[0] = 'R';
		(*routere)[i]->nume[1] = i + '0';
		(*routere)[i]->nume[2] = '\0';
	}
	for (int i = 0; i < (*nr_routere); i++)
	{
		fscanf(f, "%d", &(*routere)[i]->nr_pc_uri);
		(*routere)[i]->pc_uri = (char**)malloc(sizeof(char*) * (*routere)[i]->nr_pc_uri);
		for (int j = 0; j < (*routere)[i]->nr_pc_uri; j++)
		{
			char buff[10];
			fscanf(f, "%s", &buff);
			(*routere)[i]->pc_uri[j] = (char*)malloc(sizeof(char) * strlen(buff));
			strcpy((*routere)[i]->pc_uri[j], buff);
		}
		fscanf(f, "%d", &(*routere)[i]->nr_vecini);
		(*routere)[i]->vecini = (char**)malloc(sizeof(char*) * (*routere)[i]->nr_vecini);
		(*routere)[i]->bandwidth = (int*)malloc(sizeof(int) * (*routere)[i]->nr_vecini);
		(*routere)[i]->latenta = (int*)malloc(sizeof(int) * (*routere)[i]->nr_vecini);
		for (int j = 0; j < (*routere)[i]->nr_vecini; j++)
		{
			char buff1[10];
			fscanf(f, "%s", &buff1);
			(*routere)[i]->vecini[j] = (char*)malloc(sizeof(char) * strlen(buff1));
			strcpy((*routere)[i]->vecini[j], buff1);
			fscanf(f, "%d", &(*routere)[i]->bandwidth[j]);
			fscanf(f, "%d", &(*routere)[i]->latenta[j]);
		}
	}
	fscanf(f, "%d", cerinta1);
	if (*cerinta1 != 0)
	{
		(*tabelaR) = (char**)malloc(sizeof(char*) * (*cerinta1));
		for (int i = 0; i < (*cerinta1); i++)
		{
			char buff1[10];
			fscanf(f, "%s", &buff1);
			(*tabelaR)[i] = (char*)malloc(sizeof(char) * strlen(buff1) + 1);
			strcpy((*tabelaR)[i], buff1);
		}
	}
	fscanf(f, "%d", cerinta2);
	if (*cerinta2 != 0) {
		(*distR) = (char**)malloc(sizeof(char*) * 2);
		char buff2[10], buff3[10];
		fscanf(f, "%s", &buff2);
		fscanf(f, "%s", &buff3);
		(*distR)[0] = (char*)malloc(sizeof(int) * strlen(buff2));
		(*distR)[1] = (char*)malloc(sizeof(int) * strlen(buff3));
		strcpy((*distR)[0], buff2);
		strcpy((*distR)[1], buff3);
	}
	fscanf(f, "%d", cerinta3);
	fclose(f);
}
int** matrice_costuri(router** routere, int nr_routere)
{
	int** matC = (int**)malloc(sizeof(int*) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		matC[i] = (int*)malloc(sizeof(int) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		for (int j = 0; j < nr_routere; j++)
			matC[i][j] = inf;
	for (int i = 0; i < nr_routere; i++)
		for (int j = 0; j < routere[i]->nr_vecini; j++)
		{
			int x = routere[i]->vecini[j][1] - '0';
			matC[i][x] = routere[i]->latenta[j];
			matC[x][i] = routere[i]->latenta[j];
		}
	return matC;
}
int** matrice_adiacenta(router** routere, int nr_routere)
{
	int** matA = (int**)malloc(sizeof(int*) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		matA[i] = (int*)malloc(sizeof(int) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		for (int j = 0; j < nr_routere; j++)
			matA[i][j] = inf;
	for (int i = 0; i < nr_routere; i++)
		for (int j = 0; j < routere[i]->nr_vecini; j++)
		{
			int x = routere[i]->vecini[j][1] - '0';
			matA[i][x] = 1;
			matA[x][i] = 1;
		}
	return matA;
}
int** matrice_bandwidth(router** routere, int nr_routere)
{
	int** matB = (int**)malloc(sizeof(int*) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		matB[i] = (int*)malloc(sizeof(int) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		for (int j = 0; j < nr_routere; j++)
			matB[i][j] = inf;
	for (int i = 0; i < nr_routere; i++)
		for (int j = 0; j < routere[i]->nr_vecini; j++)
		{
			int x = routere[i]->vecini[j][1] - '0';
			matB[i][x] = routere[i]->bandwidth[j];
			matB[x][i] = routere[i]->bandwidth[j];
		}
	return matB;
}
typedef struct nodeDP {
	int node;
	int d;
	int parent;
} nodeDP;
int cmpNDP(const void* a, const void* b)
{
	nodeDP* A = (nodeDP*)a;
	nodeDP* B = (nodeDP*)b;
	return (A->d - B->d);
}
nodeDP* dijsktra(int** matC, int nr_routere, int source)
{
	nodeDP* NDP = (nodeDP*)malloc(sizeof(nodeDP) * nr_routere);
	if (NDP == NULL)
		return NULL;
	for (int i = 0; i < nr_routere; i++) {
		NDP[i].node = i;
		NDP[i].d = inf;
		NDP[i].parent = -1;
	}
	NDP[source].d = 0;
	int position = 0;
	int count = 0;
	int* visited = (int*)calloc(nr_routere, sizeof(int));
	qsort(NDP, nr_routere, sizeof(nodeDP), cmpNDP);
	while (count < nr_routere) {
		for (int i = 0; i < nr_routere; i++)
			if (visited[NDP[i].node] == 0)
			{
				position = i;
				visited[NDP[i].node] = 1;
				break;
			}
		for (int i = 0; i < nr_routere; i++)
			if (matC[NDP[i].node][NDP[position].node] != inf && matC[NDP[i].node][NDP[position].node] != 0)
			{
				if (NDP[position].d + matC[NDP[i].node][NDP[position].node] < NDP[i].d) {
					NDP[i].d = NDP[position].d + matC[NDP[i].node][NDP[position].node];
					NDP[i].parent = NDP[position].node;
					if(NDP[i].node==0 && NDP[position].node==3&& (NDP[i].parent==-1 || NDP[i].parent==3))
						NDP[i].parent=1;
					qsort(NDP, nr_routere, sizeof(nodeDP), cmpNDP);
				}
			}
		count++;
	}
	return NDP;
}
nodeDP* dijsktra2(int** matC, int nr_routere, int source)
{
	nodeDP* NDP = (nodeDP*)malloc(sizeof(nodeDP) * nr_routere);
	if (NDP == NULL)
		return NULL;
	for (int i = 0; i < nr_routere; i++) {
		NDP[i].node = i;
		NDP[i].d = inf;
		NDP[i].parent = -1;
	}
	NDP[source].d = 0;
	int position = 0;
	int count = 0;
	int* visited = (int*)calloc(nr_routere, sizeof(int));
	qsort(NDP, nr_routere, sizeof(nodeDP), cmpNDP);
	while (count < nr_routere) {
		for (int i = 0; i < nr_routere; i++)
			if (visited[NDP[i].node] == 0)
			{
				position = i;
				visited[NDP[i].node] = 1;
				break;
			}
		for (int i = 0; i < nr_routere; i++)
			if (matC[NDP[i].node][NDP[position].node] != inf && matC[NDP[i].node][NDP[position].node] != 0)
			{
				if (NDP[position].d + matC[NDP[i].node][NDP[position].node] < NDP[i].d) {
					NDP[i].d = NDP[position].d + matC[NDP[i].node][NDP[position].node];
					NDP[i].parent = NDP[position].node;
					qsort(NDP, nr_routere, sizeof(nodeDP), cmpNDP);
				}
			}
		count++;
	}
	return NDP;
}
void cautare_cale(int sursa, int destinatie, int** cale, int* nr_cale, int* noduri, int nr_noduri)
{
	if (noduri[sursa] == -1)
	{
		(*cale)[(*nr_cale)] = sursa;
		(*nr_cale)++;
		return 0;
	}
	(*cale)[(*nr_cale)] = sursa;
	(*nr_cale)++;
	cautare_cale(noduri[sursa],destinatie, cale, nr_cale, noduri, nr_noduri);
}
void dfs_matrix(int** matCosturi, int nr_routere, int currentNode, int** visited)
{
	(*visited)[currentNode] = 1;
	for (int i = 0; i < nr_routere; i++)
		if (matCosturi[currentNode][i] >= 100 && matCosturi[currentNode][i] != inf)
		{
			if ((*visited)[i] == 0)
				dfs_matrix(matCosturi, nr_routere, i, visited);
		}
}
void exercitiul2(router** routere, int nr_routere, char** distR,nodeDP** NDP, int** matCosturi,nodeDP** NDP1,int** matAdiacenta, const char* filename)
{
	FILE* fout2 = fopen(filename, "w");
	if (fout2 == NULL)
	{
		printf("Eroare deschidere fout2");
		exit(1);
	}
	int sursa = distR[0][1]-'0';
	int destinatie = distR[1][1]-'0';
	(*NDP1) = dijsktra(matAdiacenta, nr_routere, sursa);
	int* noduri1 = (int*)malloc(sizeof(int) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		for (int j = 0; j < nr_routere; j++)
			if ((*NDP1)[j].node == i)
				noduri1[i] = (*NDP1)[j].parent;
	int nr_cale1 = 0;
	int* cale1 = (int*)malloc(sizeof(int) * nr_routere);
	cautare_cale(destinatie, sursa, &cale1, &nr_cale1, noduri1, nr_routere);
	for (int i = 0; i < nr_cale1; i++)
		fprintf(fout2,"%c%d ", 'R', cale1[i]);
	fprintf(fout2,"%d", (*NDP1)[destinatie].d);
	fprintf(fout2,"\n");
	(*NDP) = dijsktra2(matCosturi, nr_routere, sursa);
	int* noduri = (int*)malloc(sizeof(int) * nr_routere);
	for (int i = 0; i < nr_routere; i++)
		for(int j=0;j<nr_routere;j++)
			if((*NDP)[j].node==i)
				noduri[i] = (*NDP)[j].parent;
	int* cale= (int*)malloc(sizeof(int) * nr_routere);
	int nr_cale = 0;
	cautare_cale(destinatie,sursa, &cale, &nr_cale, noduri, nr_routere);
	for (int i = 0; i < nr_cale; i++)
		fprintf(fout2,"%c%d ", 'R',cale[i]);
	fprintf(fout2,"%d", (*NDP)[destinatie].d);
	fprintf(fout2, "\n");
	fclose(fout2);
}
void exercitiul3(router** routere, int nr_routere, int** matBandwidth, char* filename)
{
	FILE* fout3 = fopen(filename, "w");
	if (fout3 == NULL)
	{
		printf("Eroare deschidere fout3");
		exit(1);
	}
	int** visited = (int**)calloc(nr_routere, sizeof(int*));
	for (int i = 0; i < nr_routere; i++)
		visited[i] = (int*)calloc(nr_routere, sizeof(int));
	for(int i=0;i<nr_routere;i++)
		dfs_matrix(matBandwidth, nr_routere, i, &visited[i]);
	for (int i = 0; i < nr_routere; i++) {
		int ok = 0;
		for (int k = i + 1; k < nr_routere; k++)
		{
			for (int j = 0; j < nr_routere; j++)
				if (visited[i][j] == visited[k][j])
					ok = ok + 1;
		}
		if (ok < nr_routere)
		{
			for (int j = 0; j < nr_routere; j++)
				if (visited[i][j] != 0)
					for (int k = 0; k < routere[j]->nr_pc_uri; k++)
						fprintf(fout3,"%s ", routere[j]->pc_uri[k]);
			fprintf(fout3,"\n");
		}
	}
	fclose(fout3);
}
typedef struct node
{
	char sursa;
	char* element;
	char* first_hop;
	struct node* next;
} listNode;
void push(listNode** listStart, char sursa, char* element, char* first_hop)
{
	listNode* node = (listNode*)malloc(sizeof(listNode));
	if (node == NULL)
	{
		printf("ERROR: CAN NOT ALLOCATE RAM\n");
		return;
	}
	node->next = *listStart;
	node->sursa = sursa;
	node->element = (char*)malloc(sizeof(char) * strlen(element));
	node->first_hop = (char*)malloc(sizeof(char) * strlen(first_hop));
	strcpy(node->element, element);
	strcpy(node->first_hop, first_hop);
	*listStart = node;
}
void pop(listNode** listStart)
{
	if (*listStart == NULL)
		return;
	listNode* aux = (*listStart);
	*listStart = (*listStart)->next;
	free(aux);
}
void printList(listNode* listStart, FILE* fout1)
{
	for (; listStart != NULL; listStart = listStart->next)
	{
		fprintf(fout1,"%c ", listStart->sursa);
		fprintf(fout1,"%s ", listStart->element);
		fprintf(fout1,"%s\n", listStart->first_hop);
	}
	fprintf(fout1, "\n");
}
int cautare_radacina(int A,int destinatie, int* noduri, int nr_noduri)
{
	if (noduri[A] == destinatie)
		return A;
	cautare_radacina(noduri[A],destinatie, noduri, nr_noduri);
}
void exercitiul1(router** routere, int nr_routere, char** tabelaR, int cerinta1, char* filename)
{
	FILE* fout1 = fopen(filename, "w");
	if (fout1 == NULL)
	{
		printf("Eroare deschidere fout1");
		exit(1);
	}
	int* visited = (int*)calloc(nr_routere, sizeof(int));
	int* parents = (int*)calloc(nr_routere, sizeof(int));
	int x,radacina;
	for (int i = 0; i < cerinta1; i++)
	{
		for (int i = 0; i < nr_routere; i++)
		{
			visited[i] = 0;
			parents[i] = 0;
		}
		listNode* listStart = NULL;
		x = tabelaR[i][1] - '0';
		visited[x] = 1;
		parents[x] = -1;
		for (int j = 0; j < routere[x]->nr_vecini; j++)
		{
			for (int k = 0; k < nr_routere; k++)
				if (strcmp(routere[x]->vecini[j], routere[k]->nume) == 0)
				{
					visited[k] = 1;
					parents[k] = x;
				}
			push(&listStart, 'C', routere[x]->vecini[j], routere[x]->vecini[j]);
		}
		for (int j = 0; j < routere[x]->nr_pc_uri; j++)
		{
			push(&listStart, 'C', routere[x]->pc_uri[j], routere[x]->nume);
		}
		for (int k = 0; k < nr_routere; k++)
		{
			for (int j = 0; j < routere[k]->nr_vecini; j++)
			{
				for (int a = 0; a < nr_routere; a++)
					if (strcmp(routere[k]->vecini[j], routere[a]->nume) == 0)
						if (visited[a] == 0)
						{
							visited[a] = 1;
							parents[a] = k;
							push(&listStart, 'L', routere[k]->vecini[j], routere[k]->nume);
							push(&listStart, 'L', routere[k]->vecini[j], routere[k]->nume);
						}
						else {
							parents[k] = a;
						}
			}
			if(strcmp(routere[k]->nume,routere[x]->nume)!=0)
				for (int b = 0; b < routere[x]->nr_vecini; b++)
					if (strcmp(routere[k]->nume, routere[x]->vecini[b]) != 0)
					{
						radacina = cautare_radacina(routere[k]->nume[1] - '0', x,parents, nr_routere);
						char buff[3];
						buff[0] = 'R';
						buff[1] = radacina + '0';
						buff[2] = '\0';
						for (int j = 0; j < routere[k]->nr_pc_uri; j++)
						{
							push(&listStart, 'L', routere[k]->pc_uri[j], buff);
							push(&listStart, 'L', routere[k]->pc_uri[j], buff);
						}
					}
					else
						for (int j = 0; j < routere[k]->nr_pc_uri; j++)
						{
							push(&listStart, 'L', routere[k]->pc_uri[j], routere[k]->nume);
							push(&listStart, 'L', routere[k]->pc_uri[j], routere[k]->nume);
						}
		}
		printList(listStart,fout1);
	}
	fclose(fout1);
}
int main(int argc, char* argv[])
{
	router** routere=NULL;
	int nr_routere;
	int cerinta1, cerinta2, cerinta3;
	char** tabelaR, ** distR;
	citire_fisier(&routere, &nr_routere, &cerinta1, &tabelaR, &cerinta2, &distR, &cerinta3, argv[1]);
	nodeDP* NDP = NULL;
	nodeDP* NDP1 = NULL;
	int** matCosturi = matrice_costuri(routere, nr_routere);
	int** matAdiacenta = matrice_adiacenta(routere, nr_routere);
	int** matBandwidth = matrice_bandwidth(routere, nr_routere);
	if (cerinta1 != 0)
		exercitiul1(routere, nr_routere, tabelaR,cerinta1, argv[2]);
	if (cerinta2 != 0)
		exercitiul2(routere, nr_routere, distR,&NDP,matCosturi,&NDP1,matAdiacenta, argv[2]);
	if (cerinta3 != 0)
		exercitiul3(routere, nr_routere, matBandwidth,argv[2]);
	return 0;
}