# include <stdio.h> 
# include <string.h> 
# include <pthread.h>
# include <stdlib.h>
# include <math.h>

# define NUMBER_OF_COLUMNS 20
# define NUMBER_OF_CLASSES 4
// void * solve(void *id){
//     int tid;
//     tid = (int) id;

// }
int main(){
    FILE *traincsv, *traincsvnorm, *weightscsv;
    char buffer[1024];
    char* token;
    double temp, max[NUMBER_OF_COLUMNS], min[NUMBER_OF_COLUMNS];
    int EOFchecker = 1;
    int number_of_datas = 0, number_of_wrong_predictions = 0;
    double vectors[NUMBER_OF_CLASSES][NUMBER_OF_COLUMNS + 1];
    double sum[NUMBER_OF_CLASSES];
    int winner = 0;

    for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        min[i] = INFINITY;
    }
    
    
    traincsv = fopen("train.csv","r");
    traincsvnorm = fopen("trainnorm.csv","w+");
    weightscsv = fopen("weights.csv","r");
    // fprintf(traincsvnorm,"%s\n",buffer);

    //load vectors to array last one is bias
    fscanf(weightscsv,"%s",buffer);//first junk line
    for (size_t i = 0; i < NUMBER_OF_CLASSES; i++)
    {
        fscanf(weightscsv,"%s",buffer);
        token = strtok(buffer,",");
        for (size_t j = 0; j < NUMBER_OF_COLUMNS + 1; j++)
        {
            temp = atof(token);
            vectors[i][j] = temp;
            token = strtok(NULL,",");//next token of last string
        }
    }

    //find min and max
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
    // Normilize
    double maxMinusMin[NUMBER_OF_COLUMNS];
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        maxMinusMin[i] = max[i] - min[i];
    }
    EOFchecker = 1;
    fseek(traincsv,0,SEEK_SET);
    fscanf(traincsv,"%s",buffer);//first line is just names
    fscanf(traincsv,"%s",buffer);
    while (EOFchecker != EOF)
    {
        token = strtok(buffer,",");
        //reset sum of wieghts to bias;
        for (size_t i = 0; i < NUMBER_OF_CLASSES; i++)
        {
            sum[i] = vectors[i][NUMBER_OF_COLUMNS];
        }
        for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
        {
            temp = atof(token);
            temp = (temp-min[i])/(maxMinusMin[i]);
            for (size_t j = 0; j < NUMBER_OF_CLASSES; j++)
            {
                sum[j] += vectors[j][i] * temp;
            }
            token = strtok(NULL,",");//next token of last string
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
            number_of_wrong_predictions ++;
        }
        number_of_datas ++;
        EOFchecker = fscanf(traincsv,"%s",buffer);
    }

    printf("wrong predictions : %d \nall predictions : %d \n",number_of_wrong_predictions,number_of_datas);
    fclose(traincsv);
    fclose(traincsvnorm);
    fclose(weightscsv);
}