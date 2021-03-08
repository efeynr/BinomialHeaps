#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

typedef struct binomialnodes { // this is our struct for binomial nodes
    int frequency; // frequency is the number of recurrence time in the files
    int degree; // degree of a B-tree
    char fname[40]; //name of file
    struct binomialnodes *child, *sibling, *parent; //pointers
}binomialnodes;

binomialnodes* insertNodeToHeap(binomialnodes* header, binomialnodes* node);

binomialnodes *header_1=NULL; //this is the main header to keep main binomial heap
binomialnodes *header_2=NULL; // this is the second header to use when extracting a node
void analyzeBinomHeapFunc(binomialnodes* binomHeap);
void revertHeapOperationFunc (binomialnodes* h1);

binomialnodes* createNode(int freq_1, char name[]){
    binomialnodes *newNode =(binomialnodes*)malloc(sizeof(binomialnodes));
    newNode->frequency = freq_1;
    newNode->degree = 0;
    strcpy(newNode->fname,name);
    newNode->child = newNode->parent = newNode->sibling = NULL;
    return newNode;

}
void linkOperationFunc(binomialnodes* heap_1, binomialnodes* heap_2){ //this is for making first node as a child of second node and updating degrees
    heap_1->parent = heap_2;
    heap_1->sibling = heap_2->child;
    heap_2->child = heap_1;
    heap_2->degree = heap_2->degree + 1;
}
binomialnodes* mergeOperationFunc(binomialnodes* heap_1, binomialnodes *heap_2){ // this is the merge function that we merge 2 binomial heaps.
    binomialnodes *temporaryHeap1, *temporaryHeap2;
    binomialnodes *temporaryNode1, *temporaryNode2;
    binomialnodes *head=NULL;
    temporaryHeap1=heap_1;
    temporaryHeap2=heap_2;
    binomialnodes *tempHeap3;
    tempHeap3=NULL;

    if (temporaryHeap1 != NULL) { // first heap should not be NULL
        if (temporaryHeap2 != NULL && temporaryHeap1->degree <= temporaryHeap2->degree) { // check their degrees and second heaps condition
            head = temporaryHeap1;
        } else if (temporaryHeap2 != NULL && temporaryHeap1->degree > temporaryHeap2->degree) {
            head = temporaryHeap2;
        } else { // if the second heap is null we need to assing head as our first heap
            head = temporaryHeap1;
        }
    }
    else {  // if the first heap is null we need to assign our in-function as a second heap
        head = temporaryHeap2;
    }
    while (temporaryHeap1 && temporaryHeap2) { // we need to do our operations in case both of our heaps are not NULL
        
        if (temporaryHeap1->degree < temporaryHeap2->degree) {
            temporaryHeap1 = temporaryHeap1->sibling;
        }
        else if (temporaryHeap1->degree == temporaryHeap2->degree) {
            temporaryNode1 = temporaryHeap1->sibling;
            temporaryHeap1->sibling = temporaryHeap2;
            temporaryHeap1 = temporaryNode1;
        }
        else {
            temporaryNode2 = temporaryHeap2->sibling;
            temporaryHeap2->sibling = temporaryHeap1;
            temporaryHeap2 = temporaryNode2;
        }
    }
    return head; // eventually we return our merged heap

}
void analyzeBinomHeapFunc(binomialnodes* binomHeap) //this is for keeping the childs after delete operation to make our heap right.
{
    binomialnodes* tempCurrent = NULL;
    int controllerInt=0;
//in this function we are adding the childs to our heap to not lose them
    while (binomHeap) {
            controllerInt=controllerInt+2;
        tempCurrent = createNode( -1, "\0"); // this is for creating a node with meaningless inputs
        tempCurrent->frequency = binomHeap->frequency; // we are filling our current node
        strcpy(tempCurrent->fname, binomHeap->fname);
        header_1 = insertNodeToHeap(header_1, tempCurrent); // and add it into our main header

        analyzeBinomHeapFunc(binomHeap->child);// we need to recursively check all the childs
        binomHeap = binomHeap->sibling; // and also for the siblings
    }
}
binomialnodes *unionOperationfunc(binomialnodes* heap_1, binomialnodes*heap_2){
    binomialnodes* curNode;
    binomialnodes * prevNode;
    binomialnodes* nextNode;
    binomialnodes *heap1=NULL;
    heap1= mergeOperationFunc(heap_1, heap_2);
    int unionCounter=0;
    if(heap1==NULL){
        return heap1;
    }
    prevNode = NULL;
    curNode = heap1 ;
    nextNode = curNode->sibling;

    while(nextNode!=NULL){
        unionCounter++;
        if ((curNode->degree != nextNode->degree) ||
            ((nextNode->sibling != NULL) && (nextNode->sibling)->degree == curNode->degree)) {
            prevNode = curNode;
            curNode = nextNode;
        } else {
            if (curNode->frequency <= nextNode->frequency) {
                curNode->sibling = nextNode->sibling;
                linkOperationFunc(nextNode, curNode);
            }
            else {
                if (prevNode == NULL) {
                    heap1 = nextNode;
                }
                else {
                    prevNode->sibling = nextNode;
                }
                linkOperationFunc(curNode, nextNode);
                curNode = nextNode;
            }
        }
        nextNode = curNode->sibling;
    }
    //printf("%d",unionCounter);
    return heap1;


}

