/*========================================================================*/

#include <stdio.h>
#include <strings.h>
#include "patt.h"

/* draw reciprocal space vectors to PS file */
int ps_draw_vectors(FILE *out_stream, int i_dom, int ii, char *color, 
        float *a1, float *a2, float spot_radius, char dummystr[][STRSZ], 
        int spot, char *vectors_str)
{
  int compare[2];
  char dom_str[STRSZ];
  char v_type;
  
  strcpy(dom_str, "\0");
  compare[0] = 0;
  compare[1] = 1;
  v_type = 'a';
  
  if (spot == SPOT_SS)
  {
	sprintf(dom_str, "_%idom", i_dom);
	compare[0] += 3;
	compare[1] += 3;
	v_type = 'b';
  }

  /* do comparisons with previous & write new vectors if neccessary */
  sprintf(dummystr[compare[1]],
	"%.1f %.1f %.1f %.1f", a1[0], a1[1], a2[0], a2[1]);
  if (strcmp(dummystr[compare[0]], dummystr[compare[1]])) 
  {
	
    fprintf(out_stream,"%%\n%% Lattice vectors\n");
  
    fprintf(out_stream, "/%c1_%i%s {\ngsave %s\n", 
        v_type, ii, dom_str, color);
    fprintf(out_stream, 
      "0 0 %.1f %.1f %.1f arrow\n", a1[0], a1[1], spot_radius*0.5);
		
	if (v_type == 'a')
	  sprintf(dummystr[2], "%i", 1);	
	else
	  sprintf(dummystr[2], "%i,%i", 1, i_dom);
	  
    fprintf(out_stream,
      "0 0 %.1f %.1f (%c) (%s) %.1f %.1f rotatedlabel\n",
      a1[0], a1[1], v_type, dummystr[2], spot_radius*2.5, spot_radius*1.25);
    fprintf(out_stream, "grestore\n} def\n");
  
    fprintf(out_stream, "/%c2_%i%s {\ngsave %s\n", 
        v_type, ii, dom_str, color);
    fprintf(out_stream,
      "0 0 %.1f %.1f %.1f arrow\n", a2[0], a2[1], spot_radius*0.5);
	
	if (v_type == 'a')
	  sprintf(dummystr[2], "%i", 2);	
	else
	  sprintf(dummystr[2], "%i,%i", 2, i_dom);

    fprintf(out_stream,
      "0 0 %.1f %.1f (%c) (%s) %.1f %.1f rotatedlabel\n",
      a2[0], a2[1], v_type, dummystr[2], spot_radius*2.5, spot_radius*1.25);
    fprintf(out_stream, "grestore\n} def\n");
  
    sprintf(dummystr[2], "\n%c1_%i%s %c2_%i%s", 
	  v_type, ii, dom_str, v_type, ii, dom_str);
	
	strcat(vectors_str, dummystr[2]);
	
  }

  sprintf(dummystr[compare[0]], "%.1f %.1f %.1f %.1f", 
    a1[0], a1[1], a2[0], a2[1]);

  return 0;
}





