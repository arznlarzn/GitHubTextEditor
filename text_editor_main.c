#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

/*
    This is my first actual C project. I am following along from a tutorial on a website which can be found at link -
    https://viewsourcecode.org/snaptoken/kilo/

    This is a Command Line Interface text editor.
    I am building this using Visual Studio Code and WSL (Windows Subsystem for Linux), and, like in the tutorial, I am using GCC and Make.
    My purpose is to learn more, not only about C, but about things like managing state, memory managment, etc.
    
    
    I tried to follow this a few years ago, but did not understand enough fundamentally about programming in general, about the C language,
    or about other neccessary concepts, such as compiling.
*/

/*
    It may not be necessary, but I included a Makefile and added quite a bit to make it more readable, maintainable, and expandable.
    I've included a clean 'rule' in the Makefile, which will remove the compiled file by running 'make clean'.
*/
/*
    At the beginning of each comment below, it will begin with a number within a pipe, such as |1| which represents the step at which this piece of code was written.
    There are many steps where I will be going back and forth to create this project. Helps me - Helps you.
*/

/* |15|
    Okay, I'm starting to finally get bitwise AND and OR operators.
    Here we are adding a define prepoccessor for to create CTRL keys.
    We are comparing the key pressed to the ASCII value of the control character, which is the key pressed ANDed with 0x1f, or 31 in decimal.
    This is a bitwise AND operator, which compares the bits of the two numbers and returns a new number with the bits that are set in both numbers.
    BITS IN BOTH NUMBERS - 0x1f is 00011111 in binary, and the ASCII value of the key pressed in binary, such as A being 01000001, will return 00000001, which is 1 in decimal.
    I'm feeling excited to work more with bitwise operators... mostly o.O
|15|*/
#define CTRL_KEY(k) ((k) & 0x1f)


//|3| Creating a struct that holds the original termios attributes, before we change them- starting with the prototype.
//Prototypes//

void editorRefreshScreen();

struct editorConfig {
    struct termios orig_termios;
};

struct editorConfig E;
/* |13|
    Now, we are adding error handling. We are going to create a function that will print an error message and exit the program if there is an error.
    We are going to use the perror() function from the stdlib.h library to print the error message.
    exit() comes from stdlib.h and not only exits, but returns a value of 1. (Common indicator of an error)
|13| */
void die(const char *s) {
    //When we have an error, we are going to refresh the screen.
    editorRefreshScreen();
    perror(s);
    exit(1);
}

/* |14
    Now we are adding our die function for error handling, to each of our functions.
14 */
//|3| Creating a disableRawMode function
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