void revertHeapOperationFunc (binomialnodes* h1){ // we are reverting  our headers
    if (h1->sibling != NULL) {
        revertHeapOperationFunc(h1->sibling); // to point to the most sibling one
        (h1->sibling)->sibling = h1;
    }
    else
        header_2 = h1;
}


binomialnodes *extractMinOperation(binomialnodes *heapHeader) { // this is for deleting the nodes which is also can be called dequeue operation
    binomialnodes * nodeCurrent=heapHeader;
    binomialnodes *nodeMin=heapHeader;
    binomialnodes *nodeTemporary_1=NULL;
    binomialnodes *nodeTemporary_2=heapHeader;
    binomialnodes *nodeTemporary_3=NULL;

    header_2=NULL; // this is our second global header to use in this function
    //we have to make it null before do our operations

    if(nodeCurrent==NULL){ // if we dont have any node to extract in our main heap.
        printf("The node to extract was not found.");
        return nodeCurrent;
    }

    while(nodeTemporary_2->sibling){ //we have to run this loop to look the siblings of our header node

        if((nodeTemporary_2->sibling)->frequency < nodeMin->frequency){ // we are comparing the frequency of the nodes
            nodeMin=nodeTemporary_2->sibling;
            nodeTemporary_1=nodeTemporary_2;
            nodeCurrent=nodeTemporary_2->sibling;
        }
        nodeTemporary_2=nodeTemporary_2->sibling; // iterating tempNode2's pointer

    }if(nodeCurrent->child){ // if nodeCurrent has a chil

        revertHeapOperationFunc(nodeCurrent->child);//call the revertHeapFunction
        (nodeCurrent->child)->sibling=NULL;//
    }

    if(nodeTemporary_1==NULL && nodeCurrent->sibling==NULL){// if our first tempheap is null and current node's sibling is null
        heapHeader=NULL;//we assign heapheader as null and header1 and 2 has the same value
         header_1=header_2;
    }
    // in operations below we need to reanalyze the heap to keep everything as it should be
    else if(nodeTemporary_1==NULL){
        heapHeader=nodeCurrent->sibling;
        header_1 = heapHeader;
        analyzeBinomHeapFunc(header_2);
    }
    else if(nodeTemporary_1->sibling==NULL){
        nodeTemporary_1=NULL;
        analyzeBinomHeapFunc(header_2);
    }
    else{
        nodeTemporary_1->sibling=nodeCurrent->sibling;
        analyzeBinomHeapFunc(header_2);
    }
    nodeTemporary_3=nodeCurrent;
    return nodeCurrent; // lastly we pop the node to delete


}

binomialnodes *insertNodeToHeap(binomialnodes* header, binomialnodes* node1){ //inserting node to heap
    binomialnodes * temporaryNode=node1; //temporaryNode=node1;
    header= unionOperationfunc(header, temporaryNode); // we have to call union function
    return header;
}


