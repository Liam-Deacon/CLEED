/*
 * test_patt_ps_old.c
 *
 *  Created on: 22 Sep 2014
 *      Author: Liam
 */

    default:
      /**************** use old PostScript routines ***************************/
      /* write initial lines to output */
      ps_draw_init(out_stream, ifiles, title_str, pos_title, footnote, pos_footnote,
                   screen_flag, screen_thickness, fill_screen, ev_flag, eV, pos_ev,
                   clip_flag, vectors_flag);

      for (i = 0; i < drawing->n_files; i++)
      {

        /* create section header */
        fprintf(out_stream, "\n"
                "%%======================================================================="
                "\n%% BEGIN SECTION %i:\n"
                "%%======================================================================="
                "\n\n", i+1);
        if (fileno(in_stream) != fileno(stdin))
            in_stream = fopen(drawing->input_files[i], "r");

        if (in_stream == NULL)
        {
            fprintf(stderr, "error (patt_draw): cannot read file '%s'\n",
                    drawing->input_files[i]);
            exit(1);
        }

        /* read comment */


        ps_draw_title(in_stream, out_stream, i, drawing->n_files,
                      ps_color(color_flag, i, drawing->n_files, SPOT_GS, 0),
                      &i_line_offset);

        /* set draw routines for reciprocal lattice vectors */
        if ((drawing->show_vectors) && (do_gs_vectors[ii]))
        ps_draw_vectors(out_stream, -1, ii,
                     ps_color(color_flag, ii, ifiles, SPOT_GS, 0),
                     a1, a2, spot_gs, dummystr, SPOT_GS, vectors_str);

        fprintf(out_stream, "%% radius: %.1f\n%%\n", radius);

        /* print position of GS spots to output */
        fprintf(out_stream,"%s\n","% GS spots");
        fprintf(out_stream,"%s\n", ps_color(color_flag, ii, ifiles, SPOT_GS, 0));
        fprintf(out_stream," %.1f setlinewidth stroke\n", LINE_WIDTH);


        if (strlen(drawing->eV.label) > 0)
        {
          /* calculate radius of Ewald construction at given energy */
          lambda = sqrt(2*9.11*1.6018*atof(drawing->eV.label))/6.62;
          lambda /= 2*M_PI;
          radius = lambda;
        }

        for (i=0; i<GS_spot;i++)
        {
           /* draw spot */
           fprintf(out_stream,
             " %.1f %.1f %.1f 0 360 arc gsave %s %s grestore stroke\n",
            ix, iy, spot_gs, ps_color(color_flag, ii, ifiles, SPOT_GS, i_dom),
            fill_gs);

            /* draw label */
            sprintf(text, "%d,%d",i1,i2);
            fprintf(out_stream, "/Times-Roman findfont %.1f scalefont setfont \n",
                    spot_gs*2);
            fprintf(out_stream, "%.1f %.1f moveto (%s) show stroke\n",
                       ix - 1.5*spot_gs, iy + spot_gs + 4., text);
        }

        /* loop over domains */

        fprintf(out_stream, "%s\n","% SS spots");
        ps_set_linewidth(out_stream, LINE_WIDTH);

          /* Print comments to outputs */
          fprintf(out_stream,"%s Domain No. %d\n","%", i_dom+1);
          printf("%d:\tDet: %.1f\n",i_dom+1, det);
          printf("N = (%5.1f %5.1f)\n", N11, N12);
          printf("    (%5.1f %5.1f)\n", N21, N22);
          printf("M = (%5.1f %5.1f)\t(M^-1)t = (%5.2f %5.2f)",
                m11,  m12, aux1*m22,-aux1*m21);
          printf("\tb1* = (%6.2f)\tb2* = (%6.2f)\n", b1[0]/radius, b2[0]/radius);
          printf("    (%5.1f %5.1f)\t          (%5.2f %5.2f)",
                m21,  m22, -aux1*m12, aux1*m11);
          printf("\t      (%6.2f)\t      (%6.2f)\n", b1[1]/radius, b2[1]/radius);

          for each domain:



            for each spot:
                if (symbols)
                {
                 ps_draw_spot(out_stream, x, y, spot_ss, i_dom,
                    ps_color(color_flag, ii, ifiles, SPOT_SS, i_dom), fill_ss);
                }
                else
                {
                 ps_draw_spot(out_stream, x, y, spot_ss, 0,
                    ps_color(color_flag, ii, ifiles, SPOT_SS, i_dom), fill_ss);
                }

                       /* check if integral order spot */
        if ((index) && (do_ss_indices[ii]) &&
            ((fabs(ind_1*aux1) - (int)(fabs(ind_1*aux1)+0.1) > 0.05) ||
            (fabs(ind_2*aux1) - (int)(fabs(ind_2*aux1)+0.1) > 0.05) ))
        {
          fprintf(out_stream,"/Times-Roman findfont %.1f scalefont setfont \n",
                  spot_ss*2);

          if (!commensurate)
          {
            /* incommensurate */
           fprintf(out_stream, "%.1f %.1f moveto (%.2f,%.2f) show stroke\n",
                   x - 4*spot_ss, y + spot_ss + 4., ind_1*aux1, ind_2*aux1);
           printf("incommensurate\n");
           fprintf(out_stream, "%s Mult scatt. \n", "%");
          }
          else
          {
            /* commensurate (fractions) */

            ind_1_int = (int) (ind_1*1.01);
            ind_2_int = (int) (ind_2*1.01);
            det_int =   (int) (det*1.01);

            switch (nice_frac(&ind_1_int, &det_int))
            {
              case(0):
              {
                sprintf(text, "0, ");
                break;
              }

              case(1):
              {
                sprintf(text, "%d, ", ind_1_int);
                break;
              }

              case(2):
              {
                sprintf(text, "%d/%d, ", ind_1_int, det_int);
                break;
              }
            }

            det_int = (int) (det*1.01);
            switch (nice_frac(&ind_2_int, &det_int))
            {
              case(0):
              {
                sprintf(text2, "0");
                break;
              }

              case(1):
              {
                sprintf(text2, "%d", ind_2_int);
                break;
              }

              case(2):
              {
                sprintf(text2, "%d/%d", ind_2_int, det_int);
                break;
              }
            }

          fprintf(out_stream,"%.1f %.1f moveto (%s%s) show stroke\n",
                  x - 3*spot_ss, y + spot_ss + 2., text, text2);
          }
        }

            /* print domain lattice vectors */
            if ((vectors_flag) && (do_ss_vectors[ii][i_dom]))
            {
                ps_draw_vectors(out_stream, i_dom, ii,
                     ps_color(color_flag, ii, ifiles, SPOT_SS, i_dom),
                     b1, b2, spot_ss, dummystr, SPOT_SS, vectors_str);
            }

        /* final drawing strokes */
          ps_draw_finalize(out_stream, gun_flag, gun_thickness, gun_radians,
            screen_thickness, screen_flag, vectors_flag, vectors_str,
            ifiles, ev_flag, eV, title_str);
          fclose(out_stream);

        /* clean up */
        if (fileno(in_stream) != fileno(stdin))
          fclose(in_stream);
      }
      break;
  }

}
