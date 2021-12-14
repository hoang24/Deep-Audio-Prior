#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define DEFAULT_FIL "INPUT.DAT"  /*input default file name*/
#define MAX_LINES 4	/*max number of lines in the input file*/
#define MIN_LINES 3	/*min number of lines in the input file*/
#define MAXLINE 1000    /*max line length*/
#define SIZE 25		/*larger of N_ECCB, N_TO or N_CB plus 4*/
#define N_TO 18 	/*number of critical bands*/
#define N_CB 21 	/*number of third octaves*/
#define N_ECCB 17 	/*number of equally contributiong critical bands*/
#define N_OCT 6         /*number of octaves*/
#define YES 1
#define NO 0
#define PEAK 15
#define S_SN 24         /*self-speech masking, to eliminate it: S_SN 100 */
#define SOAL 62.35	/*overall level of standard speech at normal effort*/

void usage(const char *program_name);
void no_fil(const char *program_name, const char *p);
void oct(double *p_limit, double *p_x, double *p_u, double *p_i_avg);
void cb(double *p_limit, double *p_x, double *p_u, double *p_i_avg);
void eccb(double *p_limit, double *p_x, double *p_u, double *p_i_avg);
void to(double *p_limit, double *p_x, double *p_u, double *p_i_avg);

int
main(int argc, char *argv[]) /*BEGIN MAIN*/
{
  FILE *fp;
  char line[MAXLINE];
  double dummy, disturbance, audibility, sii_avg, sii_other;
  int n;        /*number of bands*/
  int i, k;     /*these are just indexes*/
  int flag_other_if, flag_to, flag_cb, flag_eccb, flag_oct, n_lines, n_tot;
  double high_l[SIZE], alpha[SIZE], beta[SIZE];
  double e_prime[SIZE], n_prime[SIZE], t_prime[SIZE], x_prime[SIZE];
  double limit[SIZE], x[SIZE], i_avg[SIZE], u[SIZE], z[SIZE], i_other[SIZE];

/*OPENING THE INPUT FILE */

  if (argc == 1) 	/*input file is not specified as an argument*/
    {

      fp = fopen(DEFAULT_FIL,"r");
      if(fp == NULL)
  	 {
	    no_fil(argv[0],DEFAULT_FIL);
	    exit(1);
  	 }
    }
  else if (argc == 2)   /*input file name given as argument*/
    {
      fp = fopen(argv[1],"r");
      if(fp == NULL)
	{
	  no_fil(argv[0],argv[1]);
	  exit(1);
	}
    }
  else
    {
       usage(argv[0]);
       exit(1);
    }

/*OBTAINING THE NUMBER OF LINES IN THE INPUT FILE*/

    n_lines=0;
    while(fgets(line, MAXLINE, fp) != NULL)
    {
      if(sscanf(line,"%lf",&dummy) != EOF)
	n_lines++;
    }
    if(n_lines < MIN_LINES || n_lines > MAX_LINES)
    {
      fprintf(stderr,"\n%s: Too many or too few lines in the input file!\n",argv[0]);
      exit(1);
    }

/*DECIDING WHETHER AN ALTERNATIVE IMPORTANCE FUNCTION WILL BE USED*/

    flag_other_if = NO;
    if(n_lines == MAX_LINES)
	flag_other_if = YES;

/*OBTAINING THE NUMBER OF ENTRIES IN THE INPUT FILE*/

    rewind(fp);
    n_tot = 0;
    while((i = fscanf(fp,"%lf",&dummy)) != EOF)
    {
      if(i == 1)
	n_tot++;
      else
      {
	fprintf(stderr,"\n%s: Typing error in the input file!\n",argv[0]);
	exit(1);
      }
    }

/*DETERMINING THE DESIRED SII PROCEDURE*/

    flag_to = NO;
    flag_cb = NO;
    flag_eccb = NO;
    flag_oct = NO;
    n = (int)(n_tot/n_lines);
    if(n == N_TO)
	flag_to = YES;
    else if(n == N_CB)
	flag_cb = YES;
    else if(n == N_ECCB)
	flag_eccb = YES;
    else if(n == N_OCT)
	flag_oct = YES;
    else
    {
	fprintf(stderr,"\n%s: Number of entries in the input file is incorrect!\n",argv[0]);
	exit(1);
    }

/*READING INPUT DATA*/

	rewind(fp);
	for(i=0;i<n;i++)
		fscanf(fp,"%lf",&e_prime[i]);
	for(i=0;i<n;i++)
		fscanf(fp,"%lf",&n_prime[i]);
	for(i=0;i<n;i++)
		fscanf(fp,"%lf",&t_prime[i]);
	if(flag_other_if == YES)
	{
		for(i=0;i<n;i++)
			fscanf(fp,"%lf",&i_other[i]);
	}
	fclose(fp);

/*INITIALIZING NON-USER DEFINED DATA ARRAYS FOR THE DESIRED SII PROCEDURE*/

	if(flag_to == YES)
		to(&limit[0],&x[0],&u[0],&i_avg[0]);
	else if(flag_cb == YES)
		cb(&limit[0],&x[0],&u[0],&i_avg[0]);
	else if(flag_eccb == YES)
		eccb(&limit[0],&x[0],&u[0],&i_avg[0]);
	else if(flag_oct == YES)
		oct(&limit[0],&x[0],&u[0],&i_avg[0]);
	else
	{
		fprintf(stderr,"\n %s: error in the program!!!\n",argv[0]);
	}

/*EQUIVALENT INTERNAL NOISE*/

  for(i=0;i<n;i++)
    x_prime[i] = x[i] + t_prime[i];

/*  SELECTING EQUIVALENT NOISE OR SELF SPEECH MASKING */

      for(i=0;i<n;i++)
      {
	  if(n_prime[i] < (e_prime[i] - S_SN))
	    beta[i] = e_prime[i] - S_SN;
	  else
	    beta[i] = n_prime[i];
      }

/*SPREAD OF MASKING*/
/*For one third octave procedure the variable named limit is in fact the center frequency */

   if(flag_oct == YES)
   {
	  for(i=0;i<n;i++)
	  z[i] = n_prime[i];	      
   }
   else if(flag_to == YES)
   {
	  z[0] = beta[0];
	  for(i=0;i<n;i++)
	  {
	    alpha[i] = -80.0 + 0.6*(beta[i] + 10.*log10(limit[i]) - 6.353);
	  }
	  for(i=1;i<n;i++)
	  {
	    dummy = pow(10.,(n_prime[i]/10.));
	    for(k=0;k<i;k++)
	       dummy = dummy + pow(10.,(beta[k]+3.32*alpha[k]*log10(0.89*limit[i]/limit[k]))/10.);
	    z[i] = 10.*log10(dummy);
	  }

   }
   else
   {
	  z[0] = beta[0];
	  for(i=0;i<n;i++)
	  {
	    alpha[i] = -80.0 + 0.6*(beta[i] + 10.*log10(limit[i+1] - limit[i]));
	  }
	  for(i=1;i<n;i++)
	  {
	    dummy = pow(10.,(n_prime[i]/10.));
	    for(k=0;k<i;k++)
	       dummy = dummy + pow(10.,(beta[k]+3.32*alpha[k]*log10(sqrt(limit[i+1]*limit[i])/limit[k+1]))/10.);
	    z[i] = 10.*log10(dummy);
	  }
   }

/*SPEECH LEVEL DISTORTION FACTOR */

  for(i=0;i<n;i++)
    {
	high_l[i] = 1 - (e_prime[i] - (u[i] + 72.35 - SOAL))/160.;
	if(high_l[i] > 1.)
	  high_l[i] = 1.;
	else if (high_l[i] < 0.)
	  high_l[i] = 0.;
	else
	  ;
    }

/*BASIC SII CALCULATIONS*/

  sii_avg = 0;
  sii_other = 0;
  for(i=0; i<n; i++)
  {
    if(z[i] >= x_prime[i])
      disturbance = z[i];
    else
      disturbance = x_prime[i];
    audibility = (e_prime[i] + PEAK) - disturbance;
    if(audibility > 30.)
      audibility = 30.;
    else if (audibility < 0)
      audibility = 0.;
    else
      ;
    sii_avg = sii_avg + (audibility/30.)*i_avg[i]*high_l[i];
    if(flag_other_if == YES)
    sii_other = sii_other + (audibility/30.)*i_other[i]*high_l[i];
  }

/*PRINTING RESULTS ON THE STANDARD OUTPUT*/

	if(flag_other_if == YES)
		printf("%7.3f",sii_other);
	else
		printf("%7.3f",sii_avg);

  return 0;
} /*END_MAIN*/

