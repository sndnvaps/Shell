/*	Command Line Interface (Command Shell) for microcontrollers.
	Copyright (C) 2014 Jesus Ruben Santa Anna Zamudio.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
	Author website: http://www.geekfactory.mx
	Author e-mail: ruben at geekfactory dot mx
 */
#ifndef SHELL_H
#define SHELL_H
/*-------------------------------------------------------------*/
/*		Includes and dependencies			*/
/*-------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

/*-------------------------------------------------------------*/
/*		Macros and definitions				*/
/*-------------------------------------------------------------*/
#if !defined(CONFIG_SHELL_MAX_COMMANDS)
/**
 * Defines the maximum number of commands that can be registered
 */
#define CONFIG_SHELL_MAX_COMMANDS		5
#endif
#if !defined(CONFIG_SHELL_MAX_INPUT)
/**
 * Defines the maximum characters that the input buffer can accept
 */
#define CONFIG_SHELL_MAX_INPUT			100
#endif
#if !defined(CONFIG_SHELL_MAX_COMMAND_ARGS)
/**
 * Configures the maximum number of arguments per command tha can be accepted
 */
#define CONFIG_SHELL_MAX_COMMAND_ARGS		10
#endif

#define SHELL_ASCII_NUL				0x00
#define SHELL_ASCII_BEL				0x07
#define SHELL_ASCII_BS				0x08
#define SHELL_ASCII_HT				0x09
#define SHELL_ASCII_LF				0x0A
#define SHELL_ASCII_CR				0x0D
#define SHELL_ASCII_ESC				0x1B
#define SHELL_ASCII_DEL				0x7F
#define SHELL_ASCII_US				0x1F
#define SHELL_ASCII_SP				0x20
#define SHELL_VT100_ARROWUP			'A'
#define SHELL_VT100_ARROWDOWN			'B'
#define SHELL_VT100_ARROWRIGHT			'C'
#define SHELL_VT100_ARROWLEFT			'D'

#define SHELL_RET_SUCCESS			0
#define SHELL_RET_FAILURE			1
#define SHELL_RET_IOPENDING			-1

/*-------------------------------------------------------------*/
/*		Typedefs enums & structs			*/
/*-------------------------------------------------------------*/

/**
 *  Type definition for all the programs invoked by the shell (function pointer)
 */
typedef int (*shell_program_t) (int, char **);

/*
 * Type definition for a function that sends a single character to the remote
 * terminal screen (function pointer)
 */
typedef void (*shell_writer_t) (char);

/*
 * Type definition for a function that reads a single character from the remote
 * terminal (function pointer)
 */
typedef int (*shell_reader_t) (char *);

/**
 * This enumeration defines the errors printed by the programs called by the
 * shell
 */
enum shell_errors {
	E_SHELL_ERR_ARGCOUNT = 0,	//!< There are missing arguments for the command
	E_SHELL_ERR_OUTOFRANGE,		//!< The program received an argument that is out of range
	E_SHELL_ERR_VALUE,		//!< The program received an argument with a value different than expected
	E_SHELL_ERR_ACTION,		//!< Invalid action requested for the current state
	E_SHELL_ERR_PARSE,		//!< Cannot parse the user input
	E_SHELL_ERR_STORAGE,		//!< Cannot access storage device or memory device
	E_SHELL_ERR_IO,			//!< IO device error caused program interruption
};

/**
 * This structure holds the data for every command registered on the shell
 */
struct shell_command_entry {
	shell_program_t shell_program;
	const char * shell_command_string;
};

/*-------------------------------------------------------------*/
/*		Function prototypes				*/
/*-------------------------------------------------------------*/

#ifdef	__cplusplus
extern "C" {
#endif
/**
 * @brief Prepares the command line interface
 *
 * Initializes the resources used by the command line interface. This function
 * also tries to start the command line task if the option is enabled. The main
 * program for the command line interface where all the data is handled is
 * shell_task().
 *
 * @param reader The callback function used to get a character from the stream.
 *
 * @param writer The callback function used to write a character to the stream.
 *
 * @param msg The message to display upon startup of the program
 *
 * @return Returns TRUE if the shell was succesfully initialized, returns FALSE
 * otherwise.
 */
uint8_t shell_init(shell_reader_t reader, shell_writer_t writer, char * msg);

/**
 * @brief Register a command with the command interpreter
 *
 * Registers a command on the available command list. The name of the command
 * and the function that implements it�s functionality should be provided.
 *
 * @param program The type shell_program_t is a pointer to a function
 * that is executed when the written command matches the associated name
 *
 * @param string A string containing the name of the command to be registered.
 *
 * @return Returns TRUE if command was successfully added to the command list,
 * or returns FALSE if something fails (no more commands can be registered).
 */
uint8_t shell_register(shell_program_t program, const char * string);

/**
 * @brief Unregister all commands
 *
 * Errases all entries on the command list, returning it to it�s default status.
 */
void shell_unregister_all();

/**
 * @brief Prints the list of registered commands
 *
 * Prints a list of available commands to the terminal using the callback
 * functions provided on initialization.
 */
void shell_print_commands();

/**
 * @brief Prints error messages to the terminal screen
 *
 * This function presents an alternative for displaying program errors in a
 * uniform format.
 *
 * @param error The code (ID) of the error to print
 *
 * @param field The name of the parameter or variable where the error was
 * detected.
 */
void shell_print_error(int error, const char * field);

/**
 * @brief Prints a null terminated string to the terminal
 *
 * Displays a string on the terminal. The string should be null terminated.
 *
 * @param string The string to send to the terminal
 */
void shell_print(const char * string);

/**
 * @brief Prints a string and moves the cursor to the next line
 *
 * Displays a string on the terminal and moves the cursor to the next line. The
 * string should be null terminated.
 *
 * @param string The string to send to the terminal
 */
void shell_println(const char * string);

/**
 * @brief Prints formatted text to the terminal
 *
 * Displays a string (fmt) on the terminal. If the string includes format
 * specifiers (subsequences beginning with '%'), the additional arguments
 * following format are formatted and inserted in the resulting string
 * replacing their respective specifiers.
 *
 * This function implements it�s own mechanism for text formatting. It doesn�t
 * rely on the native print functions.
 *
 * @param fmt The string to send to the terminal, the string can include format
 * specifiers in a similar fashion to printf standard function.
 *
 * @param ... Aditional arguments that are inserted on the string as text
 */
void shell_printf(char * fmt, ...);

/**
 * @brief Main Shell processing loop
 *
 * This function implements the main functionality of the command line interface
 * this function should be called frequently so it can handle the input from the
 * data stream.
 */
void shell_task();

#ifdef	__cplusplus
}
#endif

#endif
// End of Header file