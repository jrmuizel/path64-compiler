/*!
 *
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 *
 *
 *
 *  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it would be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement 
 *  or the like.  Any license provided herein, whether implied or 
 *  otherwise, applies only to this software file.  Patent licenses, if 
 *  any, provided herein do not apply to combinations of this program with 
 *  other software, or any other product whatsoever.  
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write the Free Software Foundation, Inc., 59
 *  Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 *  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
 *  Mountain View, CA 94043, or:
 *
 *  http://www.sgi.com
 *
 *  For further information regarding this notice, see:
 *
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan
 *
 *
 * sort the options list so the read_options can group things properly
 */


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>

using namespace std;

vector<string> field(1);

int NF;
ifstream infile;

static const size_t npos = (size_t)-1;

int next()
{
    string line, str;
    size_t start, end;

    /* clear old vector */
    field.clear();

    /* Read a line into field[0] */
    if (!getline(infile, line)) return 0;
    field.push_back(line);

    NF = 0;
    start = 0;
    end = 0;
    while (1)
    {
        /* Find start end end of the next field within the line */
        start = field[0].find_first_not_of(" \t", end);
        end = field[0].find_first_of(" \t", start);

        /* Bail out, if nothing is found */
        if (start == npos && end == npos) break;

        /* Add the new string to the vector */
        field.push_back(field[0].substr(start, end-start));
        NF++;
    }

    return 1;
}

bool init(char * infilename)
{
    NF = 0;
    infile.open(infilename);
    return infile.good();
}


int main(int argc, char * argv[])
{
	int options = 0;
	FILE *optfile, *comfile;

    init(argv[1]);

    optfile = fopen("tmp.options", "w");
    if (!optfile)
    {
        fprintf(stderr, "Error: could not open tmp.options\n");
        return -1;
    }

    comfile = fopen("tmp.options.combo", "w");
    if (!comfile)
    {
        fprintf(stderr, "Error: could not open tmp.options.combo\n");
        fclose(optfile);
        return -1;
    }

    while ( next() )
    {
        /* Sanity check */
        if (NF < 1) continue;

        if (field[1] == "%%%" && field[2] == "OPTIONS")
        {
            options = 1;
            continue;
        }
        else if (field[1] == "%%%" && field[2] == "COMBINATIONS")
        {
            options = 0;
            continue;
        }

        if (options)
        {
            /* combine -name lines with help msg, onto one line,
             * so sorting is easier.
             * add | character between implies and help msg. */
            char firstchar = field[1][0];
            char lastchar = field[1][field[1].length() - 1];

            if (firstchar == '-' || firstchar == 'I')
            {
                fprintf(optfile, "%s | ", field[0].c_str());
            }

            /* KEY:  Support double-quoting the option name. */
            else if (firstchar == '\"' && lastchar == '\"' && NF > 1)
            {
                /* Print out first record without double quotes. */
                size_t length = field[1].length();
                fprintf(optfile, "%s", field[1].substr(2, length - 2).c_str());

                /* Print out rest of records. */
                for (int i = 2; i <= NF; i++)
                {
                    fprintf(optfile, " %s", field[i].c_str());
                }

                fprintf(optfile, " | ");
            }
            else if (firstchar == '\"')
            {
                fprintf(optfile, "%s\n", field[0].c_str());
            }
        }
        else
        {
            fprintf(comfile, "%s\n", field[0].c_str());
        }
    }

    fclose(optfile);
    fclose(comfile);

    return 0;
}


