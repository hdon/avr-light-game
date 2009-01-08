function chr(n) {
    return String.fromCharCode(n);
}
function ord(c) {
    return c.charCodeAt(0);
}
function hex(n) {
    return n.toString(16);
}
var level_data="", home_pos="\t";
var line_no=0, level_no=-1;
while (1) {
    var line = readline();
    switch (line) {
        /* Begin a new level drawing */
        case 'BEGIN':
            line_no = 0;
            level_no++;
            if (level_no > 0) level_data += '},\n';
            level_data += '\t{';
            break;
        /* End the file */
        case 'END':
            print("uint8_t level_data[][8] = {");
            print(level_data+'}');
            print("}; /* level_data */");
            print("");
            print("uint8_t level_data_home_pos[] = {");
            print(home_pos);
            print("}; /* level_data_home_pos */");
            quit();
        /* Assume: a line of level data */
        default:
            var n = 0;
            line.split('').forEach(function(v, i) {
                if (v == 'x' || v == 'X') n |= 1 << i;
                if (v == 'X' || v == 'V') {
                    if (level_no > 0) home_pos += ', ';
                    home_pos += "0x" + hex(i) + hex(line_no);
                }
            });

            if (line_no > 0) level_data += ', ';
            level_data += n;

            line_no++;
    }
}
