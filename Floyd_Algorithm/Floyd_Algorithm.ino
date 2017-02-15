/*
 * Floyd Algorithm - Inspired from a number of bits of code off of the web and this white paper
 * http://www.nowherenearithaca.com/2013/05/robert-floyds-tiny-and-beautiful.html
 * Alex Judd - January 2017
 *
 */

int vektor[255];

void setup() {
    Serial.begin(9600);
    Serial.println("Initialising:"); //debug
    #define M 255
    #define N 255    

    unsigned char is_used[N] = { 0 }; /* flags */
    int in, im;
    im = 0;
  
    for (in = N - M; in < N && im < M; ++in) {
        int r = rand() % (N + 1); /* generate a random number 'r' */
        while (is_used[r])
        {
            /* we already have 'r' */
            r = rand() % (N + 1);
        }
        vektor[im++] = r; /* +1 since your range begins from 1 */
        is_used[r] = 1;
    }
}

void loop() {
    for (int i=0; i<255; i++)
    {
        Serial.println(vektor[i], DEC);
    }
    while(1); //end
}

