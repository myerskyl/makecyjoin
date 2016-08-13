/*
	MAKECYJOIN:  A program for reading the coincident node output list from FEMAP
				 in file cyjoin_list.txt and to make CYJOIN cards in the file
				 cyjoin.bdf for input to NASTRAN

	Written by:  Kyle R. Myers
				 Applied Research Laboratory
				 The Pennsylvania State University
				 University Park, PA 16804
				 December 2015
*/

#include <stdio.h>
#include <time.h>

int isdigit(int);
long strtol(char *, char **, int);

int main()
{
	clock_t start = clock(), diff;

	FILE *fi = fopen("cyjoin_list.txt","r");
	FILE *fo = fopen("cyjoin.bdf","w");
	char line[128], *p;

	if ((!fi) || (!fo)) {
		printf("Missing cyjoin_list.txt or could not open cyjoin.bdf");
		return(1);
	}

	// initialize node array
	int nLines = 0;
    int c = '\0';
    int pc = '\n';

    while (c = fgetc(fi), c != EOF) {
        if (c == '\n'  &&  pc != '\n')
            nLines++;
        pc = c;
    }
    if (pc != '\n')
        nLines++;

    rewind(fi);
	const int nGridPts = nLines*2;
	int gridPts[nGridPts];

	// read in the grid point pairs
	int ii = 0;
	while (!feof(fi)) {

		int pairNum = 0;
		fgets(line, 128, fi);
		p = line;

		while (*p) {

			if (pairNum == 2)
				break;

            int lineChar = *p;
			if (isdigit(lineChar)) {
				pairNum++;
				long val = strtol(p, &p, 10);
				gridPts[ii] = val;
				ii++;
			}
			else
				p++;
		}
	}

	// write CYJOIN cards
	fprintf(fo,"$ <<BEGIN CYJOINS>>\nPARAM      CTYPE     ROT\nPARAM      NSEGS       ?\n");
	fprintf(fo,"CYJOIN         1       C");
	ii = 0;
	int fieldNum = 3;
	for(ii = 0; ii < nGridPts; ii = ii + 2) {
        fprintf(fo,"%8i", gridPts[ii]);
		fieldNum++;
		if (fieldNum == 9) {
			fprintf(fo," +\n+       ");
			fieldNum = 1;
		}
    }

	fprintf(fo,"\nCYJOIN         2       C");
	ii = 1;
	fieldNum = 3;
	for(ii = 1; ii < nGridPts; ii = ii + 2) {
        fprintf(fo,"%8i", gridPts[ii]);
		fieldNum++;
		if (fieldNum == 9) {
			fprintf(fo," +\n+       ");
			fieldNum = 1;
		}
    }

	fclose(fi);
	fclose(fo);

	printf("%i pairs written to file\n",(ii-1)/2);
	diff = clock() - start;
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("CPU time: %d sec, %d ms", msec/1000, msec%1000);

	return(0);

}
