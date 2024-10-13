#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAXCHAR 1000
#define numberOfProcessors 3
   struct task{
        int c , t , d, r, ad , taskId , ra , delay  ;
        float u ;
        bool faultyCore;
        bool seen ;
    };
    struct core{
        int taskId;
        bool faulty;
        int c; // remainig time
        int end;
    };
    struct core cores[numberOfProcessors];//array of core

    struct task getTask( char *str , int id  ){

            char *token;
            int val;//for atoi
            struct task oneTask;
            int  taskArray[4];//save ci , ti ,di integer
            int i=0;
            /* get the first token */
            token = strtok(str, ",");//for spilit
            /* walk through other tokens */
            while( token != NULL ) {
                val = atoi(token);//strint to int
                taskArray[i]=val;
                token = strtok(NULL, ",");
                i++;
            }
        oneTask.c=taskArray[0];
        oneTask.t=taskArray[1];
        oneTask.d=taskArray[2];
        oneTask.r=taskArray[3];
        oneTask.u= (float)oneTask.c/ (float) oneTask.d;
        oneTask.taskId=id;
        oneTask.seen = false;
        return oneTask;
    }

    int findIdleCore( ){
        int idleCore = -1 ;
        int j;
        for(j=0 ; j < numberOfProcessors ; j++){
            printf("--cores[%d].taskId  : %d\n" , j,cores[j].taskId  );
            if(cores[j].taskId == -1 ){
                idleCore= j;
            }
        }
        return idleCore;
    }

    void setFaultyCore( ){
        int random = rand()%numberOfProcessors;
        int i ;
        for ( i=0 ; i < numberOfProcessors ; i++){
            if (i == random ){
                cores[i].faulty= false;
            }else{
                cores[i].faulty=true;
            }
        }
    }

    int findMinTimeOfProcessor (){
        int min = cores[0].c ;
        int i ;
        for( i=0 ; i < numberOfProcessors ; i++){
            if( cores[i].c < min ){
                min = cores[i].c;
            }
        }
    }

    void subtractTime(int thisTime , struct task *tasks) {
        int min =findMinTimeOfProcessor();
        int i ;
        for ( i =0 ; i < numberOfProcessors ; i++){
            cores[i].c = cores[i].c - min ;
            if ( cores[i].c == 0 ){
                printf("____________________________________________________________________\n");
                printf("subtractTime^core number: %d ,  task number : %d ,  start executing : %d ,  end of executing : %d \n " ,
                      i , cores[i].taskId , tasks[cores[i].taskId].ra , cores[i].end );
                printf("____________________________________________________________________\n");
                cores[i].taskId = -1 ;
                cores[i].faulty = true;
            }
        }
    }

    void changeCoresExecutionTime(int thisTime , struct task *tasks){
        int i ;
        for ( i =0 ; i < numberOfProcessors ; i++ ){
            if( cores[i].taskId != -1 ){
                cores[i].c --;
            }
            if( cores[i].c == 0 && cores[i].taskId != -1 ){

                printf("____________________________________________________________________\n");
                printf("changeCoresExecutionTime//^core number: %d ,  task number : %d , start executing : %d , end of executing : %d \n " ,
                      i , cores[i].taskId , tasks[cores[i].taskId].ra , cores[i].end  );
                printf("____________________________________________________________________\n");
                cores[i].taskId = -1 ;
                cores[i].faulty = true;
            }
        }
    }

    void algorithm(  struct task *tasks , int numberOfTask , int *executionCount){//m == number of processor
        float theta = 0.5 ;
        int numOfReadeyList=0;
        bool feasibility  = true ;
        int thisTime =0 ;
        int realTotal=0;
        int temp ;

        struct task readyList[numberOfTask];
        int total = 0 ; //total number of perocess that execute
        while (total <numberOfTask ){//task ha tamoom beshan

            int i;
            for(i =0 ; i< numberOfTask ; i++){
                if(thisTime >= tasks[i].r){
                    if (tasks[i].seen == false){
                    tasks[i].ad=tasks[i].r + tasks[i].d;// tasks[i].ad=thistime + tasks[i].d;
                    readyList[numOfReadeyList] = tasks[i];
                    numOfReadeyList++;

                    }
                }
            }//task ha dorost add  mishn
            //printf("number of ready list %d \n", numOfReadeyList);
            while (  numOfReadeyList > 0){//readyList!= NULL

                selectionSort(readyList , numOfReadeyList);//sort by rd

                struct task Ti = readyList[numOfReadeyList -1];//array ro nozooli gereftm
                printf("--ti.c : %d \n" , Ti.c);
                printf("--number of ready list: %d\n" , numOfReadeyList);
                int idleCore = findIdleCore();
                printf("--time %d \n" , thisTime);
                if ( idleCore!= -1 ){
                    temp = -1;
                    if (executionCount[Ti.taskId] >1  ){
                        temp = tasks[Ti.taskId].ra; }
                    tasks[Ti.taskId].seen = true;
                    tasks[Ti.taskId].ra = thisTime;
                    Ti.ra = thisTime;
                    Ti.delay = Ti.ra - Ti.r;
                    if ( executionCount[Ti.taskId] == 1){
                        printf("**if ( executionCount[Ti.taskId] == 1) \n");
                        if((thisTime+ Ti.c) > Ti.ad){
                            printf("**if((thisTime+ Ti.c) > Ti.ad) and feasebility == false");
                            feasibility = false;
                            numOfReadeyList--;
                            total++;
                        }else {
                            if( Ti.u <= theta ){
                                printf("**if( Ti.u < theta )\n");
                               setFaultyCore( );//yeki az core ha ro be onvane faulty set mikone
                               // if bara checkPoint
                               if ( cores[idleCore].faulty == false){//inja faulty mise
                                   int intPart = (int) (Ti.c * 1.2);
                                   float float_intPart = (float) intPart;
                                   if (float_intPart == (Ti.c * 1.2)){
                                        Ti.c = intPart;
                                   }else{
                                       Ti.c = intPart+1;
                                   }
                                   Ti.faultyCore = false; // ****in estefade ii nadare ehtemaln
                                    printf("--Ti.faultyCore%d \n" ,Ti.faultyCore);
                                    printf("--ti.c in checkpoint %d \n " , Ti.c);
                               }else{
                                   Ti.faultyCore = true ;
                               }
                               //appaly core busy within ci
                               cores[idleCore].taskId = Ti.taskId;
                               cores[idleCore].c = Ti.c;
                               cores[idleCore].end = Ti.c + Ti.ra;// befahmim akharesh keye
                               numOfReadeyList--;// akharin onsor ro hazd mikonim
                               total++; // chan ta pardazesh karesh anjam shode
                               realTotal++;
                               printf("--cores[idleCore].taskId : %d \t,cores[idleCore].c : %d \t, numOfReadeyList :%d \t ,total :%d\t(non-critical)\n"
                                      ,cores[idleCore].taskId,cores[idleCore].c,numOfReadeyList, total);

                            }else{//ui > theta
                                printf("**ui > theta \n");//*********************************************************
                                setFaultyCore( );//yeki az core ha ro be onvane faulty set mik
                                cores[idleCore].taskId = Ti.taskId;//set core busy
                                cores[idleCore].c = Ti.c;
                                numOfReadeyList--;// akharin onsor ro hazd mikonim
                                total++;
                                realTotal++;
                                executionCount[Ti.taskId]++ ;
                                Ti.faultyCore = cores[idleCore].faulty ;
                                cores[idleCore].end = Ti.c + Ti.ra;
                                int idleCore2 = findIdleCore();
                                if ( idleCore2!= -1 ){
                                    cores[idleCore2].taskId = Ti.taskId;
                                    cores[idleCore2].c = Ti.c;
                                    Ti.faultyCore = cores[idleCore].faulty | cores[idleCore2].faulty  ;
                                    cores[idleCore2].end = Ti.c + Ti.ra;
                                }else{
                                    numOfReadeyList++;
                                    total-- ;
                                    realTotal--;
                                    printf("--idle core 2vom peida nmishe ");
                                }
                            }
                        }// inke  feasible bashe
                    }else if(executionCount[Ti.taskId] > 1){
                        printf("**else if(executionCount[Ti.taskId] > 1)\n");
                        if ( Ti.faultyCore == false ){
                            numOfReadeyList--;
                            total++;
                            realTotal++;
                             tasks[Ti.taskId].ra= temp;
                            printf("--oon ja ii ke mige age core aval faulty nabood az list hazf she");
                        }else{//if ( Ti.faultyCore == false )
                            if((thisTime+Ti.c) < Ti.ad){
                                printf("**((thisTime+Ti.c) < Ti.ad) tooye entekhab core 3vom \n");
                                int idleCore3 = findIdleCore();
                                cores[idleCore3].taskId = Ti.taskId;
                                cores[idleCore3].c = Ti.c;
                                cores[idleCore].end = Ti.c + Ti.ra;
                                Ti.faultyCore = Ti.faultyCore |cores[idleCore3].faulty;
                                numOfReadeyList --;
                                total ++;
                                realTotal++;
                                cores[idleCore].end = Ti.c + Ti.ra;
                            }else{
                                feasibility = false;
                                numOfReadeyList --;
                                total++;
                                printf("**else -->((thisTime+Ti.c) < Ti.ad ) and feasebility : false ");
                            }
                        }
                        if ( Ti.faultyCore == true ){
                            feasibility = false ;
                            realTotal--;
                        }

                    }//else if(executionCount[Ti.taskId] > 1)


                }// if ( idlecore ! = -1 )
                else{//else idlecore == -1//faghat idle core nadarim
                    int minTime = findMinTimeOfProcessor();//#1
                    subtractTime(thisTime , tasks);
                    thisTime = thisTime + minTime;
                    printf("change time when all cores working: this time : %d , min TIme %d \n" , thisTime , minTime);
                    printf("number of ready list in change time: %d \n " , numOfReadeyList);
                }
              /*  int idleCore4 = findIdleCore();
                printf("idleCore4 = findIdleCore() , %d \n  " ,idleCore4 );

            if ( findIdleCore()== -1 &&numOfReadeyList== 0 ){
                   int minTime = findMinTimeOfProcessor();
                    subtractTime(thisTime , tasks);
                    thisTime = thisTime + minTime;
                    printf("--change time when idle core nadashte bashim va ready list sefr bashe : %d ,time : %d \n" ,numOfReadeyList, thisTime);
                   // printf("number of ready list in change time: %d \n " , numOfReadeyList);
            }else if ( idleCore4 != -1 && numOfReadeyList== 0 ){
                    thisTime++ ;
                    changeCoresExecutionTime(thisTime , tasks);
                    printf(" \n--now change time when we have idle core and number of ready list is : %d \n ", numOfReadeyList);
                }*/
                printf("\n--number of ready list in end of while : %d \n", numOfReadeyList);
            }//readyList!= NULL




               if ( findIdleCore() != -1 && numOfReadeyList== 0 ){
                    thisTime++ ;
                    changeCoresExecutionTime(thisTime , tasks);
                    printf(" \n--now change time when we have idle core and number of ready list is : %d \n ", numOfReadeyList);
                }else if ( findIdleCore()==-1){
                    int minTime = findMinTimeOfProcessor();//#1
                    subtractTime(thisTime , tasks);
                    thisTime = thisTime + minTime;
                    printf("change time when all cores working: this time : %d , min TIme %d \n" , thisTime , minTime);
                    printf("number of ready list in change time: %d \n " , numOfReadeyList);
                }
           // printf("\n--ready list ==  null bere bebine task ha tamoom shodn ya na\n--total :%d , numberOfReadyList : %d , time : %d\n" , total , numOfReadeyList , thisTime);
        }//task ha tamoom beshan
        int i;
        for ( i=0 ; i< numberOfProcessors ; i++){
            if( cores[i].taskId != -1 ){
                    printf("____________________________________________________________________\n");
                    printf("end^core number: %d ,  task number : %d ,  start executing : %d ,  end of executing : %d  , c :%d ,  \n  " ,
                      i , cores[i].taskId , tasks[cores[i].taskId].ra , cores[i].end , tasks[cores[i].taskId].c  );
                    printf("____________________________________________________________________\n");


            }
        }
        printf("\n feasibility :%s",feasibility ? "true" : "false");
        printf("\actual task that execute without error :%d " , realTotal);
        printf(" \n--finish");
    }
    //__________________selection sort  __________________

