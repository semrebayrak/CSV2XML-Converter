#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define SPACE ' '
#define MAXFLDS 200   /* maximum possible number of fields */
#define MAXFLDSIZE 32 /* longest possible field + 1 = 31 byte field */
void parse(char *record, char *delim, char arr[][MAXFLDSIZE], int *fldcnt)
{
	char *p = strtok(record, delim);
	int fld = 0;

	while (p)
	{
		strcpy(arr[fld], p);
		fld++;
		p = strtok('\0', delim);
	}
	*fldcnt = fld;
}
int main(int argc, char *argv[])
{
	char tmp[1024] = {0x0};
	char tmp2[1024] = {0x0};
	int fldcnt = 0;
	char arr[MAXFLDS][MAXFLDSIZE] = {0x0};
	char arr1[1][MAXFLDSIZE];
	int recordcnt = 0;

	// INPUT CHECKS (( SADECE XML uzantısını kontrol ettirmedim çünkü txt dosyası 
	// HER ŞEKİLDE AÇILABILIYOR))
    if(argc!=7){
		perror("Missing value");
		exit(EXIT_FAILURE);
	}
	if (strcmp(argv[6], "1") != 0 && strcmp(argv[6], "0") != 0 && strcmp(argv[6], "2") != 0)
	{
		perror("Wrong value");
		exit(EXIT_FAILURE);
	}
   
	if (!strcmp(argv[3], "-seperator") == 0)
	{

		perror("Wrong value");
		exit(EXIT_FAILURE);
	}
	if (!strcmp(argv[5], "-opsys") == 0)
	{

		perror("Wrong value");
		exit(EXIT_FAILURE);
	}
	if (strcmp(argv[4], "tab") == 0)
		strcpy(argv[4], "\t");

	else if (!(strcmp(argv[4], "-") == 0 || strcmp(argv[4], ",") == 0))
	{
		perror("Wrong Delimeter");
		exit(EXIT_FAILURE);
	}

	FILE *in = fopen(argv[1], "r"); /* open file on command line */

	if (in == NULL)
	{
		perror("File open error");
		exit(EXIT_FAILURE);
	}

	strtok(argv[1], ".");
	fgets(tmp, sizeof(tmp), in);

	FILE *fpc;

	/* open the file for writing*/ /// PRINT OUT FILE
	fpc = fopen(argv[2], "w");

	/////////////// ISLETIM SISTEMI AYIRMA
		if (strcmp(argv[6],"0")==0)
		{ // WİNDOWS

			tmp[strcspn(tmp, "\r\n")] = 0;
			
		}
		else if (strcmp(argv[6],"1")==0)
		{ // LİNUX  
          
			tmp[strcspn(tmp, "\n")] = 0;
			
		}
		else if (strcmp(argv[6],"2")==0){ //MACOS
			tmp[strcspn(tmp, "\r")] = 0;
			
		}

	/* close the file*/

	char string[100], *start, pv;
	int c, d;

	/// BOŞLUKLARI _ ile değiştir
	start = malloc(strlen(tmp) + 1);
	if (start == NULL)
		exit(EXIT_FAILURE);

	pv = 0; //previous character
	/// ELEMENTLERI _ li şekilde değiştiren kısım
	for (d = c = 0; tmp[c]; ++c)
	{
		if (tmp[c] == SPACE)
		{
			if (pv != SPACE)
				start[d++] = '_';
			pv = SPACE;
		}
		else
		{
			pv = start[d++] = tmp[c];
		}
	}
	start[d] = '\0';
	for (int i = 0; start[i]; i++)
	{ // Lower case yapımı
		start[i] = tolower(start[i]);
	}

	

	parse(start, argv[4], arr1, &fldcnt);

	char den[sizeof(arr1)][200]; // attributes
	int attct = 0;
	for (int i = 0; i < sizeof(arr1); i++)
	{
		strcpy(den[i], arr1[i]);
		if (arr1[i][0] != 0)
			attct++;
	}

	fprintf(fpc, "<%s>", argv[1]);
	int len;
	char *pbuff;
	while (fgets(tmp, sizeof(tmp), in) != 0) /* read a record */
	{

		/////////////// ISLETIM SISTEMI AYIRMA
		if (strcmp(argv[6],"0")==0)
		{ // WİNDOWS

			tmp[strcspn(tmp, "\r\n")] = 0;
			
		}
		else if (strcmp(argv[6],"1")==0)
		{ // LİNUX  
       
			tmp[strcspn(tmp, "\n")] = 0;
		}
		else if (strcmp(argv[6],"2")==0){ //MACOS
			tmp[strcspn(tmp, "\r")] = 0;
			
		}
		char temp[sizeof(tmp) + 1];
		char *substring;
		char *substring2;

		int commact = 0;
		char *lastchar;

		for (int i = 0; i < sizeof(tmp); i++)
		{

			if (tmp[i] == 44)
			{
				commact++; // toplam virgül sayısı
				if (tmp[i + 3] == 0 && tmp[i + 1] == 0 && tmp[i - 1] != 44)
				{
					// eğer en son harf virgülse ve sonraki satır unutulmuş ise
					strcat(tmp, ",none");
					commact--;
				}
			}
			if (tmp[i + 2] == 0 && tmp[i + 1] == 0 && tmp[i] != 0 && tmp[i] != 44 && commact < attct - 1)
			{

				if(attct-commact==2){
					strcat(tmp, ",none");
					strcat(tmp, ",none");
					strcat(tmp, ",none");
					strcat(tmp, ",none");
					
				}
                 
				
				strcat(tmp, ",none");
              
				break;
			}
		}
		if (commact < attct - 1) // address sütununa değer girilmemişse 1 virgül eksik olacaktır
			for (int i = 0; i < attct - commact - 1; i++)
			{
				strcat(tmp, ",");
			}

		

		int flag = 0;
		for (int i = 0; i < sizeof(tmp); i++)
		{

			// tutulan stringi düzenli 0 lamak için
			substring = (char *)malloc(100 * sizeof(char));
			substring2 = (char *)malloc(100 * sizeof(char));

			if (tmp[i] == 44 && tmp[i + 1] == 44) // 2 peşpeşe virgül
			{

				// bu iki virgül en sonda ise 1 eksik parse edilecektir bunu önlemek için
				if (tmp[i + 2] == 0 && tmp[i + 4] == 0 && !flag)
				{
					flag = 1;
					if (commact == attct - 1)
						strcat(tmp, ",");
					if (commact == attct - 2)
						strcat(tmp, ",none,");
				}

				//
				// BAZI ELEMENTLER HER SATIRDA YOK OLMAYAN YERE BOSLUK ATAMAK ICIN
				strncpy(substring, tmp, i + 1);

				strcat(substring, "none");
				strcat(substring, tmp + i + 1);

				for (int i = 0; i < sizeof(tmp); i++)
				{
					
					tmp[i] = substring[i];
				}

				
			}
			
		}
            free(substring);
			free(substring2);
		int i = 0;
		recordcnt++;
		fprintf(fpc, "\n");

		fprintf(fpc, "     <row id=”%d”>\n", recordcnt);

		parse(tmp, argv[4], arr, &fldcnt); /* whack record into fields */

		for (i = 0; i < fldcnt; i++) // PRINT ISLEMLERI
		{

			// }

			fprintf(fpc, "\t<%s> %s </%s>\n", den[i], arr[i], den[i]);
			if (i + 1 == attct) // toplam sütun kadar yazdırdıysa break
				break;
		}

		fprintf(fpc, "     <row id=”%d”>\n", recordcnt);
	}
	fclose(in);
	fclose(fpc);
	fprintf(fpc, "<%s>", argv[1]);
	fprintf(fpc, "\n");

	return 0;
}
void app(char *s, char c)
{
	int len = strlen(s);
	s[len] = c;
	s[len + 1] = '\0';
}