/* ...................................................*/

/*AUXILIARY FUNCTIONS*/

/*-----correct syntax for calling the program-------*/

void usage(const char *program_name)
  {
    fprintf(stderr,"%s: too many parameters in your command line!",program_name);
    fprintf(stderr," \n   correct syntax: sii [input file name]\n");
    fprintf(stderr,"   example:  sii new.dat\n");
  }

/*-------input file does not exist-------*/

void no_fil(const char *program_name, const char *p)
  {
    fprintf(stderr,"%s: cannot open file %s\n",program_name,p);
  }

/*----------data for octaves----------*/

void oct(double *p_limit, double *p_x, double *p_u, double *p_i_avg)
{
    int i;
    double limit[N_OCT+1] = {177,354,707,1414,2828,5657,11314};
    double x[N_OCT] = {-3.9,-9.7,-12.5,-17.7,-25.9,-7.1};
    double u[N_OCT] = {34.75,34.27,25.01,17.32,9.33,1.13};
    double i_avg[N_OCT] = {0.0617,0.1671,0.2373,0.2648,0.2142,0.0549};

    for(i=0;i<N_OCT;i++)
    {
	*(p_limit+i) = limit[i];
	*(p_x+i) = x[i];
	*(p_u+i) = u[i];
	*(p_i_avg+i) = i_avg[i];
    }
    *(p_limit+N_OCT) = limit[N_OCT];
}