int main() {
    //folder part ( we need to access the files from folder)
    DIR *directory=NULL;
    int sizeOfArray=0; // this is for to know how many files do we have to read
    struct dirent *sd;
    directory  =opendir("./files");

    if(directory==NULL){
        printf("Can't read the file");
    }

    while( (sd=readdir(directory)) !=NULL) { // this is for counting all the files in the folder
        //taking files step by step in every loop
        if (!strcmp(sd->d_name, "."))
            continue;
        if (!strcmp(sd->d_name, ".."))
            continue;
        sizeOfArray++;
    }
    closedir(directory);
    directory=opendir("./files");
    sd=NULL;


    //file part
    FILE *filePointer=NULL;
    char fileName[40]="\0"; // name of the files which we are going to read inside loop

    char takenkeyword[32]; // this is the real input
    char keyword[32]="\0"; // this is the punctuation-free form of the input
    printf("Please enter the keyword: ");
    scanf("%s", takenkeyword);
    int i=0;
    int o=0;
    //my code is not sensitive for punctuation there is no difference with can't or cant i am looking for both of them.
    while(takenkeyword[i]!='\0') {
       char tempChar = takenkeyword[i];

        if ((tempChar >= 97 && tempChar <= 122) || (tempChar >= 65 && tempChar <= 90)) { //if its in alphabet look for it as keyword.
            keyword[o]=tempChar;
            o++;
        }
        i++;
    }

    binomialnodes* heapArray[sizeOfArray]; // array of nodes
    int indexofArray=0; // this is for searching in the heapArray index by index
    while( (sd=readdir(directory)) !=NULL){
        //taking files step by step in every loop
        if(!strcmp(sd->d_name,".")) // these two lines is for the skipping outer folders
            continue;
        if(!strcmp(sd->d_name,".."))
            continue;

        strcpy(fileName,"\0");
        strcat(fileName,"./files/");
        strcat(fileName,sd->d_name);
        filePointer=fopen(fileName,"r");

        if(filePointer==NULL){
            printf("failed to open file : %s \n",fileName);
            fclose(filePointer);
        }
        // file is now read--------------------------------------------------------


        char ch;// char variable. I write all the chars inside our file to here
        char checker[32]="\0"; //keyword checker
        int freq=0; // we have no matches at first each time we read new file its going to be 0
        while ((ch = fgetc(filePointer)) != EOF) // ı am checking the file char by char
        {
            if((ch>=97&&ch<=122)||ch>=65&&ch<=90) { // if its in the alphabet put it into checker
                strncat(checker,&ch,1);
            }
            if((char)ch==32||ch==58||ch==46||ch==45){ // if its one of these symbols stop and check their equality
                // printf("%s\n",checker);
                if(stricmp(keyword,checker)==0){
                    freq++;
                }
                strcpy(checker,"\0");
            }
        }
        // ------------------end of frequency part--------------------------------
        heapArray[indexofArray] = createNode(500-freq,sd->d_name); // creating nodes and initialize them into our heap array
        header_1=insertNodeToHeap(header_1,heapArray[indexofArray]); // we are calling to insert function to insert them into our main heap header
        indexofArray++; // we are incrementing it to pass the next slot of heapArray
        fclose(filePointer);
    }
    printf(" \n all the nodes  before extracting most five min: \n ");
    int z;
    for(z=0;z<sizeOfArray;z++){ // this is for seeing all the files in our heap array
        printf("%s is: %d \n",heapArray[z]->fname,500-heapArray[z]->frequency);
    }

    printf("\n\n\n\n");
    printf(" keyword is : %s \n\n ",takenkeyword);
    int t;

    for(t=0;t<5;t++){ // this is for extracting most five element in our heap
        binomialnodes* initialMinNode=extractMinOperation(header_1);// we are creating a new node which is going to be the node that extracted.
        char extractedFileName[40]; // we need to read the extracted files so these are the exactly same steps to read eat first.
        char fch;
        strcpy(extractedFileName,"\0");
        strcat(extractedFileName,"./files/");
        strcat(extractedFileName,initialMinNode->fname);

        FILE * ftoread = NULL;
        ftoread=fopen(extractedFileName,"r");

        if(ftoread==NULL){
            printf("failed to open file : %s \n",fileName);
            fclose(ftoread);
        }

        printf("\n extracted binomial node : %s -----> frequency=> %d \n\n",initialMinNode->fname,500-initialMinNode->frequency);
        while ((fch = fgetc(ftoread)) != EOF) {
            printf("%c",fch);
        }
        printf("--------------------------------------------------------------------------------------------\n\n");
        fclose(ftoread);
    }



    closedir(directory);

    return 0;
}