void swap(struct task *xp, struct task  *yp){
    struct task temp = *xp;
    *xp = *yp;
    *yp = temp;
}
//nozoooli
void selectionSort(struct task *arr, int n){
    int i, j, min_idx;
    // One by one move boundary of unsorted subarray
    for (i = 0; i < n-1; i++){
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i+1; j < n; j++){
           // printf( "  min idx : %d\n", arr[min_idx].c );
            if (arr[j].c > arr[min_idx].c){
            min_idx = j;
            }
        }
        // Swap the found minimum element with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}
    //__________________selection sort __________________

    int main()
    {
       char ch ,file_name[25];
       FILE *fp;
       char str[MAXCHAR];
       struct task tasks[20000];
       int taskNumber=0;

        //____________________read from file _____________________________________________________
       strcpy(file_name, "E:\\6s\\os\\os project 2\\task3.txt");
       fp = fopen(file_name, "r"); // read mode
       if (fp == NULL)
       {
          perror("Error while opening the file.\n");
          exit(EXIT_FAILURE);
       }
        while (fgets(str, MAXCHAR, fp) != NULL){
          tasks[taskNumber]=getTask(str , taskNumber);
          taskNumber++;
        }
    fclose(fp);
        printf("-number of task  : %d \n" , taskNumber);//:)
    //________________set cores___________
        int i ;
        for ( i =0 ; i < numberOfProcessors ; i++){
            cores[i].faulty = true ;
            cores[i].taskId=-1;
            cores[i].c =0 ;
            cores[i].end=-1;
        }
        printf("-number of processor : %d \n" , numberOfProcessors);//:)
    //________________set execution count __________
        int executionCount[taskNumber] ;
        for ( i =0 ; i< taskNumber ; i++){
            executionCount[i] = 1 ;
        }
    //_________________algorithm _______________________
        algorithm(tasks ,taskNumber , executionCount );// task number mishe te'dad task ha

       return 0;
    }