/*----------data for third octaves----------*/

void to(double *p_limit, double *p_x, double *p_u, double *p_i_avg)
/*For one third octave procedure the variable named limit is in fact the center frequency */
{ 
    int i;
    double limit[N_TO] = {160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6300,8000};
    double x[N_TO] = {0.6,-1.7,-3.9,-6.1,-8.2,-9.7,-10.8,-11.9,-12.5,-13.5,-15.4,-17.7,-21.2,-24.2,-25.9,-23.6,-15.8,-7.1};
    double u[N_TO] = {32.41,34.48,34.75,33.98,34.59,34.27,32.06,28.30,25.01,23,20.15,17.32,13.18,11.55,9.33,5.31,2.59,1.13};
    double i_avg[N_TO] = {0.0083,0.0095,0.0150,0.0289,0.0440,0.0578,0.0653,0.0711,0.0818,0.0844,0.0882,0.0898,0.0868,0.0844,0.0771,0.0527,0.0364,0.0185};

    for(i=0;i<N_TO;i++)
    {
	*(p_limit+i) = limit[i];
	*(p_x+i) = x[i];
	*(p_u+i) = u[i];
	*(p_i_avg+i) = i_avg[i];
    }
}

/*----------data for critical bands----------*/

void cb(double *p_limit, double *p_x, double *p_u, double *p_i_avg)
{
    int i;
    double limit[N_CB+1] = {100,200,300,400,510,630,770,920,1080,1270,1480,1720,2000,2320,2700,3150,3700,4400,5300,6400,7700,9500};
    double i_avg[N_CB] = {0.0103,0.0261,0.0419,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0577,0.0460,0.0343,0.0226,0.011};
    double u[N_CB] = {31.44,34.75,34.14,34.58,33.17,30.64,27.59,25.01,23.52,22.28,20.15,18.29,16.37,13.80,12.21,11.09,9.33,5.84,3.47,1.78,-0.14};
    double x[N_CB] = {1.5,-3.9,-7.2,-8.9,-10.3,-11.4,-12,-12.5,-13.2,-14,-15.4,-16.9,-18.8,-21.2,-23.2,-24.9,-25.9,-24.2,-19,-11.7,-6};

    for(i=0;i<N_CB;i++)
    {
	*(p_limit+i) = limit[i];
	*(p_x+i) = x[i];
	*(p_u+i) = u[i];
	*(p_i_avg+i) = i_avg[i];
    }
    *(p_limit+N_CB) = limit[N_CB];
}

/*----------data for equally-contributing critical bands----------*/

void eccb(double *p_limit, double *p_x, double *p_u, double *p_i_avg)
{
    int i;
    double limit[N_ECCB+1] = {300,400,510,630,770,920,1080,1270,1480,1720,2000,2320,2700,3150,3700,4400,5300,6400};
    double i_avg[N_ECCB] = {0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588,0.0588};
    double u[N_ECCB] = {34.14,34.58,33.17,30.64,27.59,25.01,23.52,22.28,20.15,18.29,16.37,13.80,12.21,11.09,9.33,5.84,3.47};
    double x[N_ECCB] = {-7.2,-8.9,-10.3,-11.4,-12,-12.5,-13.2,-14,-15.4,-16.9,-18.8,-21.2,-23.2,-24.9,-25.9,-24.2,-19.0};

    for(i=0;i<N_ECCB;i++)
    {
	*(p_limit+i) = limit[i];
	*(p_x+i) = x[i];
	*(p_u+i) = u[i];
	*(p_i_avg+i) = i_avg[i];
    }
    *(p_limit+N_ECCB) = limit[N_ECCB];

}
