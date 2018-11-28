#!/usr/bin/awk -f

BEGIN {
        lines=0;
}
{
        ++lines;
        if (lines >= 6 && lines <= 35) {
                print $7;
        }
}
END {
}

