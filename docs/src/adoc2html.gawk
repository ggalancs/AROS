# $Id$
#
# This script converts autodoc sources into HTML files. Just call it with the
# names of all source to convert. It will create cross references as
# neccessary.
#
BEGIN {
    stderr="/dev/stderr";

    # Here is a list of special items in the sources which should be
    # replaced by a link
    special_item["NEWLIST()"]="<A HREF=\"../srcs/include/exec/lists.h\">NEWLIST()</A>";

    # Long names for directories
    LIBS["clib"]="ANSI C linklib";
    LIBS["alib"]="amiga.lib";
    LIBS["devs"]="Devices";
    LIBS["aros"]="AROS";
    LIBS["arossupport"]="AROS Support";
    LIBS["intuition"]="Intuition";
    LIBS["exec"]="Exec";
    LIBS["boopsi"]="BOOPSI";

    file=ARGV[1];
    cvslog=ARGV[2];

    bn=basename(file);

    # Clear the name of the resulting output. This will be set
    # to the correct name when I write the first line into the
    # file. It will be checked, when I have to write the footer to
    # avoid to have a file which contains *only* a footer.
    out="";

    # mode can be head, field or footer. head means that I'm currently
    # looking for the headers. field means that I found the header
    # and that I'm currently processing the fields in the header
    # (eg. NAME, FUNCTION, RETURNS). footer means that I have
    # processed the header.
    mode="head";

    # Here I count the lines while I read them. This is used below
    # the the EXAMPLES section.
    fnr=0;

    # Read the file line by line
    while ((getline < file) > 0)
    {
	fnr ++; # Count the lines.

	if (mode=="head") # Looking for the header
	{
	    if (match($0,/^.\*\*\*\*+/)) # Found it ?
	    {
		fname=""; # No function name yet

		# Distinguish to which part of AROS this function
		# belongs. To achieve this, I examine the filename.
		# It contains the name of the part as the last
		# directory in the path.
		lib=file;
		gsub(/\/[^/]+$/,"",lib); # Strip off the filename
		gsub(/^.*\//,"",lib); # Strip off all leading dirs

		# The filename of the resulting HTML file
		out=CURDIR "/../html/autodocs/" bn ".html";
		#out="/dev/stdout";

		location=0; # The field LOCATION hasn't been read yet

		# Emit the header
		print "<HTML><HEAD>\n<TITLE>AROS - The Amiga Replacement OS - AutoDocs</TITLE>\n</HEAD>\n<BODY>\n" > out;
		print "<CENTER><P>(C) 1996 AROS - The Amiga Replacement OS</P></CENTER>\n<P><HR></P>\n\n" >> out;

		# Next mode
		mode="field";
		lastfield="";
	    }
	}
	else if (mode=="field") # Reading the header
	{
	    # This might be the name of a new field.
	    newfield=$1;

	    # If we found something, make sure it's not a comment
	    # If it is a comment, read the next word. This must be
	    # the fields' name.
	    if (newfield!="")
	    {
		if (newfield=="#" || newfield=="/*")
		    newfield=$2;
	    }

	    # Check if this is a field. We do this by comparing
	    # the name of the field against all known field names.
	    if (newfield=="NAME" || newfield=="SYNOPSIS" || newfield=="LOCATION" ||
		newfield=="FUNCTION" || newfield=="INPUTS" || newfield=="RESULT" ||
		newfield=="NOTES" || newfield=="EXAMPLE" || newfield=="BUGS" ||
		newfield=="SEE" || newfield=="INTERNALS" || newfield=="HISTORY")
	    {
		# The last field was an example ? Then it might be
		# neccessary to close a pending <PRE>.
		if (lastfield=="EXAMPLE")
		{
		    if (example_is=="here" || example_is=="example")
			printf ("</PRE>") >> out;
		}
		else if (lastfield=="INPUTS" || lastfield=="HISTORY")
		{
		    # If the last field as a list, then we must
		    # close that, too.
		    print "</DL>\n" >> out;
		}

		# To get a nice format, we stuff every field in a HTML
		# list.
		if (lastfield!="")
		    print "</DL>\n" >> out;

		mode="field"; # Obsolete ?
		# Store the name of the field so we know how to
		# terminate it correctly when we encounter the
		# next one.
		lastfield=newfield;

		# Set a flag that we are at the top of the field.
		# Some fields need this to emit HTML code at the
		# beginning.
		first=1;

		# Store the name of the current field
		field=newfield;

		# Emit the header of the field. This includes special
		# code for SEE ALSO (that's two words. The code
		# above can handle only the first).
		if (newfield=="SEE")
		    print "<DL>\n<DT>SEE ALSO\n<DD>" >> out;
		else
		    print "<DL>\n<DT>"newfield"\n<DD>" >> out;

		# Special handling for certain fields.
		if (field=="EXAMPLE")
		{
		    # Clear the variable with the information about the
		    # kind of example in this file
		    example_is="";
		}
		else if (field=="INPUTS" || field=="HISTORY")
		{
		    # These fields are lists
		    print "<DL COMPACT>\n" >> out;
		}
	    }
	    else if (match($0,/^.\*\*\*\*\*+\/?$/)) # Is this the end ?
	    {
		# The last field was an example ? Then it might be
		# neccessary to close a pending <PRE>.
		if (lastfield=="EXAMPLE")
		{
		    if (example_is=="here" || example_is=="example")
			printf ("</PRE>") >> out;
		}
		else if (lastfield=="INPUTS" || lastfield=="HISTORY")
		{
		    # If the last field as a list, then we must
		    # close that, too.
		    print "</DL>\n" >> out;
		}

		# To get a nice format, we stuff every field in a HTML
		# list.
		if (lastfield!="")
		    print "</DL>\n" >> out;

		# New mode: Process the footer next (the header is
		# complete)
		mode="footer";
	    }
	    else
	    {
		# Strip off comment characters at the beginning of the
		# lines.
		if ($1=="#")
		    line=substr($0,2);
		else
		    line=$0;

		# Now do the special processing for each kind of field.
		if (field=="NAME")
		{
		    if (match(line,/[(]/))
		    {
			gsub(/,[ \t]/,",",line);
			if (split(line,a,",")==3)
			{
			    gsub(/AROS_(L|UF)H.*[(]/,"",a[1]);
			    gsub(/[ \t]*[(][ \t]*/,"",a[1]);
			    print prefix a[1] " " a[2] "()<BR>" >> out;
			    fname=a[2];
			}
			else
			{
			    fname=line;
			    gsub(/[ \t]*[(][ \t]*$/,"",fname);
			    type=fname;
			    gsub(/^.*[ \t]+/,"",fname);
			    gsub(/[ \t]+[^ \t]+$/,"",type);
			    print prefix type " " fname "()<BR>" >> out;
			}
		    }
		    else if (match(line,"#include"))
		    {
			match (line,/<.*>/);
			hfile=substr(line,RSTART+1,RLENGTH-2);
			link="srcs/include/" hfile;

			if (exists("../html/" link))
			    line="<A HREF=\"../"link"\">"hfile"</A>";
			else
			    line=hfile;

			print prefix "#include &lt;"line"&gt;<BR>" >> out;
		    }
		}
		else if (field=="SYNOPSIS")
		{
		    gsub(/AROS_(L|UF)HA[(]/,"",line);
		    gsub(/[)][ \t]*,?/,"",line);
		    gsub(/,[ \t]/,",",line);

		    split(line,a,",");
		    print "<TT>"a[1]" "a[2]"</TT><BR>" >> out;
		}
		else if (field=="LOCATION")
		{
		    if (match(line,/,/))
		    {
			gsub(/[)]/,"",line);
			gsub(/,[ \t]/,",",line);

			split(line,a,",");
			print "In " a[2] " at offset " a[3] >> out;
			lib=a[2];
			location=1;
		    }
		}
		else if (field=="INPUTS")
		{
		    if (match(line,/[ \t]*([a-zA-Z_]+,[ \t]*)*[a-zA-Z_]+[ \t]*-/))
		    {
			print "<DT>"substr(line,RSTART,RLENGTH-1) >> out;

			line=substr(line,RSTART+RLENGTH);
			gsub(/&/,"\\&amp;",line);
			gsub(/</,"\\&lt;",line);
			gsub(/>/,"\\&gt;",line);
			gsub(/"/,"\\&quot;",line);

			print "<DD>"line >> out;
		    }
		    else
		    {
			gsub(/&/,"\\&amp;",line);
			gsub(/</,"\\&lt;",line);
			gsub(/>/,"\\&gt;",line);
			gsub(/"/,"\\&quot;",line);

			print line >> out;
		    }

		    #if (line=="" && !first)
		    #	 print "<P>\n" >> out;
		    #else
		    #	 print line >> out;
		    #
		    #first=0;
		}
		else if (field=="FUNCTION" || field=="RESULT" ||
		    field=="NOTES" || field=="BUGS" || field=="INTERNALS")
		{
		    if (line=="" && !first)
			print "<P>\n" >> out;
		    else
		    {
			gsub(/&/,"\\&amp;",line);
			gsub(/</,"\\&lt;",line);
			gsub(/>/,"\\&gt;",line);
			gsub(/"/,"\\&quot;",line);

			if (match(line,/\\begin{.*}/))
			{
			    stack[sp] = env;
			    sp ++;
			    pre=substr(line,1,RSTART-1);
			    env=substr(line,RSTART+7,RLENGTH-8);
			    post=substr(line,RSTART+RLENGTH);

			    if (env=="description")
			    {
				line=pre"\n\n<DL>"post;
			    }
			    else if (env=="itemize")
			    {
				line=pre"\n\n<UL>"post;
			    }
			    else if (env=="enumeration")
			    {
				line=pre"\n\n<OL TYPE=1>"post;
			    }
			}

			if (match(line,/\\item({.*})?/))
			{
			    pre=substr(line,1,RSTART-1);
			    item=substr(line,RSTART+6,RLENGTH-7);
			    post=substr(line,RSTART+RLENGTH);
			    if (env=="description")
			    {
				line=pre"\n\n<DT><B>"item"</B><DD>"post;
			    }
			    else if (env=="itemize")
			    {
				line=pre"\n\n<LI>"post;
			    }
			    else if (env=="enumeration")
			    {
				line=pre"\n\n<OL TYPE=1>"post;
			    }
			}

			if (match(line,/\\end{.*}/))
			{
			    pre=substr(line,1,RSTART-1);
			    endenv=substr(line,RSTART+5,RLENGTH-6);
			    post=substr(line,RSTART+RLENGTH);

			    if (endenv != env)
			    {
				print "ERROR: \\end{"endenv"} doesn't match \\begin{"env"}" >> "/dev/stderr"
				exit 10;
			    }


			    if (env=="description")
			    {
				line=pre"\n</DL>"post;
			    }
			    else if (env=="itemize")
			    {
				line=pre"\n</UL>"post;
			    }
			    else if (env=="enumeration")
			    {
				line=pre"\n</OL>"post;
			    }

			    sp --;
			    env = stack[sp];
			}

			print line >> out;
		    }

		    first=0;
		}
		else if (field=="EXAMPLE")
		{
		    if (first)
		    {
			if (match (line,/#[ \t]*ifdef[ \t]+EXAMPLE/))
			{
			    printf ("<PRE>")>>out;
			    example_is = "example";
			    ifdeflevel = 0;
			    skip = 1;
			}
			else if (match (line,/See[ \t]+[^ \t]+[ \t]*[(][)]/))
			{
			    example_is = line;
			    sub(/^.*See[ \t]+/,"",example_is);
			    sub(/[ \t]*[(][)].*$/,"",example_is);
			    fn = tolower(example_is) ".html";
			    print "See <A HREF=\""fn"\">"example_is"()</A>">>out;
			    skip=1;
			}
			else if (match (line,/See[ \t]below/))
			{
			    example_is="below";
			    close (file);
			    state = "skip";
			    ifdeflevel = 0;

			    printf ("<PRE>")>>out;
			    while ((getline line < file) > 0)
			    {
				if (state=="skip")
				{
				    if (match (line,/^#[ \t]*ifdef[ \t]+TEST/))
					state="found";
				}
				else if (state=="found")
				{
				    if (match (line,/^#[ \t]*ifdef/))
					ifdeflevel ++;
				    else if (match (line,/^#[ \t]*endif/))
				    {
					if (!ifdeflevel)
					{
					    state="skip";
					    continue;
					}
					else
					    ifdeflevel --;
				    }

				    print line >> out;
				}
			    }
			    printf ("</PRE>")>>out;

			    close (file);

			    for (pos=0; pos<fnr; pos++)
				getline line < file;

			    skip=1;
			}
			else
			{
			    example_is="here";
			    printf ("<PRE>")>>out;
			}

			first = 0;
		    }

		    if (example_is=="example" && !skip)
		    {
			if (match (line,/^#[ \t]*ifdef/))
			    ifdeflevel ++;
			else if (match (line,/^#[ \t]*endif/))
			{
			    if (!ifdeflevel)
				skip=1;

			    ifdeflevel --;
			}
		    }

		    if (!skip)
		    {
			gsub(/^(        |\t)/,"",line);
			gsub(/&/,"\\&amp;",line);
			gsub(/</,"\\&lt;",line);
			gsub(/>/,"\\&gt;",line);
			gsub(/"/,"\\&quot;",line);

			print line >> out;
		    }
		    else
			skip --;
		}
		else if (field=="SEE")
		{
		    if (line!="")
		    {
			rest=line;
			line="";

			while (rest!="")
			{
			    if (match(rest,/^[a-zA-Z_][a-zA-Z0-9]*[(][)]/))
			    {
				link=substr(rest,RSTART,RLENGTH-2);
				rest=substr(rest,RSTART+RLENGTH);

				lfile="../html/autodocs/" tolower(link) ".html";

				if (link in special_item)
				    line = line special_item[link];
				else if (link"()" in special_item)
				    line = line special_item[link"()"];
				else if (exists(lfile))
				    line=line "<A HREF=\""tolower(link)".html\">"link"()</A>";
				else
				    line=line " " link "()";
			    }
			    else
			    {
				if (match(rest,/^[^a-zA-Z_]+/))
				    len=RLENGTH;
				else
				    len=1;

				post=substr(rest,1,len);

				gsub(/&/,"\\&amp;",post);
				gsub(/</,"\\&lt;",post);
				gsub(/>/,"\\&gt;",post);
				gsub(/"/,"\\&quot;",post);

				line=line post;
				rest=substr(rest,len+1);
			    }
			}

			print line >> out;
		    }
		}
		else if (field=="HISTORY")
		{
		    if (first)
		    {
			while ((getline < cvslog) > 0)
			    print >> out;

			close (cvslog);

			first=0;
		    }
		}
		else
		    print line >> out;
	    }
	}
	else if (mode=="footer")
	{
	}
    }

    # Anything written to this file ?
    if (out!="")
    {
	# Then emit the footer
	print "</BODY></HTML>" >> out;
    }

    # Close all files
    close (out);
    close (file);

#    printf ("\n") >> stderr;
}

# Return the filename from a complete path
function basename(file) {
    return gensub(/.*\/([a-zA-Z0-9_]+)(\.[a-zA-Z0-9_]+)?$/,"\\1",1,file) "";
}

# Check if a file exists
function exists(file        ,err) {
    # Try to read a line from the file
    err=getline < file;

    # No Error ?
    if (err >= 0)
    {
	# Close the file
	close (file);

	# The file exists
	return 1;
    }

    # The file doesn't exist
    return 0;
}
