/*
 * Copyright (C) 2006. PathScale Inc. All Rights Reserved.
 */

/*

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

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


