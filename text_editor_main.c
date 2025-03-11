#include <stdio.h>
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

//|2| creating a function that will begin to enable raw mode. Many steps following will be disabling flags to create a truly 'raw' mode.
void enableRawMode() {
    struct termios raw;
    //|2| struct termios is a struct variable that holds terminal attributes. Comes in the termios.h library.
    tcgetattr(STDIN_FILENO, &raw);
    //|2| tcgetattr() gets the Current Terminal attributes- getting the attributes of the file descriptor STDIN_FILENO (keyboard input stream), and we are storing them at the address of the variable 'raw'.
    raw.c_lflag &= ~(ECHO);
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
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
    /* 
    |1|-Now, we need to create a loop that continually reads for the input from the keyboard stream and saves it to the variable 'c'.

    We will do this with the read() function, reading from STDIN_FILENO which is the keyboard input stream. The second argument is the buffer where we will store the input,
    which is &c (address of c), and the third argument is the number of bytes to read, which is 1 byte. Because we are doing this in a 'while' loop, the condition is that the read() function is equal to 1 byte.
    If the read() function is not equal to 1 byte, then the loop will break and the program will end.

    We also added a second conditional to the while loop, which is that the character is NOT equal to 'q'. So, if the user types a word or the letter q and hit's enter, the program will only read up until the 'q'.
    Anything typed after the letter q will overflow into terminal. - |1|
    */
    return 0;
}