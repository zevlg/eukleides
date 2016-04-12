#!/usr/bin/env perl
# Eukleides localized keywords extraction script
# Copyright (c) Christian Obrecht 2004-2010

sub unaccent {
	$arg = shift;
	$arg =~ tr/ÀÁÂÃÄÅÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜİàáâãäåçèéêëìíîïñòóôõöøùúûüıÿ \
		  /AAAAAACEEEEIIIINOOOOOOUUUUYaaaaaaceeeeiiiinoooooouuuuyy/;
	return $arg;
}

open($in, "<", $ARGV[0]) or die "Unable to open input file.";

open($out, ">", $ARGV[1]) or die "Unable to open output file.";

$PAPER = ",a4paper" if ($ARGV[2] eq "A4");

while (<$in>) {
	if (/"Language-Team: ([^\\]+)\\n"/) {
		$lang = $1;
	}
	if (/msgid "([^"]+)"/) {
		$key = $1;
	}
	if (/msgstr "([^"]+)"/ and $key ne $1) {
		$table{$key} = $1;
	}
	last if (/^### End of keywords ###/);
}

%elbat = reverse %table;

print $out <<LaTeX;
\\documentclass[twocolumn$PAPER]{article}
\\usepackage[latin1]{inputenc}
\\usepackage[T1]{fontenc}
\\usepackage{ae}
\\def\\entry#1#2{\\textbf{#1}\\quad#2\\\\}
\\title{$lang Localized Eukleides Keywords}
\\date{}
\\nofiles
\\begin{document}
\\maketitle
\\section*{English to $lang}
LaTeX

for $k (sort keys %table) {
	print $out "\\entry{$k}{$table{$k}}\n";
}

print $out "\\section*{$lang to English}\n";

for $k (sort {unaccent($a) cmp unaccent($b)} keys %elbat) {
	print $out "\\entry{$k}{$elbat{$k}}\n";
}

print $out "\\end{document}\n";
