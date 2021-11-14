#ifndef HEADER_LINEREADER
#define HEADER_LINEREADER

// Struct that footballs our input around.
typedef struct {
    short int cmd;      // FLOCK COMMAND
    short int l_type;   // FLOCK TYPE
    int start;          // START INDEX
    int length;         // OFFSET
    short int whence;   // sᴉ sᴉɥʇ ɹǝʌǝʇɐɥʍ
    unsigned char quit; // SHOULD IT QUIT ? SHOULD IT ?
    unsigned char help; // HELP, NEVER NEEDED UNTIL SOMEONE UNCHARACTERISTICALLY YELLS AT YOU THROUGH AN IDE.
} Options;

void getLockingOptions(Options *options);
void setToDefault(Options *options);

#endif