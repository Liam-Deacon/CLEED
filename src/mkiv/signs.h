
/**************************************************************************<
    >
    >         File Name: signs.h
    >
>**************************************************************************/

/**************************************************************************/
/* #define    U     200  */         /* pixel values to plot the numbers with */
#define    U       1          /* pixel values to plot the numbers with */
#define    O       0
#define    L       0
/**************************************************************************/

int a[34][77] = {
	      {L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,U,U,U,U,U,L,
	       L,U,U,U,U,U,L,   /* '-' */
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L},

	      {L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,   /* '.' */
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,U,L,L,L},

	      {L,L,L,L,U,U,L,
	       L,L,L,L,U,U,L,
	       L,L,L,L,U,U,L,
	       L,L,L,U,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,U,U,L,L,L,   /* '/' */
	       L,L,U,U,L,L,L,
	       L,U,U,L,L,L,L,
	       L,U,U,L,L,L,L,
	       L,U,U,L,L,L,L,
	       L,L,L,L,L,L,L},

              {L,L,U,U,U,L,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,   /* '0' */
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,L,L},

              {L,L,L,L,L,U,L,
	       L,L,L,L,U,U,L,
	       L,L,L,U,L,U,L,
	       L,L,U,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,L,L,L,U,L,   /* '1' */
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
               L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L},

              {L,L,U,U,U,L,L,
	       L,U,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,   /* '2' */
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,U,U,U,U,L},

              {L,L,U,U,U,L,L,
    	       L,U,L,L,L,U,L,
               L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,U,U,U,L,L,   /* '3' */
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,L,L},

              {L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,U,L,L,
	       L,U,L,L,U,L,L,   /* '4' */
	       L,U,U,U,U,U,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L},

              {L,U,U,U,U,U,L,
 	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,U,U,U,L,L,   /* '5' */
               L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,L,L},

              {L,L,U,U,U,L,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,U,U,U,L,L,   /* '6' */
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,L,L},

              {L,U,U,U,U,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,   /* '7' */
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,L,L,L,L,
	       L,U,L,L,L,L,L,
	       L,U,L,L,L,L,L},

              {L,L,U,U,U,L,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,L,L,   /* '8' */
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,L,L},

              {L,L,U,U,U,L,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,U,L,   /* '9' */
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,L,L,L,L,U,L,
	       L,U,L,L,L,U,L,
	       L,L,U,U,U,L,L},

              {L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,U,L,L,L,   /* ':' */
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,U,L,L,L},

              {L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,L,L,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,U,L,L,L,   /* ';' */
	       L,L,L,L,L,L,L,
	       L,L,L,U,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L},

	      {L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,U,U,U,U,U,L,   /* '1/2' */
	       L,L,U,U,L,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,U,U,L,L},

              {L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,U,U,U,U,U,L,   /* '1/3' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,U,U,U,L,L},

              {L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
               L,L,U,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,L,U,L,L,
	       L,U,U,U,U,U,L,   /* '1/6' */
	       L,L,U,U,U,L,L,
               L,L,U,L,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L},

              {L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,L,L,U,L,L,
	       L,U,U,U,U,U,L,   /* '1/7' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,L,L,L,L},

              {L,L,U,U,L,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '2/3' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,U,U,U,L,L},

              {L,L,U,U,U,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,U,L,L,L,
	       L,L,L,L,U,L,L,
	       L,L,U,U,L,L,L,
	       L,U,U,U,U,U,L,   /* '5/6' */
	       L,L,U,U,U,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L},

              {U,O,O,O,O,O,U,
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U,   /* 'I' */
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U,
	       U,O,O,O,O,O,U},

              {O,U,U,U,U,U,O,
	       O,U,U,U,U,U,O,
	       O,U,U,U,U,U,O,
	       U,O,U,U,U,O,U,
	       U,O,U,U,U,O,U,
	       U,O,U,U,U,O,U,   /* 'V' */
               U,U,O,U,O,U,U,
	       U,U,O,U,O,U,U,
	       U,U,O,U,O,U,U,
	       U,U,U,O,U,U,U,
	       U,U,U,O,U,U,U},
		
              {L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,L,L,U,L,L,
	       L,U,U,U,U,U,L,   /* '1/8' */
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
               L,L,U,U,U,L,L},
		
              {L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '3/8' */
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
               L,L,U,U,U,L,L},
		
              {L,L,U,U,U,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '5/8' */
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
               L,L,U,U,U,L,L},
		
              {L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,L,L,L,L,
	       L,U,U,U,U,U,L,   /* '7/8' */
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
               L,L,U,U,U,L,L},
		
              {L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '2/7' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,L,L,L,L},
		
              {L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '3/7' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,L,L,L,L},
		
              {L,L,U,L,L,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,L,L,U,L,L,
	       L,U,U,U,U,U,L,   /* '4/7' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,L,L,L,L},
		
              {L,L,U,U,U,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '5/7' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,L,L,L,L},
		
              {L,L,U,U,U,L,L,
	       L,L,U,L,L,L,L,
	       L,L,U,U,U,L,L,
	       L,L,U,L,U,L,L,
               L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '6/7' */
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,L,L,L,
	       L,L,U,L,L,L,L,
               L,L,U,L,L,L,L},
		
              {L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,U,L,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,L,L,U,L,L,
	       L,U,U,U,U,U,L,   /* '1/4' */
	       L,L,U,L,L,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,L,L,U,L,L},

              {L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
	       L,L,L,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,U,U,U,L,L,
	       L,U,U,U,U,U,L,   /* '3/4' */
	       L,L,U,L,L,L,L,
	       L,L,U,L,U,L,L,
	       L,L,U,U,U,L,L,
	       L,L,L,L,U,L,L,
               L,L,L,L,U,L,L}

};
/**************************************************************************/
