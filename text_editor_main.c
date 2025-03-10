#include <stdio.h>
#include <termios.h>

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

int main() {
    printf("Hello World!\n");
    return 0;
}