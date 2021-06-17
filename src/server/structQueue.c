
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "./headers/structQueue.h"

void merge(struct coda queue[], int l, int m, int r);
 
/* mergeSort iterativo */
void mergeSort(struct coda queue[]);


void initQueue(struct coda* queue){
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {//inzializzo il mutex
        fprintf(stderr, "Error in pthread_mutex_init()\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {//inizializzo la condition variables
        fprintf(stderr, "Error in pthread_cond_init()\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&queue->not_full, NULL) != 0) {//inizializzo la condition variables
        fprintf(stderr, "Error in pthread_cond_init()\n");
        exit(EXIT_FAILURE);
    }
    for(int i =0;i<QUEUE_SIZE;i++){//inizializzo le stringhe
        memset(queue->buffer[i].txt,'\0',MES_SIZE);
        memset(queue->buffer[i].time,'\0',DATA_SIZE);
    }
    queue->start = queue->end =queue->real_size= 0; //inizializzo l'inizio e la fine
}

void storeMessage(struct coda *queue,char* msg,char* time, struct client* client,char* color){
    int rEnd; //real end
    if (pthread_mutex_lock(&queue->mutex) != 0) {//blocco
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }
    rEnd = (queue->end + 1) % QUEUE_SIZE; //calcolo la fine del buffer se inserissi un messaggio 
        
    while (rEnd == queue->start) { //se aggiungendo un elemento, la fine e' uguale all'inizio allora e' pieno
            
        if (pthread_cond_wait(&queue->not_full, &queue->mutex) != 0) {//sblocco il mutex per dare la possibilita' di consumare, aspetto che qualsuno me lo segnali e rimetto il lock
            perror("pthread_cond_wait");
            exit(EXIT_FAILURE);
        }
        rEnd = (queue->end + 1) % QUEUE_SIZE;//ricalcolo la fine
    }
    queue->real_size++;
    queue->buffer[queue->end].color =color;//aggiungo il colore al messaggio
    strncpy(queue->buffer[queue->end].txt,msg,MES_SIZE ); //se non e' pieno aggiungo il messaggio alla fine
    queue->buffer[queue->end].client = client ; //aggiungo il mittente al messaggio
    strncpy(queue->buffer[queue->end].time,time,DATA_SIZE);//copio nel buffer l'ora di invio del messaggio
    queue->end = rEnd; //aggiorno il puintatnore che punta alla fine

    if (pthread_cond_signal(&queue->not_empty) != 0) {//dopo aver inserito, segnalo che non e' piu vuoto
        perror("pthread_cond_signal");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_unlock(&queue->mutex) != 0) {//sblocco il mutex
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
}

struct message* getMessage(struct coda *queue) {
    if (pthread_mutex_lock(&queue->mutex) != 0) {//blocco il mutex
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }
    while (queue->start == queue->end) { // vuol dire che e' vuoto
        if (pthread_cond_wait(&queue->not_empty, &queue->mutex) != 0) { //sblocco il mutex per permettere a qualcuno di riempirlo, aspetto finche non ricevo un segnale che mi dice che non e' piu vuoto e riblocco il mutex
            perror("pthread_cond_wait");
            exit(EXIT_FAILURE);
        }
    }

    struct message* msg =&( queue->buffer[queue->start]); //consumo il messaggio
    queue->start = (queue->start+1)%QUEUE_SIZE; // e incremento il contantore(rendendo invalido questo dato)
    queue->real_size--;
    if (pthread_cond_signal(&queue->not_full) != 0) { //segnalo che il buffer non e' piu pieno in quanto ho appena consumato
        perror("pthread_cond_signal");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_unlock(&queue->mutex) != 0) { //sblocco il mutex
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
    return msg;

}

void sort(struct coda *queue){
    if (pthread_mutex_lock(&queue->mutex) != 0) {//blocco il mutex
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }
    while (queue->start == queue->end) { // vuol dire che e' vuoto
        if (pthread_cond_wait(&queue->not_empty, &queue->mutex) != 0) { //sblocco il mutex per permettere a qualcuno di riempirlo, aspetto finche non ricevo un segnale che mi dice che non e' piu vuoto e riblocco il mutex
            perror("pthread_cond_wait");
            exit(EXIT_FAILURE);
        }
    }
    
    mergeSort(queue);//ordino la coda con il mergeSort

    if (pthread_mutex_unlock(&queue->mutex) != 0) { //sblocco il mutex
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
}

//funzione che resituisce il minimo fra due interi
int min(int x, int y) { 
    return (x<y)? x :y; 
}

 

void mergeSort(struct coda queue[])
{
   int curr_size;  // grandezza di un sottoarray che deve fare il merge, puo andare da 1 a n/2
                   
   int left_start; // indice dell'inizio dell'array da fare il merge
   int n = queue->real_size; //grandezza effettiva dell'array
    //(queue->end-queue->start+QUEUE_SIZE+1)%QUEUE_SIZE;
   /*
   a differenza del metodo ricorsivo, qui ho utilizzato un approccio bottom-up 
   ordino sottoarray di dimensione 1 e unisco per creare array di dimensione 2
   poi ordine array di dimensione 2 e unisco per creare array di dimensione 4 e cosi via...
   */
   for (curr_size=1; curr_size<= n-1; curr_size = 2*curr_size)
   {
       // prendo l'inizio di grandezza cur_size
       for (left_start=0; left_start<n-1; left_start += 2*curr_size)
       {
           
           //il punto centrale e' la fine della parte sinistra, mid+1 e' l'inizio della parte destra
           int mid = min(left_start + curr_size -1, n-1);
           //la fine della parte destra
           int right_end = min(left_start + 2*curr_size -1, n-1);
 
           // unisce i due array
           merge(queue, left_start, mid, right_end);
       }
   }
}
 
//prende una data in stringa e lo inserisce nell'array
void stroke(char* date,int d[]){
    int j=0;//indice dell array
    char buff[2];//buffer
    for(int i =0;i<strlen(date);i++){
        if(date[i+1]!=':'){//se la ora/sec/min e' formato da due caratteri allora prendine due
            buff[0] =date[i];
            buff[1] =date[i+1];
            i +=2;
            d[j++] =atoi(buff);
        }
        else{//senno' uno
            buff[0] =date[i];
            i++;
            d[j++] =atoi(buff); 
        }
    }
}
//comparo le date sottoforma di stringhe(le transformo in interi)
int compare(char* date1,char* date2){
    int d1[3];//data1 
    int d2[3];// data2
  
   stroke(date1,d1);//converto in interi
   stroke(date2,d2);
    for(int i =0;i<3;i++){//confronto
        if(d1[i]>d2[i])
            return 1;
        if(d1[i]<d2[i])
            return -1;
    }
    return 0;
}


//unisce due array : arr[l..m] e arr[m+1..r] ====> arr[]
void merge(struct coda queue[], int l, int m, int r)
{
    int i, j, k;// inidici dei 3 array
    int n1 = m - l + 1;//grandezza del primo array
    int n2 =  r - m;//grandezza del secondo array
   
 
    /* creo array temporanei */
    struct message L[n1], R[n2];
 
    /* copio i dati nei corrispettivi array L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = queue->buffer[(queue->start+l + i)%QUEUE_SIZE];
    for (j = 0; j < n2; j++)
        R[j] = queue->buffer[(queue->start+m + 1+ j)%QUEUE_SIZE];
 
    /* unione arr[l..r]*/
    i = 0;//indice del primo array
    j = 0;//indice del secondo array
    k = l;//indice del mio buffer (uso queue->start come offset in quanto e' una coda circolare e quindi non inizia necessariamente da l)
    while (i < n1 && j < n2){
        
        if (compare(L[i].time,R[j].time)<0){
            queue->buffer[(queue->start+k)%QUEUE_SIZE] = L[i];
            i++;
        }
        else{
            queue->buffer[(queue->start+k)%QUEUE_SIZE] = R[j];
            j++;
        }
       k++;
    }
 
    //copio i rimanenti elementi
    while (i < n1){
        queue->buffer[(queue->start+k)%QUEUE_SIZE] = L[i];
        i++;
        k++;
    }
 
    //copio i rimanenti elementi
    while (j < n2){
        queue->buffer[(queue->start+k)%QUEUE_SIZE] = R[j];
        j++;
        k++;
    }
}