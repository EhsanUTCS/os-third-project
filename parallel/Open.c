# include <stdio.h> 
# include <string.h> 
# include <pthread.h>
# include <stdlib.h>
# include <math.h>
# include <limits.h>

# define NUMBER_OF_COLUMNS 20
# define NUMBER_OF_CLASSES 4
# define NUMBER_OF_THREADS 4
# define NUMBER_OF_COLUMNS_PLUS_ONE 21 //STUPID C WONT LET ME MAKE GLOBAL VECTORS
double max[NUMBER_OF_COLUMNS], min[NUMBER_OF_COLUMNS];
double maxMinusMin[NUMBER_OF_COLUMNS];
int number_of_datas = 0, number_of_wrong_predictions = 0;
pthread_t threads[NUMBER_OF_THREADS];
pthread_mutex_t mutex_minmax;


void * solve(void *id){
    FILE *traincsv, *weightscsv;
    long tid;
    tid = (long) id;
    char buffer[1024];
    char* token;
    int EOFchecker = 1;
    double temp;
    double sum[NUMBER_OF_CLASSES];
    double vectors[NUMBER_OF_CLASSES][NUMBER_OF_COLUMNS_PLUS_ONE];
    int winner = 0;
    int number_of_datas_local = 0, number_of_wrong_predictions_local = 0;
    char filename[16];
    char* saveptr;
    sprintf(filename, "trainq%ld.csv", (tid + 1));
    // printf("Hello World! Thread ID, %ld\n", tid);
    // printf("%s\n", filename);
    traincsv = fopen(filename,"r");
    weightscsv = fopen("weights.csv","r");


    //load vectors to array last one is bias
    fscanf(weightscsv,"%s",buffer);//first junk line
    for (size_t i = 0; i < NUMBER_OF_CLASSES; i++)
    {
        fscanf(weightscsv,"%s",buffer);
        token = strtok_r(buffer,",",&saveptr);
        for (size_t j = 0; j < NUMBER_OF_COLUMNS + 1; j++)
        {
            temp = atof(token);
            vectors[i][j] = temp;
            token = strtok_r(NULL,",",&saveptr);//next token of last string
        }
    }
    fscanf(traincsv,"%s",buffer);//first line is just names
    fscanf(traincsv,"%s",buffer);
    while (EOFchecker != EOF)
    {
        token = strtok_r(buffer,",",&saveptr);
        //reset sum of wieghts to bias;
        // printf("thread id %ld token %s\n",tid,token);
        for (size_t i = 0; i < NUMBER_OF_CLASSES; i++)
        {
            sum[i] = vectors[i][NUMBER_OF_COLUMNS];
        }
        for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
        {
            temp = atof(token);

            //Normalize
            temp = (temp-min[i])/(maxMinusMin[i]);

            for (size_t j = 0; j < NUMBER_OF_CLASSES; j++)
            {
                sum[j] += vectors[j][i] * temp;
            }
            token = strtok_r(NULL,",",&saveptr);//next token of last string
        }
        //check which vector has most points
        for (size_t i = 0; i < NUMBER_OF_CLASSES; i++)
        {
            if (sum[i] > sum[winner])
            {
                winner = i;
            }
            
        }
        if(winner != atof(token)){
            number_of_wrong_predictions_local ++;
        }
        number_of_datas_local ++;
        EOFchecker = fscanf(traincsv,"%s",buffer);
    }
    pthread_mutex_lock(&mutex_minmax);
    number_of_datas+=number_of_datas_local;
    number_of_wrong_predictions+=number_of_wrong_predictions_local;
    pthread_mutex_unlock(&mutex_minmax);
    pthread_exit(NULL);
}
int main(){
    void * status;
    FILE *traincsv, *traincsvnorm, *weightscsv;
    char buffer[1024];
    char* token;
    double temp;
    int EOFchecker = 1;
    int return_code;
    
    
    if (pthread_mutex_init(&mutex_minmax, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        min[i] = INFINITY;
    }
    
    
    traincsv = fopen("train.csv","r");
    traincsvnorm = fopen("trainnorm.csv","w+");

    // find min and max
    fscanf(traincsv,"%s",buffer);//first line is just names
    fscanf(traincsv,"%s",buffer);
    while (EOFchecker != EOF)
    {
        token = strtok(buffer,",");
        for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
        {
            temp = atof(token);
            if(max[i]<temp) max[i] = temp;
            if(min[i]>temp) min[i] = temp;
            token = strtok(NULL,",");//next token of last string
        }
        EOFchecker = fscanf(traincsv,"%s",buffer);
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        maxMinusMin[i] = max[i] - min[i];
    }
    for(long tid = 0; tid < NUMBER_OF_THREADS; tid++)
	{
		return_code = pthread_create(&threads[tid],
				NULL, solve, (void *) tid);

		if (return_code)
		{
			printf("ERROR; return code from pthread_create() is %d\n",
					return_code);
			exit(-1);
		}
	}

    for(long i = 0; i < NUMBER_OF_THREADS; i++)
		pthread_join(threads[i], &status);
    printf("wrong predictions : %d \nall predictions : %d \n",number_of_wrong_predictions,number_of_datas);
    pthread_mutex_destroy(&mutex_minmax);
    fclose(traincsv);
    fclose(traincsvnorm);
    pthread_exit(NULL);
}