//|2| Creating a function that will begin to enable raw mode. Many steps following will be disabling flags to create a truly 'raw' mode.
void enableRawMode() {
//|3| getting original termios attributes and storing them at the memory address of 'orig_termios'
    // |14| the if statement wrapping our function is for error handling. If there is an error, we will call the die function, which will print the error message and exit the program.
    if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
        die("tcgetattr");
    //|3| using atexit() from the stdlib.h library and calling disableRawMode() which will set termios back to its original attributes
    atexit(disableRawMode);
    //|2| struct termios is a struct variable that holds terminal attributes. Comes in the termios.h library.
    //struct termios raw;
    //|3|changing struct termios raw; to- this way it takes the original attributes, and gives us a struct at which we can begin to make changes.
    struct termios raw = E.orig_termios;
    /* |7|
    Now, we are removing an input flag, the first REAL input flag - IXON.
    The local flags below, ICANON and ISIG, both begin with I, which is supposed to indicate that they are input flags, but they are actually local flags.
    IXON is an flag that allows the terminal to send a software flow control signal, which is CTRL + S and CTRL + Q. CTRL + S stops the terminal from sending output to the screen, and CTRL + Q resumes output.
    We are turning this off by using the bitwise AND operator to turn off IXON in the c_iflag bitmask, below-
    |7| */
    //|9| We are removing the Carriage Return, New Line flag, so that Ctrl + M, or ASCII 13, does not get translated to a New Line character, or ASCII 10.
    //|9| Both Ctrl+M and ENTER now return 13 now, instead of 10. 10 is now, correctly, when we press Ctrl+J.
    // |11| Now, I removing 4 more flags which are likely already off, but it sounds like these used to be critcally assumed part of getting into Raw Mode. They are-
    // |11| BRKINT, INPCK, ISTRIP, and CS8. These flags are not necessary for modern terminals, but are being removed for the sake of the tutorial.
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    //|2| tcgetattr() gets the Current Terminal attributes- getting the attributes of the file descriptor STDIN_FILENO (keyboard input stream), and we are storing them at the address of the variable 'raw'.
    //tcgetattr(STDIN_FILENO, &raw);   |3| we are moving this line to the top of the function and changing it to-
    //|4| - adding the removal of conanical mode to the line below. Now, instead of waiting for the user the press ENTER or RETURN, we are reading input byte-by-byte, also meaning we will exit the moment we press 'Q'.
    //raw.c_lflag &= ~(ECHO); becomes - raw.c_lflag &= ~(ECHO | ICANON);
    //|6| removing the CTRL + C and CTRL + Z signals. This is done by turning off the ISIG flag in the c_lflag bitmask.
    //raw.c_lflag &= ~(ECHO | ICANON); becomes - raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    /* |10|
    We are now turning of output processing, which is our \n or new line character. Before, pressing enter would cause our new line to also carriage return- after adding this, it will not.
    |10| */
    raw.c_oflag &= ~(OPOST);
    // |11| Below is removing CS8, which is a flag that sets the character size to 8 bits per byte. This is not necessary for modern terminals, but is being removed for the sake of the tutorial.
    raw.c_cflag |= (CS8);
    /* |12|
    So that read() doesn't time out while waiting for user input, we will set it so that it returns ever 100ms, even if no input is given. This is done by setting the VMIN and VTIME flags in the c_cc bitmask.
    |12| */
    raw.c_cc[VMIN] = 0; //sets the minimum number of bytes of input needed before read() can return to 0.
    raw.c_cc[VTIME] = 1; //sets the maximum amount of time to wait before read() returns to 1/10th of a second.
    /* |8|
    We are going to add Ctrl + V's control character, by added the IEXTEN flag to the c_lflag bitmask. This flag will allow us to send a literal character to the terminal by pressing CTRL + V, followed by the character.
    Right now, when pressing Ctrl + V, if you have copied anything to your system with Ctrl + C, it will paste, character by character, in our program. We are removing this built in ability by turning off IEXTEN.
    So, raw.c_lflag &= ~(ECHO | ICANON | ISIG); becomes - raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    This is the last local flag we will be turning off.
    |8| */
    // 8!!! So, in VSC or WSL, when pressing Ctrl + V, it still has the shortcut - paste, so I thought it wasn't working. HOWEVER
    // 8!!! In Cygwin, it works. I am going to use Cygwin to test this project from now on, and it could be that I am missing something, however, the intended consequences are happening-
    // 8!!! I am learning and more importantly, UNDERSTANDING AND RETAINING! LETS GO!!!!
    raw.c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);
    //|2| c_lflag is a flag that Controls the Local behavior of the terminal. We are using the bitwise AND operator to turn off ECHO, a bitmask within c_lflag, a bitfield.
        //&= is the bitwise AND assignment operator. If we just had &= ECHO, we would turn off ALL other flags except for ECHO. We want to keep the current value, except we turn OFF ECHO by adding the bitwise NOT operator ~.
        //&= clears specific bits while leaving others unchanged.
        //!!!DIVE FURTHER INTO BITFIELDS AFTER THIS PROJECT. We are flipping ones and zeros and it'd be too lengthy to explain here.!!!
    //|14| the IF statment around our function is for errorhandling. If there is an error, we will call the die function, and our printf will tell us it is related to tcsetattr.
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
    //|2| tcsetattr() sets the terminal attributes. We are setting the attributes of the file descriptor STDIN_FILENO (keyboard input stream), and we are setting them to the attributes stored/changed in the variable 'raw'.
        //TCSAFLUSH is an argument that specifies when to apply the change. TCSAFLUSH disregards any input that hasn't been read and output that hasn't been written.
}
//|17| We are moving th emain loop in main to functions- this is going to read our input, it is going to continue even if we don't get 1 byte of data to c because of the while loop, and will return the character that does get read. We've moved the error handling to this function as well.
//By using nread != 1, we only stop when exactly one byte is read successfully.
char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

