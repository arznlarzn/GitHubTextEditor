#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

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

//|3| Creating a struct that holds the original termios attributes, before we change them- starting with the prototype.
//Prototypes//
struct termios orig_termios;

//|3| Creating a disableRawMode function
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

//|2| Creating a function that will begin to enable raw mode. Many steps following will be disabling flags to create a truly 'raw' mode.
void enableRawMode() {
    //|3| getting original termios attributes and storing them at the memory address of 'orig_termios'
    tcgetattr(STDIN_FILENO, &orig_termios);
    //|3| using atexit() from the stdlib.h library and calling disableRawMode() which will set termios back to its original attributes
    atexit(disableRawMode);
    //|2| struct termios is a struct variable that holds terminal attributes. Comes in the termios.h library.
    //struct termios raw;
    //|3|changing struct termios raw; to- this way it takes the original attributes, and gives us a struct at which we can begin to make changes.
    struct termios raw = orig_termios;
    /* |7|
    Now, we are removing an input flag, the first REAL input flag - IXON.
    The local flags below, ICANON and ISIG, both begin with I, which is supposed to indicate that they are input flags, but they are actually local flags.
    IXON is an flag that allows the terminal to send a software flow control signal, which is CTRL + S and CTRL + Q. CTRL + S stops the terminal from sending output to the screen, and CTRL + Q resumes output.
    We are turning this off by using the bitwise AND operator to turn off IXON in the c_iflag bitmask, below-
    |7| */
    raw.c_iflag &= ~(IXON);
    //|2| tcgetattr() gets the Current Terminal attributes- getting the attributes of the file descriptor STDIN_FILENO (keyboard input stream), and we are storing them at the address of the variable 'raw'.
    //tcgetattr(STDIN_FILENO, &raw);   |3| we are moving this line to the top of the function and changing it to-
    //|4| - adding the removal of conanical mode to the line below. Now, instead of waiting for the user the press ENTER or RETURN, we are reading input byte-by-byte, also meaning we will exit the moment we press 'Q'.
    //raw.c_lflag &= ~(ECHO); becomes - raw.c_lflag &= ~(ECHO | ICANON);
    //|6| removing the CTRL + C and CTRL + Z signals. This is done by turning off the ISIG flag in the c_lflag bitmask.
    //raw.c_lflag &= ~(ECHO | ICANON); becomes - raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    /* |8|
    We are going to add Ctrl + V's control character, by added the IEXTEN flag to the c_lflag bitmask. This flag will allow us to send a literal character to the terminal by pressing CTRL + V, followed by the character.
    Right now, when pressing Ctrl + V, if you have copied anything to your system with Ctrl + C, it will paste, character by character, in our program. We are removing this built in ability by turning off IEXTEN.
    So, raw.c_lflag &= ~(ECHO | ICANON | ISIG); becomes - raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    This is the last local flag we will be turning off.
    |8| */
    //   |!8!| Okay, for some reason, after adding IEXTEN, I could not get the program to print out it's ASCII character or be recognized at all. It just would print what was copied to the keyboard. I had to make some changes within visual studio codes shortcuts.
    //   |!8!| It doesn't make sense, but I had to delete and re-add the shortcut for 'paste'. However, for some reason, it worked.
    //   |!8!| This is not a proper solution, because if you ran it on your computer using VS Code, well, if you had the same problem, then the ASCII character 22 would not work appropriately, however, for the sake of this entire lesson
                                    // THE SHOW GOES ON!!!! XDXDXD
    raw.c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);
    //|2| c_lflag is a flag that Controls the Local behavior of the terminal. We are using the bitwise AND operator to turn off ECHO, a bitmask within c_lflag, a bitfield.
        //&= is the bitwise AND assignment operator. If we just had &= ECHO, we would turn off ALL other flags except for ECHO. We want to keep the current value, except we turn OFF ECHO by adding the bitwise NOT operator ~.
        //&= clears specific bits while leaving others unchanged.
        //!!!DIVE FURTHER INTO BITFIELDS AFTER THIS PROJECT. We are flipping ones and zeros and it'd be too lengthy to explain here.!!!
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    //|2| tcsetattr() sets the terminal attributes. We are setting the attributes of the file descriptor STDIN_FILENO (keyboard input stream), and we are setting them to the attributes stored/changed in the variable 'raw'.
        //TCSAFLUSH is an argument that specifies when to apply the change. TCSAFLUSH disregards any input that hasn't been read and output that hasn't been written.
}

int main() {
    //|2| calling the function to enable raw mode.
    enableRawMode();
    //|1|-Creating a variable that holds input from the keyboard input stream and name it 'c'
    //|1|-char's hold 1 byte of memory, or 8 bits, so this is enough to hold a single character
    char c;

    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') 
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
    {
        if (iscntrl(c)) {
            printf("%d\n", c);
        } else {
            printf("%d ('%c')\n", c, c);
        }
    }
    return 0;
}

