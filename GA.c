/* GENETIC ALGORITHM PROGRAM */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
struct gene{
    int variable;
    struct gene *next;
};
typedef struct gene Gene;
struct chromosome{
    Gene *genes;
    int fitness;
    struct chromosome *next;
};
typedef struct chromosome Chromosome;
/* FUNCTION SIGNATURES */
Gene *newGene(); /* to initialize first gene */
Chromosome *newChromosome(); /* to initialize first chromosome */
void addGene(Chromosome *,int); /* to add new gene to chromosome */
void addChromosome(Chromosome *); /* to add new chromosome to population */
Gene *getGene(Gene *,int); /* to get gene given index */
Chromosome *getChromosome(Chromosome *,int); /* to get chromosome given index */
void geneTransfer(Gene *,int); /* to copy given genes into the chromosome */
void generate(Gene *,Gene *,int,int,int); /* to generate the chromosomes */
void updateFitness(Chromosome *); /* to updete fitness of chromosomes */
int fitnessCalculator(Gene *); /* to calculate fitness of chromosomes */
void sortChromosomes(Chromosome *,int); /* to sort chromosomes with respect to fitness */
void updateBestChromosome(Gene *,Gene *); /* to update best chromosome that have best fitness */
void printContent(Chromosome *,Chromosome *,int); /* to print content of population */
void printGene(Gene *); /* to print content of chromosomes */
char **split(char *,char *); /* to split string by the specified seperator */
int sizeProb(Gene *); /* to calculate length of chromomes */
void freeGene(Gene *); /* to free content of chromosomes */
/* MAIN FUNCTION */
int main(int argc,char *argv[]){
    int PROB_SIZE=atoi(argv[1]);
    int POP_SIZE=atoi(argv[2]);
    int MAX_GEN=atoi(argv[3]);
    int i,j;
    char c;
    Chromosome *chromosome=newChromosome();
    Chromosome *bestChromosome=newChromosome();
    FILE *population=fopen("population","r");
    if(population!=NULL){
        while(!feof(population)){
            c=getc(population);
            if((int)c!=-1 && c!='\n' && c!='\r' && c!=':')
                addGene(chromosome,(int)(c-'0'));
            else if((int)c==-1)
                break;
            else if(c=='\n'){
                c=getc(population);
                if((int)c==-1)
                    break;
                else{
                    addChromosome(chromosome);
                    ungetc(c,population);
                } 
            }
        }
    }fclose(population);
    updateFitness(chromosome);
    sortChromosomes(chromosome,POP_SIZE);
    bestChromosome->fitness=getChromosome(chromosome,1)->fitness;
    updateBestChromosome(bestChromosome->genes,getChromosome(chromosome,1)->genes);
    printContent(chromosome,bestChromosome,0);
    FILE *selection=fopen("selection","r");
    FILE *mutate=fopen("mutate","r");
    FILE *xover=fopen("xover","r");
    fseek(selection,0,SEEK_END);
    int lenSelectionFile=ftell(selection);
    rewind(selection);
    fseek(xover,0,SEEK_END);
    int lenXoverFile=ftell(xover);
    rewind(xover);
    int mutateGene;
    char *lineSelectionFile=(char *)calloc(lenSelectionFile,sizeof(char));
    char *lineXoverFile=(char *)calloc(lenXoverFile,sizeof(char));
    for(i=1;i<=MAX_GEN;i++){
        fscanf(mutate,"%d",&mutateGene);
        fscanf(xover,"%s",lineXoverFile);
        fgets(lineSelectionFile,lenSelectionFile,selection);
        char **splittedSelectionLine=split(lineSelectionFile," ");
        char **xoverGenes=split(lineXoverFile,":");
        int startXover=atoi(xoverGenes[0]);
        int stopXover=atoi(xoverGenes[1]);
        for(j=0;j<POP_SIZE/2;j++){
            char **selectedChromosomes=split(splittedSelectionLine[j],":");
            int firstChromosome=atoi(selectedChromosomes[0]);
            int secondChromosome=atoi(selectedChromosomes[1]);
            generate(getChromosome(chromosome,firstChromosome)->genes,getChromosome(chromosome,secondChromosome)->genes,startXover,stopXover,mutateGene);   
        }
        updateFitness(chromosome);
        sortChromosomes(chromosome,POP_SIZE);
        if(getChromosome(chromosome,1)->fitness<bestChromosome->fitness){
            bestChromosome->fitness=getChromosome(chromosome,1)->fitness;
            updateBestChromosome(bestChromosome->genes,getChromosome(chromosome,1)->genes);
        }
        printContent(chromosome,bestChromosome,i);
    }fclose(selection); fclose(mutate); fclose(xover);
    return 0;
}
/* FUNCTIONS */
Gene *newGene(){
    Gene *gene=(Gene *)calloc(1,sizeof(Gene));
    gene->next=NULL;
    return gene;
}
Chromosome *newChromosome(){
    Chromosome *chromosome=(Chromosome *)calloc(1,sizeof(Chromosome));
    chromosome->next=(Chromosome *)calloc(1,sizeof(Chromosome));
    chromosome->genes=newGene();
    chromosome->next->genes=newGene();
    chromosome->next->next=NULL;
    return chromosome;
}
void addGene(Chromosome *chromosome,int item){
    chromosome=chromosome->next;
    while(chromosome->next!=NULL)
        chromosome=chromosome->next;
    geneTransfer(chromosome->genes,item);
}
void addChromosome(Chromosome *chromosome){
    chromosome=chromosome->next;
    while(chromosome->next!=NULL)
        chromosome=chromosome->next;
    Chromosome *tempChromosome=(Chromosome *)calloc(1,sizeof(Chromosome));
    tempChromosome->genes=newGene();
    tempChromosome->next=chromosome->next;
    chromosome->next=tempChromosome;
}
Gene *getGene(Gene *gene,int index){
    int i;
    gene=gene->next;
    for(i=1;i<index;i++)
        gene=gene->next;
    return gene;
}
Chromosome *getChromosome(Chromosome *chromosome,int index){
    int i;
    chromosome=chromosome->next;
    for(i=1;i<index;i++)
        chromosome=chromosome->next;
    return chromosome;
}
void geneTransfer(Gene *gene,int item){
    while(gene->next!=NULL)
        gene=gene->next;
    Gene *tempGene=(Gene *)calloc(1,sizeof(Gene));
    tempGene->variable=item;
    tempGene->next=gene->next;
    gene->next=tempGene;
}
void generate(Gene *firstChromosome,Gene *secondChromosome,int startXover,int stopXover,int mutateGene){
    int i;
    for(i=startXover;i<=stopXover;i++){
        int temp=getGene(firstChromosome,i)->variable;
        getGene(firstChromosome,i)->variable=getGene(secondChromosome,i)->variable;
        getGene(secondChromosome,i)->variable=temp;
    }
    getGene(firstChromosome,mutateGene)->variable=abs(getGene(firstChromosome,mutateGene)->variable-1);
    getGene(secondChromosome,mutateGene)->variable=abs(getGene(secondChromosome,mutateGene)->variable-1);
}
void updateFitness(Chromosome *chromosome){
    chromosome=chromosome->next;
    while(chromosome!=NULL){
        chromosome->fitness=fitnessCalculator(chromosome->genes);
        chromosome=chromosome->next;
    }
}
int fitnessCalculator(Gene *gene){
    int counter=sizeProb(gene);
    gene=gene->next;
    int fitness=0;
    while(gene!=NULL){
        fitness+=gene->variable*pow(2,counter-1);
        counter--;
        gene=gene->next;
    }
    return fitness;
}
void sortChromosomes(Chromosome *chromosome,int POP_SIZE){
    int i,j;
    Chromosome *root=chromosome;
    for(i=0;i<POP_SIZE-1;i++){
        for(j=i;j<POP_SIZE-1;j++){
            if(chromosome->next->fitness>chromosome->next->next->fitness){
                Chromosome *tempWorst=chromosome->next->next;
                chromosome->next->next=chromosome->next->next->next;
                tempWorst->next=chromosome->next;
                chromosome->next=tempWorst;            
            }
            chromosome=chromosome->next;
        }
        chromosome=root;
    }
}
void updateBestChromosome(Gene *receiver,Gene *transmitter){
    int i;
    int sizeTransmitter=sizeProb(transmitter);
    freeGene(receiver);
    for(i=1;i<=sizeTransmitter;i++)
        geneTransfer(receiver,getGene(transmitter,i)->variable);
}
void printContent(Chromosome *chromosome,Chromosome *bestChromosome,int generation){
    printf("GENERATION: %d\n",generation);
    chromosome=chromosome->next;
    while(chromosome!=NULL){
        printGene(chromosome->genes);
        printf(" -> %d\n",chromosome->fitness);
        chromosome=chromosome->next;
    }
    printf("Best chromosome found so far: ");
    printGene(bestChromosome->genes);
    printf(" -> %d\n",bestChromosome->fitness);
}
void printGene(Gene *gene){
    gene=gene->next;
    while(gene->next!=NULL){
        printf("%d:",gene->variable);
        gene=gene->next;
    }
    printf("%d",gene->variable);
}
char **split(char *string,char *chr){
    int i;
    char *character=(char *)calloc(strlen(chr)+3,sizeof(char));
    strcpy(character,chr);
    strcat(character,"\r\n");
    char **stringArray=(char **)calloc(1,sizeof(char *));
    char *token=strtok(string,character);
    for(i=0;token!=NULL;i++){
        stringArray[i]=(char *)calloc(strlen(token)+1,sizeof(char));
        strcpy(stringArray[i],token);
        stringArray=realloc(stringArray,(i+2)*sizeof(char *));
        token=strtok(NULL,character);
    }
    return stringArray;
}
int sizeProb(Gene *gene){
    int size;
    for(size=0;gene->next!=NULL;size++)
        gene=gene->next;
    return size;
}
void freeGene(Gene *gene){
    while(sizeProb(gene)!=0){
        Gene *tempGene=gene->next;
        gene->next=gene->next->next;
        free(tempGene);
    }
}