// |19| We are drawing tildes ~ down the left side of the screen, 50 of them. This is called in 'refresh screen' function.
void editorDrawRows() {
    int y;
    for (y = 0; y < 50; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}


//|18| We are creating a function that will refresh the screen. We are going to use the ANSI escape code to clear the screen. This is done by writing the escape code to the standard output.
//|18| \x means hexadecimal, 1b is 27 in hexadecimal, so ESC(Escape).
//|18| Escape sequences all start with [. Then we have J which means clear the screen, and 2 which tells it the entire screen. By default it is zero.
//|18| Finally, the 4 tells it there will be 4 bytes- 1b, [, J, and 2.
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    //|19|adding escape sequence to move our cursor to position 1;1H/ H is the command to position cursor, so you'd put numbers seperated by a semicolon to position it / since both arguments would be the same we can just put one H / This is only 3 bytes.
    write(STDOUT_FILENO, "\x1b[H", 3);
//|19| We are calling the function to draw the rows of tildes.
    editorDrawRows();
//|19| Then we are moving the cursor back to the top left of the screen.
    write(STDOUT_FILENO, "\x1b[H", 3);
}

//|17| We are creating a function that 'processes' the input and basically checks for Ctrl Q to quit, while returning a 0, telling us there was no error. We are no longer printing out anythign upon input. Not yet.
void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            editorRefreshScreen();
            exit(0);
            break;
    }
}

int main() {
    //|2| calling the function to enable raw mode.
    enableRawMode();
    //|18| adding screen refresh to while loop-
    // |17| replacing the code below with functions editorReadKey and editorProcessKeypress
    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    //|1|-Creating a variable that holds input from the keyboard input stream and name it 'c'
    //|1|-char's hold 1 byte of memory, or 8 bits, so this is enough to hold a single character- line below: char c;
    //char c;
    /* |12| *removing above line char c;*
    Updating our while loop from 
        while  (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') 
    to -
    |12| */
            /* |17| moving all of this code up into functions and replacing with those functions
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");
            |17| */
            // |14| We are checking if errno is equal to EAGAIN, because in Cygwin, if read() times out, it returns -1 with an errno of EAGAIN, instead of zero (like it's supposed to o.O - have to think about different terminals!)
            // |14| So, we are making sure Cygwin behaves correctly.
    /* |1|
    -Now, we need to create a loop that continually reads for the input from the keyboard stream and saves it to the variable 'c'.

    We will do this with the read() function, reading from STDIN_FILENO which is the keyboard input stream. The second argument is the buffer where we will store the input,
    which is &c (address of c), and the third argument is the number of bytes to read, which is 1 byte. Because we are doing this in a 'while' loop, the condition is that the read() function is equal to 1 byte.
    If the read() function is not equal to 1 byte, then the loop will break and the program will end.

    We also added a second conditional to the while loop, which is that the character is NOT equal to 'q'. So, if the user types a word or the letter q and hit's enter, the program will only read up until the 'q'.
    Anything typed after the letter q will overflow into terminal. -
    |1| */
    /* |5|
    - We are going to beginning printing characters as the user enters them, however, we are going to check if keys pressed are control characters - characters that cannot be printed- and if they are, we will print the ASCII value.
    If the characters pressed are NOT control characters, we will print both their ASCII numbers, followed by their printable characters.
    Below is the if loop that checks if they are control characters, and also prints both their ASCII numbers (and) their printable characters.
    |5| */
    // |10| To fix our new lines, which no longer carriage return, we must add \r for return, to correctly bring our cursor back to the left side of the screen.
        /* |17| moving all of our code into functions and replacing with those functions
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        // |16| we are changing the statement so that it no longer breaks on 'q' but on CTRL + q. This is made possible (and EASIER) because of our CTRL_KEY macro.
        if (c == CTRL_KEY('q')) break;
    }
    return 0;
    |17| */
}

