/*
 * Copyright (C) 2010. PathScale Inc. All Rights Reserved.
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

/*
# Print routines to return constant name for associated value.
# The input is dwarf.h
# For each set of names with a common prefix, we create a routine
# to return the name given the value.
# Also print header file that gives prototypes of routines.
# To handle cases where there are multiple names for a single
# value (DW_AT_* has some due to ambiguities in the DWARF2 spec)
# we take the first of a given value as the definitive name.
# TAGs, Attributes, etc are given distinct checks.
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

bool contains(vector<string> v, string str)
{
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i] == str) return true;
    }
    return false;
}

int main(int argc, char * argv[])
{
	int options = 0;
	int skipit = 0;
	int second_underscore;
	FILE *outfile, *header;
	string prefix = "foo";
	string prefix_id = "foo";
	int prefix_len = prefix.length();
	string dw_prefix = "DW_";
	int dw_len = dw_prefix.length();
	int start_routine = 0;
    vector<string> dup_arr(1);
    string post_dw, main_part;

    if (!init(argv[1]))
    {
        fprintf(stderr, "Error: could not open input file\n");
        return -1;
    }

    outfile = fopen(argv[2], "w");
    if (!outfile)
    {
        fprintf(stderr, "Error: could not open '%s'\n", argv[2]);
        return -1;
    }

    header = fopen(argv[3], "w");
    if (!header)
    {
        fprintf(stderr, "Error: could not open '%s'\n", argv[3]);
        return -1;
    }

	fprintf(outfile, "#include \"globals.h\"\n\n");
	fprintf(outfile, "#include \"makename.h\"\n\n");

	fprintf(header, "/* automatically generated routines */\n");
	dup_arr.empty();

    while ( next() )
    {
        /* Sanity check */
        if (NF < 1) continue;

        if (skipit && field[1] == "#endif") {
            skipit = 0;
            continue;
        }
        if (field[1] == "#if 0" || skipit) {
            // if 0, skip to endif
            skipit = 1;
            continue;
        }
        if (field[1] == "#define") {
            if (field[2].substr(0,prefix_len) != prefix) {
                // new prefix
                if (field[2].substr(0,dw_len) != dw_prefix) {
                    // skip
                    continue;
                } else if (field[2].substr(0,dw_len+3) == "DW_CFA") {
                    // skip, cause numbers conflict
                    // (have both high-order and low-order bits)
                    continue;
                } else {
                    // New prefix, empty the dup_arr
                    dup_arr.empty();
                    if (start_routine) {
                        // end routine
                        fprintf(outfile, "\tdefault:\n");
                        fprintf(outfile, "\t\t{ \n");
                        fprintf(outfile, "\t\t    char buf[100]; \n");
                        fprintf(outfile, "\t\t    char *n; \n");
                        fprintf(outfile, "\t\t    sprintf(buf,\"<Unknown %s value 0x%%x>\",(int)val);\n", prefix_id.c_str());
                        fprintf(outfile, "\t\t fprintf(stderr,\"%s of %%d (0x%%x) is unknown to dwarfdump. \" \n ", prefix_id.c_str());
                        fprintf(outfile, "\t\t \"Continuing. \\n\",(int)val,(int)val );  \n");
                        fprintf(outfile, "\t\t    n = makename(buf);\n");
                        fprintf(outfile, "\t\t    return n; \n");
                        fprintf(outfile, "\t\t} \n");
                        fprintf(outfile, "\t}\n");
                        fprintf(outfile, "/*NOTREACHED*/\n");
                        fprintf(outfile, "}\n\n");
                    }
                    start_routine = 1;
                    post_dw = field[2].substr(dw_len, field[2].length());
                    second_underscore = post_dw.find_first_of("_");
                    prefix = field[2].substr(0,second_underscore+dw_len);
                    prefix_len = prefix.length();
                    // prefix id is unique part after DW_, e.g. LANG
                    prefix_id = prefix.substr(dw_len,prefix_len-dw_len);
                    fprintf(outfile, "/* ARGSUSED */\n");
                    fprintf(outfile, "extern string\n");
                    fprintf(outfile, "get_%s_name (Dwarf_Debug dbg, Dwarf_Half val)\n", prefix_id.c_str());
                    fprintf(outfile, "{\n");
                    fprintf(outfile, "\tswitch (val) {\n");
                    fprintf(header, "extern string get_%s_name (Dwarf_Debug dbg, Dwarf_Half val);\n\n", prefix_id.c_str());
                }
            }
            if (field[2].substr(0,prefix_len) == prefix) {
                if (field[2].substr(0,dw_len+8) == "DW_CHILDREN"
                    || field[2].substr(0,dw_len+8) == "DW_children"
                    || field[2].substr(0,dw_len+4) == "DW_ADDR") {
                    main_part = field[2].substr(dw_len, field[2].length());
                }
                else {
                    post_dw = field[2].substr(dw_len, field[2].length());
                    second_underscore = post_dw.find_first_of("_");
                    main_part = field[2].substr(dw_len+second_underscore, field[2].length());
                }
                if (!contains(dup_arr, field[3])) {
                  // Take first of those with identical value,
                  // ignore others.
                  dup_arr.push_back(field[3]);
                  fprintf(outfile, "\tcase %s:\n", field[2].c_str());
                  fprintf(outfile, "\t\tif (ellipsis)\n");
                  fprintf(outfile, "\t\t\treturn \"%s\";\n", main_part.c_str());
                  fprintf(outfile, "\t\telse\n");
                  fprintf(outfile, "\t\t\treturn \"%s\";\n", field[2].c_str());
                }
            }
        }
    }

	if (start_routine) {
		fprintf(outfile, "\tdefault:\n");
		fprintf(outfile, "\t\tprint_error(dbg, \"get_%s_name unexpected value\",DW_DLV_OK, err);\n", prefix_id.c_str());
		fprintf(outfile, "\t}\n");
		fprintf(outfile, "\t return \"unknown-name-dwarf-error\";\n");
		fprintf(outfile, "}\n\n");
	}

    fclose(header);
    fclose(outfile);

    return 0;
}



