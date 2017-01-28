//
//  main.cpp
//  CA_simulation
//  Vichniac model for 2 immiscible fluids
//

#include <stdlib.h>
#include <stdio.h>

int **grid;
int **new_grid;
int N;

FILE *moviefile;
FILE *resultsfile;
int t;

void write_nr_white_nr_black(int funct);

void init_movie(int funct) {
    char filename[50];
    sprintf(filename, "%d-movie.dat", funct);
    puts(filename);
    moviefile = fopen(filename, "wb");
}


int countHowManyOnesInBinaryRepresentation(int i) {
    // Java: use >>> instead of >>
    // C or C++: use uint32_t
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

//int main() {
//    std::cout << countHowManyOnesInBinaryRepresentation(1000000);
//}



void init_grid() {
    int i, j;
    double p;


    N = 100;
    p = 0.5;

    grid = (int **) malloc(N * sizeof(int *));
    for (i = 0; i < N; i++)
        grid[i] = (int *) malloc(N * sizeof(int));

    //I need this to hold the copy of the old grid in the original
    new_grid = (int **) malloc(N * sizeof(int *));
    for (i = 0; i < N; i++)
        new_grid[i] = (int *) malloc(N * sizeof(int));


    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (rand() / (RAND_MAX + 1.0) < p) grid[i][j] = 0;
            else grid[i][j] = 1;

    printf("Initialized the %d x %d grid\n ", N, N);
}

int calculate_sum(int i, int j) {
    int ii, jj;
    int ni, nj;
    int sum;

    sum = 0;

    for (ii = -1; ii <= 1; ii++)
        for (jj = -1; jj <= 1; jj++) {
            ni = i + ii;
            nj = j + jj;

            //PBC: periodic fold back
            if (ni >= N) ni -= N;
            if (nj >= N) nj -= N;
            if (ni < 0) ni += N;
            if (nj < 0) nj += N;

            sum += grid[ni][nj];

        }

    return sum;
}


int vichniac_value(int rule, int sum) {
    int vvalue;

// 0 1 2 3 5 return 0
// 4 6 7 8 9 return 1

    // sum of neighb = 1, values:      0 1 2 3 4 5 6 7 8 9
    // return value by sum of neighb:  0 0 0 0 1 0 1 1 1 1
    int i;
    int array[10];

    for (i = 0; i < 10; ++i) {  // assuming a 10 bit int
        array[i] = rule & (1 << i) ? 1 : 0;
    }

    return array[sum];

//
//    if ((sum < 4) || (sum == 5)) vvalue = 0;
//    else vvalue = 1;
//
//    return vvalue;
}

int game_of_life_value(int sum, int own_value) {
    int gvalue;
    int neigh_sum;

// 0 1 - return 0
// 2 3 and it's alive - return 1
// 3 and it's dead - becomes alive
// more than 3 - dies - return 0

//at this point the sum has its own value included

    neigh_sum = sum - own_value;

    if (((neigh_sum < 2) || (neigh_sum > 3)) || ((neigh_sum == 2) && (own_value == 0))) gvalue = 0;
    else gvalue = 1;

    return gvalue;
}

void apply_Vichniac_step_to_grid(int rule) {
    int i, j;
    int sum;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            sum = calculate_sum(i, j);
            new_grid[i][j] = vichniac_value(rule, sum);
        }

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            grid[i][j] = new_grid[i][j];

}

void apply_Game_of_Life_step_to_grid() {
    int i, j;
    int sum;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            sum = calculate_sum(i, j);
            new_grid[i][j] = game_of_life_value(sum, grid[i][j]);
        }

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            grid[i][j] = new_grid[i][j];


}

void write_cmovie() {
    int i, j;
    float floatholder;
    int intholder;

    intholder = N * N;
    fwrite(&intholder, sizeof(int), 1, moviefile);

    intholder = t;
    fwrite(&intholder, sizeof(int), 1, moviefile);

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            //color of the particles
            intholder = grid[i][j];
            fwrite(&intholder, sizeof(int), 1, moviefile);
            intholder = i;//ID
            fwrite(&intholder, sizeof(int), 1, moviefile);
            floatholder = i * 1.0;
            fwrite(&floatholder, sizeof(float), 1, moviefile);
            floatholder = j * 1.0;
            fwrite(&floatholder, sizeof(float), 1, moviefile);
            floatholder = 1.0;//cum_disp, cmovie format
            fwrite(&floatholder, sizeof(float), 1, moviefile);
        }

}


int main(int argc, const char *argv[]) {
    for (int rule = 0; rule <=992; rule++) {
//    for (int rule = 0; rule <= 10; rule++) {
        if (countHowManyOnesInBinaryRepresentation(rule)==5) {
//        if (countHowManyOnesInBinaryRepresentation(rule) > 0) {
            init_grid();
            init_movie(rule);
            for (t = 0; t < 500; t++) {
                apply_Vichniac_step_to_grid(rule);
                //apply_Game_of_Life_step_to_grid();
//                if (t%100)
                {
                    write_cmovie();
                    printf("time = %d\n", t);
                }
            }
            write_nr_white_nr_black(rule);
            fclose(moviefile);
        }
    }
    return 0;
}

void write_nr_white_nr_black(int funct) {

    int nrw = 0;
    int nrb = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 0) {
                nrw++;
            } else {
                nrb++;
            }
        }
    resultsfile = fopen("stat.txt", "a");
    fprintf(resultsfile, "W=%d\t B=%d\t F=%d\n", nrw, nrb, funct);

}
